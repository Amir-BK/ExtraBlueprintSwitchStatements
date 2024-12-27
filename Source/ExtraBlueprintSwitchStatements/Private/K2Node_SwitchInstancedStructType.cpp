// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_SwitchInstancedStructType.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "BlueprintNodeSpawner.h"
#include "InstancedStruct.h"
#include "EdGraphUtilities.h"
#include "KismetCompiler.h"
#include "KismetCompiledFunctionContext.h"
#include "Internationalization/Internationalization.h"
#include "Kismet2/CompilerResultsLog.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetCompiledFunctionContext.h"
#include "UObject/UnrealType.h"
#include "UObject/Field.h"
#include "UObject/FieldPath.h"
#include "UObject/UnrealType.h"
#include "BlueprintActionDatabaseRegistrar.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_SwitchInstancedStructType)

#define LOCTEXT_NAMESPACE "K2Node_SwitchInstancedStructType"
#pragma region UFKCHandler_SwitchInstancedStructType

/////// ///////////////////////////////////////////////////////////////////
// FKCHandler_SwitchInstancedStructType

class FKCHandler_SwitchInstancedStructType : public FNodeHandlingFunctor
{
public:
	FKCHandler_SwitchInstancedStructType(FKismetCompilerContext& InCompilerContext)
		: FNodeHandlingFunctor(InCompilerContext)
	{
	}

	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_SwitchInstancedStructType* SwitchNode = CastChecked<UK2Node_SwitchInstancedStructType>(Node);

		// Register the selection pin
		if (UEdGraphPin* SelectionPin = SwitchNode->GetSelectionPin())
		{
			FBPTerminal* SelectionTerm = Context.CreateLocalTerminal();
			SelectionTerm->Type.PinCategory = UEdGraphSchema_K2::PC_Struct;
			SelectionTerm->Type.PinSubCategoryObject = FInstancedStruct::StaticStruct();
			SelectionTerm->Source = Node;
			SelectionTerm->Name = Context.NetNameMap->MakeValidName(Node) + TEXT("_Selection");

			// Using FindUField instead of FindFProperty
			SelectionTerm->AssociatedVarProperty = FInstancedStruct::StaticStruct()->FindPropertyByName(TEXT("ScriptStruct"));




			Context.NetMap.Add(SelectionPin, SelectionTerm);
		}

		// Register nets for all case value pins
		for (int32 Idx = 0; Idx < SwitchNode->PinStructs.Num(); ++Idx)
		{
			if (UEdGraphPin* ValuePin = SwitchNode->GetValuePin(Idx))
			{
				FBPTerminal* ValueTerm = Context.CreateLocalTerminal();
				ValueTerm->Type = ValuePin->PinType;
				ValueTerm->Source = Node;
				ValueTerm->Name = Context.NetNameMap->MakeValidName(ValuePin) + TEXT("_Value");
				if (ValuePin->PinType.PinSubCategoryObject.IsValid())
				{
					ValueTerm->AssociatedVarProperty = CastField<FProperty>(ValuePin->PinType.PinSubCategoryObject.Get()->GetClass()->FindPropertyByName(TEXT("Value")));
				}
				Context.NetMap.Add(ValuePin, ValueTerm);
			}
		}
	}

	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_SwitchInstancedStructType* SwitchNode = CastChecked<UK2Node_SwitchInstancedStructType>(Node);

		// Create the entry point
		FBlueprintCompiledStatement& EntryPoint = Context.AppendStatementForNode(Node);
		//EntryPoint.Type = KCST_Nothing;
		Context.GotoFixupRequestMap.Add(&EntryPoint, SwitchNode->GetExecPin());

		// Get the selection pin
		UEdGraphPin* SelectionPin = SwitchNode->GetSelectionPin();
		if (!SelectionPin)
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("NoSelectionPin", "No selection pin found for @@").ToString(), Node);
			return;
		}

		// Get selection term
		UEdGraphPin* SelectionNet = FEdGraphUtilities::GetNetFromPin(SelectionPin);
		FBPTerminal** SelectionTermPtr = Context.NetMap.Find(SelectionNet);
		if (!SelectionTermPtr)
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("NoSelectionTerm", "Selection term not found for @@").ToString(), Node);
			return;
		}



		// Store the selection term
		FBPTerminal* SelectionTerm = *SelectionTermPtr;

		if (!SelectionTerm->AssociatedVarProperty)
		{
			CompilerContext.MessageLog.Error(TEXT("Failed to find ScriptStruct property in FInstancedStruct"));
		}

		// Create statements for each case
		for (int32 i = 0; i < SwitchNode->PinStructs.Num(); ++i)
		{
			if (!SwitchNode->PinStructs[i])
			{
				continue;
			}

			// Create comparison statement
			FBlueprintCompiledStatement& CompareStmt = Context.AppendStatementForNode(Node);
			CompareStmt.Type = KCST_CallFunction;
			CompareStmt.FunctionToCall = FindUField<UFunction>(UK2Node_SwitchInstancedStructType::StaticClass(), TEXT("GetStructTypeFromInstancedStruct"));

			// Create and setup the comparison result terminal
			FBPTerminal* ComparisonTerm = Context.CreateLocalTerminal();
			ComparisonTerm->Type.PinCategory = UEdGraphSchema_K2::PC_Boolean;
			ComparisonTerm->Name = Context.NetNameMap->MakeValidName(Node) + FString::Printf(TEXT("_Case_%d_Matches"), i);
			ComparisonTerm->Source = Node;
			ComparisonTerm->AssociatedVarProperty = CastField<FProperty>(UKismetMathLibrary::StaticClass()->FindPropertyByName(TEXT("bReturnValue")));


			CompareStmt.LHS = ComparisonTerm;
			CompareStmt.RHS.Add(SelectionTerm);

			// Generate conditional statement
			UEdGraphPin* CasePin = SwitchNode->FindPin(SwitchNode->PinStructs[i]->GetFName());
			if (CasePin && CasePin->LinkedTo.Num() > 0)
			{
				FBlueprintCompiledStatement& GotoStmt = Context.AppendStatementForNode(Node);
				GotoStmt.Type = KCST_GotoIfNot;
				GotoStmt.LHS = ComparisonTerm;
				Context.GotoFixupRequestMap.Add(&GotoStmt, CasePin);
			}
		}

		// Handle default case
		UEdGraphPin* DefaultPin = SwitchNode->GetDefaultPin();
		if (DefaultPin && DefaultPin->LinkedTo.Num() > 0)
		{
			FBlueprintCompiledStatement& GotoDefault = Context.AppendStatementForNode(Node);
			GotoDefault.Type = KCST_UnconditionalGoto;
			Context.GotoFixupRequestMap.Add(&GotoDefault, DefaultPin);
		}

		// Generate the exit point
		FBlueprintCompiledStatement& ExitPoint = Context.AppendStatementForNode(Node);
		ExitPoint.Type = KCST_EndOfThread;
	}
};


