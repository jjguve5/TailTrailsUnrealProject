// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGamePlayercontrollerBase.h"

#include "MainGameGamemodeBase.h"
#include "MainGamePlayerstateBase.h"
#include "GameFramework/GameStateBase.h"
#include "AuthenticationSubsystem.h"
#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"

void AMainGamePlayercontrollerBase::ClientReceiveExistingPlayerState_Implementation(AMainGamePlayerstateBase* ExistingPlayerState)
{
 //   if (ExistingPlayerState)
 //   {
 //       UE_LOG(LogTemp, Log, TEXT("Received PlayerState: %s"), *ExistingPlayerState->GetName());

	//	ATP_ThirdPersonCharacter* ThirdPersonCharacter = Cast<ATP_ThirdPersonCharacter>(ExistingPlayerState->GetPawn());
	//	if (ThirdPersonCharacter)
	//	{
	//		// Call the Blueprint event
	//		UFunction* SetPlayerColorFunction = ThirdPersonCharacter->FindFunction(FName("SetPlayerColor"));
	//		if (SetPlayerColorFunction)
	//		{
	//			struct FSetPlayerColorParams
	//			{
	//				int32 ColorId;
	//			};

	//			FSetPlayerColorParams Params;
	//			Params.ColorId = ExistingPlayerState->GetPlayerColorID();

	//			ThirdPersonCharacter->ProcessEvent(SetPlayerColorFunction, &Params);
	//		}
	//		else
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("SetPlayerColor function not found on TP_ThirdPersonCharacter!"));
	//		}
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Pawn is not of type TP_ThirdPersonCharacter!"));
	//	}
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Received NULL PlayerState"));
	//}
}

void AMainGamePlayercontrollerBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay: IsLocalPlayerController: %d"), IsLocalPlayerController());

	//if is local player
	if (IsLocalPlayerController())
	{
		// Get the Authentication Subsystem
		UAuthenticationSubsystem* AuthenticationSubsystem = GetGameInstance()->GetSubsystem<UAuthenticationSubsystem>();
		if (AuthenticationSubsystem)
		{
			AuthenticationSubsystem->SendTokenToServer();
		}
	}
}

void AMainGamePlayercontrollerBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!IsLocalPlayerController()) return;

	if (PlayerState)
	{
		UE_LOG(LogTemp, Log, TEXT("PlayerState is now replicated: %s"), *PlayerState->GetName());

		// Process PlayerState now that it's valid
		HandleExistingPlayerStates();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("PlayerState is NULL, cum catano"));
	}
}

void AMainGamePlayercontrollerBase::HandleExistingPlayerStates()
{
	//// Get all PlayerStates and send them to the new player
	//for (APlayerState* MyPlayerState : GetWorld()->GetGameState()->PlayerArray)
	//{
	//	if (MyPlayerState != PlayerState)
	//	{
	//		ATP_ThirdPersonCharacter* ThirdPersonCharacter = Cast<ATP_ThirdPersonCharacter>(MyPlayerState->GetPawn());
	//		AMainGamePlayerstateBase* PlayerStateBase = Cast<AMainGamePlayerstateBase>(MyPlayerState);
	//		if (ThirdPersonCharacter)
	//		{
	//			// Call the Blueprint event
	//			UFunction* SetPlayerColorFunction = ThirdPersonCharacter->FindFunction(FName("SetPlayerColor"));
	//			if (SetPlayerColorFunction)
	//			{
	//				struct FSetPlayerColorParams
	//				{
	//					int32 ColorId;
	//				};

	//				FSetPlayerColorParams Params;
	//				Params.ColorId = PlayerStateBase->GetPlayerColorID();

	//				ThirdPersonCharacter->ProcessEvent(SetPlayerColorFunction, &Params);
	//			}
	//			else
	//			{
	//				UE_LOG(LogTemp, Warning, TEXT("SetPlayerColor function not found on TP_ThirdPersonCharacter!"));
	//			}
	//		}
	//		else
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("Pawn is not of type TP_ThirdPersonCharacter!"));
	//		}
	//	}
	//}
}

void AMainGamePlayercontrollerBase::ServerSendPlayerToken_Implementation(const FString& Token)
{
    AMainGameGamemodeBase* GameMode = Cast<AMainGameGamemodeBase>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
        GameMode->StorePlayerToken(this, Token);
    }
}

bool AMainGamePlayercontrollerBase::ServerSendPlayerToken_Validate(const FString& Token)
{
    return !Token.IsEmpty();
}