local PROJECT_DIR          = (path.getabsolute("..") .. "/")
local PROJECT_OBJ_DIR      = path.join(PROJECT_DIR, ".obj")
local PROJECT_PROJECTS_DIR = path.join(PROJECT_DIR, ".projects")
local PROJECT_RUNTIME_DIR  = path.join(PROJECT_DIR, ".out/")

solution "Texeled"
	language				"C++"
	configurations			{ "Debug", "Release" }
	platforms				{ "x32", "x64" }
	
	location				(path.join(PROJECT_PROJECTS_DIR, _ACTION))
	objdir					(path.join(PROJECT_OBJ_DIR, _ACTION))

	project "Texeled"
		uuid				"6b4b6661-91b7-4678-9ee9-a3f251ddf595"
		kind				"WindowedApp"
		targetdir			(PROJECT_RUNTIME_DIR)

		files {
							--ImWindow
							"../Externals/ImWindow/ImWindow/**.cpp",
							"../Externals/ImWindow/ImWindow/**.h",
							"../Externals/ImWindow/ImWindowEasyWindow/**.cpp",
							"../Externals/ImWindow/ImWindowEasyWindow/**.h",
							"../Externals/ImWindow/ImWindowDX11/Imw*.cpp",
							"../Externals/ImWindow/ImWindowDX11/Imw*.h",
							"../Externals/ImWindow/Externals/EasyWindow/EasyWindow*.cpp",
							"../Externals/ImWindow/Externals/EasyWindow/EasyWindow*.h",
							--ImGui
							"../Externals/ImWindow/Externals/imgui/imconfig.h",
							"../Externals/ImWindow/Externals/imgui/imgui.h",
							"../Externals/ImWindow/Externals/imgui/imgui_internal.h",
							"../Externals/ImWindow/Externals/imgui/imgui.cpp",
							"../Externals/ImWindow/Externals/imgui/imgui_draw.cpp",
							"../Externals/ImWindow/Externals/imgui/stb_rect_pack.h",
							"../Externals/ImWindow/Externals/imgui/stb_textedit.h",
							"../Externals/ImWindow/Externals/imgui/stb_truetype.h",
							"../Externals/stb/*",
							"../Externals/tinyexr/*",
							"../Externals/Compressonator/CMP_Core/*",
							"../Externals/libpng-1.6.37/*",
							"../Externals/zlib-1.2.11/*",

							"../src/**",
							"../src/**.ico",
							"../src/Default.rc",

							"../scripts/Texeled.natvis"
		}
		
		includedirs {
							"../src", -- For ImwConfig.h
							"../Externals/ImWindow",
							"../Externals/ImWindow/ImWindow",
							"../Externals/ImWindow/ImWindowEasyWindow",
							"../Externals/ImWindow/ImWindowDX11",
							"../Externals/ImWindow/Externals/imgui",
							"../Externals/ImWindow/Externals/EasyWindow",

							"../Externals/stb",
							"../Externals/tinyexr",
							"../Externals/Compressonator/CMP_Core",
							"../Externals/libpng-1.6.37",
							"../Externals/zlib-1.2.11"
		}
		
		flags				"ExtraWarnings"

		buildoptions {
							"/openmp"
		}

		defines {
							"_CRT_SECURE_NO_WARNINGS"
		}

		configuration 		"**.ico"
			buildaction 	"Embed"

		configuration		"Debug"
			flags			{ "Symbols" }
			defines 		{ "DEBUG" }
			
		configuration		"Release"
			flags			{ "Optimize", "Symbols" }

		configuration 		{ "x32", "Debug" }
			targetsuffix	"_d"
		configuration 		{ "x32", "Release" }
			targetsuffix	""
		
		configuration 		{ "x64", "Debug" }
			targetsuffix	"_x64_d"
		configuration 		{ "x64", "Release" }
			targetsuffix	"_x64"
	