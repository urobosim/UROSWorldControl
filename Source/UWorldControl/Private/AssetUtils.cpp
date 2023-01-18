#include "AssetUtils.h"

void UAssetUtils::ResetOrientation(AStaticMeshActor* InActor, FRotator InRotator)
{
  UE_LOG(LogTemp, Log, TEXT("[%s]: Reset Orientation."), *FString(__FUNCTION__));
  if(InActor)
    {
      InActor->SetActorRotation(InRotator,  ETeleportType::ResetPhysics);
    }
}
