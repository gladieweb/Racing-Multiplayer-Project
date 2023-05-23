// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

/**
 * 
 */
USTRUCT()
struct FServerData
{
	GENERATED_BODY()
	FString Name;
	uint16 MaxPlayers;
	uint16 CurrentPlayers;
	FString HostUserName;
};
UCLASS()
class MULTIPLAYERPROJECT_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()
public:
	UMainMenu(const FObjectInitializer& ObjectInitializer );
	void SetServerList(TArray<FServerData> ServerNames);
	void SelectIndex(uint32 index);
protected:
	virtual bool Initialize() override;

private:
	TSubclassOf<class UUserWidget> ServerRowClass;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinPanelButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* HostPanelButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* CancelJoinMenuButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* CancelHostMenuButton;
	
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;
	
	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* IPAddressField;
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerHostName;
	
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;

	UFUNCTION()
	void HostServer();
	UFUNCTION()
	void JoinServer();
	UFUNCTION()
	void OpenJoinMenu();
	UFUNCTION()
	void OpenHostMenu();
	UFUNCTION()
	void OpenMainMenu();
	UFUNCTION()
	void QuitPressed();
	
	TOptional<uint32> SelectedIndex; //pointers are stored in the heap. (heap expensive and messy, stack is safe and cheaper but structured optional can be null but is a stack pointers are heap.

	void UpdateChildren();
};
