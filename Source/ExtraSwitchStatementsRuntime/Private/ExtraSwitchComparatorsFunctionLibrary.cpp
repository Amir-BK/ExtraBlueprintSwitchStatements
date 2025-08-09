// Copyright Amir Ben-Kiki 2025

#include "ExtraSwitchComparatorsFunctionLibrary.h"
#include "Engine/Engine.h"



bool UExtraSwitchComparatorsFunctionLibrary::AreIntsNotEqual(int32 A, int32 B)
{
    // Return TRUE to SKIP this path (when values don't match)
    // Return FALSE to EXECUTE this path (when values match)
    bool Result = (A != B);


    return Result;
}

bool UExtraSwitchComparatorsFunctionLibrary::IsColorWithToleranceNotNearlyEqual(const FLinearColor& A, const FColorAndTolerance& B)
{
    float temp = FLinearColor::Dist(A, B.Color);
    bool result = temp > B.Tolerance;


    return result;
}

bool UExtraSwitchComparatorsFunctionLibrary::IsColorWithToleranceNotNearlyEqualNoAlpha(const FLinearColor& A, const FColorAndTolerance& B)
{
    FLinearColor AConverted = A.HSVToLinearRGB();
    FLinearColor BConverted = B.Color.HSVToLinearRGB();
    float temp = FLinearColor::Dist(AConverted, BConverted);

    return temp > B.Tolerance;
}

bool UExtraSwitchComparatorsFunctionLibrary::IsFloatWithToleranceNotNearlyEqual(float A, const FFloatAndTolerance& B)
{
    // Check if we're receiving the expected value
    bool result = !FMath::IsNearlyEqual(A, B.Value, B.Tolerance);

    return result;
}

bool UExtraSwitchComparatorsFunctionLibrary::IsFloatNotWithinRange(float A, const FExtraSwitchFloatRange& B)
{

    // Validate range bounds
    if (B.RangeMin > B.RangeMax)
    {
        return true;  // Skip this pin if range is invalid
    }
    
    // Create range object
    TRange<float> Range = TRange<float>();
    Range = B.Inclusive ? Range.Inclusive(B.RangeMin, B.RangeMax) : Range.Exclusive(B.RangeMin, B.RangeMax);
    
    bool bContains = Range.Contains(A);
    bool bNotWithinRange = !bContains;  // UK2Node_Switch skips pins that return true, so we want true when NOT in range
        
    return bNotWithinRange;  // Return true when value is NOT within range (so UK2Node_Switch skips this pin)
}

bool UExtraSwitchComparatorsFunctionLibrary::IsVectorWithToleranceNotNearlyEqual(const FVector& A, const FVectorAndTolerance& B)
{
    FVector BVector(B.X, B.Y, B.Z);
    float Distance = FVector::Dist(A, BVector);
    bool Result = Distance > B.Tolerance;


    return Result;
}

bool UExtraSwitchComparatorsFunctionLibrary::IsIntNotWithinRange(int& A, FSwitchIntRange& B)
{
    // Create range object
    TRange<int32> Range = TRange<int32>();
    Range = B.Inclusive ? Range.Inclusive(B.RangeMin, B.RangeMax) : Range.Exclusive(B.RangeMin, B.RangeMax);

    bool bContains = Range.Contains(A);
    bool bNotWithinRange = !bContains;  // UK2Node_Switch skips pins that return true, so we want true when NOT in range


    return bNotWithinRange;

}
