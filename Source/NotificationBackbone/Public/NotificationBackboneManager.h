// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NotificationBackbone.h"
#include "CoreMinimal.h"
#include "NotificationBackboneBPTypes.h"
#include "NotificationBackboneListener.h"
#include "Editor.h"
#include "NotificationBackboneNotificationFeed.h"
#include "QueueCustom.h"
#include "NotificationBackboneDeclarations.h"


/**
 * TODO Check at some point if still valid
 *
 *This is the core of the NotificationBackbone. Here messages come in, get managed and rerouted to subscribers.
 *
 *	Notifications:
 *	Notifications are handled per feed. A feed is defined by a name the user chooses.
 *  Notifications for a particular feed get dispatched immediately as soon as they come in and we have subscribers for that feed.
 *  Per default: Notifications that could not get dispatched because there is no subscriber, will get lost.
 *	You can change that in the settings via the editor.
 *  Each notification feed can have multiple subscribers.
 *
 *	Queries:
 *  Queries get dispatched one at a time. If a query comes in and there is a outstanding query,
 *	it gets cached and dispatched as soon as the current query gets answered by the user.
 *  Queries get cached if there is no subscriber currently.
 *	There can only be one subscriber for queries.
 */
class NOTIFICATIONBACKBONE_API FNotificationBackboneManager
{
public:
	static FNotificationBackboneManager& Get()
	{
		// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
		static FNotificationBackboneManager instance;
		return instance;
	}

	// This is for UObjects only
	void RegisterForNotificationsUObject(TScriptInterface<INotificationBackboneListener> listenerObject, FName feed);
	void UnregisterFromNotificationsUObject(TScriptInterface<INotificationBackboneListener> listenerObject, FName feed);

	// This is for raw objects only.
	void RegisterForNotifications(TSharedRef<INotificationBackboneListenerRaw> listener, FName feed);
	void UnregisterFromNotifications(TSharedRef<INotificationBackboneListenerRaw> listener, FName feed);

	void DispatchNotification(const FNotificationBackboneNotification& notification);
	// Clear the notifications of the specified feed.
	// Returns false when the feed does not exist
	bool ClearNotificationFeedNotifications(const FName& feed);

	// This is for UObjects only
	void RegisterForQueriesUObject(TScriptInterface<INotificationBackboneListener> listenerObject);
	void UnregisterFromQueriesUObject(const TScriptInterface<INotificationBackboneListener> listenerObject);

	// This is for raw objects only.
	void RegisterForQueries(TSharedRef<INotificationBackboneListenerRaw> listener);
	void UnregisterFromQueries(TSharedRef<INotificationBackboneListenerRaw> listener);

	/**
	 * When declaring the query options, only one of each should be defined.
	 */
	bool EnqueueQuery(const FNotificationBackboneQueryData& queryData, const FNotificationBackboneDynamicDelegate& callback);
	void AnswerQuery(const FNotificationBackboneQueryId& queryId, ENotificationBackboneQueryOptions optionChoosen);

	bool GetIsSomeoneListeningForQueries() const
	{
		return queryListenerRaw.IsValid() || queryListenerObject.GetObject()->IsValidLowLevelFast();
	}

	// Returns empty string if no listener
	FString GetQueryListenerName()
	{
		if (queryListenerRaw.IsValid())
		{
			TSharedPtr<INotificationBackboneListenerRaw> pinnedRaw = queryListenerRaw.Pin();
			if (pinnedRaw.IsValid())
			{
				return pinnedRaw->GetNotificationBackboneListenerName().ToString();
			}
		}
		else if (queryListenerObject.GetObject() && queryListenerObject.GetObject()->IsValidLowLevelFast())
		{
			return queryListenerObject.GetObject()->GetName();
		}
		return FString();
	}

	/**
	 * Do NOT hold the reference to the returned feed. When the feed is empty
	 * we will get rid of it and create a new one if necessary. 
	 * Returns the a pointer to the feed if it exists, nullptr otherwise
	 */
	TSharedRef<FNotificationBackboneNotificationFeed>* GetNotificationFeed(const FName& name)
	{
		return notificationFeeds.Find(name);
	}

	bool GetDoesNotificationFeedExist(const FName& feed) const
	{
		return notificationFeeds.Contains(feed);
	}

	// Returns the names of the existing feeds
	void GetNotificationFeedNames(TArray<FName>& feeds) const
	{
		feeds.Reset(notificationFeeds.Num());
		notificationFeeds.GetKeys(feeds);
	}

protected:
	virtual void OnQueryListenerChanges();
	// Create a new notification feed if it does not exist yet.
	virtual void CreateNotificationFeedWhenNotExists(const FName& feed);
	virtual void RemoveNotificationFeedWhenEmpty(const FName& feed);

	virtual void ClearNotificationFeeds();
	virtual void ClearQueryListener();
	virtual void ClearListeners()
	{
		ClearNotificationFeeds();
		ClearQueryListener();
	}

	virtual void OnEndPlayInEditor(bool bIsSimulating)
	{
		ClearListeners();
		querieQueue.Empty(); // clear the queue
		queryIdWaitingForAnswer.Invalidate();
	}

	virtual ~FNotificationBackboneManager()
	{
		ClearListeners();
		querieQueue.Empty(); // clear the queue
		queryIdWaitingForAnswer.Invalidate();
	}
	FNotificationBackboneManager()
	{
		FEditorDelegates::EndPIE.AddRaw(this, &FNotificationBackboneManager::OnEndPlayInEditor);
	}
private:
	// Make singleton class
	FNotificationBackboneManager(const FNotificationBackboneManager& other) = delete;
	FNotificationBackboneManager(FNotificationBackboneManager&& rvalue) = delete;
	FNotificationBackboneManager& operator=(const FNotificationBackboneManager& other) = delete;
	FNotificationBackboneManager& operator=(FNotificationBackboneManager&& rvalue) = delete;

	void DispatchQueryFromQueue();

#pragma region Query
	// Only one of these should ever be active!
	TWeakPtr<INotificationBackboneListenerRaw> queryListenerRaw;
	TScriptInterface<INotificationBackboneListener> queryListenerObject;

	struct FCachedQuery
	{
		FCachedQuery() {} // Do not want that, but the queue needs it
		FCachedQuery(FNotificationBackboneQuery&& in_query, const FNotificationBackboneDynamicDelegate& in_callback)
			: query(MoveTemp(in_query)), callback(in_callback)
		{}
		FNotificationBackboneQuery query;
		FNotificationBackboneDynamicDelegate callback;
	};
	TQueueCustom<FCachedQuery, EQueueMode::Spsc> querieQueue;
	FNotificationBackboneQueryId queryIdWaitingForAnswer; // Query we dispatched last and needs to get answered
#pragma endregion Query

#pragma region Notification
	TMap<FName, TSharedRef<FNotificationBackboneNotificationFeed>> notificationFeeds;
#pragma endregion Notification
};
