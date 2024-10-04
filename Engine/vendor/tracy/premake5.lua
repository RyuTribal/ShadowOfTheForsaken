project "Tracy"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "Off"

	--toolset "clang"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"public/TracyClient.cpp",
	}

	includedirs { "public/" }

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		links({
			"pthread",
			"X11",
			"m", "dl",
			"c",
		})

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		conformancemode "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		conformancemode "On"

		defines {
			"TRACY_ENABLE",
			"TRACY_ON_DEMAND"
		}

	filter "configurations:Dist"
		runtime "Release"
		optimize "Full"
		conformancemode "On"

		defines {
			"TRACY_ENABLE",
			"TRACY_ON_DEMAND"
		}