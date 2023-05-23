// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERPROJECT_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	IMenuInterface* MenuInterface;
	
public:
	void SetMenuInterface(IMenuInterface* MenuInterfaceInstance);
	void Setup();
	void Teardown();
};
