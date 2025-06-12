// Fill out your copyright notice in the Description page of Project Settings.

#include "PoissonDiskSamplingUsingMesh.h"
#include "Math/UnrealMathUtility.h"


// //Does the poisson disk sampling (main function)
TArray<FVector> UPoissonDiskSamplingUsingMesh::DoPoissonDiskSamplingUsingMesh(TArray<FVector> &MeshVertices, TArray<int32> & MeshTriangles, float Radius, int32 MaxAttempts)
// TArray<FVector> UPoissonDiskSamplingUsingMesh::DoPoissonDiskSamplingUsingMesh(TArray<FVector> &MeshVertices, float Radius, int32 MaxAttempts)
{ 
    TArray<FVector> ActivePoints;
    TArray<FVector> Points;
	TArray<FVector> FinalPoints;
    
    FBox BoundingBox(MeshVertices);
        FVector Min = BoundingBox.Min;
        FVector Max = BoundingBox.Max;
        FVector Size = Max - Min;

    DEFINE_LOG_CATEGORY_STATIC(LogMeshBounds, Log, All);

    
    float Width = Max.X - Min.X;
	float Height = Max.Y - Min.Y;
	float Depth = Max.Z - Min.Z;
	UE_LOG(LogTemp, Warning, TEXT("Width: %f, Height: %f, Depth: %f"), Width, Height, Depth);

    
    float CellSize = Radius / FMath::Sqrt(3.0f);
    int32 GridWidth = FMath::CeilToInt(Size.X / CellSize);
    int32 GridHeight = FMath::CeilToInt(Size.Y / CellSize);
    int32 GridDepth = FMath::CeilToInt(Size.Z / CellSize);
    
    TArray<FVector> Grid;
    Grid.SetNumZeroed(GridWidth * GridHeight * GridDepth);
    
    auto GetCellIndex = [=](const FVector& Point) -> int32 {
        int32 X = FMath::FloorToInt(Point.X / CellSize);
        int32 Y = FMath::FloorToInt(Point.Y / CellSize);
        int32 Z = FMath::FloorToInt(Point.Z / CellSize);
        return Z * GridWidth * GridHeight + Y * GridWidth + X;
    };
    
    // FVector InitialPoint(FMath::FRandRange(0.0f, Height), FMath::FRandRange(0.0f, Height), FMath::FRandRange(0.0f, Depth));
    // ActivePoints.Add(InitialPoint);
    // Points.Add(InitialPoint);
    // Grid[GetCellIndex(InitialPoint)] = InitialPoint;

    FVector InitialPoint (
           FMath::FRandRange(Min.X, Max.X),
           FMath::FRandRange(Min.Y, Max.Y),
           FMath::FRandRange(Min.Z, Max.Z)
       );

    UE_LOG(LogTemp, Warning, TEXT("InitialPoint: %f, %f, %f"), InitialPoint.X, InitialPoint.Y, InitialPoint.Z);

    
     ActivePoints.Add(InitialPoint);
     Points.Add(InitialPoint);
     Grid[GetCellIndex(InitialPoint)] = InitialPoint;
    
	int32 LoopCounter = 0;
    while (ActivePoints.Num() > 0)	
    {

		// if (++LoopCounter > 1000000)
  //   {
  //       UE_LOG(LogTemp, Error, TEXT("Breaking out of loop to prevent freeze"));
  //       break;
  //   }
        int32 Index = FMath::RandRange(0, ActivePoints.Num() - 1);
        FVector Point = ActivePoints[Index];
        bool bFound = false;
        
        for (int32 i = 0; i < MaxAttempts; i++)
        {
            FVector NewPoint = GenerateRandomPointAroundMesh(Point, Radius, Radius, 2 * Radius);
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

	
	for (FVector& Point : Points)
	{
		UE_LOG(LogTemp, Warning, TEXT("Point: %s"), *Point.ToString());

		if (PointInMesh(Point, MeshVertices, MeshTriangles))
		{
			FinalPoints.Add(Point);
		}
	}
    
    
     return FinalPoints;
}


//Generates a random point
FVector UPoissonDiskSamplingUsingMesh::GenerateRandomPointAroundMesh(const FVector& Point, float Radius, float MinDist, float MaxDist)
{
    float Theta = FMath::FRandRange(0.0f, 2 * PI);
    float Phi = FMath::Acos(FMath::FRandRange(-1.0f, 1.0f));
    float Dist = FMath::FRandRange(MinDist, MaxDist);
    
    return Point + Dist * FVector(FMath::Sin(Phi) * FMath::Cos(Theta), FMath::Sin(Phi) * FMath::Sin(Theta), FMath::Cos(Phi));
}

bool UPoissonDiskSamplingUsingMesh::IsValidPoint(
										const FVector& Point,
										float Radius,
										const TArray<FVector>& Grid,
										int32 GridWidth,
										int32 GridHeight,
										int32 GridDepth,
										float CellSize)
{

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

bool UPoissonDiskSamplingUsingMesh::PointInMesh(FVector& Point, const TArray<FVector>& Vertices, const TArray<int32>& Triangles)
{
	// Ray origin and direction
	FVector RayOrigin = Point;
	FVector RayDirection = FVector(1, 0, 0); // Arbitrary direction (e.g., +X)

	int32 IntersectionCount = 0;

	// Loop through each triangle in the mesh
	for (int32 i = 0; i < Triangles.Num(); i += 3)
	{
		const FVector& V0 = Vertices[Triangles[i]];
		const FVector& V1 = Vertices[Triangles[i + 1]];
		const FVector& V2 = Vertices[Triangles[i + 2]];

		FVector Edge1 = V1 - V0;
		FVector Edge2 = V2 - V0;
		FVector H = FVector::CrossProduct(RayDirection, Edge2);
		float A = FVector::DotProduct(Edge1, H);

		if (FMath::Abs(A) < KINDA_SMALL_NUMBER)
			continue; // Ray is parallel to triangle

		float F = 1.0f / A;
		FVector S = RayOrigin - V0;
		float U = F * FVector::DotProduct(S, H);

		if (U < 0.0f || U > 1.0f)
			continue;

		FVector Q = FVector::CrossProduct(S, Edge1);
		float V = F * FVector::DotProduct(RayDirection, Q);

		if (V < 0.0f || U + V > 1.0f)
			continue;

		float T = F * FVector::DotProduct(Edge2, Q);

		if (T > 0.0001f) // Intersection occurs in positive ray direction
		{
			IntersectionCount++;
		}
	}

	// Point is inside if ray hits an odd number of triangles
	return (IntersectionCount % 2) == 1;
}
