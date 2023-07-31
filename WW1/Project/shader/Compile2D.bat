set NowDir=%CD%
cd %~dp0

ShaderCompiler.exe /Tvs_4_0 DepthVS.fx
ShaderCompiler.exe /Tps_4_0 DepthPS.fx
ShaderCompiler.exe /Tvs_4_0 NormalMesh_DirLight_PhongVS.fx
ShaderCompiler.exe /Tps_4_0 NormalMesh_DirLight_PhongPS.fx
@rem pause

cd %NowDir%