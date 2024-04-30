// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_SwitchOnFloatRanges.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphNode.h"

#include "Internationalization/Internationalization.h"

#include "UObject/Class.h"

#include "UObject/UnrealType.h"



UK2Node_SwitchOnFloatRanges::UK2Node_SwitchOnFloatRanges()
{
	//Set the default function name and class, this is the function that will be called when the switch is executed, if it returns true the output pin will be executed
    FunctionName = TEXT("IsFloatNotWithinRange");
	FunctionClass = UK2Node_SwitchOnFloatRanges::StaticClass();

}

void UK2Node_SwitchOnFloatRanges::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    UClass* ActionKey = GetClass();
    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(NodeSpawner);

        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
    }
}

FText UK2Node_SwitchOnFloatRanges::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return INVTEXT("Switch on Float Range");

}

bool UK2Node_SwitchOnFloatRanges::IsFloatNotWithinRange(float& A, FSwitchFloatRange& B)
{
    TRange<float> Range = TRange<float>();
    Range = B.Inclusive ? Range.Inclusive(B.RangeMin, B.RangeMax) : Range.Exclusive(B.RangeMin, B.RangeMax);
 
    return !Range.Contains(A);
}






inline FText UK2Node_SwitchOnFloatRanges::GetTooltipText() const
{
    return INVTEXT("Switch statement using float ranges, \nany execution path that matches the equality condition will execute \nif none do then default will execute");
}



void UK2Node_SwitchOnFloatRanges::CreateSelectionPin()
{
    const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
    UEdGraphPin* Pin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Real, TEXT("Selection"));
    K2Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pin);

}



FName UK2Node_SwitchOnFloatRanges::GetPinNameGivenIndex(int32 Index) const
{
    check(Index);
    return PinNames[Index];
}

FEdGraphPinType UK2Node_SwitchOnFloatRanges::GetInnerCaseType() const
{
	FEdGraphPinType PinType;
	PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	PinType.PinSubCategoryObject = FSwitchFloatRange::StaticStruct();
	return PinType;
}

FEdGraphPinType UK2Node_SwitchOnFloatRanges::GetPinType() const
{
    FEdGraphPinType PinType;
    PinType.PinCategory = UEdGraphSchema_K2::PC_Real;
    return PinType;
}

void UK2Node_SwitchOnFloatRanges::CreateCasePins()
{
    while (PinValues.Num() > PinNames.Num())
    {
        const FName PinName = GetUniquePinName();
        PinNames.Add(PinName);
    }

    if (PinNames.Num() > PinValues.Num())
    {
        PinNames.SetNum(PinValues.Num());
    }

    for (int32 Index = 0; Index < PinValues.Num(); ++Index)
    {

         UEdGraphPin* Pin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinNames[Index]);

         if (PinValues[Index].Inclusive)
         {
			 Pin->PinFriendlyName = FText::FromString(FString::Printf(TEXT("[%.3f,%.3f]"), PinValues[Index].RangeMin, PinValues[Index].RangeMax));
		 }
         else
         {
             Pin->PinFriendlyName = FText::FromString(FString::Printf(TEXT("(%.3f,%.3f)"), PinValues[Index].RangeMin, PinValues[Index].RangeMax));
         }

         Pin->SourceIndex = Index;

    }


}



FName UK2Node_SwitchOnFloatRanges::GetUniquePinName()
{
    FName NewPinName;
    int32 Index = 0;
    while (true)
    {
        NewPinName = *FString::Printf(TEXT("Case_%d"), Index++);
        if (!FindPin(NewPinName))
        {
            break;
        }
    }
    return NewPinName;
}

FString UK2Node_SwitchOnFloatRanges::GetExportTextForPin(const UEdGraphPin* InPin) const
{
	
	return FString::Printf(TEXT("(RangeMax=%f,RangeMin=%f, Inclusive=%s)"), PinValues[InPin->SourceIndex].RangeMax, PinValues[InPin->SourceIndex].RangeMin,
        PinValues[InPin->SourceIndex].Inclusive ? TEXT("true") : TEXT("false"));
}



void UK2Node_SwitchOnFloatRanges::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    bool bIsDirty = false;
    FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    if (PropertyName == TEXT("PinValues"))
    {
        bIsDirty = true;
    }

    if (PropertyName == TEXT("RangeMax"))
    {
        bIsDirty = true;
    }

    if (PropertyName == TEXT("RangeMin"))
    {
        bIsDirty = true;
    }

    if (PropertyName == TEXT("Inclusive"))
    {
        bIsDirty = true;
    }

    if (bIsDirty)
    {
         ReconstructNode();
    }
    Super::PostEditChangeProperty(PropertyChangedEvent);
    GetGraph()->NotifyNodeChanged(this);

}

void UK2Node_SwitchOnFloatRanges::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
    Super::ValidateNodeDuringCompilation(MessageLog);

    TArray<int> InvalidRangePinIndex;
    for (int32 Index = 0; Index < PinValues.Num(); ++Index)
    {
        TRange<float> Range = TRange<float>();
        Range = PinValues[Index].Inclusive ? Range.Inclusive(PinValues[Index].RangeMin, PinValues[Index].RangeMax) : Range.Exclusive(PinValues[Index].RangeMin, PinValues[Index].RangeMax);

        if (Range.IsEmpty())
        {
            InvalidRangePinIndex.Add(Index);

        }
    }

    if (InvalidRangePinIndex.Num() > 0)
    {
        FString WarningPins = TEXT("");
        for (int32 Index = 0; Index < InvalidRangePinIndex.Num(); ++Index)
        {
            WarningPins += FString::FromInt(InvalidRangePinIndex[Index]);
            if (Index < InvalidRangePinIndex.Num() - 1)
            {
                WarningPins += TEXT(", ");
            }
        }
        MessageLog.Warning(*FText::Format(INVTEXT("@@ : Empty range detected in pin(s): {0}"), FText::FromString(WarningPins)).ToString(), this);

    }
}


