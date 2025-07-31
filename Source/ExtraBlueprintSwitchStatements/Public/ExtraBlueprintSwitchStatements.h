// Copyright Amir Ben-Kiki 2025

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FExtraBlueprintSwitchStatementsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
