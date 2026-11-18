using UnrealBuildTool;

public class SomethingStrangeTarget : TargetRules
{
	public SomethingStrangeTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("SomethingStrange");
	}
}
