// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NotificationBackboneSettings.h"
#include "NotificationBackboneDeclarations.h"
#include "QueueCustom.h"
#include "CoreMinimal.h"

/**
 * Class that represents a message feed that will send out messages to subscribers.
 */
class NOTIFICATIONBACKBONE_API FNotificationBackboneNotificationFeed
{
	friend class FNotificationBackboneManager;
public:
	FNotificationBackboneNotificationFeed(const FName& in_feedName);
	~FNotificationBackboneNotificationFeed();

	void EnqueueNotification(const FNotificationBackboneNotification& notification);

	bool GetDoesHaveListeners() const
	{
		return GetNumListeners() != 0;
	}

	bool GetDoesHaveNotifications() const
	{
		return !notificationQueue.IsEmpty();
	}

	uint32 GetNumListeners() const
	{
		return listenersObject.Num() + listenersRaw.Num();
	}

	uint32 GetNumNotifications() const
	{
		return notificationQueue.Num();
	}

	// This only holds until the feed is completely empty (no listeners, no notifications), as the feed will get destroyed.
	// This can be useful for map changes or when you have to load, during a NPC conversation, while the player is in the inventory, ...
	void BlockDispatching()
	{
		bBlockDispatch = true;
	}

	void ContinueDispatching()
	{
		bBlockDispatch = false;
		StartDispatchTicker();
	}

	// Returns whether the feed blocked and does not dispatch or not.
	bool GetIsBlocked() const
	{
		return bBlockDispatch;
	}

	void GetListenerNames(TArray<FString>& outNames) const
	{
		outNames.Reset(GetNumListeners());
		// Object
		for (const TScriptInterface<INotificationBackboneListener>& listener : listenersObject)
		{
			if (listener.GetObject() && listener.GetObject()->IsValidLowLevelFast())
			{
				outNames.Add(listener.GetObject()->GetName());
			}
		}

		//Raw
		for (const TWeakPtr<INotificationBackboneListenerRaw>& listener : listenersRaw)
		{
			TSharedPtr<INotificationBackboneListenerRaw> pinnedRaw = listener.Pin();
			if (pinnedRaw.IsValid())
			{
				outNames.Add(pinnedRaw->GetNotificationBackboneListenerName().ToString());
			}
		}
	}

private:
	// For raw objects
	void AddListener(TSharedRef<INotificationBackboneListenerRaw> listener);
	void RemoveListener(TSharedRef<INotificationBackboneListenerRaw> listener);

	//For UObjects
	void AddListenerObject(TScriptInterface<INotificationBackboneListener> listener);
	void RemoveListenerObject(TScriptInterface<INotificationBackboneListener> listener);

	// Clear the pending notifications of a feed.
	void ClearNotifications()
	{
		notificationQueue.Empty();
	}

	/**
	 * This function gets fired via the ticker.
	 * If it returns true, the delay is reset and will fire again.
	 * If it returns false, the ticker does not consider us anymore.
	 */
	bool DispatchNotificationFromQueue(float deltaSeconds = 0.f);

	void StartDispatchTicker();

	TSet<TScriptInterface<INotificationBackboneListener>> listenersObject; // UObject listeners
	TSet<TWeakPtr<INotificationBackboneListenerRaw>> listenersRaw;	// Raw C++ listeners

	TQueueCustom<FNotificationBackboneNotification, EQueueMode::Spsc> notificationQueue;

	// Name of the feed this object is for.
	FName feedName;

	FNotificationBackboneFeedSettings settings;

	// Delegate that connects the ticker to our function
	FTickerDelegate tickerDelegate;
	// Handle to the delegate in the ticker
	FDelegateHandle tickerDelegateHandle;
	// The ticker is a bit of a mess. It has no way of querying it for an active delegate,
	// nor does it invalidate the handle in case the in case the ticker finished (see return value
	// of ticked function). Thus we try to keep track of the ticker on our own.
	bool bIsTickerActive = false;

	bool bBlockDispatch = false;
};
