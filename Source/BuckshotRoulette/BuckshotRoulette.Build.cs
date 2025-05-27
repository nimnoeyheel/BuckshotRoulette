// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BuckshotRoulette : ModuleRules
{
	public BuckshotRoulette(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