#pragma endregion

#pragma region UK2Node_SwitchInstancedStructType

//////////////////////////////////////////////////////////////////////////
// UK2Node_SwitchInstancedStructType


UK2Node_SwitchInstancedStructType::UK2Node_SwitchInstancedStructType(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FunctionName = TEXT("NotEqual_StructType");
	FunctionClass = this->GetClass();
	OrphanedPinSaveMode = ESaveOrphanPinMode::SaveNone;
}


// In K2Node_SwitchInstancedStructType.cpp

UEdGraphPin* UK2Node_SwitchInstancedStructType::GetValuePin(int32 CaseIndex) const
{
	if (CaseIndex < 0 || CaseIndex >= PinStructs.Num())
	{
		return nullptr;
	}

	// Number of base pins before the case pins start
	const int32 BasePinCount = 3; // ExecInPin, SelectionPin, FunctionPin (hidden)

	// Each case has two pins: Exec pin and Value pin
	const int32 PinsPerCase = 2;

	// Index of the value pin in the Pins array
	const int32 ValuePinIndex = BasePinCount + (PinsPerCase * CaseIndex) + 1; // +1 to skip the Exec pin

	if (Pins.IsValidIndex(ValuePinIndex))
	{
		return Pins[ValuePinIndex];
	}

	return nullptr;
}


void UK2Node_SwitchInstancedStructType::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	bool bIsDirty = false;
	const FName PropertyName = (PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None);
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UK2Node_SwitchInstancedStructType, PinStructs))
	{
		bIsDirty = true;
	}
	if (bIsDirty)
	{
		ReconstructNode();
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UK2Node_SwitchInstancedStructType::PostLoad()
{
	Super::PostLoad();
	if (UEdGraphPin* FunctionPin = FindPin(FunctionName))
	{
		FunctionPin->DefaultObject = FunctionClass->GetDefaultObject();
	}
}

FText UK2Node_SwitchInstancedStructType::GetTooltipText() const
{
	return NSLOCTEXT("K2Node", "Switch_Struct", "Checks if the input instanced struct matches the designated struct type of the output pin");
}

FText UK2Node_SwitchInstancedStructType::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("K2Node", "Switch_Struct", "Switch on Instanced Struct Type");
}

void UK2Node_SwitchInstancedStructType::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that 
// actions might have to be updated (or deleted) if their object-key is  
// mutated (or removed)... here we use the node's class (so if the node 
// type disappears, then the action should go with it)
	UClass* ActionKey = GetClass();
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the 
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FNodeHandlingFunctor* UK2Node_SwitchInstancedStructType::CreateNodeHandler(FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_SwitchInstancedStructType(CompilerContext);
}

