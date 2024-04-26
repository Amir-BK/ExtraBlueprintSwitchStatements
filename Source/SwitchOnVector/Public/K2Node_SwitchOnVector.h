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

	UPROPERTY()
	TArray<FName> PinNames;


	UPROPERTY(EditAnywhere, Category = PinOptions)
	TArray<FVector> PinValues;

public:

	UPROPERTY(EditAnywhere, Category = PinOptions)
	float Tolerance = 0.1f;

	UK2Node_SwitchOnVector();

	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual bool SupportsAddPinButton()  const override { return false; }

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	UFUNCTION(BlueprintCallable, Category = "SwitchOnVector")
	static bool IsVectorNearlyEqual(FVector& A, FVector& B);


	virtual bool ShouldShowNodeProperties() const override { return true; }

protected:
	virtual void CreateSelectionPin() override;

	virtual FName GetPinNameGivenIndex(int32 Index) const override;
	virtual FEdGraphPinType GetPinType() const override;

	virtual void CreateCasePins() override;
	virtual FName GetUniquePinName() override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
};
