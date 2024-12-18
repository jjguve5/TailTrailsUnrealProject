// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGamePlayerstateBase.h"
#include "Net/UnrealNetwork.h"
#include "MainGamePlayercontrollerBase.h"
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
	ATP_ThirdPersonCharacter* ThirdPersonCharacter = Cast<ATP_ThirdPersonCharacter>(GetPawn());
	if (ThirdPersonCharacter)
	{
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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn is not of type TP_ThirdPersonCharacter!"));
	}
}

void AMainGamePlayerstateBase::SetPlayerShirtID(int32 NewShirtID)
{
	if (HasAuthority())
	{
		PlayerShirtID = NewShirtID;
		OnRep_PlayerShirtID();
	}
}

void AMainGamePlayerstateBase::OnRep_PlayerShirtID()
{
	ATP_ThirdPersonCharacter* ThirdPersonCharacter = Cast<ATP_ThirdPersonCharacter>(GetPawn());
	if (ThirdPersonCharacter)
	{
		// Call the Blueprint event
		UFunction* SetPlayerShirtFunction = ThirdPersonCharacter->FindFunction(FName("SetPlayerShirt"));
		if (SetPlayerShirtFunction)
		{
			struct FSetPlayerShirtParams
			{
				int32 ShirtId;
			};

			FSetPlayerShirtParams Params;
			Params.ShirtId = PlayerShirtID;

			ThirdPersonCharacter->ProcessEvent(SetPlayerShirtFunction, &Params);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SetPlayerShirt function not found on TP_ThirdPersonCharacter!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn is not of type TP_ThirdPersonCharacter!"));
	}
}

void AMainGamePlayerstateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMainGamePlayerstateBase, PlayerColorID);
	DOREPLIFETIME(AMainGamePlayerstateBase, PlayerShirtID);
}
