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
 *	This is the core of the NotificationBackbone. Here messages come in, get managed and rerouted to subscribers.
 *
 *	Notifications:
 *	Notifications are handled per feed. A feed is defined by a name the user chooses.
 *	Per default:
 *		Notifications for a particular feed get dispatched immediately as soon as they come in and we have subscribers for that feed.
 *		Notifications that could not get dispatched because there is no subscriber, will get dropped.
 *	You can change that in the settings via the editor or config files.
 *  Each notification feed can have multiple subscribers.
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
	// Create a new notification feed if it does not exist yet.
	virtual void CreateNotificationFeedWhenNotExists(const FName& feed);
	virtual void RemoveNotificationFeedWhenEmpty(const FName& feed);

	virtual void ClearNotificationFeeds();
	virtual void ClearListeners()
	{
		ClearNotificationFeeds();
	}

	virtual void OnEndPlayInEditor(bool bIsSimulating)
	{
		ClearListeners();
	}

	virtual ~FNotificationBackboneManager()
	{
		ClearListeners();
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

#pragma region Notification
	TMap<FName, TSharedRef<FNotificationBackboneNotificationFeed>> notificationFeeds;
#pragma endregion Notification
};
