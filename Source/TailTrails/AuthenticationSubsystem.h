// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "AuthenticationSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FPlayerData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 color;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 shirt;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDataReceived, FPlayerData, LoggedPlayerData);

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
	void Create(FString SteamWebTicket, int32 Color);
	UFUNCTION(BlueprintCallable, Category = "Authentication")
	void GetPlayerData();
	UFUNCTION(BlueprintCallable, Category = "Authentication")
	int32 GetPlayerId();
	UFUNCTION(BlueprintCallable, Category = "Authentication")
	void SendTokenToServer();

	UPROPERTY(BlueprintAssignable, Category = "Authentication")
	FOnPlayerDataReceived OnPlayerDataReceived;

	UFUNCTION(BlueprintCallable, Category = "Editor")
	int32 GetPlayInEditorWindowID()
	{
		#if WITH_EDITOR
				return GPlayInEditorID;
		#else
				return -1; // Return -1 if not in the editor
		#endif
	}



private:
	const FString ApiBaseUrl = TEXT("http://198.7.113.75:3000/api");
	const FString AuthenticateUrl = ApiBaseUrl + TEXT("/auth/authenticate");
	const FString CreateUrl = ApiBaseUrl + TEXT("/auth/create");
	const FString GetPlayerDataUrl = ApiBaseUrl + TEXT("/users/steam");
	FString AuthToken;
	int32 PlayerId;

	void HandleAuthError(int32 ErrorCode);
};
