local dev_path = "dev"
local build_path = "build"
local dependency_path = "dependencies"

--Project: aurora
local aurora_include_path = dev_path .. "/aurora/include"
local aurora_src_path = dev_path .. "/aurora/src"

--Project: numa [dependency static library]
local numa_include_path = dependency_path .. "/numa/dev/numa/include"
local numa_src_path = dependency_path .. "/numa/dev/numa/src"

workspace ( "aurora" )
   configurations ( { "Debug", "Release" } )
   platforms ( { "x64" } )
   location ( build_path )

   -- filter ( { "system:windows", "action:vs*" } )
   startproject ("aurora")

project ( "numa" )
   kind ( "StaticLib" )
   language ( "C++" )
   cppdialect ( "C++17" )
   location ( build_path .. "/numa" )

   targetdir ( build_path .. "/bin/%{cfg.platform}-%{cfg.buildcfg}" )
   objdir ( build_path .. "/bin-int/numa/%{cfg.platform}-%{cfg.buildcfg}" )

   includedirs {
      numa_include_path
   }

   files {
      numa_include_path .. "/**.h",
      numa_include_path .. "/**.hpp",
      numa_src_path .. "/**.cpp"
   }

   filter ( "configurations:Debug" )
      defines ( { "DEBUG", "_DEBUG" } )
      symbols ( "On" )

   filter ( "configurations:Release" )
      defines ( { "NDEBUG", "_NDEBUG" } )
      optimize ( "On" )

   filter ( { "system:windows", "action:vs*" } )
      vpaths {
         ["Include/*"] = {
            numa_include_path .. "/**.h",
            numa_include_path .. "/**.hpp"
         },
         ["Sources/*"] = {
            numa_src_path .. "/**.cpp"
         },
      }

project ( "aurora" )
   kind ( "ConsoleApp" )
   language ( "C++" )
   cppdialect ( "C++17" )
   location ( build_path .. "/aurora" )

   targetdir ( build_path .. "/bin/%{cfg.platform}-%{cfg.buildcfg}" )
   objdir ( build_path .. "/bin-int/sandbox/%{cfg.platform}-%{cfg.buildcfg}" )

   includedirs {
      numa_include_path,
      aurora_include_path
   }
   libdirs {
      build_path .. "/bin/%{cfg.platform}-%{cfg.buildcfg}"
   }

   links {
      "numa",
   }

   files {
      aurora_include_path .. "/**.h",
      aurora_include_path .. "/**.hpp",
      aurora_src_path .. "/**.cpp"
   }

   filter ( "configurations:Debug" )
      defines ( { "DEBUG", "_DEBUG" } )
      symbols ( "On" )

   filter ( "configurations:Release" )
      defines ( { "NDEBUG", "_NDEBUG" } )
      optimize ( "On" )

   filter ( { "system:windows", "action:vs*" } )
      vpaths {
         ["Include/*"] = {
            aurora_include_path .. "/**.h",
            aurora_include_path .. "/**.hpp"
         },
         ["Sources/*"] = {
            aurora_src_path .. "/**.cpp"
         },
      }