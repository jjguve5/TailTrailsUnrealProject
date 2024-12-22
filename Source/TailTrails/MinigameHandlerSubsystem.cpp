// Fill out your copyright notice in the Description page of Project Settings.


#include "MinigameHandlerSubsystem.h"

void UMinigameHandlerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UMinigameHandlerSubsystem::Deinitialize()
{
}

FString UMinigameHandlerSubsystem::GetCurrentJoiningMinigameName() const
{
	return CurrentJoiningMinigameName;
}

void UMinigameHandlerSubsystem::JoinMinigameLobby(FString MinigameName)
{
	CurrentJoiningMinigameName = MinigameName;

	//TODO: get from minigame properties the max players
	HostLevel("MinigameLobby", false, 4);
}

void UMinigameHandlerSubsystem::StartMinigame()
{
    FString TravelURL = FString::Printf(TEXT("%s?listen"), *CurrentJoiningMinigameName);
    GetWorld()->ServerTravel(TravelURL, true); // Seamless travel is enabled
}


void UMinigameHandlerSubsystem::HostLevel(FName LevelName, bool bIsLAN, int32 MaxPlayers)
{
    // Get the Player Controller for the first player
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (!PlayerController) return;

    // Create the URL for the server
    FString URL = LevelName.ToString();
    URL += bIsLAN ? TEXT("?listen?bIsLAN=true") : TEXT("?listen");

    // Set maximum players (use "MaxPlayers=x" in URL if needed)
    FString MaxPlayersStr = FString::Printf(TEXT("?MaxPlayers=%d"), MaxPlayers);
    URL += MaxPlayersStr;

    // Travel to the map as a listen server
    PlayerController->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
}