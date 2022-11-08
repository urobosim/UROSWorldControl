#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "ROSBridgeSrvServer.h"
#include "ROSBridgeHandler.h"
#include "world_control_msgs/srv/ResetLevel.h"


class FROSResetLevelServer : public FROSBridgeSrvServer
{
protected:

	FROSResetLevelServer()
	{
          UE_LOG(LogTemp, Error, TEXT("ResetLevelserver created"));
	};

  UWorld* World;
  UGameInstance* Owner;
  FThreadSafeBool bAllSuccessfull;
private:

public:

  //Deprected
  FROSResetLevelServer(FString Namespace, FString Name, UObject* InOwner) :
		FROSBridgeSrvServer(Namespace + TEXT("/") + Name, TEXT("world_control_msgs/ResetLevel"))
	{
          Owner = Cast<UGameInstance>(InOwner);
          World = Owner->GetWorld();
	}

  FROSResetLevelServer(FString Namespace, FString Name, UWorld* InWorld) :
		FROSBridgeSrvServer(Namespace + TEXT("/") + Name, TEXT("world_control_msgs/ResetLevel"))
	{
          World = InWorld;
	}

	TSharedPtr<FROSBridgeSrv::SrvRequest> FromJson(TSharedPtr<FJsonObject> JsonObject) const override;

	TSharedPtr<FROSBridgeSrv::SrvResponse> Callback(TSharedPtr<FROSBridgeSrv::SrvRequest> Request) override;
};
