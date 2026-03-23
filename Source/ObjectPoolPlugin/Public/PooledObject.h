// PooledObject.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"  // For replication
#include "PooledObject.generated.h"

UCLASS()
class OBJECTPOOLPLUGIN_API APooledObject : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    APooledObject();

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPooledObjectDespawn, APooledObject*, PoolActor);
    FOnPooledObjectDespawn OnPooledObjectDespawn;

    UFUNCTION(BlueprintCallable, Category = "Pooled Object")
    void Deactivate();

    void SetActive(bool IsActive);
    void SetLifeSpan(float LifeTime);
    void SetPoolIndex(int Index);

    bool IsActive();
    int GetPoolIndex();

    // Virtual function called when the object is activated (spawned from pool)
    // Override in child classes to handle custom activation (e.g., activate Niagara, unhide mesh, play sound)
    // Passes the spawn transform and instigator (owner of the pool component)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pooled Object")
    void PoolObjectActivate(FTransform SpawnTransform, AActor* PoolInstigator);
    virtual void PoolObjectActivate_Implementation(FTransform SpawnTransform, AActor* PoolInstigator);

    // Virtual function called when the object is deactivated (returned to pool)
    // Override in child classes to handle custom deactivation (e.g., deactivate Niagara, hide mesh, stop sound)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pooled Object")
    void PoolObjectDeactivate();
    virtual void PoolObjectDeactivate_Implementation();

protected:
    UPROPERTY(ReplicatedUsing = OnRep_Active)
    bool Active;

    float LifeSpan = 0.0f;
    int PoolIndex;

    FTimerHandle LifeSpanTimer;

    UFUNCTION()
    void OnRep_Active();  // Handles client-side state changes when Active replicates
};