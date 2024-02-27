#include "AssetModifier.h"
#if ENGINE_MINOR_VERSION > 23 || ENGINE_MAJOR_VERSION >4
#include "HAL/FileManagerGeneric.h"
#else
#include "FileManagerGeneric.h"
#endif
#include "Tags.h"
#include "AssetUtils.h"
#include "WorldControlSettings.h"
#if WITH_EDITOR
#include "Editor.h"
#endif
#include "PhysicsEngine/PhysicsConstraintComponent.h"

bool FAssetModifier::RemoveAsset(UWorld* World, FString Id)
{

	//Get Actor with ID
	TArray<AActor*> Actors = FTags::GetActorsWithKeyValuePair(World, TEXT("SemLog"), TEXT("Id"), Id);
	for (auto ActorToBeRemoved : Actors)
	{
#if WITH_EDITOR
		GEditor->BeginTransaction(FText::FromString(TEXT("Destroing: ")
			+ ActorToBeRemoved->GetActorLabel()));
#endif
		bool bSuccess = ActorToBeRemoved->Destroy();
#if WITH_EDITOR
		GEditor->EndTransaction();
#endif
		return bSuccess;
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s]: Actor with id:\"%s\" does not exist and can therefore not be removed."), *FString(__FUNCTION__), *Id);



	return false;
}

bool FAssetModifier::Relocate(AActor* Actor, FVector Location, FRotator Rotator)
{
#if WITH_EDITOR
	GEditor->BeginTransaction(FText::FromString(TEXT("Relocating: ")
		+ Actor->GetActorLabel()));
#endif
        bool bSuccess = false;
        bool bTimeout = false;
        FVector Offset = FVector(0, 0, 0);
        uint32 Count = 0;

        UAssetUtils* AssetUtil = NewObject<UAssetUtils>(Actor);
        const UWorldControlSettings* Settings = GetDefault<UWorldControlSettings>();

        TArray<UPhysicsConstraintComponent*> Constraints;
        UStaticMeshComponent* Oj1 = nullptr;
        UStaticMeshComponent* Oj2 = nullptr;
        Actor->GetComponents<UPhysicsConstraintComponent>(Constraints, false);

        for(auto& Constraint : Constraints)
          {
            if(!Constraint->IsBroken())
              {
                //TODO: Should it be checked if phisics is enabled or not?
                if(AActor* Actor1 = Constraint->ConstraintActor1)
                  {
                    Oj1 = Cast<UStaticMeshComponent>(Actor1->GetDefaultSubobjectByName(Constraint->ComponentName1.ComponentName));
                    if(Oj1)
                      {
                        Oj1->SetSimulatePhysics(false);

                        if(Cast<USceneComponent>(Oj1) != Actor->GetRootComponent())
                          {
                            Oj1->AttachToComponent(Actor->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, false), NAME_None);
                          }
                      }
                  }

                if(AActor* Actor2 = Constraint->ConstraintActor2)
                  {
                    Oj2 = Cast<UStaticMeshComponent>(Actor2->GetDefaultSubobjectByName(Constraint->ComponentName2.ComponentName));
                    if(Oj2)
                      {
                        Oj2->SetSimulatePhysics(false);
                      }
                    if(Cast<USceneComponent>(Oj2) != Actor->GetRootComponent())
                      {
                        Oj2->AttachToComponent(Actor->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, false), NAME_None);
                      }
                  }
              }

            FTimerDelegate ConstraintInit = FTimerDelegate::CreateUObject( AssetUtil,  &UAssetUtils::ReinitConstraint, Constraint, Oj1, Oj2);
            Actor->GetWorldTimerManager().SetTimerForNextTick(ConstraintInit);
          }



        while(!bSuccess and !bTimeout)
          {
            FHitResult SweepHitResult;

            Actor->SetActorLocationAndRotation(Location, Rotator, false, nullptr, ETeleportType::ResetPhysics);
            Actor->SetActorLocationAndRotation(Location + Offset, Rotator, true, &SweepHitResult,  ETeleportType::ResetPhysics);
            Count++;
            bSuccess = !SweepHitResult.bBlockingHit;
            FVector Normal = SweepHitResult.ImpactNormal;

            Offset = Offset + Normal;
            if(Count >= 10)
              {
                bTimeout = true;
                UE_LOG(LogTemp, Warning, TEXT("[%s]: Timeout"), *FString(__FUNCTION__));
              }
          }


        if(Settings->bUseResetOrientation)
          {
            FTimerHandle MyTimerHandle;
            // InTimerManager.SetTimer(MyTimerHandle, this, &UPrologQueryClient::CallService, 1.0f, false);
            FTimerDelegate ResetOrientationDelegate = FTimerDelegate::CreateUObject( AssetUtil,  &UAssetUtils::ResetOrientation, Cast<AStaticMeshActor>(Actor), Rotator);
            Actor->GetWorldTimerManager().SetTimerForNextTick(ResetOrientationDelegate);
          }

        FTimerDelegate ResetPhysics1 = FTimerDelegate::CreateUObject( AssetUtil,  &UAssetUtils::SetPhysicsEnabled, Oj1, true);
        FTimerDelegate ResetPhysics2 = FTimerDelegate::CreateUObject( AssetUtil,  &UAssetUtils::SetPhysicsEnabled, Oj2, true);
        Actor->GetWorldTimerManager().SetTimerForNextTick(ResetPhysics1);
        Actor->GetWorldTimerManager().SetTimerForNextTick(ResetPhysics2);

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s]: Could not set %s to locaiton: %s, with Rotation: %s"), *FString(__FUNCTION__),
			*Actor->GetName(), *Location.ToString(), *Rotator.ToString());
	}

