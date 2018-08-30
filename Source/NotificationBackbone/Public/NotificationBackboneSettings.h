// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NotificationBackboneBPTypes.h"
#include "NotificationBackboneSettings.generated.h"

/**
 * 
 */
UCLASS(config = "Game", defaultconfig)
class NOTIFICATIONBACKBONE_API UNotificationBackboneSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UNotificationBackboneSettings()
	{
		CategoryName = TEXT("Plugins");
	}

	static const UNotificationBackboneSettings *const Get()
	{
		return GetDefault<UNotificationBackboneSettings>();
	}

	// Per notification feed settings. 
	UPROPERTY(config, EditAnywhere, Category = "Notifications")
		TArray<FNotificationBackboneFeedSettings> feedSettings;

};
