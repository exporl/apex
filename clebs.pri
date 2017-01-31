# Specify CLEBS if you need libraries or special build settings. Mark optional
# ones with a leading - (the directory will be compiled without it) and
# mandatory ones with a leading + (the project will fail completely).
# Specify CLEBS_INSTALL to add install commands to the make file.

# clebs/
#   external: external libraries/programs, you can define whether configuration
#             fails or a depending subdirectory is omitted from the build with
#             -/+ as described above.
#   internal: internal libraries, the .pri file needs to have the same name as
#             the subdirectory in contrib or src/lib the contains the
#             corresponding library.
#   build:    special build settings (dll, plugin, pch, etc.)

# Some macros ==================================================================

# clebsFixupSubdirs(): Configures directories of the project (dependencies,
# compile order, disabled). Compile order will be obeyed if the directory pro
# file has a CLEBS line that depends on libs in clebs/internal that have the
# same name as a directory in contrib or src/lib.
defineTest(clebsFixupSubdirs) {
    clear(CLEBS_SUBDIRS)
    clear(CLEBS_EXTERNALDEPS)
    for(subdir, SUBDIRS) {
        SUBDIRS -= $$subdir
        subdir ~= s|/$||
        pro = $$basename(subdir)
        deps = $$fromfile("$$subdir/$${pro}.pro", "CLEBS")
        libs = $$clebsInternalLibDependencies($$deps)
        external = $$clebsExternalDependencies($$deps)
        missingexternal = $$clebsMissingDependencies($$external)
        CLEBS_EXTERNALDEPS *= $$external
        contains(CLEBS_DISABLED, $$subdir) {
            CLEBS_SUBDIRS += "$${subdir}_"
            next()
        }
        !isEmpty(missingexternal) {
            CLEBS_SUBDIRS += "$${subdir}-"
            next()
        }
        CLEBS_SUBDIRS += "$${subdir}"
        name = $$replace(subdir, [/\\\\], _)
        eval($${name}.subdir = $$subdir)
        export($${name}.subdir)
        for(dep, libs):eval($${name}.depends *= $$replace(dep, [/\\\\], _))
        export($${name}.depends)
        SUBDIRS *= $$name
    }
    CLEBS_SUBDIRS = $$clebsSort($$CLEBS_SUBDIRS)
    export(SUBDIRS)
    export(CLEBS_EXTERNALDEPS)
    export(CLEBS_SUBDIRS)
}

