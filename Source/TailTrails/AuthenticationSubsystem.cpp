// Fill out your copyright notice in the Description page of Project Settings.


#include "AuthenticationSubsystem.h"
#include <Kismet/GameplayStatics.h>


void UAuthenticationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    UE_LOG(LogTemp, Warning, TEXT("UAuthenticationSubsystem::Initialize"));
}

void UAuthenticationSubsystem::Deinitialize()
{
    UE_LOG(LogTemp, Warning, TEXT("UAuthenticationSubsystem::Deinitialize"));
}

void UAuthenticationSubsystem::SendRequest(FString Url, FString Verb, FString Content, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> OnResponseReceived)
{
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb(Verb);
    Request->SetHeader("Content-Type", "application/json");
	if (AuthToken.Len() > 0)
	{
		Request->SetHeader("Authorization", FString::Printf(TEXT("Bearer %s"), *AuthToken));
	}
    if (Content.Len() > 0) {
		Request->SetContentAsString(Content);
    }
    Request->OnProcessRequestComplete().BindLambda([OnResponseReceived](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (Request.IsValid() && Response.IsValid())
            {
                OnResponseReceived(Request, Response, bWasSuccessful);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("UAuthenticationSubsystem::SendRequest: Request or Response is invalid."));
            }
        });
    Request->ProcessRequest();
}


void UAuthenticationSubsystem::Authenticate(FString SteamWebTicket)
{
	SendRequest(AuthenticateUrl, "POST", FString::Printf(TEXT("{\"steamWebTicket\":\"%s\"}"), *SteamWebTicket), [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (bWasSuccessful)
            {
                FString ResponseContent = Response->GetContentAsString();
                TSharedPtr<FJsonObject> JsonObject;
                TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

                if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
                {
                    if (JsonObject->GetBoolField("hasError"))
                    {
                        int32 ErrorCode = JsonObject->GetIntegerField("error");
                        UE_LOG(LogTemp, Error, TEXT("Authentication failed. Error Code: %d"), ErrorCode);
                        HandleAuthError(ErrorCode);
                        return;
                    }

                    bool HasAccount = JsonObject->GetBoolField("hasAccount");

                    if (HasAccount)
                    {
                        AuthToken = JsonObject->GetStringField("token");
                        UE_LOG(LogTemp, Log, TEXT("Authentication successful. Token: %s"), *AuthToken);
                        // Proceed with game logic (e.g., storing the token, proceeding to the next step)
                        UGameplayStatics::OpenLevel(GetWorld(), "MainLogged");
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Authentication successful but no account found."));
                        // Handle case where the user is authenticated but has no account linked
                        //open level by name Create
                        UGameplayStatics::OpenLevel(GetWorld(), "Create");
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to parse server response: %s"), *ResponseContent);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("UAuthenticationSubsystem::AuthenticateWithSteam: Request failed, Response: %s"), *Response->GetContentAsString());
            }
        });
}

void UAuthenticationSubsystem::Create(FString SteamWebTicket)
{
	SendRequest(CreateUrl, "POST", FString::Printf(TEXT("{\"steamWebTicket\":\"%s\"}"), *SteamWebTicket), [](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (bWasSuccessful)
            {
                UE_LOG(LogTemp, Log, TEXT("UAuthenticationSubsystem::CreateAccount: Success, Response: %s"), *Response->GetContentAsString());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("UAuthenticationSubsystem::CreateAccount: Request failed, Response: %s"), *Response->GetContentAsString());
            }
        });
}

void UAuthenticationSubsystem::GetPlayerData()
{
	SendRequest(GetPlayerDataUrl, "GET", "", [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (bWasSuccessful)
			{
				UE_LOG(LogTemp, Log, TEXT("UAuthenticationSubsystem::GetPlayerData: Success, Response: %s"), *Response->GetContentAsString());

                FString ResponseContent = Response->GetContentAsString();

                TSharedPtr<FJsonObject> JsonObject;
                TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

                if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
                {
                    if (JsonObject->GetIntegerField("color"))
                    {
                        int32 PlayerColor = JsonObject->GetIntegerField("color");
                        OnPlayerDataReceived.Broadcast(PlayerColor);
					}
					else
					{
						// TODO:REMOVE, for debugging reasons use color as 1
						OnPlayerDataReceived.Broadcast(1);
					}
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to parse server response: %s"), *ResponseContent);
                }
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UAuthenticationSubsystem::GetPlayerData: Request failed, Response: %s"), *Response->GetContentAsString());
			}
		});
}

void UAuthenticationSubsystem::HandleAuthError(int32 ErrorCode)
{
    switch (ErrorCode)
    {
    case 1:
        UE_LOG(LogTemp, Error, TEXT("Error: Steam web ticket is missing or invalid."));
        break;
    case 2:
        UE_LOG(LogTemp, Error, TEXT("Error: Steam user authentication failed."));
        break;
    case 3:
        UE_LOG(LogTemp, Error, TEXT("Error: User is VAC-banned or publisher-banned."));
        break;
    default:
        UE_LOG(LogTemp, Error, TEXT("Unknown authentication error."));
        break;
    }
}


