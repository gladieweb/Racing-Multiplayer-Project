// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameInstance.h"
#include "OnlineSessionSettings.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

const static FName SESSION_NAME = NAME_GameSession;
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

UMultiplayerGameInstance::UMultiplayerGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass (TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (!ensure(MenuBPClass.Class != nullptr)) return;
	MenuClass = MenuBPClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass (TEXT("/Game/MenuSystem/WBP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class != nullptr)) return;
	InGameMenuClass = InGameMenuBPClass.Class;
}

void UMultiplayerGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* Subsystem =  IOnlineSubsystem::Get();
	if(Subsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Subsystem %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			UE_LOG(LogTemp, Display, TEXT("Found Session Interface"));
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Found no Subsystem"));
	}

	if(GEngine != nullptr)
	{
		GEngine->OnNetworkFailure().AddUObject( this, &UMultiplayerGameInstance::OnNetworkFailure );
	}
}

void UMultiplayerGameInstance::Host(FString ServerName)
{
	DesiredServerName = ServerName;
	if(SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if(ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}

	}
}

void UMultiplayerGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if(!SessionSearch.IsValid()) return;
	if(Menu != nullptr)
	{
		//Menu->SetServerList({"test", "test2"});
		Menu->Teardown();
	}
	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Green,FString::Printf(TEXT("Joining %d"), Index));

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index] );
}

void UMultiplayerGameInstance::LoadMainManu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;
	PlayerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UMultiplayerGameInstance::LoadMenuWidget()
{
	if (!ensure(MenuClass != nullptr)) return;
	Menu = CreateWidget<UMainMenu>(this, MenuClass);

	if (!ensure(Menu != nullptr)) return;
	Menu->Setup();
	Menu->SetMenuInterface(this);
}

void UMultiplayerGameInstance::InGameLoadMenu()
{
	if (!ensure(InGameMenuClass != nullptr)) return;

	UMenuWidget* GameInstanceMenu = CreateWidget<UMenuWidget>(this,InGameMenuClass);
	GameInstanceMenu->Setup();
	GameInstanceMenu->SetMenuInterface(this);
}

void UMultiplayerGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if(SessionSearch.IsValid())
	{
				
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults= 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Warning, TEXT("Starting Find sessions"));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SessionSearch is not valid");
	}
}

void UMultiplayerGameInstance::StartSession()
{
	if(SessionInterface.IsValid())
	{
		SessionInterface->StartSession(SESSION_NAME);
	}
}

void UMultiplayerGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if(!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
		return;
	}
	if(Menu != nullptr)
	{
		Menu->Teardown();
	}
	
	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Green, "Hosting");

	UWorld* World = GetWorld();
	if(!ensure(World != nullptr)) return;
	
	World->ServerTravel("/Game/Maps/Lobby?listen");
}

void UMultiplayerGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if(Success)
	{
		CreateSession();
	}
}

void UMultiplayerGameInstance::OnFindSessionComplete(bool Success)
{
	if(Success && SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Sessions Success"));
		
		TArray<FServerData> ServerNames;
		//ServerNames.Add("Test01");
		//ServerNames.Add("Test02");

		for (FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults )
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Sessions Name %s"), *SearchResult.GetSessionIdStr());
			FServerData Data;
			Data.Name = SearchResult.GetSessionIdStr();
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			Data.HostUserName = SearchResult.Session.OwningUserName;
			FString ServerName;
			if(SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
			{
				Data.Name = ServerName;
			}
			else
			{
				Data.Name = "Could not find name.";
			}
			ServerNames.Add(Data);
		}
		
		if( Menu != nullptr)
		{
			Menu->SetServerList(ServerNames);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Menu in OnFindSessionsComplete is null");
		}
	}
}

void UMultiplayerGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(!SessionInterface.IsValid()) return;
	FString Address;
	if(!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get connect string"));
		return;
	}
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;
	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UMultiplayerGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	const FString& ErrorString)
{
	LoadMainManu();
}

void UMultiplayerGameInstance::CreateSession()
{
	if(SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		if(IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			SessionSettings.bIsLANMatch = true;
		}
		else
		{
			SessionSettings.bIsLANMatch = false;
		}
		SessionSettings.NumPublicConnections = 4;
		//si es una sesion publica y puedo buscarlo con search
		SessionSettings.bShouldAdvertise = true;
		//steam usa bUsesPresence para saber si crear lobby session o internet Session, true es lobby.
		SessionSettings.bUsesPresence = true;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionInterface->CreateSession(0, SESSION_NAME,SessionSettings );
		
	}
}
