// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NotificationBackboneBPTypes.generated.h"

// Options that are used to create a user query
UENUM(BlueprintType)
enum class ENotificationBackboneQueryOptions : uint8
{
	QO_OK				UMETA(DisplayName = "Ok"),
	QO_Yes				UMETA(DisplayName = "Yes"),
	QO_No				UMETA(DisplayName = "No"),
	QO_Cancel			UMETA(DisplayName = "Cancel"),
	QO_Save				UMETA(DisplayName = "Save"),
	QO_Load				UMETA(DisplayName = "Load"),
	QO_Retry			UMETA(DisplayName = "Retry"),
	QO_Open				UMETA(DisplayName = "Open"),
	QO_Close			UMETA(DisplayName = "Close"),
	QO_New				UMETA(DisplayName = "New"),
	QO_Delete			UMETA(DisplayName = "Delete"),
	QO_Accept			UMETA(DisplayName = "Accept"),
	QO_Reject			UMETA(DisplayName = "Reject"),
	QO_Quit				UMETA(DisplayName = "Quit"),
	QO_Continue			UMETA(DisplayName = "Continue"),
	QO_Reserved1		UMETA(DisplayName = "Reserved1"),
	QO_Reserved2		UMETA(DisplayName = "Reserved2"),
	QO_Reserved3		UMETA(DisplayName = "Reserved3"),
	QO_Reserved4		UMETA(DisplayName = "Reserved4"),
	QO_Reserved5		UMETA(DisplayName = "Reserved5"),
	QO_Custom1			UMETA(DisplayName = "Custom1"),
	QO_Custom2			UMETA(DisplayName = "Custom2"),
	QO_Custom3			UMETA(DisplayName = "Custom3"),
	QO_Custom4			UMETA(DisplayName = "Custom4"),
	QO_Custom5			UMETA(DisplayName = "Custom5"),
};

// We wrap the ID so people from BP can not put in another Guid or create a completely new one.
USTRUCT(BlueprintType)
struct FNotificationBackboneQueryId
{
	GENERATED_BODY();

	bool operator==(const FNotificationBackboneQueryId& other) const
	{
		return idInternal == other.idInternal;
	}

	void Invalidate()
	{
		idInternal.Invalidate();
	}

	bool IsValid() const
	{
		return idInternal.IsValid();
	}

	FGuid idInternal;
};

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

USTRUCT(BlueprintType, meta = (HasNativeMake = "NotificationBackbone.NotificationBackboneLibrary.MakeNotificationBackboneQueryData"))
struct FNotificationBackboneQueryData : public FNotificationBackboneMessageData
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		TArray<ENotificationBackboneQueryOptions> options;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (MakeStructureDefaultValue = ""))
		TArray<FString> customOptions;
};

USTRUCT(BlueprintType)
struct FNotificationBackboneQuery
{
	GENERATED_BODY();
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FNotificationBackboneQueryData data;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FNotificationBackboneQueryId id;
};


USTRUCT(BlueprintType)
struct FNotificationBackboneQueryResult
{
	GENERATED_BODY();
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FNotificationBackboneMessageData data;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		ENotificationBackboneQueryOptions optionChoosen;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString customOptionChoosen;
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