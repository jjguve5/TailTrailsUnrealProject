// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameGamemodeBase.h"
#include "MainGamePlayerstateBase.h"
#include "MainGamePlayercontrollerBase.h"
#include "ItemManagerSubsystem.h"
#include "GameFramework/GameStateBase.h"

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

void AMainGameGamemodeBase::HandleInventoryRequest(APlayerController* PlayerController)
{
    if (!PlayerController) return;

    FString* Token = PlayerTokens.Find(PlayerController);
    if (!Token) return;

    // Example HTTP request setup
    FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AMainGameGamemodeBase::OnPlayerItemsReceived, PlayerController);
    Request->SetURL(GetPlayerItems);
    Request->SetVerb("GET");
    Request->SetHeader("Authorization", "Bearer " + *Token);
    Request->ProcessRequest();
}

void AMainGameGamemodeBase::HandleDressItemRequest(APlayerController* PlayerController, int32 ItemID)
{
	if (!PlayerController) return;

	FString* Token = PlayerTokens.Find(PlayerController);
	if (!Token) return;

	// Example HTTP request setup
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AMainGameGamemodeBase::OnDressItemReceived, PlayerController, ItemID);
	Request->SetURL(DressItemUrl);
	Request->SetVerb("POST");
	Request->SetHeader("Authorization", "Bearer " + *Token);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString("{\"item_id\":" + FString::FromInt(ItemID) + "}");
	Request->ProcessRequest();
}

void AMainGameGamemodeBase::HandlePurchaseItemRequest(APlayerController* PlayerController, int32 ItemID)
{
	if (!PlayerController) return;

	FString* Token = PlayerTokens.Find(PlayerController);
	if (!Token) return;

	// Example HTTP request setup
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AMainGameGamemodeBase::OnPurchaseItemReceived, PlayerController, ItemID);
	Request->SetURL(PurchaseItemUrl);
	Request->SetVerb("POST");
	Request->SetHeader("Authorization", "Bearer " + *Token);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString("{\"item_id\":" + FString::FromInt(ItemID) + "}");
	Request->ProcessRequest();
}

void AMainGameGamemodeBase::OnDressItemReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, APlayerController* PlayerController, int32 ItemID)
{
	if (!bWasSuccessful || !PlayerController) return;

	UE_LOG(LogTemp, Warning, TEXT("Received dress item response"));

	// Parse response JSON (example using Unreal's JSON utilities)
	FString ResponseString = Response->GetContentAsString();
	UE_LOG(LogTemp, Warning, TEXT("Response: %s"), *ResponseString);
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		bool bSuccess = JsonObject->GetBoolField("success");
		if (bSuccess)
		{
			UItemManagerSubsystem* ItemManager = GetGameInstance()->GetSubsystem<UItemManagerSubsystem>();
			FItem Item = ItemManager->GetItemByID(ItemID);
			AMainGamePlayerstateBase* PlayerState = PlayerController->GetPlayerState<AMainGamePlayerstateBase>();
			if (Item.Type == EItemType::Color)
			{
				PlayerState->SetPlayerColorID(ItemID);
				PlayerState->ForceNetUpdate();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Item type not supported"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to dress item"));
		}
	}
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

void AMainGameGamemodeBase::OnPlayerItemsReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, APlayerController* PlayerController)
{
	if (!bWasSuccessful || !PlayerController) return;

	UE_LOG(LogTemp, Warning, TEXT("Received player items"));

	// Parse response JSON (example using Unreal's JSON utilities)
	FString ResponseString = Response->GetContentAsString();
	UE_LOG(LogTemp, Warning, TEXT("Response: %s"), *ResponseString);
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		TArray<int32> PlayerItems;
		const TArray<TSharedPtr<FJsonValue>>* ItemsArray;
		if (JsonObject->TryGetArrayField("items", ItemsArray))
		{
			for (const TSharedPtr<FJsonValue>& ItemValue : *ItemsArray)
			{
				int32 ItemID = ItemValue->AsNumber();
				PlayerItems.Add(ItemID);
			}
		}

		// Send the items to the player controller
		AMainGamePlayercontrollerBase* PlayerControllerBase = Cast<AMainGamePlayercontrollerBase>(PlayerController);
		if (PlayerControllerBase)
		{
			PlayerControllerBase->ClientReceivePlayerItems(PlayerItems);
		}
	}
}

void AMainGameGamemodeBase::OnPurchaseItemReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, APlayerController* PlayerController, int32 ItemID)
{
	if (!bWasSuccessful || !PlayerController) return;

	UE_LOG(LogTemp, Warning, TEXT("Received purchase item response"));

	// Parse response JSON (example using Unreal's JSON utilities)
	FString ResponseString = Response->GetContentAsString();
	UE_LOG(LogTemp, Warning, TEXT("Response: %s"), *ResponseString);
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		bool bSuccess = JsonObject->GetBoolField("success");
		if (bSuccess)
		{
			//do nothing for now
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to purchase item"));
		}
	}
}