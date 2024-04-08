# Script to help hide warnings from framework & libraries
# From https://community.platformio.org/t/silence-warnings-for-dependencies-external-libraries/33387/8

from pathlib import Path
import shlex
import sys

# PlatformIO injected stuff (I wish it was typed... https://github.com/platformio/platformio-docs/issues/342)
Import('env' ,'projenv') # type: ignore
env, projenv, DefaultEnvironment = env, projenv, DefaultEnvironment # type: ignore

platform = env.PioPlatform()
if env.GetProjectOption('board_build.core') == 'earlephilhower':
	FRAMEWORK_DIR = Path(platform.get_package_dir('framework-arduinopico'))
else:
	FRAMEWORK_DIR = Path(platform.get_package_dir('framework-arduino-mbed'))
framework_includes = list()
filtered_cpppath = list()

def expand_file_arguments(args):
	def parse_file_contents(filename):
		try:
			with open(filename, 'r') as file:
				# Use shlex to properly handle spaces, quotes, and escaped characters.
				content = file.read()
				return shlex.split(content)
		except IOError as e:
			# If the file cannot be read, it's up to caller to report problem and treat the option literally or skip
			return None

	processed_args = []
	for arg in args:
		if isinstance(arg, str) and arg.startswith('@'):
			file_args = parse_file_contents(arg[1:])
			if file_args is None:
				# If the file cannot be read, treat the option literally and pray for the best
				print(f"Warning: Failed to read file'{arg}' while expanding file-read arguments, skipping", file=sys.stderr)
				processed_args.extend(arg)
			else:
				# Recursively expand any file-included arguments within the file.
				processed_args.extend(expand_file_arguments(file_args))
		else:
			# Pass unexpected types (like tuple) or other arguments
			processed_args.append(arg)

	return processed_args

def unroll_include_prefixes(args, change_to_system=False):
	iprefix = None
	processed_args = []

	def handle_with_prefix(arg, type, foo):
		if arg.startswith(type):
			if change_to_system:
				foo = '-isystem' # or '-idirafter'
			if iprefix is None:
				print(f"Warning: Missing '-iprefix' before '${type}' while unrolling include prefixes, skipping", file=sys.stderr)
				return False
			else:
				processed_args.append((foo, iprefix + arg[len(type):]))
				return True
		return False

	for arg in args:
		if isinstance(arg, str):
			if arg.startswith('-iprefix'):
				iprefix = arg[len('-iprefix'):]
			elif handle_with_prefix(arg, '-iwithprefixbefore', '-I'):
				pass
			elif handle_with_prefix(arg, '-iwithprefix', '-idirafter'):
				pass
			else:
				# Pass other arguments
				processed_args.append(arg)
		else:
			# Pass unexpected types arguments (like tuple)
			processed_args.append(arg)

	return processed_args

# Apply these changes to current working env, the project env and the global env
for e in (env, projenv, DefaultEnvironment()):
	# Replace include paths provided via CPPPATH
	for p in e['CPPPATH']:
		# Is the current include path inside the framework directory?
		if FRAMEWORK_DIR in Path(p).parents:
			framework_includes.append(p)
		else:
			filtered_cpppath.append(p)
	e.Replace(CPPPATH=filtered_cpppath)
	e.Append(CCFLAGS=[('-isystem', p) for p in framework_includes])

	# Some include paths are added via CCFLAGS
	# In my case: reading arguments using `@file` and `-iprefix`/`-iwithprefix`/`-iwithprefixbefore` logic
	# Also see https://github.com/earlephilhower/arduino-pico/issues/2095
	# For now only prefixes are changed to system, so I opted for `change_to_system` argument,
	# instead writing more proper `-I` to `-isystem` (even outside `i*prefix` things).
	expanded = expand_file_arguments(e['CCFLAGS'])
	processed = unroll_include_prefixes(expanded, change_to_system=True)
	e.Replace(CCFLAGS=processed)

