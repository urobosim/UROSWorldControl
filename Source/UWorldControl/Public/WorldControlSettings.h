// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "WorldControlSettings.generated.h"

/**
 *
 */
UCLASS(Config=Ros, defaultconfig, meta = (DisplayName="WorldControl"))
class UWORLDCONTROL_API UWorldControlSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
      bool bEnableWorldControl = true;

  UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
    FString Namespace = FString("UnrealSim");

  UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
    bool bUseResetOrientation = false;

  UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
    float ResetOrientationDelay = 1.0;
};
