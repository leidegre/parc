
require "tundra.syntax.glob"

local extensions = { ".c", ".h" }

local win32_env = {
  Env = {
    GENERATE_PDB = "1",
    CCOPTS = {
      "/Za", -- Disable all Microsoft Visual C language extensions
      "/W3",
      "/we4002", --too many actual parameters for macro
      "/we4013", --undefined; assuming extern returning int
      "/we4047", --differs in levels of indirection
      "/we4129", --unrecognized character escape sequence
      "/we4133", --incompatible types
      "/we4287",
      "/we4716", --must return a value (missing return statement?)
    },
    PROGOPTS = {
      "/MACHINE:X64"
    },
  },
}

local macosx_env = {
  Env = {
    CLANG_PATH = "~/Downloads/clang+llvm-3.7.0-x86_64-apple-darwin/bin",
    CCOPTS = {
      "-fcolor-diagnostics",
      -- Treat these warnings as errors
      "-Werror=return-type", -- control reaches end of non-void function
      "-Werror=format", -- printf style format string
      -- C99
      "-std=c99",
      "-pedantic-errors",
      -- Debug
      "-g",
      -- "-O1",
      -- QA
      "-fsanitize=address",
      "-fno-omit-frame-pointer",
    },
    PROGOPTS = {
      "-fsanitize=address",
    },
  },
}

Build {
  ScriptDirs = { "t2-scripts" },
  Units = function (args)
    local lib = StaticLibrary {
      Name = "parclib",
      Sources = {
        FGlob {
          Dir = "src",
          Extensions = extensions,
          Filters = {
            { Pattern = "_test.c$"; Config = "ignore" },
            { Pattern = "main.c"; Config = "ignore" },
          }
        }
      },
    }

    --dynamic node gen for test programs
    local t2_native = require "tundra.native"
    local t2_path = require "tundra.path"
    local _, files = t2_native.list_directory "src"
    for _, v in pairs(files) do
      if v:find("_test.c$") then
        local fn = "src/" .. v
        local name = t2_path.drop_suffix(v)
        Program {
          Name = name,
          Depends = { lib },
          Sources = {
            fn,
          },
          IdeGenerationHints = {
            Msvc = {
              SolutionFolder = "tests",
            }
          },
        }
      end
    end

    local main = Program {
      Name = "particle",
      Depends = { lib },
      Sources = {
        "src/main.c",
      },
    }
    Default(main)
  end,

  Configs = {
    Config {
      Name = "win64-msvc",
      Tools = { { "msvc-vs2015"; TargetArch = "x64" } },
      SupportedHosts = { "windows" },
      DefaultOnHost = "windows",
      Inherit = win32_env,
    },
    Config {
      Name = "macosx-clang",
      Tools = { { "clang_path-osx" } },
      SupportedHosts = { "macosx" },
      DefaultOnHost = "macosx",
      Inherit = macosx_env,
    },
  },

  IdeGenerationHints = {
    Msvc = {
      PlatformMappings = {
        ['win64-msvc'] = 'x64',
        ['win32-msvc'] = 'Win32',
      },
      VariantMappings = {
        ['release']    = 'Release',
        ['debug']      = 'Debug',
        ['production'] = 'Production',
      },
    },
    MsvcSolutionDir = 't2-vsproj',
    MsvcSolutions = {
      ['particle.sln'] = {},
    },
  }
}
