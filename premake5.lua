rootPath = path.getabsolute(".")
require("cmake")

workspace("ShadowOfTheForsaken")
architecture("x64")
startproject("Game")

PCHFile = "%{wks.location}/Engine/src/pch.cpp"


configurations({
	"Debug",
	"Release",
	"Dist",
})

filter "action:vs*"
	linkoptions { "/ignore:4099" }
	disablewarnings { "4068" }

	filter "language:C++ or language:C"
	architecture "x86_64"

	filter "configurations:Debug or configurations:Debug-AS"
	optimize "Off"
	symbols "On"

	filter { "system:windows", "configurations:Debug-AS" }	
	flags { "NoRuntimeChecks", "NoIncrementalLink" }

	filter "configurations:Release"
	optimize "On"
	symbols "Default"

	filter "configurations:Dist"
	optimize "Full"
	symbols "Off"

	filter "system:windows"
	buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Engine/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Engine/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Engine/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Engine/vendor/glm"
IncludeDir["Box2D"] = "%{wks.location}/Engine/vendor/box2d/include"
IncludeDir["miniaudio"] = "%{wks.location}/Engine/vendor/miniaudio/include"

group("Dependencies")
include("Engine/vendor/GLFW")
include("Engine/vendor/Glad")
include("Engine/vendor/imgui")
include("Engine/vendor/box2d")

group("Tools")
include("Engine")

group("")
include("Game")
