-- premake5.lua
workspace "WeekendRaytracer"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "WalnutApp"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "WeekendRaytracer"