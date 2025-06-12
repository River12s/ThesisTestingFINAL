// Copyright (c) 2024 Marina Eremina
// This code is distributed under the MIT License.

#include "PoissonPoints.h"
#include "Math/UnrealMathUtility.h"

//#define LOCTEXT_NAMESPACE "FPoissonPointsModule"
//#define LOCTEXT_NAMESPACE "UPoissonPoints"


void FPoissonPointsModule::StartupModule()
{}

void FPoissonPointsModule::ShutdownModule()
{}

TArray<FVector> UPoissonPoints::GeneratePoissonPoints(float Width, float Height, float Depth, float Radius, int32 MaxAttempts)
{
    TArray<FVector> ActivePoints;
    TArray<FVector> Points;

    float CellSize = Radius / FMath::Sqrt(3.0f);
    int32 GridWidth = FMath::CeilToInt(Width / CellSize);
    int32 GridHeight = FMath::CeilToInt(Height / CellSize);
    int32 GridDepth = FMath::CeilToInt(Depth / CellSize);

    TArray<FVector> Grid;
    Grid.SetNumZeroed(GridWidth * GridHeight * GridDepth);

    auto GetCellIndex = [=](const FVector& Point) -> int32 {
        int32 X = FMath::FloorToInt(Point.X / CellSize);
        int32 Y = FMath::FloorToInt(Point.Y / CellSize);
        int32 Z = FMath::FloorToInt(Point.Z / CellSize);
        return Z * GridWidth * GridHeight + Y * GridWidth + X;
    };

    FVector InitialPoint(FMath::FRandRange(0.0f, Width), FMath::FRandRange(0.0f, Height), FMath::FRandRange(0.0f, Depth));
    ActivePoints.Add(InitialPoint);
    Points.Add(InitialPoint);
    Grid[GetCellIndex(InitialPoint)] = InitialPoint;

    while (ActivePoints.Num() > 0)
    {
        int32 Index = FMath::RandRange(0, ActivePoints.Num() - 1);
        FVector Point = ActivePoints[Index];
        bool bFound = false;

        for (int32 i = 0; i < MaxAttempts; i++)
        {
            FVector NewPoint = GenerateRandomPointAround(Point, Radius, Radius, 2 * Radius);
            if (IsValidPoint(NewPoint, Radius, Grid, GridWidth, GridHeight, GridDepth, CellSize))
            {
                ActivePoints.Add(NewPoint);
                Points.Add(NewPoint);
                Grid[GetCellIndex(NewPoint)] = NewPoint;
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            ActivePoints.RemoveAt(Index);
        }
    }

    return Points;
}


FVector UPoissonPoints::GenerateRandomPointAround(const FVector& Point, float Radius, float MinDist, float MaxDist)
{
    float Theta = FMath::FRandRange(0.0f, 2 * PI);
    float Phi = FMath::Acos(FMath::FRandRange(-1.0f, 1.0f));
    float Dist = FMath::FRandRange(MinDist, MaxDist);

    return Point + Dist * FVector(FMath::Sin(Phi) * FMath::Cos(Theta), FMath::Sin(Phi) * FMath::Sin(Theta), FMath::Cos(Phi));
}

//static float SphereSDF(const FVector& Point, const FVector& Center, float Radius)
//{
//    return (Point - Center).Size() - Radius;
//}
//
//// Example: Blobby union of three spheres
//static float BlobbySDF(const FVector& Point)
//{
//    float r = 75.0f;
//    float d1 = SphereSDF(Point, FVector(0, 0, 0), r);
//    float d2 = SphereSDF(Point, FVector(100, 0, 0), r);
//    float d3 = SphereSDF(Point, FVector(50, 100, 0), r);
//
//    // Union of SDFs = min of the SDF values
//    return FMath::Min3(d1, d2, d3);
//}


bool UPoissonPoints::IsValidPoint(
    const FVector& Point,
    float Radius,
    const TArray<FVector>& Grid,
    int32 GridWidth,
    int32 GridHeight,
    int32 GridDepth,
    float CellSize)
{
    
//    if (BlobbySDF(Point) > 0.0f)
//    {
//        return false; // Outside the shape
//    }
    
    if (Point.X < 0 || Point.X >= GridWidth * CellSize ||
        Point.Y < 0 || Point.Y >= GridHeight * CellSize ||
        Point.Z < 0 || Point.Z >= GridDepth * CellSize)
    {
        return false;
    }

    auto GetCellCoords = [CellSize](const FVector& P) -> FIntVector {
        return FIntVector(
            FMath::FloorToInt(P.X / CellSize),
            FMath::FloorToInt(P.Y / CellSize),
            FMath::FloorToInt(P.Z / CellSize)
        );
    };

    FIntVector Cell = GetCellCoords(Point);

    int32 StartX = FMath::Max(Cell.X - 2, 0);
    int32 StartY = FMath::Max(Cell.Y - 2, 0);
    int32 StartZ = FMath::Max(Cell.Z - 2, 0);
    int32 EndX = FMath::Min(Cell.X + 2, GridWidth - 1);
    int32 EndY = FMath::Min(Cell.Y + 2, GridHeight - 1);
    int32 EndZ = FMath::Min(Cell.Z + 2, GridDepth - 1);

    for (int32 x = StartX; x <= EndX; x++)
    {
        for (int32 y = StartY; y <= EndY; y++)
        {
            for (int32 z = StartZ; z <= EndZ; z++)
            {
                int32 Index = z * GridWidth * GridHeight + y * GridWidth + x;
                FVector Neighbor = Grid[Index];
                if (Neighbor != FVector::ZeroVector)
                {
                    float Dist = FVector::Dist(Point, Neighbor);
                    if (Dist < Radius)
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}


//#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPoissonPointsModule, PoissonPoints)
