project "Minecraft"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	files { "src/**.h", "src/**.cpp", "src/**.c" }
	removefiles { "src/main.cpp" } -- The old main file, new is in src/application/

	includedirs
	{
		"src/",
		"include/"
	}

	links 
	{ 
		"opengl32.lib"
	}

	targetdir ("../bin/" .. outputdir .. "-%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "-%{prj.name}")

	filter "system:windows"
		systemversion "latest"
		defines { "MC_PLATFORM_WINDOWS" }
		
	filter "system:linux"
		systemversion "latest"
		defines { "MC_PLATFORM_LINUX" }

	filter "configurations:Debug"
		defines { "MC_DEBUG" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "MC_RELEASE" }
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		kind "WindowedApp"
		defines { "MC_DIST" }
		runtime "Release"
		optimize "on"
