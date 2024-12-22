// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MinigameHandlerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TAILTRAILS_API UMinigameHandlerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	FString GetCurrentJoiningMinigameName() const;

	UFUNCTION(BlueprintCallable)
	void JoinMinigameLobby(FString MinigameName);

	UFUNCTION(BlueprintCallable)
	void StartMinigame();

	UFUNCTION(BlueprintCallable)
	void HostLevel(FName LevelName, bool bIsLAN, int32 MaxPlayers);

	FString CurrentJoiningMinigameName;
};
