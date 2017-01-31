error_trap() {
    echo "Trapped an unexpected error" 1>&2
    exit 1
}

trap error_trap ERR


cd /tmp
rm -rf build-environment
mkdir build-environment
cd build-environment

echo 'export BUILD_BASE=`pwd`' > setup-vars.sh
echo 'export STREAMAPPBUILDDIR="$BUILD_BASE/streamapp/obj"' >> setup-vars.sh
echo 'export STREAMAPPDIR="$BUILD_BASE/streamapp/srcV2"' >> setup-vars.sh
echo 'export STREAMAPPLIB="streamapp-2.0"' >> setup-vars.sh
echo 'export STREAMAPPLIBDIR="$BUILD_BASE/streamapp/.libs"' >> setup-vars.sh
chmod +x setup-vars.sh
. ./setup-vars.sh

cd $BUILD_BASE
mkdir streamapp
cd streamapp
svn co svn://134.58.34.2:3690/svn_stijn/srcV2 srcV2
mkdir objd
cd srcV2
cp config-template.pro config.pro
# Linux has different opinions on include paths
sed -i 's/INCLUDEPATH += $(QWTDIR)\/include/unix {\n        INCLUDEPATH += $(QWTDIR)\n        } else {\n        &\n        }/' srcV2.pro
# No QWT for use with QT4
sed -i 's/CONFIG += USEQWT/#&/' config.pro

cd $BUILD_BASE
mkdir apex
cd apex
svn co https://gilbert.med.kuleuven.be/svn/apex3/trunk .
mkdir apex/bin
mkdir apex/debug
ln -s ../bin apex/debug/bin
cp apex/config/mainconfig-template.xml apex/config/mainconfig.xml
cd apex/src
cp localconfig-template.pro localconfig.pro

cd $BUILD_BASE/streamapp/srcV2
qmake-qt4
make
cd $BUILD_BASE/apex/apex/src
qmake-qt4
make
