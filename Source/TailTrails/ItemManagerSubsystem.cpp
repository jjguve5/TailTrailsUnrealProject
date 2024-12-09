// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemManagerSubsystem.h"


void UItemManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("UItemManagerSubsystem::Initialize"));
	LoadItems();
}

void UItemManagerSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("UItemManagerSubsystem::Deinitialize"));
}

void UItemManagerSubsystem::SendRequest(FString Url, FString Verb, FString Content, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> OnResponseReceived)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(Verb);
	Request->SetHeader("Content-Type", "application/json");
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
				UE_LOG(LogTemp, Error, TEXT("UItemManagerSubsystem::SendRequest: Request or Response is invalid."));
			}
		});
	Request->ProcessRequest();
}

void UItemManagerSubsystem::LoadItems()
{
	Items.Empty();

	SendRequest(GetAllItemsUrl, "GET", TEXT(""), [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (bWasSuccessful)
			{
				FString ResponseContent = Response->GetContentAsString();
				TSharedPtr<FJsonObject> JsonObject;
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

				if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
				{
					TArray<TSharedPtr<FJsonValue>> ItemsJson = JsonObject->GetArrayField("items");

					for (TSharedPtr<FJsonValue> ItemJson : ItemsJson)
					{
						TSharedPtr<FJsonObject> ItemObject = ItemJson->AsObject();
						FItem Item;
						Item.ID = ItemObject->GetIntegerField("id");
						Item.Name = ItemObject->GetStringField("name");
						Item.Type = (EItemType)ItemObject->GetIntegerField("type");

						//properties is a json like this for colors for example '{"Color": "(R=1, G=0.5, B=0, A=1)", "Material": "Orange"}'
						FString PropertiesString = ItemObject->GetStringField("properties");
						TSharedPtr<FJsonObject> PropertiesObject;
						TSharedRef<TJsonReader<>> PropertiesReader = TJsonReaderFactory<>::Create(PropertiesString);
						if (!FJsonSerializer::Deserialize(PropertiesReader, PropertiesObject))
						{
							UE_LOG(LogTemp, Error, TEXT("UItemManagerSubsystem::LoadItems: Failed to deserialize properties JSON."));
							continue;
						}
						else
						{
							for (TPair<FString, TSharedPtr<FJsonValue>> Pair : PropertiesObject->Values)
							{
								Item.Properties.Add(Pair.Key, Pair.Value->AsString());
							}
						}

						Items.Add(Item);
					}
					UE_LOG(LogTemp, Warning, TEXT("UItemManagerSubsystem::LoadItems: Loaded %d items."), Items.Num());
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("UItemManagerSubsystem::LoadItems: Failed to deserialize JSON."));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UItemManagerSubsystem::LoadItems: Request failed."));
			}
		});
}

TArray<FItem> UItemManagerSubsystem::GetItemsByType(EItemType Type) const
{
	TArray<FItem> ItemsByType;

	for (FItem Item : Items)
	{
		if (Item.Type == Type)
		{
			ItemsByType.Add(Item);
		}
	}

	return ItemsByType;
}

FItem UItemManagerSubsystem::GetItemByID(int32 ID) const
{
	for (FItem Item : Items)
	{
		if (Item.ID == ID)
		{
			return Item;
		}
	}

	return FItem();
}