void UK2Node_SwitchInstancedStructType::AddPinToSwitchNode()
{
	//so we actually need to add two pins, one for the selection and one for the value
	FName PinName = GetUniquePinName();
	PinNames.Add(PinName);

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinName);
	if (PinStructs.Num() < PinNames.Num())
	{
		PinStructs.Add(nullptr);
	}

	//and we need a wildcard for the value
	FName ValuePinName = GetUniquePinName();
	ValuePinNames.Add(ValuePinName);

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, ValuePinName);

}

FName UK2Node_SwitchInstancedStructType::GetUniquePinName()
{
	FString BaseName = TEXT("Case");
	FString PinName;
	int32 Index = 0;
	do
	{
		PinName = FString::Printf(TEXT("%s_%d"), *BaseName, Index++);
	} while (FindPin(*PinName) != nullptr);
	return FName(*PinName);
}

FEdGraphPinType UK2Node_SwitchInstancedStructType::GetPinType() const
{
	FEdGraphPinType PinType;
	PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	PinType.PinSubCategoryObject = FInstancedStruct::StaticStruct();
	return PinType;
}

FEdGraphPinType UK2Node_SwitchInstancedStructType::GetInnerCaseType() const
{
	FEdGraphPinType PinType;
	PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	PinType.PinSubCategoryObject = FInstancedStruct::StaticStruct();
	return PinType;
}

FEdGraphPinType UK2Node_SwitchInstancedStructType::GetInnerCaseTypeForCaseIndex(int32 Index) const
{
	FEdGraphPinType PinType;

	if (Index < PinStructs.Num())
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		PinType.PinSubCategoryObject = PinStructs[Index];
	}
	else
	{
		PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
	}

	return PinType;
}

FName UK2Node_SwitchInstancedStructType::GetPinNameGivenIndex(int32 Index) const
{
	return PinNames[Index];
}

void UK2Node_SwitchInstancedStructType::CreateFunctionPin()
{
	// Set properties on the function pin
	UEdGraphPin* FunctionPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, FunctionClass, FunctionName);
	FunctionPin->bDefaultValueIsReadOnly = true;
	FunctionPin->bNotConnectable = true;
	FunctionPin->bHidden = true;

	UFunction* Function = FunctionClass->FindFunctionByName(FunctionName);
	const bool bIsStaticFunc = Function->HasAllFunctionFlags(FUNC_Static);
	if (bIsStaticFunc)
	{
		// Wire up the self to the CDO of the class if it's not us
		if (UBlueprint* BP = GetBlueprint())
		{
			UClass* FunctionOwnerClass = Function->GetOuterUClass();
			if (!BP->SkeletonGeneratedClass->IsChildOf(FunctionOwnerClass))
			{
				FunctionPin->DefaultObject = FunctionOwnerClass->GetDefaultObject();
			}
		}
	}
}

void UK2Node_SwitchInstancedStructType::CreateSelectionPin()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	UEdGraphPin* Pin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FInstancedStruct::StaticStruct(), TEXT("Selection"));
	K2Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pin);
}

void UK2Node_SwitchInstancedStructType::CreateCasePins()
{
	//so we need to create an exec and value pins for each case, the value might be a little tricky but possibly not, it needs to pc a PC_Struct with the value of the static struct
	// that should be easy, actually getting the value later might be trickier, let's see
	while (PinStructs.Num() > PinNames.Num())
	{
		const FName PinName = GetUniquePinName();
		PinNames.Add(PinName);
	}

	if (PinNames.Num() > PinStructs.Num())
	{
		PinNames.SetNum(PinStructs.Num());
	}

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	for (int32 Index = 0; Index < PinStructs.Num(); ++Index)
	{
		if (PinStructs[Index] != nullptr)
		{
			PinNames[Index] = PinStructs[Index]->GetFName();
		}
		else
		{
			PinNames[Index] = GetUniquePinName();
		}
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinNames[Index]);
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, PinStructs[Index], *FString::Printf(TEXT("%s_Value"), *PinNames[Index].ToString()));
		K2Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pins.Last());
	}
}

void UK2Node_SwitchInstancedStructType::RemovePin(UEdGraphPin* TargetPin)
{
	checkSlow(TargetPin);

	FName PinName = TargetPin->PinName;
	// Clean-up pin name array
	int32 Index = PinNames.IndexOfByKey(PinName);
	if (Index >= 0)
	{
		if (Index < PinStructs.Num())
		{
			PinStructs.RemoveAt(Index);
		}
		PinNames.RemoveAt(Index);
	}
}

#pragma endregion

#undef LOCTEXT_NAMESPACE