#include "AssetUtils.h"

void UAssetUtils::ResetOrientation(AStaticMeshActor* InActor, FRotator InRotator)
{
  UE_LOG(LogTemp, Log, TEXT("[%s]: Reset Orientation."), *FString(__FUNCTION__));
  if(InActor)
    {
      InActor->SetActorRotation(InRotator,  ETeleportType::ResetPhysics);
    }
}

void UAssetUtils::SetPhysicsEnabled(UStaticMeshComponent* InComponent, bool bEnable)
{
  if(InComponent)
    {
      InComponent->SetSimulatePhysics(bEnable);
    }
}

void UAssetUtils::ReinitConstraint(UPhysicsConstraintComponent* InComponent, UStaticMeshComponent* InComp1, UStaticMeshComponent* InComp2)
{
  if(InComponent)
    {
      InComponent->SetConstrainedComponents(InComp1,
                                                  NAME_None,
                                                  InComp2,
                                                  NAME_None);
    }
}
