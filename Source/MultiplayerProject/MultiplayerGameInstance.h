// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MenuSystem/MenuInterface.h"
#include "MultiplayerGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERPROJECT_API UMultiplayerGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()
public:
	UMultiplayerGameInstance(const FObjectInitializer& ObjectInitializer);
	virtual void Init() override;

	UFUNCTION(Exec)
	void Host(FString ServerName) override;

	UFUNCTION(Exec)
	void Join(uint32 Index) override;

	UFUNCTION(Exec)
	virtual void LoadMainManu() override;

	UFUNCTION(BlueprintCallable)
	void LoadMenuWidget();
	
	UFUNCTION(BlueprintCallable)
	void InGameLoadMenu();
	
	UFUNCTION(Exec)
	void RefreshServerList() override;

	void StartSession();
private:
	//obtener la clase blueprint 
	TSubclassOf<class UUserWidget> MenuClass;
	TSubclassOf<class UUserWidget> InGameMenuClass;
	class UMainMenu* Menu;
	class UInGameMenu* InGameMenu;
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnNetworkFailure(UWorld * World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString = TEXT(""));

	FString DesiredServerName;
	void CreateSession();
};