# Returns the .pri file for a dependency
defineReplace(clebsDependencyFile) {
    1 ~= s|^[-+]||
    subdirs = clebs $$files(clebs/*)
    for(subdir, subdirs):exists($${subdir}/$${1}.pri):return($${subdir}/$${1}.pri)
    # only error out if the error occured in the top-level pro file (not included)
    profile = $$_PRO_FILE_
    !isEmpty(profile):error("Build configuration requested for unknown dependency $$1!")
    # fail gracefully
    return ""
}

# Tries to temporarily include the specified dependency include files and
# returns the missing ones (without +-)
defineReplace(clebsMissingDependencies) {
    clear(missing)
    for(dep, 1) {
        contains(dep, "^[-].*"):next()
        dep ~= s|^[-+]||
        CLEBS = $$dep
        file = $$clebsDependencyFile($$dep)
        !isEmpty(file):include($$file)
        !contains(CLEBS_DEPENDENCIES, $$dep):missing *= $$dep
    }
    return($$missing)
}

# Looks through the dependencies and returns the one that seem to refer to a lib
# in src/lib or contrib
defineReplace(clebsInternalLibDependencies) {
    clear(libs)
    for(dep, 1) {
        dep ~= s|^[-+]||
        exists(clebs/internal/$${dep}.pri) {
            libdirs = src/lib contrib
            clear(foundinclude)
            for(libdir, libdirs) {
                exists($${libdir}/$${dep}) {
                    foundinclude = true
                    libs *= $${libdir}/$${dep}
                    break()
                }
            }
            isEmpty(foundinclude):error("Unable to find internal lib $$dep!")
        }
    }
    return($$libs)
}

# Looks through the dependencies and returns the one that seem to refer to external libs
defineReplace(clebsExternalDependencies) {
    clear(libs)
    for(rawdep, 1) {
        dep = $$rawdep
        dep ~= s|^[-+]||
        exists(clebs/external/$${dep}.pri):libs *= $${rawdep}
    }
    return($$libs)
}

# Looks through the dependencies and returns the ones with a - or without a +
# in front of it
defineReplace(clebsOptionalNormalDependencies) {
    clear(result)
    for(dep, 1) {
        contains(dep, "[+].*"):next()
        dep ~= s|^[-+]||
        contains(1, "[+]$$dep"):next()
        result += $$dep
    }
    return($$clebsSort($$result))
}

# Looks through the dependencies and returns the ones with a + in front of it
defineReplace(clebsMandatoryDependencies) {
    clear(result)
    for(dep, 1) {
        !contains(dep, "[+].*"):next()
        dep ~= s|^[-+]||
        result += $$dep
    }
    return($$clebsSort($$result))
}

# Selection sort
defineReplace(clebsSort) {
    clear(result)
    list = $$1
    for(ever) {
        isEmpty(list):break()
        smallest = $$first(list)
        for(value, list):lessThan(value, $$smallest):smallest = $$value
        result += $$smallest
        list -= $$smallest
    }
    return($$result)
}

# clebsCheck(dependency): Used in clebs dependency include files to mark the
# block that determines whether a dependency is available. The block should add
# the dependency name to CLEBS_DEPENDENCIES if the dependency is available
defineTest(clebsCheck) {
    contains(CLEBS, "[-+]?$$1")|contains(CLEBS_INSTALL, "[-+]?$$1"):return(true)
    return(false)
}

# clebsInstall(dependency): Used in clebs dependency include files to mark the
# block that installs the library.
defineTest(clebsInstall) {
    1 ~= s|^[-+]||
    contains(CLEBS_INSTALL, "[-+]?$$1"):contains(CLEBS_DEPENDENCIES, "[-+]?$$1"):return(true)
    return(false)
}

# clebsDependency(dependency): Used in clebs dependency include files to mark
# the block that does the setup for a dependency like adding include dirs and
# library dependencies. If the library in turn requires other additional
# libraries, add them to the CLEBS variable.
defineTest(clebsDependency) {
    1 ~= s|^[-+]||
    contains(CLEBS, "[-+]?$$1"):contains(CLEBS_DEPENDENCIES,  "[-+]?$$1"):return(true)
    return(false)
}

# clebsCheckQtVersion(major,minor,patch): Checks whether the current Qt version
# is larger than or equal to the given version consisting of major, minor and
# patch revision.
defineTest(clebsCheckQtVersion) {
    version = $$[QT_VERSION]
    major = $$section(version, '.', 0, 0)
    minor = $$section(version, '.', 1, 1)
    patch = $$section(version, '.', 2, 2)
    greaterThan(major,$$1):return(true)
    lessThan(major,$$1):return(false)
    greaterThan(minor,$$2):return(true)
    lessThan(minor,$$2):return(false)
    greaterThan(patch,$$3):return(true)
    lessThan(patch,$$3):return(false)
    return(true)
}

defineTest(clebsCheckDependencies) {
    clear(somethingmissing)
    missing = $$clebsMissingDependencies($$1)
    !isEmpty(missing):error("Please install the necessary (devel) packages for $$missing!")
}

defineTest(clebsPrintDependencies) {
    !isEmpty(2) {
        message("------------------------------------")
        message("$$1 dependencies:")
        message("------------------------------------")
        for(dep, 2) {
            found = "no"
            missing = $$clebsMissingDependencies($$dep)
            isEmpty(missing):found = "yes"
            temp = "Looking for $$dep ________________"
            temp ~= s/(.{31}).*/\\1: $$found/
            temp ~= s/_/ /
            message($$temp)
        }
    }
}