#if WITH_EDITOR
	GEditor->EndTransaction();
#endif

	return bSuccess;
}

bool FAssetModifier::ChangePhysicsProperties(UStaticMeshComponent* MeshComponent,
	EComponentMobility::Type Mobility, bool bSimulatedPhysics,
	bool bGereateOverlapEvents, bool bGravity, float Mass)
{
	if (!MeshComponent) return false;

#if WITH_EDITOR
	GEditor->BeginTransaction(FText::FromString(TEXT("Changing Phyisics on: ")
		+ MeshComponent->GetOwner()->GetActorLabel()));
#endif

	MeshComponent->SetMobility(Mobility);
	MeshComponent->SetSimulatePhysics(bSimulatedPhysics);
	MeshComponent->SetGenerateOverlapEvents(bGereateOverlapEvents);
	MeshComponent->SetEnableGravity(bGravity);
	MeshComponent->SetMassOverrideInKg(NAME_None, Mass);

#if WITH_EDITOR
	MeshComponent->Modify();
	GEditor->EndTransaction();
#endif

	return true;
}

bool FAssetModifier::ChangeVisual(UStaticMeshComponent* MeshComponent, TArray<FString> MaterialNames, TArray<FString> MaterialPaths)
{
#if WITH_EDITOR
	GEditor->BeginTransaction(FText::FromString(TEXT("Changing Visual on: ")
		+ MeshComponent->GetOwner()->GetActorLabel()));
#endif
	bool bSuccess = false;

	if (MaterialPaths.Num() > 0)
	{
		for (int i = 0; i < MaterialPaths.Num(); i++)
		{
			//Try to load Material
			UMaterialInterface* Material = Cast<UMaterialInterface>(
				StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *MaterialPaths[i]));
			if (Material)
			{
				MeshComponent->SetMaterial(i, Material);
				bSuccess = true;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[%s]: Path %s does not point to a Material"), *FString(__FUNCTION__), *MaterialPaths[i]);
			}
		}

	}
	else
	{
		for (int i = 0; i < MaterialNames.Num(); i++)
		{
			UMaterialInterface* Material = nullptr;
			Material = LoadMaterial(*MaterialNames[i], TEXT(""));

			if (Material)
			{
				MeshComponent->SetMaterial(i, Material);
				bSuccess = true;
			}

		}
	}
#if WITH_EDITOR
	if (bSuccess)
		MeshComponent->Modify();
	GEditor->EndTransaction();
#endif
	return bSuccess;
}

bool FAssetModifier::AttachToParent(AActor* Parent, AActor* Child)
{


	if (Parent && Child)
	{
#if WITH_EDITOR
		GEditor->BeginTransaction(FText::FromString(TEXT("Attaching: ")
			+ Child->GetActorLabel() + TEXT(" to: ")
			+ Parent->GetActorLabel()));
#endif
		Child->AttachToActor(Parent, FAttachmentTransformRules::KeepWorldTransform);
#if WITH_EDITOR
		Child->Modify();
		Parent->Modify();
		GEditor->EndTransaction();
#endif
		return true;
	}
	return false;
}


