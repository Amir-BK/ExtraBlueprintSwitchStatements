// Copyright Amir Ben-Kiki 2025

#include "ExtraSwitchComparatorsFunctionLibrary.h"
#include "Engine/Engine.h"

// Simple log category definition
DEFINE_LOG_CATEGORY_STATIC(LogExtraSwitch, Log, All);

bool UExtraSwitchComparatorsFunctionLibrary::IsColorWithToleranceNotNearlyEqual(const FLinearColor& A, const FColorAndTolerance& B)
{
    float temp = FLinearColor::Dist(A, B.Color);
    bool result = temp > B.Tolerance;

    // Enhanced debug logging with more details about pointer addresses
    UE_LOG(LogExtraSwitch, Warning, TEXT("IsColorWithToleranceNotNearlyEqual CALLED - A(%f,%f,%f,%f), B(%f,%f,%f,%f), Tolerance=%f, Distance=%f, Result=%s"), 
        A.R, A.G, A.B, A.A, B.Color.R, B.Color.G, B.Color.B, B.Color.A, B.Tolerance, temp, result ? TEXT("true") : TEXT("false"));

    // Display on screen if in editor or debug build
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, 
            FString::Printf(TEXT("ColorCompare: A(%f,%f,%f,%f), Dist=%f, Result=%s"),
                A.R, A.G, A.B, A.A, temp, result ? TEXT("NOT EQUAL") : TEXT("EQUAL")));
    }

    return result;
}

bool UExtraSwitchComparatorsFunctionLibrary::IsColorWithToleranceNotNearlyEqualNoAlpha(const FLinearColor& A, const FColorAndTolerance& B)
{
    FLinearColor AConverted = A.HSVToLinearRGB();
    FLinearColor BConverted = B.Color.HSVToLinearRGB();
    float temp = FLinearColor::Dist(AConverted, BConverted);
    bool result = temp > B.Tolerance;

    // Enhanced debug logging with more details
    UE_LOG(LogExtraSwitch, Warning, TEXT("IsColorWithToleranceNotNearlyEqualNoAlpha CALLED - A(%f,%f,%f,%f), B(%f,%f,%f,%f), Tolerance=%f, Distance=%f, Result=%s"),
        A.R, A.G, A.B, A.A, B.Color.R, B.Color.G, B.Color.B, B.Color.A, B.Tolerance, temp, result ? TEXT("true") : TEXT("false"));

    // Display on screen if in editor or debug build
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, 
            FString::Printf(TEXT("ColorCompareNoAlpha: A(%f,%f,%f,%f), Dist=%f, Result=%s"),
                A.R, A.G, A.B, A.A, temp, result ? TEXT("NOT EQUAL") : TEXT("EQUAL")));
    }

    return result;
}

bool UExtraSwitchComparatorsFunctionLibrary::IsFloatWithToleranceNotNearlyEqual(float A, const FFloatAndTolerance& B)
{
    // Check if we're receiving the expected value
    bool result = !FMath::IsNearlyEqual(A, B.Value, B.Tolerance);

    // Enhanced debug logging - this is the critical one for debugging
    UE_LOG(LogExtraSwitch, Warning, TEXT("IsFloatWithToleranceNotNearlyEqual CALLED - A=%f, B.Value=%f, B.Tolerance=%f, |A-B|=%f, Result=%s"), 
        A, B.Value, B.Tolerance, FMath::Abs(A - B.Value), result ? TEXT("true") : TEXT("false"));

    // Display on screen if in editor or debug build
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, 
            FString::Printf(TEXT("FloatCompare: A=%f, B.Value=%f, Diff=%f, Result=%s"),
                A, B.Value, FMath::Abs(A - B.Value), 
                result ? TEXT("NOT EQUAL") : TEXT("EQUAL")));
    }

    return result;
}

bool UExtraSwitchComparatorsFunctionLibrary::IsFloatNotWithinRange(float A, const FExtraSwitchFloatRange& B)
{
    // Enhanced debug log for tracing input value
    UE_LOG(LogExtraSwitch, Warning, TEXT("IsFloatNotWithinRange CALLED - A=%f, Range=(%f,%f), Inclusive=%s"), 
        A, B.RangeMin, B.RangeMax, B.Inclusive ? TEXT("true") : TEXT("false"));

    // Validate range bounds
    if (B.RangeMin > B.RangeMax)
    {
        UE_LOG(LogExtraSwitch, Warning, TEXT("  - INVALID RANGE: RangeMin(%f) > RangeMax(%f)"), B.RangeMin, B.RangeMax);
        return true;  // Skip this pin if range is invalid
    }
    
    // Create range object
    TRange<float> Range = TRange<float>();
    Range = B.Inclusive ? Range.Inclusive(B.RangeMin, B.RangeMax) : Range.Exclusive(B.RangeMin, B.RangeMax);
    
    bool bContains = Range.Contains(A);
    bool bNotWithinRange = !bContains;  // UK2Node_Switch skips pins that return true, so we want true when NOT in range
    
    FString rangeType = B.Inclusive ? TEXT("[inclusive]") : TEXT("(exclusive)");
    UE_LOG(LogExtraSwitch, Warning, TEXT("  - Range%s=(%f,%f), FloatValue=%f, Contains=%s, Result=%s"),
        *rangeType, B.RangeMin, B.RangeMax, A, 
        bContains ? TEXT("true") : TEXT("false"), 
        bNotWithinRange ? TEXT("true") : TEXT("false"));
    
    // Display on screen if in editor or debug build
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, 
            FString::Printf(TEXT("RangeCheck: Value=%f, Range=(%f,%f), Result=%s"),
                A, B.RangeMin, B.RangeMax, 
                bNotWithinRange ? TEXT("NOT IN RANGE") : TEXT("IN RANGE")));
    }
    
    return bNotWithinRange;  // Return true when value is NOT within range (so UK2Node_Switch skips this pin)
}