defineTest(clebsPrintSubdirs) {
    !isEmpty(1) {
        message("------------------------------------")
        message("Build overview:")
        message("------------------------------------")
        clear(oldpath)
        clear(indent)
        for(dir, 1) {
            build = "yes"
            contains(dir, ".*-$"):build = "no"
            contains(dir, ".*_$"):build = "disabled"
            dir ~= s|[-_]$||
            name = $$section(dir, '/', -1, -1)
            path = $$section(dir, '/', 0, -2)
            oldtemppath = $$oldpath
            clear(oldpath)
            clear(indent)
            !isEmpty(path):for(ever) {
                dirpart = $$section(path, '/', 0, 0)
                olddirpart = $$section(oldtemppath, '/', 0, 0)
                isEqual(dirpart, $$olddirpart) {
                } else {
                    clebsPrintSubdir("$$indent$$dirpart/")
                }
                indent = "$${indent}__"
                isEmpty(oldpath) {
                    oldpath = $$dirpart
                } else {
                    oldpath = "$$oldpath/$$dirpart"
                }
                path = $$section(path, '/', 1, -1)
                oldtemppath = $$section(oldtemppath, '/', 1, -1)
                isEmpty(path):break()
            }
            clebsPrintSubdir("$$indent$$name", $$build)
        }
    }
}

defineTest(clebsPrintSubdir) {
    dir = $$1
    build = $$2
    temp = "$$dir ____________________________"
    isEmpty(build) {
        temp ~= s/(.{31}).*/\\1/
    } else {
        temp ~= s/(.{31}).*/\\1: $$build/
    }
    temp ~= s/_/ /
    message($$temp)
}

# General settings =============================================================

# base dir as needed by the .pri files and localconfig.pri
BASEDIR = $$PWD

isEmpty(LOCALCONFIG):LOCALCONFIG = localconfig.pri
exists($$LOCALCONFIG):include($$LOCALCONFIG)

# Calculate the relative path to the .pro file
profile = $$_PRO_FILE_ # do not remove, _PRO_FILE_ needs to be copied
!isEmpty(profile):PRORELPATH = $$_PRO_FILE_PWD_
PRORELPATH ~= s|^/||
pwdparts = $$split(PWD, '/')
for(part, pwdparts):PRORELPATH = $$section(PRORELPATH, '/', 1)

