// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "MenuInterface.h"
#include "ServerRow.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "UObject/ConstructorHelpers.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer )
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;
	ServerRowClass = ServerRowBPClass.Class;
}

void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (!ensure(World != nullptr)) return;

	ServerList->ClearChildren();
	uint32 i = 0;
	for(const FServerData& ServerData : ServerNames)
	{
		UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if (!ensure(Row != nullptr)) return;

		Row->ServerName->SetText(FText::FromString(ServerData.Name));
		Row->HostUser->SetText(FText::FromString(ServerData.HostUserName));
		FString FractionText = FString::Printf(TEXT("%d/%d"),ServerData.CurrentPlayers, ServerData.MaxPlayers);
		Row->ConnectionFraction->SetText(FText::FromString(FractionText));

		Row->Setup(this, i);
		++i;
		ServerList->AddChild(Row);
	}


}

void UMainMenu::SelectIndex(uint32 index)
{
	SelectedIndex = index;
	UE_LOG(LogTemp, Warning, TEXT("Selected  %d"),SelectedIndex.GetValue());
	UpdateChildren();
}

bool UMainMenu::Initialize()
{
	bool Success =  Super::Initialize();
	if(!Success) return Success;
	if (!ensure(HostButton != nullptr)) return false;
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (!ensure(JoinPanelButton != nullptr)) return false;
	JoinPanelButton->OnClicked.AddDynamic(this,&UMainMenu::OpenJoinMenu);
	
	if (!ensure(HostPanelButton != nullptr)) return false;
	HostPanelButton->OnClicked.AddDynamic(this,&UMainMenu::OpenHostMenu);
	
	if (!ensure(CancelJoinMenuButton != nullptr)) return false;
	CancelJoinMenuButton->OnClicked.AddDynamic(this,&UMainMenu::OpenMainMenu);
	
	if (!ensure(CancelHostMenuButton != nullptr)) return false;
	CancelHostMenuButton->OnClicked.AddDynamic(this,&UMainMenu::OpenMainMenu);
	
	if (!ensure(JoinButton != nullptr)) return false;
	JoinButton->OnClicked.AddDynamic(this,&UMainMenu::JoinServer);

	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this,&UMainMenu::QuitPressed);
	return true;
}

void UMainMenu::HostServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Gonna host the server"));
	if(MenuInterface != nullptr)
	{
		const FString NameString = ServerHostName->GetText().ToString();
		MenuInterface->Host(NameString);
	}
}

void UMainMenu::JoinServer()
{
	UE_LOG(LogTemp, Warning, TEXT("joining a server"))
	if(SelectedIndex.IsSet() && MenuInterface != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Selected index %d"), SelectedIndex.GetValue() );
		MenuInterface->Join(SelectedIndex.GetValue());

	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Selected index not setted"));
	}
	//if(MenuInterface != nullptr)
	//{
	//	if (ensure(IPAddressField != nullptr))
	//	{
			//const FString Address = IPAddressField->GetText().ToString();
			//if(!Address.IsEmpty())
			//{
				//MenuInterface->Join(Address);
			//}
	//	}
	//}
}

void UMainMenu::OpenJoinMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(JoinMenu);
	if(MenuInterface != nullptr)
	{
		MenuInterface->RefreshServerList();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "No MenuInterface");
	}
}

void UMainMenu::OpenHostMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(HostMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::OpenMainMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(MainMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::QuitPressed()
{
	const UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ConsoleCommand("quit");
}

void UMainMenu::UpdateChildren()
{
	for(int32 i = 0; i< ServerList->GetChildrenCount(); ++i)
	{
		auto Row = Cast<UServerRow> (ServerList->GetChildAt(i));
		if(Row != nullptr)
		{
			Row->Selected =SelectedIndex.IsSet() && (SelectedIndex.GetValue() == i);
		}
	}
}
