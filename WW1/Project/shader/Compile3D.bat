set NowDir=%CD%
cd %~dp0
@rem ShaderCompiler.exe /Tvs_4_0 GetDepthVS.fx
@rem ShaderCompiler.exe /Tps_4_0 GetDepthPS.fx
@rem ShaderCompiler.exe /Tvs_4_0 NormalMesh_PointLightVS.fx
@rem ShaderCompiler.exe /Tps_4_0 NormalMesh_PointLightPS.fx
@rem ShaderCompiler.exe /Tvs_4_0 NormalMesh_DirLight_PhongVS.fx
@rem ShaderCompiler.exe /Tps_4_0 NormalMesh_DirLight_PhongPS.fx
ShaderCompiler.exe /Tvs_4_0_level_9_1 CubeMapTestShader_VS.fx 
ShaderCompiler.exe /Tps_4_0_level_9_1 CubeMapTestShader_PS.fx
@rem ShaderCompiler.exe /Tvs_4_0 SkinMesh4_DirLight_Toon_PhongVS.fx
@rem ShaderCompiler.exe /Tps_4_0 SkinMesh4_DirLight_Toon_PhongPS.fx
@rem pause

cd %NowDir%