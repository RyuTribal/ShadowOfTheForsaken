project "Freetype"
    kind "StaticLib"
    language "C"
    staticruntime "off"

    -- Output and intermediate directories
    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    includedirs
    {
       "include"
    }

    defines{ "FT_CONFIG_OPTION_SVG",
    "FT_CONFIG_OPTION_SDF"}
    
    files {
       "src/autofit/autofit.c",
       "src/bdf/bdf.c",
       "src/cff/cff.c",
       "src/base/ftbase.c",
       "src/base/ftbitmap.c",
       "src/cache/ftcache.c",
       "src/base/ftdebug.c",
       "src/base/ftfstype.c",
       "src/base/ftglyph.c",
       "src/gzip/ftgzip.c",
       "src/base/ftinit.c",
       "src/lzw/ftlzw.c",
       "src/base/ftstroke.c",
       "src/base/ftsystem.c",
       "src/smooth/smooth.c",
   
       "src/base/ftbbox.c",
       "src/base/ftgxval.c",
       "src/base/ftlcdfil.c",
       "src/base/ftmm.c",
       "src/base/ftotval.c",
       "src/base/ftpatent.c",
       "src/base/ftpfr.c",
       "src/base/ftsynth.c",
       --"src/base/ftxf86.c",
       "src/base/ftfstype.c",
       "src/pcf/pcf.c",
       "src/pfr/pfr.c",
       "src/psaux/psaux.c",
       "src/pshinter/pshinter.c",
       "src/psnames/psmodule.c",
       "src/raster/raster.c",
       "src/sfnt/sfnt.c",
       "src/truetype/truetype.c",
       "src/type1/type1.c",
       "src/cid/type1cid.c",
       "src/type42/type42.c",
       "src/winfonts/winfnt.c",
       "src/svg/svg.c",             
       "src/sdf/sdf.c",              
       "src/sdf/ftbsdf.c"          
   }
    
    filter "system:windows"
    systemversion "latest"
    defines
    {
        "WIN32",
        "WIN32_LEAN_AND_MEAN",
        "VC_EXTRALEAN",
        "_CRT_SECURE_NO_WARNINGS",
        "FT2_BUILD_LIBRARY",      
    }    
    
    filter "configurations:Debug"
       defines 
       {
            "FT_DEBUG_LEVEL_ERROR",
            "FT_DEBUG_LEVEL_TRACE"
        }
      
        symbols "On"
        targetname "freetype_d"
      
     filter "configurations:Release"
        defines
        {
            "NDEBUG"
        }

        optimize "On"
        targetname "freetype"
