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
						Item.Cost = ItemObject->GetIntegerField("cost");
						Item.Icon = ItemObject->GetStringField("icon");

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

UMaterialInterface* UItemManagerSubsystem::GetMaterialByName(const FString& MaterialName)
{
	FString MaterialPath = FString::Printf(TEXT("/Game/Shared/Assets/Characters/Fox/Materials/%s.%s"), *MaterialName, *MaterialName);
	UMaterialInterface* Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *MaterialPath));

	if (!Material)
	{
		UE_LOG(LogTemp, Warning, TEXT("Material %s not found! Path: %s"), *MaterialName, *MaterialPath);
	}

	return Material;
}

UTexture2D* UItemManagerSubsystem::GetIconByItemId(int32 ID) const
{
	FItem Item = GetItemByID(ID);
	if (Item.Icon.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemManagerSubsystem::GetIconByItemId: Icon is empty for item with ID %d"), ID);
		return nullptr;
	}

	FString IconPath = FString::Printf(TEXT("/Game/Shared/Assets/Clothes/Icons/%s.%s"), *Item.Icon, *Item.Icon);
	UTexture2D* Icon = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *IconPath));

	if (!Icon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Icon %s not found! Path: %s"), *Item.Icon, *IconPath);
	}

	return Icon;
}

USkeletalMesh* UItemManagerSubsystem::GetMeshByItemId(int32 ID) const
{
	FItem Item = GetItemByID(ID);
	if (Item.Properties.Contains("Mesh"))
	{
		FString MeshName = Item.Properties["Mesh"];
		FString MeshPath = FString::Printf(TEXT("/Game/Shared/Assets/Clothes/Meshes/%s.%s"), *MeshName, *MeshName);
		USkeletalMesh* Mesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, *MeshPath));

		if (!Mesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("Mesh %s not found! Path: %s"), *MeshName, *MeshPath);
		}

		return Mesh;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemManagerSubsystem::GetMeshByItemId: Mesh property not found for item with ID %d"), ID);
		return nullptr;
	}
}
