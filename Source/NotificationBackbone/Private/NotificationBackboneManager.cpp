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
