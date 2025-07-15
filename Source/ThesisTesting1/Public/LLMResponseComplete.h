// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LLMResponseComplete.generated.h"

//DECLARE_DELEGATE_RetVal_OneParam(FString, FOnLLMResponse, FString);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLLMResponseComplete, const FString&, Response);


UCLASS()
class THESISTESTING1_API ULLMResponseComplete : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "PCG")
	static void ULLMCallerfunctionComplete(FString prompt, const FOnLLMResponseComplete OnResponse);
	
	//private:
	//void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

};