# Calculate the package name
mainprofiles = $$files($$BASEDIR/*.pro)
mainprofile = $$first(mainprofiles)
mainprofile = $$basename(mainprofile)
PACKAGE = $$section(mainprofile, '.', 0, 0)

CONFIG += stl warn_on exceptions thread rtti silent
CONFIG += debug
CONFIG -= release
CONFIG -= debug_and_release

!isEmpty(RELEASE) {
    CONFIG += release
    CONFIG -= debug
}

QT -= gui
# DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII # Apex change

CONFIG(release, debug|release) {
    DESTDIR = $$PWD/bin/release
    OBJECTS_DIR = $$PWD/.build/release/$$PRORELPATH
} else {
    DESTDIR = $$PWD/bin/debug
    OBJECTS_DIR = $$PWD/.build/debug/$$PRORELPATH
}
MOC_DIR = $$OBJECTS_DIR/moc
UI_DIR = $$OBJECTS_DIR/ui
RCC_DIR = $$OBJECTS_DIR/rcc
unix {
    isEmpty(PREFIXDIR):PREFIXDIR = /usr/local
    isEmpty(CONFDIR):CONFDIR = /etc
    isEmpty(DATADIR):DATADIR = $$PREFIXDIR/share/$$PACKAGE
    isEmpty(DOCDIR):DOCDIR = $$PREFIXDIR/share/doc/$$PACKAGE
    isEmpty(APPDIR):APPDIR = $$PREFIXDIR/share/applications
    isEmpty(ICONDIR):ICONDIR = $$PREFIXDIR/share/icons/hicolor
    isEmpty(BINDIR):BINDIR = $$PREFIXDIR/bin
    isEmpty(LIBDIR):LIBDIR = $$PREFIXDIR/lib
    isEmpty(PLUGINDIR):PLUGINDIR = $$PREFIXDIR/lib/$$PACKAGE
    isEmpty(BINARYPLUGINDIR):BINARYPLUGINDIR = $$PLUGINDIR # Apex change
}
win32 {
    isEmpty(PREFIXDIR):PREFIXDIR = $${DESTDIR}-installed
    isEmpty(CONFDIR):CONFDIR = $$PREFIXDIR/config # Apex change
    isEmpty(DATADIR):DATADIR = $$PREFIXDIR/ # Apex change
    isEmpty(DOCDIR):DOCDIR = $$PREFIXDIR/doc
    isEmpty(ICONDIR):ICONDIR = $$PREFIXDIR/icons
    isEmpty(BINDIR):BINDIR = $$PREFIXDIR/bin
    isEmpty(LIBDIR):LIBDIR = $$PREFIXDIR/bin
    # the same as the executables so that SxS dlls also work for plugins
#    isEmpty(PLUGINDIR):PLUGINDIR = $$PREFIXDIR/bin # Apex change
    isEmpty(PLUGINDIR):PLUGINDIR = $$PREFIXDIR/plugins # Apex change
    isEmpty(BINARYPLUGINDIR):BINARYPLUGINDIR = $$BINDIR # Apex change
}

# Programs may need internal libraries...
LIBS *= -L$$DESTDIR
# ...and these libraries may need more internal libraries
unix:QMAKE_LFLAGS *=-Wl,-rpath-link=$$DESTDIR
# For includes from .ui files on MinGW
INCLUDEPATH *= .
DEPENDPATH *= .

# Install to the binary directory per default
target.path = $$BINDIR
INSTALLS *= target

# mingw creates scripts if there are too many source files
win32-g++|win32-x-g++*:QMAKE_CLEAN *= object_script.*

CONFIG(debug, debug|release):DEFINES += APEXDEBUG # Apex change

unix {
    CONFIG += link_pkgconfig
    !MAC:DEFINES += LINUX # Apex change
    #QMAKE_CXXFLAGS_DEBUG *= -O2

    # Code coverage tests with gcov
    #QMAKE_CXXFLAGS_DEBUG *= -fprofile-arcs -ftest-coverage
    #LIBS *= -lgcov
}

win32 {
    CONFIG += embed_manifest_exe

    # Creates a console window so we see debug messages
    CONFIG += console # Apex change
    DEFINES -= UNICODE # Apex change

    # STL iterators and fopen are unsafe, we know that
    DEFINES *= _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS
    # we want debugging symbol files (for visual studio)
    !win32-g++: QMAKE_CXXFLAGS += /Zi # Apex change
    !win32-g++: QMAKE_LFLAGS += /INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF # Apex change
}

# Fixup subdir compile order and dependencies. check dependencies  =============

clebsFixupSubdirs()
!isEmpty(CLEBS_EXTERNALDEPS) {
    CONFIG(release, debug|release) {
        message("------------------------------------")
        message("BUILDING IN RELEASE MODE")
    }
    clebsPrintDependencies("Required", $$clebsMandatoryDependencies($$CLEBS_EXTERNALDEPS))
    clebsPrintDependencies("Optional", $$clebsOptionalNormalDependencies($$CLEBS_EXTERNALDEPS))
    clebsCheckDependencies($$clebsMandatoryDependencies($$CLEBS_EXTERNALDEPS))
    clebsPrintSubdirs($$CLEBS_SUBDIRS)
    message("------------------------------------")
}

# Linking against libraries ====================================================

alldeps = $$CLEBS $$CLEBS_INSTALL
for(ever) {
    isEmpty(alldeps):break()
    dep = $$first(alldeps)
    alldeps -= $$dep
    contains(result, $$replace(dep, "^[-+]?", "[-+]?")):next()
    result += $$dep
    file = $$clebsDependencyFile($$dep)
    !isEmpty(file):include($$file)
    alldeps += $$CLEBS $$CLEBS_INSTALL
}
