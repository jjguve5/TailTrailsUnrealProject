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

public:
    void SetPlayerColorID(int32 NewColorID);

	UFUNCTION(BlueprintCallable)
    int32 GetPlayerColorID() const { return PlayerColorID; }

	UFUNCTION()
	void OnRep_PlayerColorID();
};
