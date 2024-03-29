#!python
import os, subprocess

env = Environment()

opts = Variables([], ARGUMENTS)

# Define our options
opts.Add(EnumVariable('target', "Compilation target", 'debug', ['d', 'debug', 'r', 'release']))
opts.Add(EnumVariable('platform', "Compilation platform", '', ['', 'windows', 'x11', 'linux', 'osx']))
opts.Add(EnumVariable('p', "Compilation target, alias for 'platform'", '', ['', 'windows', 'x11', 'linux', 'osx']))
opts.Add(BoolVariable('fat_binary', "(macOS Only) Produce a fat binary for Intel and Apple Silicon", 'no'))
opts.Add(BoolVariable('use_llvm', "Use the LLVM / Clang compiler", 'no'))

# only support 64 at this time..
bits = 64

# Updates the environment with the option variables.
opts.Update(env)

# Process some arguments
if env['use_llvm']:
    env['CC'] = 'clang'
    env['CXX'] = 'clang++'

if env['p'] != '':
    env['platform'] = env['p']

if env['platform'] == '':
    print("No valid target platform selected.")
    quit();

# Check our platform specifics
elif env['platform'] == "osx":
    if env['fat_binary']:
        archFlags = ['-arch', 'x86_64', '-arch', 'arm64']
    else:
        archFlags = ['-arch', 'x86_64']
        
    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS = ['-g','-O2'] + archFlags)
        env.Append(LINKFLAGS = archFlags)
    else:
        env.Append(CCFLAGS = ['-g','-O3'] + archFlags)
        env.Append(LINKFLAGS = archFlags)

elif env['platform'] in ('x11', 'linux'):
    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS = ['-fPIC', '-g3','-Og'])
        env.Append(LINKFLAGS = [
            '-Wl,-rpath,\'$$ORIGIN\''
        ])
    else:
        env.Append(CCFLAGS = ['-fPIC', '-g','-O3'])
        env.Append(LINKFLAGS = [
            '-Wl,-rpath,\'$$ORIGIN\''
        ])

elif env['platform'] == "windows":
    # This makes sure to keep the session environment variables on windows,
    # that way you can run scons in a vs 2017 prompt and it will find all the required tools
    env.Append(ENV = os.environ)

    env.Append(CCFLAGS = ['-DWIN32', '-D_WIN32', '-D_WINDOWS', '-W3', '-GR', '-D_CRT_SECURE_NO_WARNINGS'])
    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS = ['-EHsc', '-D_DEBUG', '-MDd'])
    else:
        env.Append(CCFLAGS = ['-O2', '-EHsc', '-DNDEBUG', '-MD'])

# Headers
env.SConscript("libmap/SConscript", exports='env')
env.SConscript("SConscript", exports='env')
