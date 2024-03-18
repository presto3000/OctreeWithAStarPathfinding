// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MassOctreePlugin : ModuleRules
{
	public MassOctreePlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
				"MassEntity", // Mass
				"MassCommon", // Mass
				"MassMovement", // Mass
				"MassActors", // Mass
				"MassSpawner", // Mass
				"MassGameplayDebug", // Mass
				"MassSignals",// Mass
				"MassCrowd", // Mass
				"MassActors", // Mass
				"MassSpawner", // Mass
				"MassRepresentation", // Mass
				"MassReplication", // Mass
				"MassNavigation", // Mass
				"MassSimulation", // Mass
				"MassLOD",	// Mass
				"StructUtils", // Mass
				"OctreePathfinding", 
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
