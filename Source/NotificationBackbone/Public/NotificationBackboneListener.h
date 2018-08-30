// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NotificationBackboneBPTypes.h"
#include "NotificationBackboneListener.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UNotificationBackboneListener : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class NOTIFICATIONBACKBONE_API INotificationBackboneListener
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Return value is only to make this a function, would be a event otherwise.
	UFUNCTION(BlueprintNativeEvent, Category = "NotificationBackbone|Listener")
		bool OnNotification(const FNotificationBackboneNotification& notification);
};

class NOTIFICATIONBACKBONE_API INotificationBackboneListenerRaw
{
public:
	virtual void OnNotification(const FNotificationBackboneNotification& notification) = 0;

	// Return an appropriate name so we know who is listening. Class name is always good.
	virtual FName GetNotificationBackboneListenerName() = 0;
};
