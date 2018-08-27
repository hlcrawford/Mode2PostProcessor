import sys
import os

colors = {}
colors['cyan']   = '\033[36m'
colors['purple'] = '\033[95m'
colors['blue']   = '\033[94m'
colors['green']  = '\033[92m'
colors['yellow'] = '\033[93m'
colors['red']    = '\033[91m'
colors['end']    = '\033[0m'

## If the output is not a terminal, remove the colors
if not sys.stdout.isatty():
   for key, value in colors.iteritems():
      colors[key] = ''

compile_source_message = '%sCompiling %s===================> %s$SOURCE%s' % \
   (colors['blue'], colors['purple'], colors['cyan'], colors['end'])

compile_shared_source_message = '%sCompiling shared %s============> %s$SOURCE%s' % \
   (colors['blue'], colors['purple'], colors['cyan'], colors['end'])

link_program_message = '%sLinking Program %s=============> %s$TARGET%s' % \
   (colors['red'], colors['purple'], colors['cyan'], colors['end'])

link_library_message = '%sLinking Static Library %s=====> %s$TARGET%s' % \
   (colors['red'], colors['purple'], colors['cyan'], colors['end'])

ranlib_library_message = '%sRanlib Library %s===========> %s$TARGET%s' % \
   (colors['red'], colors['purple'], colors['cyan'], colors['end'])

link_shared_library_message = '%sLinking Shared Library %s======> %s$TARGET%s' % \
   (colors['red'], colors['purple'], colors['cyan'], colors['end'])

java_library_message = '%sCreating Java Archive %s======> %s$TARGET%s' % \
   (colors['red'], colors['purple'], colors['cyan'], colors['end'])

root_dictionary_message = '%sGenerating ROOT dictionary %s==> %s$TARGET%s' % \
   (colors['red'], colors['purple'], colors['cyan'], colors['end'])
 
def rootcint(target, source, env):
    """Executes the ROOT dictionary generator over a list of headers. """
    dictname = target[0]
    headers = ""
    for f in source:
    	headers += str(f) + " "

    command = "rootcint -f %s -c -p %s" %(dictname, headers)
    ok = os.system(command)
    return ok

## Create construction environment propagating the external environment
env = Environment(ENV=os.environ,
      		  CXXCOMSTR = compile_source_message,
  		  CCCOMSTR = compile_source_message,
  		  SHCCCOMSTR = compile_shared_source_message,
  		  SHCXXCOMSTR = compile_shared_source_message,
  		  ARCOMSTR = link_library_message,
  		  RANLIBCOMSTR = ranlib_library_message,
  		  SHLINKCOMSTR = link_shared_library_message,
  		  LINKCOMSTR = link_program_message,
  		  JARCOMSTR = java_library_message,
  		  JAVACCOMSTR = compile_source_message)

## Create a rootcint builder and attach it to the environment
bld = Builder(action=Action(rootcint,root_dictionary_message))
env.Append(BUILDERS = {'RootCint':bld})

## Optimization flags ##################################################
env.Append(CCFLAGS = ['-O2', '-D_FILE_OFFSET_64'])

## Finding dependencies (ROOT)
try:
    env.ParseConfig('root-config --cflags')
    env.ParseConfig('root-config --glibs')
except OSError:
    print "scons: ROOT not found!"
    exit(1)

env.Append(LINKFLAGS = ['-lGeom'])
env.Append(CPPPATH='#')
env.Append(LIBPATH='.')

envGo = env.Clone()

## Building ControlDict and libControl #################################
controlDictTarget = 'src/ControlDict.cpp'
controlDictHeaders = ['src/ControlParameters.h', 'src/LinkDefControl.h']
env.RootCint(controlDictTarget, controlDictHeaders)

controlLibTarget = 'Control'
controlLibSources = ['src/ControlDict.cpp', 'src/ControlParameters.cpp']
env.SharedLibrary(target = controlLibTarget, source = controlLibSources,
		  SHLIBPREFIX='lib')

## Building CrystalGeoDict and libCrystalGeo #################################
crystalGeoDictTarget = 'src/CrystalGeoDict.cpp'
crystalGeoDictHeaders = ['src/crystalGeometry.h', 'src/LinkDefCrystalGeometry.h']
env.RootCint(crystalGeoDictTarget, crystalGeoDictHeaders)

crystalGeoLibTarget = 'CrystalGeo'
crystalGeoLibSources = ['src/CrystalGeoDict.cpp', 'src/crystalGeometry.cpp']
env.SharedLibrary(target = crystalGeoLibTarget, source = crystalGeoLibSources,
		  SHLIBPREFIX='lib')

## Building ScrewUpData executable #####################################
unpackTarget = 'ScrewUpData'
unpackSources = ['src/ScrewUpData.cpp', 'src/Globals.cpp']
envGo.Append(LIBS=['Control', 'CrystalGeo', 'Geom'])
envGo.Program(target = unpackTarget, source = unpackSources)

