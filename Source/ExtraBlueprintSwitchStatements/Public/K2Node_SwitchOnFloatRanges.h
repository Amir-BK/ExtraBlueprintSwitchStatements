// Copyright Amir Ben-Kiki 2025

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_Switch.h"
#include "ExtraSwitchComparatorsFunctionLibrary.h"
#include "K2Node_SwitchOnFloatRanges.generated.h"

/**
 * 
 */
UCLASS()
class EXTRABLUEPRINTSWITCHSTATEMENTS_API UK2Node_SwitchOnFloatRanges : public UK2Node_Switch
{
	GENERATED_BODY()


public:
	UPROPERTY()
	TArray<FName> PinNames;

	//This is the array that will hold the actual vector values for the pins, when the array is changed we will update the pins
	UPROPERTY(EditAnywhere, Category = PinOptions)
	TArray<FExtraSwitchFloatRange> PinValues;

	UK2Node_SwitchOnFloatRanges();

	//this method is used to register the node in the blueprint editor
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	//as we want to display the details view for the node we need to override this function and return true
	virtual bool ShouldShowNodeProperties() const override { return true; }

	FText GetTooltipText() const override;

	//K2Node_Switch Interface
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

	virtual bool SupportsAddPinButton() const { return false; }
	//End of K2Node_Switch Interface

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;

};
