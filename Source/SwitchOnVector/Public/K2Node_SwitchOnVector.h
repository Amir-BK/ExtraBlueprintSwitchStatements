// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_Switch.h"
#include "K2Node_SwitchOnVector.generated.h"

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

	UPROPERTY(EditAnywhere, Category = PinOptions)
	TArray<FVector> PinValues;



	UK2Node_SwitchOnVector();

	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	UFUNCTION(BlueprintCallable , Category = PinOptions, meta = (BlueprintInternalUseOnly = "TRUE"))
	static bool IsVectorNotNearlyEqual(FVector& A, FVector& B);


	virtual bool ShouldShowNodeProperties() const override { return true; }

	virtual void CreateSelectionPin() override;


	//K2Node_Switch Interface
	virtual FName GetPinNameGivenIndex(int32 Index) const override;
	virtual FEdGraphPinType GetPinType() const override;
	virtual void CreateCasePins() override;
	virtual FName GetUniquePinName() override;
	FString GetExportTextForPin(const UEdGraphPin* InPin) const override;
	virtual bool SupportsAddPinButton() const { return false; }


	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
};
