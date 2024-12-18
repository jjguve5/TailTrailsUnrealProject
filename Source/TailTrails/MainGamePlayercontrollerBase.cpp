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

		//create chat widget
		if (ChatWidgetClass)
		{
			ChatWidget = CreateWidget<UUserWidget>(this, ChatWidgetClass);
			if (ChatWidget)
			{
				ChatWidget->AddToViewport();
				UE_LOG(LogTemp, Log, TEXT("ChatWidget Created"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ChatWidget is nullptr"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ChatWidgetClass is not set in the PlayerController!"));
		}
	}
}

void AMainGamePlayercontrollerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("OpenInventory", IE_Pressed, this, &AMainGamePlayercontrollerBase::OpenInventory);
	InputComponent->BindAction("OpenCatalog", IE_Pressed, this, &AMainGamePlayercontrollerBase::OpenCatalog);
}

void AMainGamePlayercontrollerBase::OpenInventory()
{
    if (!bIsInventoryOpen)
    {
        if (InventoryWidgetClass)
        {
            if (!InventoryWidget)
            {
                // Cria o widget se ele ainda não existir
                InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
            }

            if (InventoryWidget)
            {
                InventoryWidget->AddToViewport();
                UE_LOG(LogTemp, Log, TEXT("Inventario Aberto"));

                bIsInventoryOpen = true;
                SetInputMode(FInputModeUIOnly()); // Mudar para input UI
                bShowMouseCursor = true;
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("InventoryWidgetClass is not set in the PlayerController!"));
        }
    }
}

