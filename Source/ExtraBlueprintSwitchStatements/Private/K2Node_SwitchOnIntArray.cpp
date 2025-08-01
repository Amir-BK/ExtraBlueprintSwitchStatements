// Copyright Amir Ben-Kiki 2025

// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_SwitchOnIntArray.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphNode.h"

#include "Internationalization/Internationalization.h"

#include "UObject/Class.h"

#include "UObject/UnrealType.h"



bool UK2Node_SwitchOnIntArray::AreIntsNotEqual(int& A, int& B)
{
    return A == B;
}

UK2Node_SwitchOnIntArray::UK2Node_SwitchOnIntArray()
{
	//Set the default function name and class, this is the function that will be called when the switch is executed, if it returns true the output pin will be executed
    FunctionName = TEXT("AreIntsNotEqual");
	FunctionClass = UK2Node_SwitchOnIntArray::StaticClass();

}

void UK2Node_SwitchOnIntArray::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    UClass* ActionKey = GetClass();
    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(NodeSpawner);

        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
    }
}

FText UK2Node_SwitchOnIntArray::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return INVTEXT("Switch on Int Array");

}






inline FText UK2Node_SwitchOnIntArray::GetTooltipText() const
{
    return INVTEXT("Switch statement using an array of ints,\nunlike the standard switch that only works for consecutive ints ");
}



void UK2Node_SwitchOnIntArray::CreateSelectionPin()
{

    const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
    UEdGraphPin* Pin = CreatePin(EGPD_Input, GetInnerCaseType().PinCategory, TEXT("Selection"));
    K2Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pin);

}



FName UK2Node_SwitchOnIntArray::GetPinNameGivenIndex(int32 Index) const
{
    check(Index);
    return PinNames[Index];
}

FEdGraphPinType UK2Node_SwitchOnIntArray::GetInnerCaseType() const
{
	FEdGraphPinType PinType;
	PinType.PinCategory = UEdGraphSchema_K2::PC_Int;
	return PinType;
}

FEdGraphPinType UK2Node_SwitchOnIntArray::GetPinType() const
{
    FEdGraphPinType PinType;
    PinType.PinCategory = UEdGraphSchema_K2::PC_Int;
    return PinType;
}

void UK2Node_SwitchOnIntArray::CreateCasePins()
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

        //PinNames[Index] = PinNames[Index];

        // UEdGraphPin* Test = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, TBaseStructure<FVector>::Get(), PinNames[Index]);
         UEdGraphPin* Pin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinNames[Index]);
         Pin->bAllowFriendlyName = false;
         Pin->PinFriendlyName = FText::FromString(FString::Printf(TEXT("%d"), PinValues[Index]));
         Pin->SourceIndex = Index;

    }


}



FName UK2Node_SwitchOnIntArray::GetUniquePinName()
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

FString UK2Node_SwitchOnIntArray::GetExportTextForPin(const UEdGraphPin* InPin) const
{
	
	return FString::Printf(TEXT("(value=%d)"), PinValues[InPin->SourceIndex]);
}



void UK2Node_SwitchOnIntArray::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    bool bIsDirty = false;
    FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    if (PropertyName == TEXT("PinValues"))
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


