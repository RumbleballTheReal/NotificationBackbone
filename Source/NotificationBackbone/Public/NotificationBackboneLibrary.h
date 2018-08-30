// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NotificationBackboneManager.h"
#include "NotificationBackboneDeclarations.h"
#include "NotificationBackboneLibrary.generated.h"

/**
 *
 */
UCLASS()
class NOTIFICATIONBACKBONE_API UNotificationBackboneLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//UFUNCTION(BlueprintPure, meta = (DeprecatedFunction, DeprecationMessage = "Use Get Timer Remaining Time by Handle", DisplayName = "Get Timer Remaining Time by Event", ScriptName = "GetTimerRemainingTimeDelegate"), Category = "Utilities|Time")
	//static float NotificationBackbone_QuerryUser(UPARAM(DisplayName = "Event") FTimerDynamicDelegate Delegate);

	UFUNCTION(BlueprintCallable, Category = "NotificationBackbone")
		static void RegisterForNotification(TScriptInterface<INotificationBackboneListener> object, FName feed)
	{
		FNotificationBackboneManager::Get().RegisterForNotificationsUObject(object, feed);
	}

	UFUNCTION(BlueprintCallable, Category = "NotificationBackbone")
		static void UnregisterFromNotification(TScriptInterface<INotificationBackboneListener> object, FName feed)
	{
		FNotificationBackboneManager::Get().UnregisterFromNotificationsUObject(object, feed);
	}

	UFUNCTION(BlueprintCallable, Category = "NotificationBackbone")
		static void DispatchNotification(const FNotificationBackboneNotification& notification)
	{
		FNotificationBackboneManager::Get().DispatchNotification(notification);
	}

	UFUNCTION(BlueprintPure, Category = "NotificationBackbone")
		static bool DoesNotificationFeedExist(const FName& feed)
	{
		return FNotificationBackboneManager::Get().GetDoesNotificationFeedExist(feed);
	}

	// Returns the names of the notification feeds that currently exist
	UFUNCTION(BlueprintPure, Category = "NotificationBackbone")
		static void GetNotificationFeedNames(TArray<FName>& feeds)
	{
		FNotificationBackboneManager::Get().GetNotificationFeedNames(feeds);
	}

	// Returns false if there is no listener or feed does not exist
	UFUNCTION(BlueprintPure, Category = "NotificationBackbone")
		static bool DoesNotificationFeedHaveListeners(const FName& feed)
	{
		TSharedRef<FNotificationBackboneNotificationFeed>* pfeed = FNotificationBackboneManager::Get().GetNotificationFeed(feed);
		if (pfeed)
		{
			return (*pfeed)->GetDoesHaveListeners();
		}

		return false;
	}

	// Returns false if there is no notification or feed does not exist
	UFUNCTION(BlueprintPure, Category = "NotificationBackbone")
		static bool DoesNotificationFeedHaveNotifications(const FName& feed)
	{
		TSharedRef<FNotificationBackboneNotificationFeed>* pfeed = FNotificationBackboneManager::Get().GetNotificationFeed(feed);
		if (pfeed)
		{
			return (*pfeed)->GetDoesHaveNotifications();
		}

		return false;
	}

	// Returns 0 if there is no listener or feed does not exist
	UFUNCTION(BlueprintPure, Category = "NotificationBackbone")
		static int32 GetNotificationFeedNumListeners(const FName& feed)
	{
		TSharedRef<FNotificationBackboneNotificationFeed>* pfeed = FNotificationBackboneManager::Get().GetNotificationFeed(feed);
		if (pfeed)
		{
			return (*pfeed)->GetNumListeners();
		}

		return 0;
	}

	// Returns 0 if there is no notification or feed does not exist
	UFUNCTION(BlueprintPure, Category = "NotificationBackbone")
		static int32 GetNotificationFeedNumNotifications(const FName& feed)
	{
		TSharedRef<FNotificationBackboneNotificationFeed>* pfeed = FNotificationBackboneManager::Get().GetNotificationFeed(feed);
		if (pfeed)
		{
			return (*pfeed)->GetNumNotifications();
		}

		return 0;
	}

	/**
	 * Blocks a feed from dispatching notifications.
	 * This only holds until the feed is completely empty (no listeners, no notifications), as the feed will get destroyed.
	 * This can be useful for map changes or when you have to load, during a NPC conversation, while the player is in the inventory, ...
	 *
	 * Returns false if feed does not exist
	 */
	UFUNCTION(BlueprintCallable, Category = "NotificationBackbone")
		static bool BlockNotificationFeed(const FName& feed)
	{
		TSharedRef<FNotificationBackboneNotificationFeed>* pfeed = FNotificationBackboneManager::Get().GetNotificationFeed(feed);
		if (pfeed)
		{
			(*pfeed)->BlockDispatching();
			return true;
		}

		return false;
	}


	//  Continues a blocked feed.
	//  Returns false if feed does not exist
	UFUNCTION(BlueprintCallable, Category = "NotificationBackbone")
		static bool UnblockNotificationFeed(const FName& feed)
	{
		TSharedRef<FNotificationBackboneNotificationFeed>* pfeed = FNotificationBackboneManager::Get().GetNotificationFeed(feed);
		if (pfeed)
		{
			(*pfeed)->ContinueDispatching();
			return true;
		}

		return false;
	}

	// Listeners are empty when there is no listener or the feed does not exist. 
	UFUNCTION(BlueprintPure, Category = "NotificationBackbone")
		static void GetNotificationFeedListenerNames(const FName& feed, TArray<FString>& listeners)
	{
		listeners.Empty();
		TSharedRef<FNotificationBackboneNotificationFeed>* pfeed = FNotificationBackboneManager::Get().GetNotificationFeed(feed);
		if (pfeed)
		{
			(*pfeed)->GetListenerNames(listeners);
		}
	}

	// Returns whether the feed blocked and does not dispatch or not.
	// Returns false when the feed does not exist.
	UFUNCTION(BlueprintPure, Category = "NotificationBackbone")
		static bool IsNotificationFeedBlocked(const FName& feed)
	{
		TSharedRef<FNotificationBackboneNotificationFeed>* pfeed = FNotificationBackboneManager::Get().GetNotificationFeed(feed);
		if (pfeed)
		{
			return (*pfeed)->GetIsBlocked();
		}

		return false;
	}

	// Clears the pending notifications of a feed.
	// Returns false when the feed does not exist.
	UFUNCTION(BlueprintCallable, Category = "NotificationBackbone")
		static bool ClearNotificationFeed(const FName& feed)
	{
		return FNotificationBackboneManager::Get().ClearNotificationFeedNotifications(feed);
	}

	// Returns false when there are no settings for that feed.
	UFUNCTION(BlueprintCallable, Category = "NotificationBackbone")
		static bool GetNotificationFeedSettings(const FName& feed, FNotificationBackboneFeedSettings& settings)
	{
		const FNotificationBackboneFeedSettings* p_settings = UNotificationBackboneSettings::Get()->feedSettings.FindByPredicate(
			[feed](const FNotificationBackboneFeedSettings& in_settings) -> bool
		{
			return in_settings.feed == feed;
		});

		if (p_settings)
		{
			settings = *p_settings;
			return true;
		}
		return false;
	}

};
