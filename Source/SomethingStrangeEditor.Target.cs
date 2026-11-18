using UnrealBuildTool;

public class SomethingStrangeEditorTarget : TargetRules
{
	public SomethingStrangeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("SomethingStrange");
	}
}
