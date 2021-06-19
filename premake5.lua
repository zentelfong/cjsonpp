
solution "cjsonpp"
	location ( "build" )
	configurations { "Debug", "Release" }
	platforms {"x64", "x32"}


configuration "Debug"
	defines { "DEBUG" }
	flags { "Symbols", "ExtraWarnings"}

configuration "Release"
	defines { "NDEBUG" }
	flags { "Optimize", "ExtraWarnings"}	
	
project "cjsonpp"
	kind "ConsoleApp"
	language "C++"

	files {
		"*.h",
		"*.c",
		"*.cpp",
		"gtest/*.h",
		"gtest/*.cc",
	}
