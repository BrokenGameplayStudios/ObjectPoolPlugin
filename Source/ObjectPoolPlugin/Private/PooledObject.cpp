// PooledObject.cpp

#include "PooledObject.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APooledObject::APooledObject()
{
    Active = false;
    bReplicates = true;  // Enable replication for multiplayer
}

void APooledObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(APooledObject, Active);
}

void APooledObject::SetActive(bool IsActive)
{
    if (Active == IsActive) return;  // Avoid unnecessary work

    Active = IsActive;
    // Note: Changes to Active will replicate, triggering OnRep_Active on clients

    if (Active && LifeSpan > 0.0f && HasAuthority())  // Timer only on server
    {
        GetWorldTimerManager().SetTimer(LifeSpanTimer, this, &APooledObject::Deactivate, LifeSpan, false);
    }
}

void APooledObject::SetLifeSpan(float LifeTime)
{
    LifeSpan = LifeTime;
}

void APooledObject::SetPoolIndex(int Index)
{
    PoolIndex = Index;
}

void APooledObject::Deactivate()
{
    SetActive(false);
    // Optional: Reset transform or other state for reuse (server only)
    if (HasAuthority())
    {
        SetActorLocation(FVector::ZeroVector);
        SetActorRotation(FRotator::ZeroRotator);
    }

    GetWorldTimerManager().ClearAllTimersForObject(this);
    PoolObjectDeactivate();  // Custom deactivate (replicated if needed)
    OnPooledObjectDespawn.Broadcast(this);
}

bool APooledObject::IsActive()
{
    return Active;
}

int APooledObject::GetPoolIndex()
{
    return PoolIndex;
}

void APooledObject::OnRep_Active()
{
    // Sync client-side visuals/behavior based on Active state
    SetActorHiddenInGame(!Active);
    SetActorEnableCollision(Active);
    SetActorTickEnabled(Active);

    // If needed, call PoolObjectActivate/Deactivate here for client-side effects
    // Or use Multicast RPCs for time-sensitive effects
}

void APooledObject::PoolObjectActivate_Implementation(FTransform SpawnTransform, AActor* PoolInstigator)
{
    // Base implementation: Can be empty or add common logic
    // Children override for projectiles: e.g., Set projectile velocity based on SpawnTransform.GetRotation().GetForwardVector()
    // Use PoolInstigator for damage attribution (e.g., SetInstigator(PoolInstigator))
}

void APooledObject::PoolObjectDeactivate_Implementation()
{
    // Base implementation: Can be empty or add common logic
    // Children override: e.g., Stop projectile movement, reset Niagara
}