// #pragma once
//
// #include "CoreMinimal.h"
// #include "Kismet/BlueprintFunctionLibrary.h"
// #include "OLLamaStreamReader.generated.h"
//
//
// DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLLMChunkReceived, const FString&, Response);
//
// UCLASS()
//
// class THESISTESTING1_API UOLLamaStreamReader : public UBlueprintFunctionLibrary
// {
//
// 	GENERATED_BODY()
// public:
// 	static void GeneratePrompt(const FString& Prompt, FOnLLMChunkReceived OnChunkReceived);
//
// private:
// 	static size_t CurlWriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
// };
