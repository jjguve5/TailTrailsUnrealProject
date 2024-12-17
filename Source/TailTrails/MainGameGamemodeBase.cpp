// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameGamemodeBase.h"
#include "MainGamePlayerstateBase.h"
#include "MainGamePlayercontrollerBase.h"
#include "GameFramework/GameStateBase.h"

void AMainGameGamemodeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // Get all PlayerStates and send them to the new player
    for (APlayerState* PlayerState : GameState->PlayerArray)
    {
        if (PlayerState != NewPlayer->PlayerState)
        {
            AMainGamePlayercontrollerBase* NewPlayerController = Cast<AMainGamePlayercontrollerBase>(NewPlayer);
			AMainGamePlayerstateBase* PlayerStateBase = Cast<AMainGamePlayerstateBase>(PlayerState);
			if (NewPlayerController && PlayerStateBase)
            {
                // Send PlayerState data to the new player
				UE_LOG(LogTemp, Log, TEXT("Sending PlayerState with player color ID %d to %s"), PlayerStateBase->GetPlayerColorID(), *NewPlayerController->GetName());
                NewPlayerController->ClientReceiveExistingPlayerState(PlayerStateBase);
            }
        }
    }
}

void AMainGameGamemodeBase::StorePlayerToken(APlayerController* PlayerController, const FString& Token)
{
    if (PlayerController)
    {
        PlayerTokens.Add(PlayerController, Token);
		UE_LOG(LogTemp, Warning, TEXT("AMainGameGamemodeBase::StorePlayerToken: %s for %s"), *Token, *PlayerController->GetName());
        FetchAndStorePlayerData(PlayerController);
    }
}

FString AMainGameGamemodeBase::GetPlayerToken(APlayerController* PlayerController) const
{
    if (PlayerController)
    {
        const FString* Token = PlayerTokens.Find(PlayerController);
        if (Token)
        {
            return *Token;
        }
    }
    return TEXT("");
}

void AMainGameGamemodeBase::FetchAndStorePlayerData(APlayerController* PlayerController)
{
    if (!PlayerController) return;

    FString* Token = PlayerTokens.Find(PlayerController);
    if (!Token) return;

    UE_LOG(LogTemp, Warning, TEXT("Fetching player data"));

    // Example HTTP request setup
    FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AMainGameGamemodeBase::OnPlayerDataReceived, PlayerController);
    Request->SetURL(GetPlayerDataUrl);
    Request->SetVerb("GET");
    Request->SetHeader("Authorization", "Bearer " + *Token);
    Request->ProcessRequest();
}

void AMainGameGamemodeBase::OnPlayerDataReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, APlayerController* PlayerController)
{
    if (!bWasSuccessful || !PlayerController) return;

    UE_LOG(LogTemp, Warning, TEXT("Received player data"));

    // Parse response JSON (example using Unreal's JSON utilities)
    FString ResponseString = Response->GetContentAsString();
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        int32 ColorID = JsonObject->GetIntegerField("color");

        // Store the data in PlayerState
        AMainGamePlayerstateBase* PlayerState = PlayerController->GetPlayerState<AMainGamePlayerstateBase>();
        if (PlayerState)
        {
			UE_LOG(LogTemp, Warning, TEXT("Setting color ID %d"), ColorID);
            PlayerState->SetPlayerColorID(ColorID);
			PlayerState->ForceNetUpdate();
        }
    }
}
