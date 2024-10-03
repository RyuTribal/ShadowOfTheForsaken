project "Box2D"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	flags {"NoPCH"}

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"include/box2d/**.h",
		"src/**.h",
		"src/**.cpp",
		"src/**.c"
	}

	includedirs
	{
		"include",
		"src"
	}

	filter "action:vs*"
		filter "system:windows"
		buildoptions { "/std:c11", "/experimental:c11atomics" } -- Enable C11 atomics support in MSVC

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"