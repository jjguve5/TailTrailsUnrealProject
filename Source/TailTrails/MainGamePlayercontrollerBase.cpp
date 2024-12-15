// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGamePlayercontrollerBase.h"

#include "MainGameGamemodeBase.h"
#include "MainGamePlayerstateBase.h"
#include "GameFramework/GameStateBase.h"
#include "AuthenticationSubsystem.h"
#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"

void AMainGamePlayercontrollerBase::BeginPlay()
{
    Super::BeginPlay();

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