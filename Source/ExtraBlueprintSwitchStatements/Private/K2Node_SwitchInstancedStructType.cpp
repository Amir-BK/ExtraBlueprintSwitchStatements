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
#include "KismetCompiledFunctionContext.h"
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

	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UK2Node_SwitchInstancedStructType* SwitchNode = CastChecked<UK2Node_SwitchInstancedStructType>(Node);

		// Get the execution and selection pins
		UEdGraphPin* ExecPin = SwitchNode->GetExecPin();
		UEdGraphPin* SelectionPin = SwitchNode->GetSelectionPin();

		if (!ExecPin || !SelectionPin)
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("NoExecOrSelectionPin", "@@ must have valid execution and selection pins").ToString(), Node);
			return;
		}

		//check that the seleciton pin has a valid struct type, it might be an empty instanced struct
		if (SwitchNode->PinStructs.Num() == 0)
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("NoStructType", "Switch node @@ must have at least one struct type").ToString(), Node);
			return;
		}

		

		// Generate the entry point statement
		FBlueprintCompiledStatement& EntryPoint = Context.AppendStatementForNode(Node);
		EntryPoint.Type = KCST_Nop;
		Context.GotoFixupRequestMap.Add(&EntryPoint, ExecPin);

		// Get the selection pin's term
		UEdGraphPin* SelectionNet = FEdGraphUtilities::GetNetFromPin(SelectionPin);
		FBPTerminal** SelectionTermPtr = Context.NetMap.Find(SelectionNet);
		if (!SelectionTermPtr)
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("NoSelectionTerm", "Failed to find selection term for @@").ToString(), Node);
			return;
		}
		FBPTerminal* SelectionTerm = *SelectionTermPtr;

		//get the instanced struct from the selection pin's subcategory object

		
		// Get the struct type from the instanced struct
		FBPTerminal* StructTypeTerm = Context.CreateLocalTerminal();
		StructTypeTerm->Type.PinCategory = UEdGraphSchema_K2::PC_Object;
		StructTypeTerm->Type.PinSubCategoryObject = UScriptStruct::StaticClass();

		// For each case, compare the struct types
		for (int32 PinIdx = 0; PinIdx < SwitchNode->PinStructs.Num(); PinIdx++)
		{
			UScriptStruct* PinStruct = SwitchNode->PinStructs[PinIdx];
			if (!PinStruct)
			{
				continue;
			}

			// Create a literal term for the case struct
			FBPTerminal* CaseStructTerm = Context.CreateLocalTerminal(ETerminalSpecification::TS_Literal);
			CaseStructTerm->Type = StructTypeTerm->Type;
			CaseStructTerm->ObjectLiteral = PinStruct;

			// Compare the structs
			FBlueprintCompiledStatement& CompareStatement = Context.AppendStatementForNode(Node);
			CompareStatement.Type = KCST_CallFunction;
			CompareStatement.FunctionToCall = FindField<UFunction>(GetClass(), TEXT("NotEqual_StructType"));
			CompareStatement.RHS.Add(SelectionTerm);
			CompareStatement.RHS.Add(CaseStructTerm);
			CompareStatement.LHS = StructTypeTerm;

			// Branch based on comparison
			FBlueprintCompiledStatement& BranchStatement = Context.AppendStatementForNode(Node);
			BranchStatement.Type = KCST_GotoIfNot;
			BranchStatement.LHS = StructTypeTerm;

			UEdGraphPin* CaseExecPin = SwitchNode->FindPin(PinStruct->GetFName());
			if (CaseExecPin && CaseExecPin->LinkedTo.Num() > 0)
			{
				Context.GotoFixupRequestMap.Add(&BranchStatement, CaseExecPin);
			}
		}

		// Handle the default case last
		UEdGraphPin* DefaultPin = SwitchNode->GetDefaultPin();
		if (DefaultPin && DefaultPin->LinkedTo.Num() > 0)
		{
			FBlueprintCompiledStatement& DefaultGoto = Context.AppendStatementForNode(Node);
			DefaultGoto.Type = KCST_UnconditionalGoto;
			Context.GotoFixupRequestMap.Add(&DefaultGoto, DefaultPin);
		}

		// Generate a return statement for the end of the switch
		FBlueprintCompiledStatement& ReturnStatement = Context.AppendStatementForNode(Node);
		ReturnStatement.Type = KCST_EndOfThread;
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


const UScriptStruct* UK2Node_SwitchInstancedStructType::GetStructTypeFromInstancedStruct(FInstancedStruct& InstancedStruct)
{
	return InstancedStruct.GetScriptStruct();
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

	UFunction* Function = FindUField<UFunction>(FunctionClass, FunctionName);
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