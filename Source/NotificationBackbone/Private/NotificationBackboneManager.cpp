// Fill out your copyright notice in the Description page of Project Settings.

#include "NotificationBackboneManager.h"


void FNotificationBackboneManager::RegisterForNotifications(TSharedRef<INotificationBackboneListenerRaw> listener, FName feed)
{
	CreateNotificationFeedWhenNotExists(feed);
	(*notificationFeeds.Find(feed))->AddListener(listener);
}

void FNotificationBackboneManager::UnregisterFromNotifications(TSharedRef<INotificationBackboneListenerRaw> listener, FName feed)
{
	TSharedRef<FNotificationBackboneNotificationFeed>* pfeed = notificationFeeds.Find(feed);
	if (pfeed)
	{
		(*pfeed)->RemoveListener(listener);
		RemoveNotificationFeedWhenEmpty(feed);
	}
}

void FNotificationBackboneManager::DispatchNotification(const FNotificationBackboneNotification& notification)
{
	CreateNotificationFeedWhenNotExists(notification.feed);
	(*notificationFeeds.Find(notification.feed))->EnqueueNotification(notification);
	RemoveNotificationFeedWhenEmpty(notification.feed);
}

bool FNotificationBackboneManager::ClearNotificationFeedNotifications(const FName& feed)
{
	TSharedRef<FNotificationBackboneNotificationFeed>* pfeed = notificationFeeds.Find(feed);
	if (pfeed)
	{
		(*pfeed)->ClearNotifications();
		RemoveNotificationFeedWhenEmpty(feed);
		return true;
	}
	return false;
}

void FNotificationBackboneManager::RegisterForQueries(TSharedRef<INotificationBackboneListenerRaw> listener)
{
	if (listener == queryListenerRaw) return;
	OnQueryListenerChanges();
	queryListenerRaw = TWeakPtr<INotificationBackboneListenerRaw>(listener);
	MF_LOG(Log, false, "We have a new raw listener for queries. Name: %s", *listener->GetNotificationBackboneListenerName().ToString());
	DispatchQueryFromQueue();
}

void FNotificationBackboneManager::UnregisterFromQueries(TSharedRef<INotificationBackboneListenerRaw> listener)
{
	if (listener == queryListenerRaw)
	{
		MF_LOG(Log, false, "The current raw listener for queries unregistered. Name: %s", *listener->GetNotificationBackboneListenerName().ToString());
		queryListenerRaw = nullptr;
	}
}

bool FNotificationBackboneManager::EnqueueQuery(const FNotificationBackboneQueryData& queryData, const FNotificationBackboneDynamicDelegate& callback)
{
	if (queryData.options.Num() == 0 && queryData.customOptions.Num() == 0)
	{
		MF_LOG(Error, true, "You tried to enqueue a query that does not have query options. Query rejected!: Title: %s, Message: %s", *queryData.title.ToString(), *queryData.message.ToString());
		return false;
	}

	if (callback.IsBound())
	{
		FNotificationBackboneQuery query;
		query.data = queryData;
		query.id.idInternal = FGuid::NewGuid();

		querieQueue.Enqueue(FCachedQuery(MoveTemp(query), callback));
		if (querieQueue.Num() > 4)
		{
			MF_LOG(Warning, true, "Queries are piling up, you should get to work! QueryCount: %d", querieQueue.Num());
		}

		DispatchQueryFromQueue();

		return true;
	}
	else
	{
		MF_LOG(Error, true, "Tried to enqueue a query with a invalid callback: Title: %s \t Message: %s", *queryData.title.ToString(), *queryData.message.ToString());
	}

	return false;
}

void FNotificationBackboneManager::AnswerQuery(const FNotificationBackboneQueryId& queryId, ENotificationBackboneQueryOptions optionChoosen)
{
	check(queryId.idInternal.IsValid());
	check(queryId == queryIdWaitingForAnswer);
	if (!querieQueue.IsEmpty())
	{
		FCachedQuery queueTail;
		if (querieQueue.Peek(queueTail) && queueTail.query.id == queryId)
		{
			if (queueTail.callback.IsBound())
			{
				check(querieQueue.Dequeue(queueTail));
				FNotificationBackboneQueryResult result;
				result.data = MoveTemp(queueTail.query.data);
				result.optionChoosen = optionChoosen;
				queueTail.callback.Execute(result);
				queryIdWaitingForAnswer.Invalidate();
				MF_LOG(Log, false, "Query got answered: ID: %s", *queueTail.query.id.idInternal.ToString()); // TODO for debugging

				// Dispatch the next query
				DispatchQueryFromQueue();
			}
			else
			{
				MF_LOG(Error, true, "Query got answered but we lost the callback. Trashing the query! ID: %s", *queueTail.query.id.idInternal.ToString());
				check(querieQueue.Pop());
				queryIdWaitingForAnswer.Invalidate();

				// Dispatch the next query
				DispatchQueryFromQueue();
			}
		}
		else
		{
			check(0); // should never appear
		}
	}
	else
	{
		MF_LOG(Error, true, "Tried to answer to a query, but there are no queries left!");
	}
}

