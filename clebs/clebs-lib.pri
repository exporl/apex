# Specify CLEBS if you need libraries or special build settings. Mark optional
# ones with a leading - (the directory will be compiled without it) and
# mandatory ones with a leading + (the project will fail completely).
# Specify CLEBS_INSTALL to add install commands to the make file.

# clebs/
#   external: external libraries/programs, you can define whether configuration
#             fails or a depending subdirectory is omitted from the build with
#             -/+ as described above.
#   build:    special build settings (dll, plugin, pch, etc.)
#
# Internal libraries must be located in a subdirectory of */lib and can be
# referenced in CLEBS without a pri file in clebs/. All */lib directories are
# added to INCLUDEPATH and DEPENDPATH so that include files are available. If
# your internal libraries needs special settings in its users (e.g. because the
# include files reference other .h files from another library), add a .pri file
# next to the .pro file for the library with the settings needed.
#
# In the base directory, targets for test, testxml and testinstalledxml are
# added that execute all testbenches defined in subdirectories of */tests.

# Some macros ==================================================================

# clebsFixupSubdirs(): Configures directories of the project (dependencies,
# compile order, disabled). Compile order will be obeyed if the directory pro
# file has a CLEBS line that depends on libs in */lib that contains a pri file
# next to the pro file.
defineTest(clebsFixupSubdirs) {
    clear(CLEBS_SUBDIRS)
    clear(CLEBS_EXTERNALDEPS)
    for(tree, CLEBS_TREES) {
        subdirs = $$files("$${BASEDIR}/$${tree}/*")
        for(subdir, subdirs) {
            shortsubdir = $$basename(subdir)
            subsubdirs = $$files("$${BASEDIR}/$${tree}/$${shortsubdir}/*")
            for(subsubdir, subsubdirs) {
                shortsubsubdir = $$basename(subsubdir)
                SUBDIRS *= "$${tree}/$${shortsubdir}/$${shortsubsubdir}"
            }
        }
    }
    for(subdir, SUBDIRS) {
        SUBDIRS -= $$subdir
        subdir ~= s|/$||
        subdir ~= s|\\\\|/|
        fullsubdir = "$${BASEDIR}/$${subdir}"
        pro = $$basename(subdir)
        !exists("$${fullsubdir}/$${pro}.pro"):next()
        deps = $$fromfile("$${fullsubdir}/$${pro}.pro", "CLEBS")
        win32:deps *= $$fromfile("$${fullsubdir}/$${pro}.pro", "win32_CLEBS")
        unix:deps *= $$fromfile("$${fullsubdir}/$${pro}.pro", "unix_CLEBS")
        libs = $$clebsInternalLibDependencies($$deps)
        external = $$clebsExternalDependencies($$deps)
        !isEmpty(libs):external *= -chrpath
        missing = $$clebsMissingDependencies($$external $$libs)
        CLEBS_EXTERNALDEPS *= $$external
        contains(CLEBS_DISABLED, $$subdir) {
            CLEBS_SUBDIRS += "$${subdir}_"
            next()
        }
        !isEmpty(missing) {
            CLEBS_SUBDIRS += "$${subdir}+$${missing}-"
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
    internal = $$clebsInternalLibDependencies($$1)
    isEmpty(internal) {
        subdirs = "$${BASEDIR}/clebs" $$files("$${BASEDIR}/clebs/*")
        for(subdir, subdirs):exists("$${subdir}/$${1}.pri"):return("$${subdir}/$${1}.pri")
    } else {
        pri = $$basename(internal)
        prifile = "$${BASEDIR}/$${internal}/$${pri}.pri"
        exists($$prifile):return($$prifile)
        return("")
    }
    # only error out if the error occured in the top-level pro file (not included)
    !isEmpty(_PRO_FILE_):error("Build configuration requested for unknown dependency $$1!")
    # fail gracefully
    return("")
}

# Recursively tries to figure out which dependencies are available
# returns the missing ones (without +-)
defineReplace(clebsMissingDependencies) {
    clear(missing)
    alldeps = $$1
    for(ever) {
        isEmpty(alldeps):break()
        dep = $$first(alldeps)
        alldeps -= $$dep
        contains(dep, "^[-].*"):next()
        dep ~= s|^[-+]||
        dep ~= s|.*/||
        contains(CLEBS_AVAILABLEDEPS, $$dep):next()
        contains(CLEBS_MISSINGDEPS, $$dep) {
            missing *= $$dep
            next()
        }
        lib = $$clebsInternalLibDependencies($$dep)
        external = $$clebsExternalDependencies($$dep)
        !isEmpty(external) {
            CLEBS = $$dep
            file = $$clebsDependencyFile($$dep)
            !isEmpty(file):include($$file)
            !contains(CLEBS_DEPENDENCIES, $$dep) {
                missing *= $$dep
                CLEBS_MISSINGDEPS *= $$dep
                export(CLEBS_MISSINGDEPS)
            } else {
                CLEBS_AVAILABLEDEPS *= $$dep
                export(CLEBS_AVAILABLEDEPS)
            }
        } else:!isEmpty(lib) {
            alldeps *= $$fromfile("$${BASEDIR}/$${lib}/$${dep}.pro", "CLEBS")
            win32:alldeps *= $$fromfile("$${BASEDIR}/$${lib}/$${dep}.pro", "win32_CLEBS")
            unix:alldeps *= $$fromfile("$${BASEDIR}/$${lib}/$${dep}.pro", "unix_CLEBS")
        }
    }

    return($$missing)
}

# Determine target for a given dependency
defineReplace(clebsTarget) {
    pro = $$basename(1)
    target = $$fromfile("$${BASEDIR}/$${1}/$${pro}.pro", "TARGET")
    isEmpty(target):target = $$pro
    return($$target)
}

# Looks through the dependencies and returns the one that seem to refer to a lib
# in */lib
defineReplace(clebsInternalLibDependencies) {
    clear(libs)
    for(dep, 1) {
        dep ~= s|^[-+]||
        contains(CLEBS_NOLIBDEPS, $$dep):next()
        clear(found)
        for(libdir, CLEBS_TREES) {
            exists("$${BASEDIR}/$${libdir}/lib/$${dep}") {
                shortlib = $$basename(libdir)
                libs *= "$${shortlib}/lib/$${dep}"
                found = 1
                break()
            }
        }
        isEmpty(found) {
            CLEBS_NOLIBDEPS *= $$dep
            export(CLEBS_NOLIBDEPS)
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
        exists("$${BASEDIR}/clebs/external/$${dep}.pri"):libs *= $${rawdep}
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
        message("----------------------------------------------")
        message("$$1 dependencies:")
        message("----------------------------------------------")
        for(dep, 2) {
            found = "no"
            missing = $$clebsMissingDependencies($$dep)
            isEmpty(missing):found = "yes"
            temp = "Looking for $$dep __________________________"
            temp ~= s/(.{41}).*/\\1: $$found/
            temp ~= s/_/ /
            message($$temp)
        }
    }
}

defineTest(clebsPrintSubdirs) {
    !isEmpty(1) {
        message("----------------------------------------------")
        message("Build overview:")
        message("----------------------------------------------")
        clear(oldpath)
        clear(indent)
        for(dir, 1) {
            build = "yes"
            contains(dir, ".*-$") {
                missing = $$section(dir, '+', 1, 1)
                missing ~= s/-$//
                build = "no ($$missing)"
            }
            contains(dir, ".*_$"):build = "disabled"
            dir ~= s/([-_]|\\+.*)$//
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
    temp = "$$dir ______________________________________"
    isEmpty(build) {
        temp ~= s/(.{41}).*/\\1/
    } else {
        temp ~= s/(.{41}).*/\\1: $$build/
    }
    temp ~= s/_/ /
    message($$temp)
}

defineTest(clebsAddTestTarget) {
    unix:xvfb = "xvfb-run -s \\\"-screen  0 1024x768x16\\\" -a"
    test.commands = @echo testing...;
    for(alldir, CLEBS_TREES) {
        shortalldir = $$basename(alldir)
        testdirs = $$files("$${BASEDIR}/$${alldir}/tests/*")
        for(testdir, testdirs) {
            pro = $$basename(testdir)
            shorttestdir = "$${shortalldir}/tests/$${pro}"
            !exists("$${BASEDIR}/$${shorttestdir}/$${pro}.pro"):next()
            contains(CLEBS_DISABLED, $${shorttestdir}):next()
            target = $$clebsTarget($$shorttestdir)
            eval(test_$${target}.commands = -@echo [tests] Entering dir "\\\\\\'$${shorttestdir}\\\\\\'" && $${xvfb} $${DESTDIR}/$${target} | $${DESTDIR}/testcolorizer)
            eval(testxml_$${target}.commands = -$${xvfb} $${DESTDIR}/$${target} -xunitxml -o $${target}-results.xml)
            eval(testinstalledxml_$${target}.commands = -$${xvfb} $${BINDIR}/$${target} -xunitxml -o $${target}-results.xml)
            win32:eval(test_$${target}.commands ~= s|/|\\\\|g)
            win32:eval(testxml_$${target}.commands ~= s|/|\\\\|g)
            win32:eval(testinstalledxml_$${target}.commands ~= s|/|\\\\|g)
            export(test_$${target}.commands)
            export(testxml_$${target}.commands)
            export(testinstalledxml_$${target}.commands)
            QMAKE_EXTRA_TARGETS *= test_$${target} testxml_$${target} testinstalledxml_$${target}
            test.depends *= test_$${target}
            testxml.depends += testxml_$${target}
            testinstalledxml.depends += testinstalledxml_$${target}
        }
    }
    export(test.depends)
    export(testxml.depends)
    export(testinstalledxml.depends)
    QMAKE_EXTRA_TARGETS *= test testxml testinstalledxml
    export(QMAKE_EXTRA_TARGETS)
}

defineTest(clebsAddFormatTarget) {
    format_clang_format.commands = -@$(COPY) clebs/.clang-format .;
    export(format_clang_format.commands)
    for(formatdir, CLEBS_SUBDIRS) {
        formatdir ~= s/([-_]|\\+.*)$//
        target = $${formatdir}
        target ~= s/[^A-Za-z]/_/
        unix:eval(format_$${target}.commands = -@find $${formatdir} -iname '*.cpp' -o -iname '*.h' -exec clang-format -i {} \;)
        export(format_$${target}.commands)
        eval(format_$${target}.depends *= format_clang_format)
        export(format_$${target}.depends)
        QMAKE_EXTRA_TARGETS *= format_$${target}
        format.depends *= format_$${target}
    }
    export(format.depends)
    QMAKE_EXTRA_TARGETS *= format format_clang_format
    export(QMAKE_EXTRA_TARGETS)
}

# General settings =============================================================

# Base dir as needed by the .pri files and localconfig.pri
BASEDIR = $$dirname(PWD)

# Current pro files
mainprofiles = $$files($$BASEDIR/*.pro)
mainprofile = $$first(mainprofiles)
mainprofile = $$basename(mainprofile)
equals(_PRO_FILE_, "$${BASEDIR}/$${mainprofile}"):CLEBS_MAINPROFILE = 1

# Calculate the package name
PACKAGE = $$section(mainprofile, '.', 0, 0)

# Get trees from main pro file
isEmpty(CLEBS_MAINPROFILE) {
    CLEBS_TREES = $$fromfile("$${BASEDIR}/$${mainprofile}", "CLEBS_TREES")
}

# Calculate the relative path to the .pro file, relative base dir
!isEmpty(_PRO_FILE_):PRORELPATH = $$_PRO_FILE_PWD_
PRORELPATH ~= s|^/||
pwdparts = $$split(BASEDIR, '/')
for(part, pwdparts):PRORELPATH = $$section(PRORELPATH, '/', 1)
RELBASEDIR = $$replace(PRORELPATH, "[^/]+", "..")

# Include local config
isEmpty(LOCALCONFIG):LOCALCONFIG = localconfig.pri
exists("$${BASEDIR}/$${LOCALCONFIG}"):include("$${BASEDIR}/$${LOCALCONFIG}")

CONFIG += stl warn_on exceptions thread rtti silent
CONFIG += debug
CONFIG -= release
CONFIG -= debug_and_release

!isEmpty(RELEASE) {
    CONFIG += release
    CONFIG -= debug
}

QT -= gui
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

# Deprecate anything before the Qt version in trusty
DEFINES *= Q_DISABLE_DEPRECATED_BEFORE=0x050200

android:DIRECTORY_PREFIX=android-
CONFIG(release, debug|release) {
    DESTDIR = $${BASEDIR}/bin/$${DIRECTORY_PREFIX}release
    OBJECTS_DIR = $${BASEDIR}/.build/$${DIRECTORY_PREFIX}release/$$PRORELPATH
} else {
    DESTDIR = $${BASEDIR}/bin/$${DIRECTORY_PREFIX}debug
    OBJECTS_DIR = $${BASEDIR}/.build/$${DIRECTORY_PREFIX}debug/$$PRORELPATH
}
MOC_DIR = $$OBJECTS_DIR/moc
UI_DIR = $$OBJECTS_DIR/ui
RCC_DIR = $$OBJECTS_DIR/rcc
android {
    # Uses INSTALL_ROOT instead of PREFIX because target path is hardcoded
    isEmpty(CONFDIR):CONFDIR = /etc
    isEmpty(DATADIR):DATADIR = /assets
    isEmpty(DOCDIR):DOCDIR = /assets/doc
    isEmpty(APPDIR):APPDIR = /assets/applications
    isEmpty(MANDIR):MANDIR = /assets/man
    isEmpty(ICONDIR):ICONDIR = /assets/icons
    isEmpty(BINDIR):BINDIR = /libs/armeabi-v7a
    isEmpty(LIBDIR):LIBDIR = /libs/armeabi-v7a
    isEmpty(INCLUDEDIR):INCLUDEDIR = /assets/include
    isEmpty(PLUGINDIR):PLUGINDIR = /libs/armeabi-v7a
} else:unix {
    isEmpty(PREFIX):PREFIX = /usr/local
    isEmpty(CONFDIR):CONFDIR = /etc
    isEmpty(DATADIR):DATADIR = $$PREFIX/share/$$PACKAGE
    isEmpty(DOCDIR):DOCDIR = $$PREFIX/share/doc/$$PACKAGE
    isEmpty(APPDIR):APPDIR = $$PREFIX/share/applications
    isEmpty(MANDIR):MANDIR = $$PREFIX/share/man
    isEmpty(ICONDIR):ICONDIR = $$PREFIX/share/icons/hicolor
    isEmpty(BINDIR):BINDIR = $$PREFIX/bin
    isEmpty(LIBDIR):LIBDIR = $$PREFIX/lib
    isEmpty(INCLUDEDIR):INCLUDEDIR = $$PREFIX/include/$$PACKAGE
    isEmpty(PLUGINDIR):PLUGINDIR = $$PREFIX/lib/$$PACKAGE
} else:win32 {
    isEmpty(PREFIX):PREFIX = $${DESTDIR}-installed
    isEmpty(CONFDIR):CONFDIR = $$PREFIX/etc
    isEmpty(DATADIR):DATADIR = $$PREFIX
    isEmpty(DOCDIR):DOCDIR = $$PREFIX/doc
    isEmpty(MANDIR):MANDIR = $$PREFIX/man
    isEmpty(ICONDIR):ICONDIR = $$PREFIX/icons
    isEmpty(BINDIR):BINDIR = $$PREFIX/bin
    isEmpty(LIBDIR):LIBDIR = $$PREFIX/bin
    # the same as the executables so that SxS dlls also work for plugins
    isEmpty(PLUGINDIR):PLUGINDIR = $$PREFIX/bin
}

# Programs may need internal libraries...
LIBS *= -L$$DESTDIR
# ...and these libraries may need more internal libraries
unix:QMAKE_LFLAGS *=-Wl,-rpath-link=$$DESTDIR
# For includes from .ui files on MinGW
INCLUDEPATH *= .
DEPENDPATH *= .
# If there are headers and sources, use them by default
headers = $$files("$${_PRO_FILE_PWD_}/*.h")
sources = $$files("$${_PRO_FILE_PWD_}/*.cpp") $$files("$${_PRO_FILE_PWD_}/*.c")
HEADERS *= $$replace(headers, "(^| )$${_PRO_FILE_PWD_}/", "\\1")
SOURCES *= $$replace(sources, "(^| )$${_PRO_FILE_PWD_}/", "\\1")
# Help a bit with dependencies between internal libraries
for(libdir, CLEBS_TREES):exists("$${BASEDIR}/$${libdir}/lib") {
    INCLUDEPATH *= "$${BASEDIR}/$${libdir}/lib"
    DEPENDPATH *= "$${BASEDIR}/$${libdir}/lib"
}

# Install to the binary directory per default
target.path = $$BINDIR
INSTALLS *= target

# mingw creates scripts if there are too many source files
win32-g++|win32-x-g++*:QMAKE_CLEAN *= object_script.*

CONFIG(debug, debug|release):DEFINES += CLEBS_DEBUG

unix {
    CONFIG += link_pkgconfig
    QMAKE_CXXFLAGS *= -std=c++11
    #QMAKE_CXXFLAGS_DEBUG *= -O2

    # Code coverage tests with gcov
    #QMAKE_CXXFLAGS_DEBUG *= -fprofile-arcs -ftest-coverage
    #LIBS *= -lgcov
}

win32 {
    CONFIG += embed_manifest_exe

    # Creates a console window so we see debug messages
    CONFIG(debug, debug|release):CONFIG += console

    # STL iterators and fopen are unsafe
    DEFINES *= _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS
    # Template warnings
    !win32-g++: QMAKE_CXXFLAGS += /wd4661

    # we want debugging symbol files (for visual studio)
    !win32-g++: QMAKE_CXXFLAGS += /Zi
    !win32-g++: QMAKE_LFLAGS += /INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF
}

# use a project config file if something of the above needs to be changed
exists("$${BASEDIR}/projectconfig.pri"):include("$${BASEDIR}/projectconfig.pri")

# Fixup subdir compile order and dependencies. check dependencies  =============

!isEmpty(CLEBS_MAINPROFILE) {
    clebsFixupSubdirs()
    CONFIG(release, debug|release) {
        message("----------------------------------------------")
        message("BUILDING IN RELEASE MODE")
    }
    clebsPrintDependencies("Required", $$clebsMandatoryDependencies($$CLEBS_EXTERNALDEPS))
    clebsPrintDependencies("Optional", $$clebsOptionalNormalDependencies($$CLEBS_EXTERNALDEPS))
    clebsCheckDependencies($$clebsMandatoryDependencies($$CLEBS_EXTERNALDEPS))
    clebsPrintSubdirs($$CLEBS_SUBDIRS)
    message("----------------------------------------------")
    CONFIG(release, debug|release) {
        message("BUILDING IN RELEASE MODE")
        message("----------------------------------------------")
    }
    clebsAddTestTarget()
    clebsAddFormatTarget()
}

# Linking against libraries ====================================================

win32:CLEBS *= $$win32_CLEBS
unix:CLEBS *= $$unix_CLEBS
alldeps = $$CLEBS $$CLEBS_INSTALL
for(ever) {
    isEmpty(alldeps):break()
    dep = $$first(alldeps)
    alldeps -= $$dep
    contains(result, $$replace(dep, "^[-+]?", "[-+]?")):next()
    result += $$dep
    internal = $$clebsInternalLibDependencies($$dep)
    file = $$clebsDependencyFile($$dep)
    !isEmpty(file):include($$file)
    !isEmpty(internal) {
        LIBS *= -L$$DESTDIR -l$$clebsTarget($$internal)
        CLEBS *= -chrpath
    }
    alldeps += $$CLEBS $$CLEBS_INSTALL
}
