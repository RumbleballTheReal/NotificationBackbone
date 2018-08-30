// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NotificationBackboneBPTypes.generated.h"

USTRUCT(BlueprintType)
struct FNotificationBackboneMessageData
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		FText title;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		FText message;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		UTexture2D* icon;
};

USTRUCT(BlueprintType)
struct FNotificationBackboneNotification : public FNotificationBackboneMessageData
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		FName feed;

	// You can use this to dynamically set the lifetime of the notification widget.
	// DO NOT SET IT, WILL GET OVERWRITTEN BY THE FEED
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float feedDispatchDelay;
};

USTRUCT(BlueprintType)
struct FNotificationBackboneFeedSettings
{
	GENERATED_BODY();

	FNotificationBackboneFeedSettings()
	{
		bCacheNotificationsNoListeners = false;
		bClearNotificationsNoListeners = false;
	}

	// Name of the feed this settings belong to
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName feed;

	// Delay between single notification dispatches in seconds
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float dispatchDelay = 0.f;

	// Check if we shall cache notifications in case there is no one listener
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		uint8 bCacheNotificationsNoListeners : 1;

	// Check if we shall clear the notification queue for the feed when the last listener quits
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		uint8 bClearNotificationsNoListeners : 1;

};