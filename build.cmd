@echo off

set cmake="C:\Program Files\CMake\bin\cmake.exe"
set msbuild="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

if not exist build (
    md build
)

if not exist bin (
    md bin
)

cd build

%cmake% ..
%msbuild% NeptuneDemo.sln -target:Build -property:WarningLevel=2;OutDir=..\bin\ -verbosity:q -maxCpuCount:4 -p:Configuration=Debug

