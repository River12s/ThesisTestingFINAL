 // Fill out your copyright notice in the Description page of Project Settings.
 
 #pragma once
 
 #include "Kismet/BlueprintFunctionLibrary.h"
 #include "PoissonDiskSamplingUsingMesh.generated.h"
 // #include "ProceduralMeshComponent.h"
 
 
 UCLASS()
 class THESISTESTING1_API UPoissonDiskSamplingUsingMesh : public UBlueprintFunctionLibrary
 {
     GENERATED_BODY()
 
 public:
     UFUNCTION(BlueprintCallable, Category = "PCG")
     static TArray<FVector> DoPoissonDiskSamplingUsingMesh(UPARAM(ref) TArray<FVector> &MeshVertices, UPARAM(ref) TArray<int32> &MeshTriangles, const float Radius = 67, const int32 MaxAttempts = 15);
     // static TArray<FVector> DoPoissonDiskSamplingUsingMesh(UPARAM(ref) TArray<FVector> &MeshVertices, const float Radius = 67, const int32 MaxAttempts = 15);
 
 private:
     static FVector GenerateRandomPointAroundMesh(const FVector& Point, float Radius, float MinDist, float MaxDist);
     static bool IsValidPoint(const FVector& Point, float Radius, const TArray<FVector>& Grid, int32 GridWidth, int32 GridHeight, int32 GridDepth, float CellSize);
     static bool PointInMesh(FVector& Point, const TArray<FVector>& Vertices, const TArray<int32>& Triangles);
     
 
 };
