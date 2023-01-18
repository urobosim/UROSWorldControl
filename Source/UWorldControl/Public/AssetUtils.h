#pragma once
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMesh.h"
#include "AssetUtils.generated.h"



UCLASS()
class UWORLDCONTROL_API UAssetUtils: public UObject
{
  GENERATED_BODY()

public:

    UFUNCTION()
    void ResetOrientation(AStaticMeshActor* InActor, FRotator InRotator);
};
