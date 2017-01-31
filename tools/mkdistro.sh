#!/bin/sh

# Create an apex distribution
TARGETPATH=/data/apex3
# local apex path
AP=/home/tom/data/apex/apex/

if [ -e $TARGETPATH ]; then
    echo "Press enter to remove contents of $TARGETPATH"
    read
    rm -fr $TARGETPATH
fi

    echo "Creating $TARGETPATH"
    mkdir $TARGETPATH

cd $TARGETPATH
mkdir apex
cd apex
mkdir bin-linux schemas config scripts data

cd bin-linux
cp $AP/bin/apex .
cp $AP/../streamapp/streamapp/.libs/* .

cd ../schemas
cp $AP/schemas/*.xsd .

cd ../config
cp $AP/config/mainconfig-template.xml ./mainconfig.xml
mkdir calibration mru

cd ../data
cp $AP/data/*.png $AP/data/*.bmp .

cd ../scripts
mkdir xslt
cp $AP/scripts/xslt/*.xsl ./xslt
cp $AP/scripts/*.pl .

echo "Copying examples..."
cd ../..
mkdir examples
cd $AP/tests
tar cf $TARGETPATH/examples/examples.tar ./ --exclude ".svn"