void AMainGamePlayercontrollerBase::CloseInventory()
{
    if (bIsInventoryOpen && InventoryWidget)
    {
        InventoryWidget->RemoveFromViewport();
        UE_LOG(LogTemp, Log, TEXT("Inventario Fechado"));

        bIsInventoryOpen = false;
        SetInputMode(FInputModeGameOnly()); // Voltar ao input do jogo
        bShowMouseCursor = false;
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

bool AMainGamePlayercontrollerBase::ServerRequestInventory_Validate()
{
    return true; // Optional: Add validation logic
}

void AMainGamePlayercontrollerBase::ServerRequestInventory_Implementation()
{
    // Get the GameMode
    if (GetWorld())
    {
        AMainGameGamemodeBase* GM = Cast<AMainGameGamemodeBase>(GetWorld()->GetAuthGameMode());
        if (GM)
        {
            GM->HandleInventoryRequest(this);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ServerRequestInventory: Cast to AMainGameGamemodeBase failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerRequestInventory: GetWorld() returned nullptr"));
	}
}

void AMainGamePlayercontrollerBase::ClientReceivePlayerItems_Implementation(const TArray<int32>& PlayerItems, const TArray<int32>& DressedItems)
{
    // This runs on the client: display or process the received items
    UE_LOG(LogTemp, Log, TEXT("Client Received Items:"));
	for (int32 ItemID : PlayerItems)
	{
		UE_LOG(LogTemp, Log, TEXT("Item ID: %d"), ItemID);
	}

	if (!InventoryWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget is nullptr"));
		return;
	}

	// Call the Blueprint event
	UFunction* SetInventoryItemsFunction = InventoryWidget->FindFunction(FName("SetInventoryItems"));
	if (SetInventoryItemsFunction)
	{
		struct FSetInventoryItemsParams
		{
			TArray<int32> Items;
			TArray<int32> DressedItems;
		};

		FSetInventoryItemsParams Params;
		Params.Items = PlayerItems;
		Params.DressedItems = DressedItems;

		InventoryWidget->ProcessEvent(SetInventoryItemsFunction, &Params);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetInventoryItems function not found"));
	}
}

void AMainGamePlayercontrollerBase::RequestInventory()
{
	// Send an RPC to the server to request the inventory
	ServerRequestInventory();
}

void AMainGamePlayercontrollerBase::DressItem(int32 ItemID)
{
	ServerDressItem(ItemID);
}

bool AMainGamePlayercontrollerBase::ServerDressItem_Validate(int32 ItemID)
{
	return true;
}

void AMainGamePlayercontrollerBase::ServerDressItem_Implementation(int32 ItemID)
{
	// Get the GameMode
	if (GetWorld())
	{
		AMainGameGamemodeBase* GM = Cast<AMainGameGamemodeBase>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->HandleDressItemRequest(this, ItemID);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ServerDressItem: Cast to AMainGameGamemodeBase failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerDressItem: GetWorld() returned nullptr"));
	}
}

void AMainGamePlayercontrollerBase::PurchaseItem(int32 ItemID)
{
	ServerPurchaseItem(ItemID);
}

bool AMainGamePlayercontrollerBase::ServerPurchaseItem_Validate(int32 ItemID)
{
	return true;
}

void AMainGamePlayercontrollerBase::ServerPurchaseItem_Implementation(int32 ItemID)
{
	// Get the GameMode
	if (GetWorld())
	{
		AMainGameGamemodeBase* GM = Cast<AMainGameGamemodeBase>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->HandlePurchaseItemRequest(this, ItemID);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ServerPurchaseItem: Cast to AMainGameGamemodeBase failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerPurchaseItem: GetWorld() returned nullptr"));
	}
}

void AMainGamePlayercontrollerBase::CloseCatalog()
{
	if (bIsCatalogOpen && CatalogWidget)
	{
		CatalogWidget->RemoveFromViewport();
		UE_LOG(LogTemp, Log, TEXT("Catalogo Fechado"));

		bIsCatalogOpen = false;
		SetInputMode(FInputModeGameOnly()); // Voltar ao input do jogo
		bShowMouseCursor = false;
	}
}

void AMainGamePlayercontrollerBase::OpenCatalog()
{
	if (!bIsCatalogOpen)
	{
		if (CatalogWidgetClass)
		{
			if (!CatalogWidget)
			{
				// Cria o widget se ele ainda não existir
				CatalogWidget = CreateWidget<UUserWidget>(this, CatalogWidgetClass);
			}

			if (CatalogWidget)
			{
				CatalogWidget->AddToViewport();
				UE_LOG(LogTemp, Log, TEXT("Catalogo Aberto"));

				bIsCatalogOpen = true;
				SetInputMode(FInputModeUIOnly()); // Mudar para input UI
				bShowMouseCursor = true;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CatalogWidgetClass is not set in the PlayerController!"));
		}
	}
}

void AMainGamePlayercontrollerBase::UnDressItem(int32 ItemID)
{
	ServerUnDressItem(ItemID);
}

bool AMainGamePlayercontrollerBase::ServerUnDressItem_Validate(int32 ItemID)
{
	return true;
}

void AMainGamePlayercontrollerBase::ServerUnDressItem_Implementation(int32 ItemID)
{
	// Get the GameMode
	if (GetWorld())
	{
		AMainGameGamemodeBase* GM = Cast<AMainGameGamemodeBase>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->HandleUnDressItemRequest(this, ItemID);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ServerUnDressItem: Cast to AMainGameGamemodeBase failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerUnDressItem: GetWorld() returned nullptr"));
	}
}

void AMainGamePlayercontrollerBase::SendChatMessage(const FString& Message)
{
	ServerSendChatMessage(Message);
}

bool AMainGamePlayercontrollerBase::ServerSendChatMessage_Validate(const FString& Message)
{
	return !Message.IsEmpty();
}

void AMainGamePlayercontrollerBase::ServerSendChatMessage_Implementation(const FString& Message)
{
	// Get the GameMode
	if (GetWorld())
	{
		AMainGameGamemodeBase* GM = Cast<AMainGameGamemodeBase>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->HandleChatMessage(this, Message);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ServerSendChatMessage: Cast to AMainGameGamemodeBase failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerSendChatMessage: GetWorld() returned nullptr"));
	}
}

void AMainGamePlayercontrollerBase::ClientReceiveChatMessage_Implementation(const FString& Author, const FString& Message)
{
	// This runs on the client: display or process the received message
	UE_LOG(LogTemp, Log, TEXT("Client Received Chat Message: %s"), *Message);

	if (!ChatWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChatWidget is nullptr"));
		return;
	}

	// Call the Blueprint event
	UFunction* ReceiveChatMessageFunction = ChatWidget->FindFunction(FName("AddMessage"));
	if (ReceiveChatMessageFunction)
	{
		struct FReceiveChatMessageParams
		{
			FString Author;
			FString Content;
		};

		FReceiveChatMessageParams Params;
		Params.Author = Author;
		Params.Content = Message;

		ChatWidget->ProcessEvent(ReceiveChatMessageFunction, &Params);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ReceiveChatMessage function not found"));
	}
}