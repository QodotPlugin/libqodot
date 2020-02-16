#!python
import os, subprocess

Import('env')
env = env.Clone()

# Local dependency paths, adapt them to your setup
godot_headers_path = "godot_headers/"

# make sure our binding library is properly includes
env.Append(CPPPATH=['.', godot_headers_path])

env.VariantDir('build', 'src', duplicate=0)

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=['src/'])
sources = Glob('src/*.c')

library = env.SharedLibrary(target='build/libqodot' , source=sources, LIBS=['libmap'], LIBPATH=['#libmap/build'], PDB = "build/libqodot.pdb")
