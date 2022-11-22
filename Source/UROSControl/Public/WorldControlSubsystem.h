// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ROSSubsystem.h"
#include "RWCManager.h"
#include "WorldControlSubsystem.generated.h"

/**
 *
 */
UCLASS()
class UROSCONTROL_API UWorldControlSubsystem : public UROSGameInstanceSubsystem
{
	GENERATED_BODY()

public:

    UWorldControlSubsystem();
    // Begin USubsystem
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    // End USubsystem


  UPROPERTY(EditAnywhere, Instanced)
    URWCManager* Manager;
};
