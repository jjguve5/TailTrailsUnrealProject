// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainGamePlayerstateBase.generated.h"

/**
 * 
 */
UCLASS()
class TAILTRAILS_API AMainGamePlayerstateBase : public APlayerState
{
	GENERATED_BODY()
	
private:
    UPROPERTY(ReplicatedUsing = OnRep_PlayerColorID)
    int32 PlayerColorID;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerShirtID)
	int32 PlayerShirtID;

public:
    void SetPlayerColorID(int32 NewColorID);

	UFUNCTION(BlueprintCallable)
    int32 GetPlayerColorID() const { return PlayerColorID; }

	UFUNCTION()
	void OnRep_PlayerColorID();

	void SetPlayerShirtID(int32 NewShirtID);

	UFUNCTION(BlueprintCallable)
	int32 GetPlayerShirtID() const { return PlayerShirtID; }

	UFUNCTION()
	void OnRep_PlayerShirtID();
};
