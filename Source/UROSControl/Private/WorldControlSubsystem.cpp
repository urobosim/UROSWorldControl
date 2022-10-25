// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldControlSubsystem.h"
#include "ResetLevelServer.h"

UWorldControlSubsystem::UWorldControlSubsystem()
{
  Manager = CreateDefaultSubobject<URWCManager>(TEXT("RWCManager"));
}

void UWorldControlSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

  const UWorldControlSettings* Settings = GetDefault<UWorldControlSettings>();
  if (Settings->bEnableWorldControl)
    {
      Super::Initialize(Collection);
      if(ROSHandler.IsValid())
        {
          UWorld* World = GetGameInstance()->GetWorld();
          if(World)
            {
              ROSHandler->AddServiceServer(MakeShareable<FROSResetLevelServer>(new FROSResetLevelServer(Settings->Namespace, TEXT("reset_level"),  World)));
              if(Manager)
                {
                  Manager->Register(Settings->Namespace, World);
                  Manager->ConnectToHandler(ROSHandler);
                }

            }
          else
            {
              UE_LOG(LogTemp, Error, TEXT("World not ready"));
            }
        }
      else
        {
          UE_LOG(LogTemp, Error, TEXT("GameInstance: Handler not valid"));
        }
    }
}