TArray<FString> FAssetModifier::FindAsset(FString Name, FString StartDir)
{
	UStaticMesh* Mesh = nullptr;
	//Look for file Recursively
        FString FoundPath = TEXT("");
	FString Filename = Name.StartsWith(TEXT("SM_")) ? TEXT("") : TEXT("SM_");
	Filename += Name;
	Filename += Name.EndsWith(TEXT(".uasset")) ? TEXT("") : TEXT(".uasset");
        UE_LOG(LogTemp, Warning, TEXT("[%s]: SpawnModel Name %s"), *FString(__FUNCTION__),*Name);

	TArray<FString> FileLocations;
    FFileManagerGeneric Fm;
	Fm.FindFilesRecursive(FileLocations, *FPaths::ProjectContentDir().Append(StartDir), *Filename, true, false, true);

	if (FileLocations.Num() == 0)
	{
		//Try again with whole ContentDir
		Fm.FindFilesRecursive(FileLocations, *FPaths::ProjectContentDir(), *Filename, true, false, true);
	}

	if (FileLocations.Num() == 0)
          {
            UE_LOG(LogTemp, Warning, TEXT("[%s] No Files found containing"), *FString(__FUNCTION__),*Name);
          }

	return FileLocations;
}

UStaticMesh* FAssetModifier::LoadMesh(FString Name, FString StartDir)
{

  const UWorldControlSettings* Settings = GetDefault<UWorldControlSettings>();

  UStaticMesh* Mesh = nullptr;
  //Look for file Recursively

  FString Filename = Name.StartsWith(TEXT("SM_")) ? TEXT("") : TEXT("SM_");
  Filename += Name;
  Filename += Name.EndsWith(TEXT(".uasset")) ? TEXT("") : TEXT(".uasset");
  UE_LOG(LogTemp, Warning, TEXT("[%s]: SpawnModel Name %s"), *FString(__FUNCTION__),*Name);

  TArray<FString> FileLocations;
  FFileManagerGeneric Fm;

  if(Settings->bDebugMode)
    {
      UE_LOG(LogTemp, Warning, TEXT("[%s]: Start looking for file"), *FString(__FUNCTION__));
    }

  Fm.FindFilesRecursive(FileLocations, *FPaths::ProjectContentDir().Append(StartDir), *Filename, true, false, true);

  if (FileLocations.Num() == 0)
    {
      //Try again with whole ContentDir
      Fm.FindFilesRecursive(FileLocations, *FPaths::ProjectContentDir(), *Filename, true, false, true);
    }

  if(Settings->bDebugMode)
    {
      UE_LOG(LogTemp, Warning, TEXT("[%s]: Finished looking for file"), *FString(__FUNCTION__));
    }

  for (auto Loc : FileLocations)
    {
      //Try all found files until one works.
      if (Mesh == nullptr)
        {

          Loc.RemoveFromStart(FPaths::ProjectContentDir());
          int Last;
          Loc.FindLastChar('.', Last);

          if(Settings->bDebugMode)
            {
              UE_LOG(LogTemp, Warning, TEXT("[%s]: Start path formating for Mesh"), *FString(__FUNCTION__));
            }
          Loc.RemoveAt(Last, Loc.Len() - Last);

          FString FoundPath = "StaticMesh'/Game/" + Loc + ".SM_" + Name + "'";

          if(Settings->bDebugMode)
            {
              UE_LOG(LogTemp, Warning, TEXT("[%s]: Start load Mesh"), *FString(__FUNCTION__));
            }
          Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *FoundPath));
        }
    }

  if(Settings->bDebugMode)
    {
      UE_LOG(LogTemp, Warning, TEXT("[%s]: Finished looking for Mesh"), *FString(__FUNCTION__));
    }
  return Mesh;
}

UMaterialInterface* FAssetModifier::LoadMaterial(FString Name, FString StartDir)
{
	UMaterialInterface* Material = nullptr;
	FString Filename;
	if (Name.StartsWith(TEXT("M_")))
		Filename = Name + TEXT(".uasset");
	else
		Filename = TEXT("M_") + Name + TEXT(".uasset");

	TArray<FString> FileLocations;
	FFileManagerGeneric Fm;
	Fm.FindFilesRecursive(FileLocations, *FPaths::ProjectContentDir().Append(StartDir), *Filename, true, false, true);

	if (FileLocations.Num() == 0)
	{
		//Try again with whole ContentDir
		Fm.FindFilesRecursive(FileLocations, *FPaths::ProjectContentDir(), *Filename, true, false, true);
	}

	for (auto Loc : FileLocations)
	{
		//Try all found files until one works.
		if (Material == nullptr)
		{
			Loc.RemoveFromStart(FPaths::ProjectContentDir());
			int Last;
			Loc.FindLastChar('.', Last);
			Loc.RemoveAt(Last, Loc.Len() - Last);

			FString FoundPath = "StaticMesh'/Game" + Loc + ".M_" + Name + "'";
			Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *FoundPath));
		}
	}

	return Material;

}