void FNotificationBackboneManager::RegisterForNotificationsUObject(TScriptInterface<INotificationBackboneListener> listenerObject, FName feed)
{
	CreateNotificationFeedWhenNotExists(feed);
	(*notificationFeeds.Find(feed))->AddListenerObject(listenerObject);
}

void FNotificationBackboneManager::UnregisterFromNotificationsUObject(TScriptInterface<INotificationBackboneListener> listenerObject, FName feed)
{
	TSharedRef<FNotificationBackboneNotificationFeed>* pfeed = notificationFeeds.Find(feed);
	if (pfeed)
	{
		(*pfeed)->RemoveListenerObject(listenerObject);
		RemoveNotificationFeedWhenEmpty(feed);
	}
}

void FNotificationBackboneManager::RegisterForQueriesUObject(TScriptInterface<INotificationBackboneListener> listenerObject)
{
	if (!listenerObject.GetObject()) return;
	if (listenerObject.GetObject() == queryListenerObject.GetObject()) return;
	OnQueryListenerChanges();
	queryListenerObject = listenerObject;
	MF_LOG(Log, false, "We have a new object listener for queries. Name: %s", *listenerObject.GetObject()->GetName());
	DispatchQueryFromQueue();
}

void FNotificationBackboneManager::UnregisterFromQueriesUObject(const TScriptInterface<INotificationBackboneListener> listenerObject)
{
	if (!listenerObject.GetObject()) return;
	if (queryListenerObject == listenerObject)
	{
		MF_LOG(Log, false, "The current object listener for queries unregistered. Name: %s", *listenerObject.GetObject()->GetName());
		queryListenerObject = nullptr;
	}
}

void FNotificationBackboneManager::OnQueryListenerChanges()
{
	TSharedPtr<INotificationBackboneListenerRaw> pinnedRaw = queryListenerRaw.Pin();
	if (pinnedRaw.IsValid())
	{
		pinnedRaw->OnQueryRegistrationStolen();
	}

	if (queryListenerObject.GetObject() && queryListenerObject.GetObject()->IsValidLowLevel())
	{
		INotificationBackboneListener::Execute_OnQueryRegistrationStolen(queryListenerObject.GetObject());
	}

	ClearQueryListener();
}

void FNotificationBackboneManager::CreateNotificationFeedWhenNotExists(const FName& feed)
{
	if (!notificationFeeds.Contains(feed))
	{
		notificationFeeds.Add(feed, TSharedRef<FNotificationBackboneNotificationFeed>(new FNotificationBackboneNotificationFeed(feed)));
	}
}

void FNotificationBackboneManager::RemoveNotificationFeedWhenEmpty(const FName& feed)
{
	TSharedRef<FNotificationBackboneNotificationFeed>* pfeed = notificationFeeds.Find(feed);
	if (pfeed)
	{
		if (!(*pfeed)->GetDoesHaveListeners() && !(*pfeed)->GetDoesHaveNotifications())
		{
			notificationFeeds.Remove(feed);
		}
	}
}

void FNotificationBackboneManager::ClearNotificationFeeds()
{
	MF_LOG(Log, false, "Clearing notification listeners.");
	notificationFeeds.Empty();
}

void FNotificationBackboneManager::ClearQueryListener()
{
	MF_LOG(Log, false, "Clearing query listener.");
	queryListenerRaw = nullptr;
	queryListenerObject = nullptr;
}

void FNotificationBackboneManager::DispatchQueryFromQueue()
{
	static bool bDispatchQueryAgain = false;

	if (!querieQueue.IsEmpty() && !GetIsSomeoneListeningForQueries())
	{
		MF_LOG(Warning, true, "We have enqueued queries but no listener to answer them! Make sure to register a listener for quries on the NotificationBackbone.");
	}

	if (!queryIdWaitingForAnswer.IsValid() && !querieQueue.IsEmpty() && GetIsSomeoneListeningForQueries())
	{
		FCachedQuery queueTail;
		if (querieQueue.Peek(queueTail))
		{
			TSharedPtr<INotificationBackboneListenerRaw> pinnedRaw = queryListenerRaw.Pin();
			if (pinnedRaw.IsValid()) // Dispatch for C++ listener
			{
				queryIdWaitingForAnswer = queueTail.query.id;
				pinnedRaw->OnQuery(queueTail.query);
				MF_LOG(Log, false, "Query got dispatched: ID: %s", *queueTail.query.id.idInternal.ToString()); // TODO for debugging
			}
			else if (queryListenerObject.GetObject()) // Dispatch for BP listener
			{
				queryIdWaitingForAnswer = queueTail.query.id;
				INotificationBackboneListener::Execute_OnQuery(queryListenerObject.GetObject(), queueTail.query);
				MF_LOG(Log, false, "Query got dispatched: ID: %s", *queueTail.query.id.idInternal.ToString()) // TODO for debugging
			}
		}
		else
		{
			check(0); // should never appear
		}
	}
}
