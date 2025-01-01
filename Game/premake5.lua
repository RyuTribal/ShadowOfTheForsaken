project("Game")
location("./")
kind("ConsoleApp")
staticruntime("off")
language("C++")
cppdialect("C++20")
targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")

flags {"NoPCH"}

files({
	"src/**.h",
	"src/**.hpp",
	"src/**.cpp",
	"src/**.c",
})

includedirs({
	"%{wks.location}/Engine/vendor/spdlog/include",
	"%{wks.location}/Engine/vendor/stb",
	"%{IncludeDir.GLFW}",
	"%{IncludeDir.Glad}",
	"%{IncludeDir.ImGui}",
	"%{IncludeDir.glm}",
	"%{IncludeDir.Box2D}",
	"%{IncludeDir.miniaudio}",
    "%{IncludeDir.Tracy}",
	"%{wks.location}/Engine/src",
    "vendor/nlohmann/include",
	"src",
})

libdirs({
	"%{wks.location}/Engine/vendor/GLFW/lib-vc2022",
})

links({
	"Engine",
    "GLFW",
	"Glad",
	"Box2D",
	"ImGui",
    "Freetype",
})

defines({
	"_CRT_SECURE_NO_WARNINGS",
	'ROOT_PATH="' .. rootPath .. "/" .. '%{prj.name}"',
	
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