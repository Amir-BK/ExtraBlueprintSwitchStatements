// Copyright Amir Ben-Kiki 2025

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

// Declare the log category
EXTRASWITCHSTATEMENTSRUNTIME_API DECLARE_LOG_CATEGORY_EXTERN(LogExtraSwitchRuntime, Log, All);

class FExtraSwitchStatementsRuntimeModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};