local CCFiles = { ".cc", ".h" }

local common = {
	Env = {
		 GENERATE_PDB = "1",
		 CXXOPTS = {
			"/FC", -- necessary for Visual Studio to grep file path in Output Window
			"/EHsc",
		},
	},
}

Build {
	Configs = {
		Config {
			Name = "generic-gcc",
			DefaultOnHost = "linux",
			Tools = { "gcc" },
		},
		Config {
			Name = "macosx-gcc",
			DefaultOnHost = "macosx",
			Tools = { "gcc-osx" },
		},
		Config {
			Name = "win64-msvc",
			DefaultOnHost = "windows",
			Inherit = common,
			Tools = { { "msvc-vs2013"; TargetPlatform = "x64" } },
		},
	},
	Units = function()

		require "tundra.syntax.glob"

		Program {
			Name = "parc",
			Sources = {
				Glob { Dir = "src", Extensions = CCFiles }
			},
		}

		Default "parc"

	end,
}
