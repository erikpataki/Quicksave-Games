// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadQuestPictureToTexture.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "QuestDataAsset.h"
#include "QuestSubsystem.h"


ULoadQuestPictureToTexture* ULoadQuestPictureToTexture::LoadQuestPicture(const UObject* WorldContextObject,
                                                                         const UQuestDataAsset* Quest)
{
	ULoadQuestPictureToTexture* Node = NewObject<ULoadQuestPictureToTexture>();
	Node->WorldContext = WorldContextObject;
	Node->QuestToLoad = Quest;
	
	return Node;
}

void ULoadQuestPictureToTexture::Activate()
{
	if (!IsValid(WorldContext) || !IsValid(QuestToLoad) || !QuestToLoad->bHasPicture)
	{
		PictureNotFound.Broadcast();
		return;
	}
	
	// Begin to load image file from background thread.
	AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [this]()
	{
		const auto Filename = WorldContext->GetWorld()->GetSubsystem<UQuestSubsystem>()->GetPictureFilenameForQuest(QuestToLoad);
		
		IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));

		TArray64<uint8> Buffer;
		if (FFileHelper::LoadFileToArray(Buffer, *Filename))
		{
			EPixelFormat PixelFormat = PF_Unknown;

			uint8* RawData = nullptr;
			int32 BitDepth = 0;
			int32 Width = 0;
			int32 Height = 0;
			
			TSharedPtr<IImageWrapper> HdrImageWrapper =  ImageWrapperModule.CreateImageWrapper(EImageFormat::HDR);
	
			if(HdrImageWrapper->SetCompressed(Buffer.GetData(), Buffer.Num()))
			{
				PixelFormat = PF_FloatRGBA;
				Width = HdrImageWrapper->GetWidth();
				Height = HdrImageWrapper->GetHeight();

				TArray64<uint8> BGREImage;
				if (HdrImageWrapper->GetRaw(ERGBFormat::BGRE, 8, BGREImage))
				{
					// Texture needs be to created on the GameThread, otherwise it can race with GC.
					AsyncTask(ENamedThreads::GameThread, [this, BGREImage, PixelFormat, Width, Height]()
					{
						auto NewTexture = UTexture2D::CreateTransient(Width, Height, PixelFormat);
						if (NewTexture)
						{
							// Texture created, move back to a background thread since very computational
							// and causes freezing.
							AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [this, NewTexture, BGREImage]()
							{
								uint8* MipData = static_cast<uint8*>(
									NewTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

								TArrayView64<FColor> SourceColors((FColor*)(BGREImage.GetData()),
																  BGREImage.Num() / sizeof(FColor));

								// Bulk data was already allocated for the correct size when we called CreateTransient above
								TArrayView64<FFloat16> Destination(reinterpret_cast<FFloat16*>(MipData),
																   NewTexture->GetPlatformData()->Mips[0].BulkData.
																   GetBulkDataSize() / sizeof(FFloat16));

								int64 DestinationIndex = 0;
								for (const FColor& Color: SourceColors)
								{
									FLinearColor LinearColor = Color.FromRGBE();
									Destination[DestinationIndex++].Set(LinearColor.R);
									Destination[DestinationIndex++].Set(LinearColor.G);
									Destination[DestinationIndex++].Set(LinearColor.B);
									Destination[DestinationIndex++].Set(LinearColor.A);
								}

								NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

								// Go back to game thread to broadcast event.
								// Since UE5, UTexture::UpdateResource must be called on GameThread, otherwise
								// you get an error message and unable to package.
								AsyncTask(ENamedThreads::GameThread, [this, NewTexture]()
								{
									NewTexture->UpdateResource();
									PictureLoaded.Broadcast(NewTexture);
								});
							});
						}
						else
						{
							PictureNotFound.Broadcast();
						}
					});
				}
			}
		}
		else
		{
			// Go back to game thread to broadcast event.
			AsyncTask(ENamedThreads::GameThread, [this]()
			{
				PictureNotFound.Broadcast();
			});
		}
	});
}
