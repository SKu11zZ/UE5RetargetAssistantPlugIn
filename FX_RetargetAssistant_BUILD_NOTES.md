# FX_RetargetAssistant Build Notes

## UE5.4 Alpha Packaging

Validated package command:

```powershell
& 'F:\Epic Games\UE_5.4\Engine\Build\BatchFiles\RunUAT.bat' BuildPlugin `
  -Plugin='C:\FXRA_UE54_Build\Source\FX_RetargetAssistant\FX_RetargetAssistant.uplugin' `
  -Package='C:\FXRA_UE54_Build\Package\FX_RetargetAssistant' `
  -TargetPlatforms=Win64 `
  -Rocket
```

Result:

- `BUILD SUCCESSFUL`
- Output root: `C:\FXRA_UE54_Build\Package\FX_RetargetAssistant`

## Path Requirement

For UE5.4 Alpha validation, run `RunUAT BuildPlugin` from a pure-English path.

Observed failure:

- Source/package path under `G:\UE5重定向插件开发\插件\...`
- Failure occurred during PCH generation.
- The same plugin built successfully after copying to `C:\FXRA_UE54_Build\...`.

Until explicitly fixed and revalidated, do not use Chinese or non-ASCII paths for release packaging.

## Clean Project Load Check

Packaged plugin was installed into:

- `C:\FXRA_UE54_Build\CleanProject\Plugins\FX_RetargetAssistant`

Log evidence:

- `LogPluginManager: Mounting Project plugin FX_RetargetAssistant`
- `Executing Class /Script/FX_RetargetAssistant.FX_RetargetAssistantSmokeTestCommandlet`

The commandlet returned missing Mixamo test assets in the clean project, which is expected because demo/test content is not packaged with MVP0.

## DX12 Environment Note

FXRA54 crashed on one machine through UE5.4's D3D12 startup path:

- `Assertion failed: Distance >= 0`
- `D3D12Adapter.cpp [Line: 1861]`

This is treated as an environment / RHI issue, not a plugin issue.

For local closure testing, FXRA54 was switched to DX11:

- `DefaultGraphicsRHI=DefaultGraphicsRHI_DX11`

Before public release, run a smoke test in a clean project using the project's default RHI.
