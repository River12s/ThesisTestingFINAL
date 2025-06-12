// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PoissonDiskSampling.generated.h"


UCLASS()
class THESISTESTING1_API UPoissonDiskSampling : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    
    UFUNCTION(BlueprintCallable, Category = "PCG")
    //not sure
    static TArray<FVector> DoPoissonDiskSampling(float Width = 1000, float Height = 1000, float Depth = 1000, float Radius = 50, int32 MaxAttempts = 15);

private:
    static FVector GenerateRandomPointAround(const FVector& Point, float Radius, float MinDist, float MaxDist);
    static bool IsValidPoint(const FVector& Point, float Radius, const TArray<FVector>& Grid, int32 GridWidth, int32 GridHeight, int32 GridDepth, float CellSize);
};
