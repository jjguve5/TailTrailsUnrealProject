// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "http.h"
#include "MainGameGamemodeBase.generated.h"

/**
 * 
 */
UCLASS()
class TAILTRAILS_API AMainGameGamemodeBase : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	TMap<APlayerController*, FString> PlayerTokens;

	const FString ApiBaseUrl = TEXT("http://localhost:3000/api");
	const FString GetPlayerDataUrl = ApiBaseUrl + TEXT("/users/steam");

public:
	void StorePlayerToken(APlayerController* PlayerController, const FString& Token);
	FString GetPlayerToken(APlayerController* PlayerController) const;

	void OnPlayerDataReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void FetchAndStorePlayerData(APlayerController* PlayerController);

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
