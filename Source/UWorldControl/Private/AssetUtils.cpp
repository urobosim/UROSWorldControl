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
      UE_LOG(LogTemp, Warning, TEXT("[%s]: 5 %s"), *FString(__FUNCTION__), *InComponent->GetName());
      InComponent->SetSimulatePhysics(bEnable);
      if(bEnable)
        {
          UE_LOG(LogTemp, Log, TEXT("[%s]: SetPhysicsEnabled for %s true"), *FString(__FUNCTION__), *InComponent->GetName());
        }
      else
        {
          UE_LOG(LogTemp, Log, TEXT("[%s]: SetPhysicsEnabled for %s false"), *FString(__FUNCTION__), *InComponent->GetName());
        }
    }
}

void UAssetUtils::ReinitConstraint(UPhysicsConstraintComponent* InComponent, UStaticMeshComponent* InComp1, UStaticMeshComponent* InComp2)
{
  if(InComponent)
    {

      // InComponent->SetConstrainedComponents(InComp1,
      //                                             NAME_None,
      //                                             InComp2,
      //                                             NAME_None);
      InComponent->InitComponentConstraint();
    }
}

void UAssetUtils::ReinitConstraintAndSetPhysics(UPhysicsConstraintComponent* InComponent, UStaticMeshComponent* InComp1, UStaticMeshComponent* InComp2, bool bInEnable)
{
  if(InComp1)
    {
      UE_LOG(LogTemp, Warning, TEXT("[%s]: Comp1 7 %s"), *FString(__FUNCTION__), *InComp1->GetName());
      SetPhysicsEnabled(InComp1, bInEnable);
    }
  if(InComp2)
    {
      UE_LOG(LogTemp, Warning, TEXT("[%s]: 3 %s"), *FString(__FUNCTION__), *InComp2->GetName());
      SetPhysicsEnabled(InComp2, bInEnable);
    }
  else
    {
      UE_LOG(LogTemp, Warning, TEXT("[%s]: 4"), *FString(__FUNCTION__));
    }
  if(InComponent)
    {
      ReinitConstraint(InComponent, InComp1, InComp2);
    }
}
