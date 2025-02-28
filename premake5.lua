workspace "Minecraft"
	architecture "x64"
	configurations { "Debug", "Release", "Dist" }
	startproject "Minecraft"
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Minecraft"
