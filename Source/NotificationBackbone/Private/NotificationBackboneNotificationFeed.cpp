// Fill out your copyright notice in the Description page of Project Settings.

#include "NotificationBackboneNotificationFeed.h"

FNotificationBackboneNotificationFeed::FNotificationBackboneNotificationFeed(const FName& in_feedName) : feedName(in_feedName)
{
	// look for settings that belong to this feed.
	const FNotificationBackboneFeedSettings* p_settings = UNotificationBackboneSettings::Get()->feedSettings.FindByPredicate(
		[in_feedName](const FNotificationBackboneFeedSettings& in_settings) -> bool
	{
		return in_settings.feed == in_feedName;
	});

	if (p_settings)
	{
		settings = *p_settings;
	}

	// Create delegate for ticker
	tickerDelegate = FTickerDelegate::CreateRaw(this, &FNotificationBackboneNotificationFeed::DispatchNotificationFromQueue);

	MF_LOG(Log, false, "New notification feed created. FeedName: %s", *feedName.ToString());
}

FNotificationBackboneNotificationFeed::~FNotificationBackboneNotificationFeed()
{
	MF_LOG(Log, false, "Notification feed got destroyed: FeedName: %s", *feedName.ToString());

	// Remove our ticker delegate from the ticker. Else bad things might happen if the ticker tries to callback to a destroyed object.
	FTicker::GetCoreTicker().RemoveTicker(tickerDelegateHandle);

	if (!notificationQueue.IsEmpty())
	{
		MF_LOG(Warning, true, "Notification feed got destroyed but there were notifications left: FeedName: %s, NumNotifications: %d", *feedName.ToString(), notificationQueue.Num());
	}
}

void FNotificationBackboneNotificationFeed::AddListener(TSharedRef<INotificationBackboneListenerRaw> listener)
{
	listenersRaw.Add(TWeakPtr<INotificationBackboneListenerRaw>(listener));
	StartDispatchTicker();
}

void FNotificationBackboneNotificationFeed::RemoveListener(TSharedRef<INotificationBackboneListenerRaw> listener)
{
	listenersRaw.Remove(listener);
	if (!GetDoesHaveListeners() && settings.bClearNotificationsNoListeners)
	{
		ClearNotifications();
	}
}

void FNotificationBackboneNotificationFeed::AddListenerObject(TScriptInterface<INotificationBackboneListener> listener)
{
	listenersObject.Add(listener);
	StartDispatchTicker();
}

void FNotificationBackboneNotificationFeed::RemoveListenerObject(TScriptInterface<INotificationBackboneListener> listener)
{
	listenersObject.Remove(listener);
	if (listenersObject.Num() == 0 && settings.bClearNotificationsNoListeners)
	{
		ClearNotifications();
	}
}

bool FNotificationBackboneNotificationFeed::DispatchNotificationFromQueue(float deltaSeconds /*= 0.f*/)
{
	if (!bBlockDispatch && GetDoesHaveListeners() && !notificationQueue.IsEmpty())
	{
		FNotificationBackboneNotification notification;
		if (notificationQueue.Dequeue(notification))
		{
			notification.feedDispatchDelay = settings.dispatchDelay;

			// object listener
			for (auto objectIter = listenersObject.CreateIterator(); objectIter; ++objectIter)
			{
				if (objectIter->GetObject() && objectIter->GetObject()->IsValidLowLevelFast())
				{
					INotificationBackboneListener::Execute_OnNotification(objectIter->GetObject(), notification);
				}
				else
				{
					objectIter.RemoveCurrent();
				}
			}

			// raw listener
			for (auto rawIter = listenersRaw.CreateIterator(); rawIter; ++rawIter)
			{
				if (rawIter->IsValid())
				{
					TSharedPtr<INotificationBackboneListenerRaw> pinnedRaw = rawIter->Pin();
					if (pinnedRaw.IsValid())
					{
						pinnedRaw->OnNotification(notification);
					}
				}
				else
				{
					rawIter.RemoveCurrent();
				}
			}
		}
		else
		{
			check(0); // Should never reach this
		}

		// We just dispatched. Even if there is no more notification enqueued, we must wait another delay.
		// Otherwise we might do a dispatch where should be a delay.
	}
	else
	{
		// This is the only place where the IsTickerActive state shall reach the false state.
		bIsTickerActive = false;
	}

	return bIsTickerActive;
}

void FNotificationBackboneNotificationFeed::EnqueueNotification(const FNotificationBackboneNotification& notification)
{
	if (GetDoesHaveListeners())
	{
		notificationQueue.Enqueue(notification);
	}
	else
	{
		if (settings.bCacheNotificationsNoListeners == true)
		{
			notificationQueue.Enqueue(notification);
		}
	}

	StartDispatchTicker();
}

void FNotificationBackboneNotificationFeed::StartDispatchTicker()
{
	if (GetDoesHaveListeners() && !notificationQueue.IsEmpty())
	{
		if (FMath::IsNearlyZero(settings.dispatchDelay))
		{
			// Special case. We do not use the ticker, instead we dispatch everything we got.
			while (!bBlockDispatch && !notificationQueue.IsEmpty())
			{
				DispatchNotificationFromQueue();
			}
		}
		else if (!bIsTickerActive)
		{
			// Ticker was not active, thus we now are beyond our delay.
			// Dispatch one directly
			DispatchNotificationFromQueue();

			// To keep our delay, we must start the ticker. Even if there is nothing enqueued anymore.
			tickerDelegateHandle = FTicker::GetCoreTicker().AddTicker(tickerDelegate, settings.dispatchDelay);
			bIsTickerActive = true;
		}
	}
}
