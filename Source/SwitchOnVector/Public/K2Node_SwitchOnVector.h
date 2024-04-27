// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_Switch.h"
#include "K2Node_SwitchOnVector.generated.h"

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
 * 
 */
UCLASS()
class SWITCHONVECTOR_API UK2Node_SwitchOnVector : public UK2Node_Switch
{
	GENERATED_BODY()


public:
	UPROPERTY()
	TArray<FName> PinNames;

	//This is the array that will hold the actual vector values for the pins, when the array is changed we will update the pins
	UPROPERTY(EditAnywhere, Category = PinOptions)
	TArray<FVector> PinValues;

	//The tolerance value for the comparison, passing this to the comparison function is a little tricky, we effectively tack it on to the
	// case switch vector data in the form of a struct, this is done in the GetExportTextForPin function
	UPROPERTY(EditAnywhere, Category = PinOptions)
	float Tolerance = 0.1f;


	UK2Node_SwitchOnVector();

	//this method is used to register the node in the blueprint editor
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;


	UFUNCTION(BlueprintPure, Category = PinOptions, meta = (BlueprintInternalUseOnly = "TRUE"))
	static bool IsVectorWithToleranceNotNearlyEqual(FVector& A, FVectorAndTolerance& B);

	//as we want to display the details view for the node we need to override this function and return true
	virtual bool ShouldShowNodeProperties() const override { return true; }



	//K2Node_Switch Interface

	//you need to override this function to create the selection pin and override its data type, in this case we are using a vector
	//it is not mandatory for the values to be the same as the case pins, you just need a static function that can take both values and return a bool
	virtual void CreateSelectionPin() override;

	//I don't think this one is actually being used
	virtual FName GetPinNameGivenIndex(int32 Index) const override;
	virtual FEdGraphPinType GetInnerCaseType() const override;
	virtual FEdGraphPinType GetPinType() const override;

	//ultimately all the pins are just Exec pins, the K2Node_Switch iterates over all output exec pins and calls GetExportTextForPin, 
	// so the indices used to populate the pin in CreateCasePins should be consistent with the indices used in your GetExportTextForPin implementation
	// and values array
	virtual void CreateCasePins() override;
	virtual FName GetUniquePinName() override;

	//Generates the export text for the case pins, this is critical as the blueprint compiler uses this text to generate the structs that are used in the comparison function
	FString GetExportTextForPin(const UEdGraphPin* InPin) const override;

	// For the use case of vectors we don't need the add pin button, the user can just add new values in the array
	virtual bool SupportsAddPinButton() const { return false; }
	//End of K2Node_Switch Interface

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
};
