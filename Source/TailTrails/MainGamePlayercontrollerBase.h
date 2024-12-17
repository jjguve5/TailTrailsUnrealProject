// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainGamePlayerstateBase.h"
#include "MainGamePlayercontrollerBase.generated.h"

/**
 * 
 */
UCLASS()
class TAILTRAILS_API AMainGamePlayercontrollerBase : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendPlayerToken(const FString& Token);

	UFUNCTION(Client, Reliable)
	void ClientReceiveExistingPlayerState(AMainGamePlayerstateBase* ExistingPlayerState);

	void HandleExistingPlayerStates();

protected:
	void BeginPlay() override;
	virtual void OnRep_PlayerState() override;
};
