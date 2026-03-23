// ObjectPoolComponent.cpp

#include "ObjectPoolComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UObjectPoolComponent::UObjectPoolComponent()
{
    // If the component needs replication (e.g., if attached to non-replicated actor), set bReplicates = true;
}

void UObjectPoolComponent::BeginPlay()
{
    Super::BeginPlay();

    // Only initialize pool on server
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority()) return;

    if (PooledObjectSubclass != nullptr)
    {
        UWorld* const World = GetWorld();
        if (World != nullptr)
        {
            for (int i = 0; i < PoolSize; i++)
            {
                FActorSpawnParameters SpawnParams;
                SpawnParams.Owner = GetOwner();  // Set owner for instigator use

                APooledObject* PoolableActor = World->SpawnActor<APooledObject>(PooledObjectSubclass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
                if (PoolableActor != nullptr)
                {
                    PoolableActor->SetActive(false);
                    PoolableActor->SetPoolIndex(i);
                    PoolableActor->OnPooledObjectDespawn.AddDynamic(this, &UObjectPoolComponent::PooledObjectDespawn);
                    ObjectPool.Add(PoolableActor);
                }
            }
        }
    }
}

APooledObject* UObjectPoolComponent::SpawnPooledObject(FTransform SpawnTransform)
{
    // This should only run on server—call via Server_SpawnPooledObject if from client
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority()) return nullptr;

    UWorld* World = GetWorld();
    if (!World || !PooledObjectSubclass) return nullptr;

    for (APooledObject* PoolableActor : ObjectPool)
    {
        if (PoolableActor != nullptr && !PoolableActor->IsActive())
        {
            PoolableActor->SetActorTransform(SpawnTransform);
            PoolableActor->SetLifeSpan(PooledObjectLifeSpan);
            PoolableActor->SetActive(true);
            PoolableActor->PoolObjectActivate(SpawnTransform, GetOwner());  // Pass transform and instigator
            SpawnedPoolIndexes.Add(PoolableActor->GetPoolIndex());

            return PoolableActor;
        }
    }

    if (SpawnedPoolIndexes.Num() > 0)
    {
        int PooledObjectIndex = SpawnedPoolIndexes[0];
        SpawnedPoolIndexes.RemoveAt(0);
        APooledObject* PoolableActor = ObjectPool[PooledObjectIndex];

        if (PoolableActor != nullptr)
        {
            PoolableActor->Deactivate();  // Deactivate (syncs via rep)
            PoolableActor->SetActorTransform(SpawnTransform);
            PoolableActor->SetLifeSpan(PooledObjectLifeSpan);
            PoolableActor->SetActive(true);
            PoolableActor->PoolObjectActivate(SpawnTransform, GetOwner());
            SpawnedPoolIndexes.Add(PoolableActor->GetPoolIndex());

            return PoolableActor;
        }
    }

    return nullptr;
}

void UObjectPoolComponent::Server_SpawnPooledObject_Implementation(FTransform SpawnTransform)
{
    SpawnPooledObject(SpawnTransform);
}

bool UObjectPoolComponent::Server_SpawnPooledObject_Validate(FTransform SpawnTransform)
{
    return true;  // Add validation logic if needed (e.g., anti-cheat checks)
}

void UObjectPoolComponent::PooledObjectDespawn(APooledObject* PoolActor)
{
    if (PoolActor)
    {
        SpawnedPoolIndexes.Remove(PoolActor->GetPoolIndex());
    }
}