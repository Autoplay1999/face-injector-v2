@echo off
:: call "%PROGRAMFILES(X86)%\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat"
call "%PROGRAMFILES%\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
MSBuild face_injector_v2.sln /t:kankoshev_driver:Clean;kankoshev_driver:Rebuild /p:Configuration=Release;Platform=x64
MSBuild face_injector_v2.sln /t:kankoshev_driver:Clean;kankoshev_driver:Rebuild /p:Configuration=Debug;Platform=x64
MSBuild face_injector_v2.sln /t:kankoshev_driver:Clean;kankoshev_driver:Rebuild /p:Configuration=Release;Platform=x86
MSBuild face_injector_v2.sln /t:kankoshev_driver:Clean;kankoshev_driver:Rebuild /p:Configuration=Debug;Platform=x86
rd .vs /S /Q