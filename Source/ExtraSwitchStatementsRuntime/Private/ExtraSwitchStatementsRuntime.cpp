// Copyright Amir Ben-Kiki 2025

#include "ExtraSwitchStatementsRuntime.h"
#include "Logging/LogMacros.h"

// Create a log category that can be used throughout the module
DEFINE_LOG_CATEGORY(LogExtraSwitchRuntime);

void FExtraSwitchStatementsRuntimeModule::StartupModule()
{
    // This code will execute after your module is loaded into memory
    UE_LOG(LogExtraSwitchRuntime, Log, TEXT("ExtraSwitchStatementsRuntime module started"));
}

void FExtraSwitchStatementsRuntimeModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module
    UE_LOG(LogExtraSwitchRuntime, Log, TEXT("ExtraSwitchStatementsRuntime module shutdown"));
}

IMPLEMENT_MODULE(FExtraSwitchStatementsRuntimeModule, ExtraSwitchStatementsRuntime)