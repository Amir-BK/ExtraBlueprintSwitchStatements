// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_Switch.h"
#include "InstancedStruct.h"

#include "K2Node_SwitchInstancedStructType.generated.h"

class FBlueprintActionDatabaseRegistrar;

/**
 * 
 */
UCLASS()
class EXTRABLUEPRINTSWITCHSTATEMENTS_API UK2Node_SwitchInstancedStructType : public UK2Node_Switch
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY()
	TArray<FName> PinNames;


	UPROPERTY()
	TArray<FName> ValuePinNames;

	//This is the array that will hold the actual vector types for the pins, when the array is changed we will update the pins
	UPROPERTY(EditAnywhere, Category = PinOptions)
	TArray<UScriptStruct*> PinStructs;

	UFUNCTION()
	static bool NotEqual_StructType(UScriptStruct* A, UScriptStruct* B)
	{
		return A != B;
	}

	UFUNCTION(BlueprintPure, Category = "Utility")
	static const UScriptStruct* GetStructTypeFromInstancedStruct(FInstancedStruct& InstancedStruct);

	// Returns the value pin associated with the given case index
	UEdGraphPin* GetValuePin(int32 CaseIndex) const;

	// UObject interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;
	// End of UObject interface

	// UEdGraphNode interface
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool ShouldShowNodeProperties() const override { return true; }
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	// End of UK2Node interface

	virtual void AddPinToSwitchNode() override;
	virtual FName GetUniquePinName() override;
	virtual FEdGraphPinType GetPinType() const override;
	virtual FEdGraphPinType GetInnerCaseType() const override;

	virtual FEdGraphPinType GetInnerCaseTypeForCaseIndex(int32 Index) const;

	virtual FName GetPinNameGivenIndex(int32 Index) const override;

protected:
	virtual void CreateFunctionPin() override;
	virtual void CreateSelectionPin() override;
	virtual void CreateCasePins() override;
	virtual void RemovePin(UEdGraphPin* TargetPin) override;
	
};
