// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ParameterLLMResponse.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FParameterLLMError, const FString&, Error);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FParameterLLMResponse, const bool, GenerateStr, const FString&, Size, const FString&, Type);

UCLASS()
class THESISTESTING1_API UParameterLLMResponse : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "PCG")
	static void UParameterLLMResponseFunction(FString prompt, const FParameterLLMResponse OnResponse, const FParameterLLMError Error);

};
