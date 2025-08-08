// Copyright Amir Ben-Kiki 2025

using UnrealBuildTool;

public class ExtraSwitchStatementsRuntime : ModuleRules
{
	public ExtraSwitchStatementsRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
			}
		);
	}
}