project("Engine")
location("./")
kind("StaticLib")
staticruntime("off")
language("C++")
cppdialect("C++20")
targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")

pchheader "pch.h"
pchsource "src/pch.cpp"

files({
	"src/**.h",
	"src/**.hpp",
	"src/**.cpp",
	"src/**.c",
	"vendor/glm/glm/**.hpp",
	"vendor/glm/glm/**.inl",
})

libdirs({
	"vendor/GLFW/lib-vc2022",
})

links({
	"GLFW",
	"Glad",
	"ImGui",
	"Box2D",
	"Tracy"
})

defines({
	"_CRT_SECURE_NO_WARNINGS",
	'ROOT_PATH="' .. rootPath .. "/" .. '%{prj.name}"',
})

includedirs({
	"vendor/spdlog/include",
	"vendor/stb",
	"%{IncludeDir.GLFW}",
	"%{IncludeDir.Glad}",
	"%{IncludeDir.ImGui}",
	"%{IncludeDir.glm}",
	"%{IncludeDir.Box2D}",
	"%{IncludeDir.miniaudio}",
	"%{IncludeDir.Tracy}",
	"src"
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
		"ws2_32", 
		"dbghelp"
    })

filter("system:linux")
    systemversion("latest")
    defines({
        "PLATFORM_LINUX",
        "GLFW_INCLUDE_NONE",
    })

    links({
        "GL",
        "pthread",
        "X11",
		"m", "dl",
		"c",
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