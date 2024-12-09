// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "AuthenticationSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDataReceived, int32, PlayerColor);

/**
 * 
 */
UCLASS()
class TAILTRAILS_API UAuthenticationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	void SendRequest(FString Url, FString Verb, FString Content, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> OnResponseReceived);

	UFUNCTION(BlueprintCallable, Category = "Authentication")
	void Authenticate(FString SteamWebTicket);
	UFUNCTION(BlueprintCallable, Category = "Authentication")
	void Create(FString SteamWebTicket);
	UFUNCTION(BlueprintCallable, Category = "Authentication")
	void GetPlayerData();

	UPROPERTY(BlueprintAssignable, Category = "Authentication")
	FOnPlayerDataReceived OnPlayerDataReceived;

private:
	const FString ApiBaseUrl = TEXT("http://localhost:3000/api");
	const FString AuthenticateUrl = ApiBaseUrl + TEXT("/auth/authenticate");
	const FString CreateUrl = ApiBaseUrl + TEXT("/auth/create");
	const FString GetPlayerDataUrl = ApiBaseUrl + TEXT("/users/steam");
	FString AuthToken;

	void HandleAuthError(int32 ErrorCode);
};
