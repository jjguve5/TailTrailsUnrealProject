// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGamePlayerstateBase.h"
#include "Net/UnrealNetwork.h"
#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"

void AMainGamePlayerstateBase::SetPlayerColorID(int32 NewColorID)
{
    if (HasAuthority())
    {
        PlayerColorID = NewColorID;
		OnRep_PlayerColorID();
    }
}

void AMainGamePlayerstateBase::OnRep_PlayerColorID()
{
	UE_LOG(LogTemp, Warning, TEXT("Player color ID changed to %d"), PlayerColorID);
    // Get the controlled Pawn
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("Pawn not found, retrying!"));
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AMainGamePlayerstateBase::OnRep_PlayerColorID);
        return;
    }

    // Cast the Pawn to your character class
    ATP_ThirdPersonCharacter* ThirdPersonCharacter = Cast<ATP_ThirdPersonCharacter>(ControlledPawn);
    if (!ThirdPersonCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Pawn is not of type TP_ThirdPersonCharacter!"));
        return;
    }

    // Call the Blueprint event
    UFunction* SetPlayerColorFunction = ThirdPersonCharacter->FindFunction(FName("SetPlayerColor"));
    if (SetPlayerColorFunction)
    {
        struct FSetPlayerColorParams
        {
            int32 ColorId;
        };

        FSetPlayerColorParams Params;
        Params.ColorId = PlayerColorID;

        ThirdPersonCharacter->ProcessEvent(SetPlayerColorFunction, &Params);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SetPlayerColor function not found on TP_ThirdPersonCharacter!"));
    }
}

void AMainGamePlayerstateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMainGamePlayerstateBase, PlayerColorID);
}
