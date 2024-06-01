local rootPath = path.getabsolute(".")
require("cmake")

workspace("2DGame")
architecture("x64")
startproject("Game")

configurations({
	"Debug",
	"Release",
	"Dist",
})

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

pchheader "pch.h"
pchsource "src/pch.cpp"

IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["ImGui"] = "vendor/imgui"
IncludeDir["glm"] = "vendor/glm"

group("Dependencies")
include("vendor/GLFW")
include("vendor/Glad")
include("vendor/imgui")

group("")
project("Game")
location("./")
kind("ConsoleApp")
staticruntime("off")
language("C++")
cppdialect("C++17")
targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")

files({
	"src/**.h",
	"src/**.hpp",
	"src/**.cpp",
	"src/**.c",
	"vendor/glm/glm/**.hpp",
	"vendor/glm/glm/**.inl",
})

removefiles({
	"src/Platform/**",
})

libdirs({
	"vendor/GLFW/lib-vc2022",
})

links({
	"GLFW",
	"Glad",
	"ImGui",
})

defines({
	"_CRT_SECURE_NO_WARNINGS",
	'ROOT_PATH="' .. rootPath .. "/" .. '%{prj.name}"',
})

includedirs({
	"vendor/spdlog/include",
	"%{IncludeDir.GLFW}",
	"%{IncludeDir.Glad}",
	"%{IncludeDir.ImGui}",
	"%{IncludeDir.glm}",
	"src/",
})

filter("system:windows")
systemversion("latest")
defines({
	"PLATFORM_WINDOWS",
	"BUILD_DLL",
	"GLFW_INCLUDE_NONE",
	"WIN32_LEAN_AND_MEAN",
})

links({
	"opengl32.lib",
})

filter("configurations:Debug")
defines("DEBUG")
runtime("Debug")
symbols("on")

filter("configurations:Release")
defines("RELEASE")
runtime("Release")
optimize("on")

filter("configurations:Dist")
defines("DIST")
runtime("Release")
optimize("on")
