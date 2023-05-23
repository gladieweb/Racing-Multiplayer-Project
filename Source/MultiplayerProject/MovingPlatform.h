// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERPROJECT_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	AMovingPlatform();
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float Speed= 20;

	UPROPERTY(EditAnywhere, meta=( MakeEditWidget = true))
	FVector TargetLocation;

	void AddActiveTrigger();
	void RemoveActiveTrigger();
	
private:
	FVector GlobalStartLocation;
	FVector GlobalTargetLocation;

	UPROPERTY(EditAnywhere)
	int ActiveTriggers = 1;
};
