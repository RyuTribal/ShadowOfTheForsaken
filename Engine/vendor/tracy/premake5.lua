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
		"public/client/**.h",
		"public/client/**.hpp",
		"public/client/**.cpp",

		"public/common/**.h",
		"public/common/**.hpp",
		"public/common/**.cpp",

		"public/tracy/**.h",
		"public/tracy/**.hpp",
		"public/tracy/**.cpp",

		"public/libbacktrace/alloc.cpp",
		"public/libbacktrace/sort.cpp",
		"public/libbacktrace/state.cpp",
	}

	includedirs { "public/" }

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		files {
			"public/libbacktrace/posix.cpp",
			"public/libbacktrace/mmapio.cpp",
			"public/libbacktrace/macho.cpp",
			"public/libbacktrace/fileline.cpp",
			"public/libbacktrace/elf.cpp",
			"public/libbacktrace/dwarf.cpp",
		}

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