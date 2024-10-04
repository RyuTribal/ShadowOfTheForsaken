project "Tracy"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "Off"

	--toolset "clang"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	flags {"NoPCH"}

	files
	{
		"public/TracyClient.cpp",
	}

	includedirs { "public" }

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		conformancemode "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		conformancemode "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "Full"
		conformancemode "On"