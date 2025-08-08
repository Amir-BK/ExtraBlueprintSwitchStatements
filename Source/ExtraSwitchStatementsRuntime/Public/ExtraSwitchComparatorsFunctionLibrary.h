// Copyright Amir Ben-Kiki 2025

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExtraSwitchComparatorsFunctionLibrary.generated.h"


/**
 * This struct is used to pass additional data to the comparison function, the comparison function is static and takes two arguments, the first is the vector to compare
 */
USTRUCT(BlueprintType)
struct FVectorAndTolerance
{
	GENERATED_BODY()

	UPROPERTY()
	float X = 0.0f;

	UPROPERTY()
	float Y = 0.0f;

	UPROPERTY()
	float Z = 0.f;

	UPROPERTY(EditAnywhere, Category = PinOptions)
	float Tolerance = 0.1f;
};

/**
 * This struct is used to pass additional data to the comparison function for color comparisons
 */
USTRUCT(BlueprintType)
struct EXTRASWITCHSTATEMENTSRUNTIME_API FColorAndTolerance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switch Comparisons")
	FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switch Comparisons")
	float Tolerance = 0.1f;
};

/**
 * This struct is used to pass additional data to the comparison function for float comparisons
 */
USTRUCT(BlueprintType)
struct EXTRASWITCHSTATEMENTSRUNTIME_API FFloatAndTolerance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switch Comparisons")
	float Value = 0.0f;  // Changed from 'value' to 'Value' to follow UE property naming convention

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switch Comparisons")
	float Tolerance = 0.1f;
};

/**
 * This struct is used to define a float range for comparisons
 */
USTRUCT(BlueprintType)
struct EXTRASWITCHSTATEMENTSRUNTIME_API FExtraSwitchFloatRange
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switch Comparisons")
	float RangeMax = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switch Comparisons")
	float RangeMin = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Switch Comparisons")
	bool Inclusive = true;
};

/**
 * Blueprint Function Library containing the comparator functions used by the ExtraBlueprintSwitchStatements plugin
 */
UCLASS()
class EXTRASWITCHSTATEMENTSRUNTIME_API UExtraSwitchComparatorsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	 * Compares a color with a color and tolerance value, considering Alpha
	 * @param A - The color to compare
	 * @param B - The target color and tolerance
	 * @return True if colors are NOT nearly equal (exceeds tolerance)
	 */
	UFUNCTION(BlueprintPure, Category = "Switch Comparisons")
	static bool IsColorWithToleranceNotNearlyEqual(const FLinearColor& A, const FColorAndTolerance& B);

	/**
	 * Compares a color with a color and tolerance value, ignoring Alpha
	 * @param A - The color to compare
	 * @param B - The target color and tolerance
	 * @return True if colors are NOT nearly equal (exceeds tolerance)
	 */
	UFUNCTION(BlueprintPure, Category = "Switch Comparisons")
	static bool IsColorWithToleranceNotNearlyEqualNoAlpha(const FLinearColor& A, const FColorAndTolerance& B);

	/**
	 * Compares a float with a float and tolerance value
	 * @param A - The float to compare
	 * @param B - The target float and tolerance
	 * @return True if floats are NOT nearly equal (exceeds tolerance)
	 */
	UFUNCTION(BlueprintPure, Category = "Switch Comparisons")
	static bool IsFloatWithToleranceNotNearlyEqual(float A, const FFloatAndTolerance& B);

	/**
	 * Checks if a float is NOT within a specified range
	 * @param A - The float to check
	 * @param B - The range to check against
	 * @return True if the float is NOT within the range
	 */
	UFUNCTION(BlueprintPure, Category = "Switch Comparisons")
	static bool IsFloatNotWithinRange(float A, const FExtraSwitchFloatRange& B);
	
	/**
	 * Compares a vector with a vector and tolerance structure
	 * This overload is specifically for compatibility with the K2Node_SwitchOnVector node
	 * @param A - The vector to compare
	 * @param B - The target vector and tolerance structure
	 * @return True if vectors are NOT nearly equal (exceeds tolerance)
	 */
	UFUNCTION(BlueprintPure, Category = "Switch Comparisons")
	static bool IsVectorWithToleranceNotNearlyEqual(const FVector& A, const FVectorAndTolerance& B);

	/**
	 * Checks if an integer is NOT within a specified range
	 * @param A - The integer to check
	 * @param RangeMin - The minimum value of the range
	 * @param RangeMax - The maximum value of the range
	 * @param Inclusive - Whether the range is inclusive or exclusive
	 * @return True if the integer is NOT within the range
	 */
	UFUNCTION(BlueprintPure, Category = "Switch Comparisons")
	static bool IsIntNotWithinRange(int32 A, int32 RangeMin, int32 RangeMax, bool Inclusive);

	/** 
	 * Simple log function for debugging in Blueprint
	 * @param Message - The message to log
	 * @param bPrintToScreen - Whether to display the message on screen
	 */
	UFUNCTION(BlueprintCallable, Category = "Switch Comparisons|Debug")
	static void DebugLog(const FString& Message, bool bPrintToScreen = true);

	/**
	 * Test float comparison with direct values
	 */
	UFUNCTION(BlueprintCallable, Category = "Switch Comparisons|Debug")
	static bool TestFloatComparison(float A, float B, float Tolerance = 0.1f);
	
	/**
	 * Test color comparison with direct values
	 */
	UFUNCTION(BlueprintCallable, Category = "Switch Comparisons|Debug")
	static bool TestColorComparison(FLinearColor A, FLinearColor B, float Tolerance = 0.1f, bool bUseAlpha = true);
    
    /**
	 * Logs the current value of a float variable to help debug switch nodes
	 * @param Value - The float value to log
	 * @param VariableName - Optional name to identify the variable in logs
	 */
	UFUNCTION(BlueprintCallable, Category = "Switch Comparisons|Debug")
 static void DebugFloatValue(float Value, FString VariableName = TEXT("Unknown"));

    /**
	 * Logs the current value of a color variable to help debug switch nodes
	 * @param Value - The color value to log
	 * @param VariableName - Optional name to identify the variable in logs
	 */
	UFUNCTION(BlueprintCallable, Category = "Switch Comparisons|Debug")
	static void DebugColorValue(FLinearColor Value, FString VariableName = TEXT("Unknown"));


	// Use int32 for both parameters, matching SwitchOnInt
	UFUNCTION(BlueprintPure, Category = PinOptions, meta = (BlueprintInternalUseOnly = "TRUE"))
	static bool AreIntsNotEqual(int32 A, int32 B);

};