void UExtraSwitchComparatorsFunctionLibrary::DebugLog(const FString& Message, bool bPrintToScreen)
{
    UE_LOG(LogExtraSwitch, Display, TEXT("%s"), *Message);
    
    if (bPrintToScreen && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
    }
}

bool UExtraSwitchComparatorsFunctionLibrary::TestFloatComparison(float A, float B, float Tolerance)
{
    bool bIsNearlyEqual = FMath::IsNearlyEqual(A, B, Tolerance);
    
    UE_LOG(LogExtraSwitch, Display, TEXT("TestFloatComparison: A=%f, B=%f, Tolerance=%f, |A-B|=%f, IsNearlyEqual=%s"),
        A, B, Tolerance, FMath::Abs(A - B), bIsNearlyEqual ? TEXT("true") : TEXT("false"));
    
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, 
            bIsNearlyEqual ? FColor::Green : FColor::Red, 
            FString::Printf(TEXT("Float Test: A=%f, B=%f, Tol=%f, Equal=%s"), 
                A, B, Tolerance, bIsNearlyEqual ? TEXT("true") : TEXT("false")));
    }
    
    return bIsNearlyEqual;
}

bool UExtraSwitchComparatorsFunctionLibrary::TestColorComparison(FLinearColor A, FLinearColor B, float Tolerance, bool bUseAlpha)
{
    float Distance;
    bool bIsWithinTolerance;
    
    if (bUseAlpha)
    {
        Distance = FLinearColor::Dist(A, B);
        bIsWithinTolerance = Distance <= Tolerance;
    }
    else
    {
        FLinearColor AConverted = A.HSVToLinearRGB();
        FLinearColor BConverted = B.HSVToLinearRGB();
        Distance = FLinearColor::Dist(AConverted, BConverted);
        bIsWithinTolerance = Distance <= Tolerance;
    }
    
    UE_LOG(LogExtraSwitch, Display, TEXT("TestColorComparison: A(%f,%f,%f,%f), B(%f,%f,%f,%f), UseAlpha=%s, Tolerance=%f, Distance=%f, Result=%s"),
        A.R, A.G, A.B, A.A, 
        B.R, B.G, B.B, B.A,
        bUseAlpha ? TEXT("true") : TEXT("false"),
        Tolerance, Distance,
        bIsWithinTolerance ? TEXT("true") : TEXT("false"));
    
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, 
            bIsWithinTolerance ? FColor::Green : FColor::Red, 
            FString::Printf(TEXT("Color Test: Distance=%f, Tol=%f, Within=%s"), 
                Distance, Tolerance, bIsWithinTolerance ? TEXT("true") : TEXT("false")));
    }
    
    return bIsWithinTolerance;
}

void UExtraSwitchComparatorsFunctionLibrary::DebugFloatValue(float Value, FString VariableName)
{
    // Log to console
    UE_LOG(LogExtraSwitch, Warning, TEXT("DEBUG FLOAT: %s = %f (memory address: %p)"), 
        *VariableName, Value, &Value);
    
    // Also show on screen for easier debugging during gameplay
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, 
            FString::Printf(TEXT("Variable [%s] = %f"), *VariableName, Value));
    }
}

void UExtraSwitchComparatorsFunctionLibrary::DebugColorValue(FLinearColor Value, FString VariableName)
{
    // Log to console
    UE_LOG(LogExtraSwitch, Warning, TEXT("DEBUG COLOR: %s = (%f, %f, %f, %f)"), 
        *VariableName, Value.R, Value.G, Value.B, Value.A);
    
    // Also show on screen for easier debugging during gameplay
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, Value.ToFColor(true), 
            FString::Printf(TEXT("Variable [%s] = (%f, %f, %f, %f)"), 
                *VariableName, Value.R, Value.G, Value.B, Value.A));
    }
}