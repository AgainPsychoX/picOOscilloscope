# Script to help hide warnings from framework & libraries
# From https://community.platformio.org/t/silence-warnings-for-dependencies-external-libraries/33387/8

from pathlib import Path

# PlatformIO injected stuff (I wish it was typed... https://github.com/platformio/platformio-docs/issues/342)
Import("env" ,"projenv") # type: ignore
env, projenv, DefaultEnvironment = env, projenv, DefaultEnvironment # type: ignore

platform = env.PioPlatform()
if env.GetProjectOption("board_build.core") == 'earlephilhower':
	FRAMEWORK_DIR = Path(platform.get_package_dir("framework-arduinopico"))
else:
	FRAMEWORK_DIR = Path(platform.get_package_dir("framework-arduino-mbed"))
framework_includes = list()
filtered_cpppath = list()

# Apply these changes to current working env, the project env and the global env
for e in (env, projenv, DefaultEnvironment()):
	for p in e["CPPPATH"]:
		# Is the current include path inside the framework directory?
		if FRAMEWORK_DIR in Path(p).parents:
			framework_includes.append(p)
		else:
			filtered_cpppath.append(p)
	e.Replace(CPPPATH=filtered_cpppath)
	e.Append(CCFLAGS=[("-isystem", p) for p in framework_includes])
