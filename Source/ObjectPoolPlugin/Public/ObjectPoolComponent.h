// ObjectPoolComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PooledObject.h"
#include "ObjectPoolComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OBJECTPOOLPLUGIN_API UObjectPoolComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UObjectPoolComponent();

    UFUNCTION(BlueprintCallable, Category = "Object Pool")
    APooledObject* SpawnPooledObject(FTransform SpawnTransform);

    // Server RPC for clients to request spawns
    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Object Pool")
    void Server_SpawnPooledObject(FTransform SpawnTransform);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Pool", meta = (AllowAbstract = "false"))
    TSubclassOf<APooledObject> PooledObjectSubclass;

    UPROPERTY(EditAnywhere, Category = "Object Pool")
    int PoolSize = 20;

    UPROPERTY(EditAnywhere, Category = "Object Pool")
    float PooledObjectLifeSpan = 0.0f;

    UFUNCTION()
    void PooledObjectDespawn(APooledObject* PoolActor);

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    TArray<APooledObject*> ObjectPool;
    TArray<int> SpawnedPoolIndexes;
};