// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MessageLog.h"
#include "NotificationBackboneDeclarations.generated.h"

//DECLARE_MULTICAST_DELEGATE(FNotificationBackboneNotificationDelegate)
//DECLARE_DELEGATE(FNotificationBackboneQueryDelegate)

// Delegates for dispatching new messages
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotificationBackboneNotificationDelegate, FNotificationBackboneNotification, notification);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotificationBackboneQueryDelegate, FNotificationBackboneQuery, query);

// Delegate for accepting query callbacks
DECLARE_DYNAMIC_DELEGATE_OneParam(FNotificationBackboneDynamicDelegate, FNotificationBackboneQueryResult, queryResult);

// NotificationBackbone log
DECLARE_LOG_CATEGORY_CLASS(LogNotificationBackbone, Log, All);
#define MF_LOG(Verbosity, printToMessageLog, Format, ...)	UE_LOG(LogNotificationBackbone, Verbosity, TEXT("%s(%s): %s"), *FString(__FUNCTION__), *FString::FromInt(__LINE__), *FString::Printf(TEXT(Format), ##__VA_ARGS__ )); \
															if (printToMessageLog) \
															{ \
																if(FString(TEXT(#Verbosity)) == FString(TEXT("Log"))) FMessageLog("PIE").Info(FText::FromString(FString::Printf(TEXT(Format), ##__VA_ARGS__))); \
																else if(FString(TEXT(#Verbosity)) == FString(TEXT("Warning"))) FMessageLog("PIE").Warning(FText::FromString(FString::Printf(TEXT(Format), ##__VA_ARGS__))); \
																else if(FString(TEXT(#Verbosity)) == FString(TEXT("Error"))) FMessageLog("PIE").Error(FText::FromString(FString::Printf(TEXT(Format), ##__VA_ARGS__))); \
															} 

USTRUCT()
struct FNotificationBackboneDummyToGenerateGeneratedFile
{
	GENERATED_BODY()
};