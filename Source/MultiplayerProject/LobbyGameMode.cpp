// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "MultiplayerGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	++NumberOfPlayers;
	UE_LOG(LogTemp, Warning, TEXT("New Number of players connected: %d"),NumberOfPlayers);

	if(NumberOfPlayers > 2)
	{
		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ALobbyGameMode::StartGame, 10);
		
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	--NumberOfPlayers;
}

void ALobbyGameMode::StartGame()
{
	auto GameInstance = Cast<UMultiplayerGameInstance>( GetGameInstance());
	if(GameInstance == nullptr) return;
	GameInstance->StartSession(); //is not visible when you call start session
	
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
}
