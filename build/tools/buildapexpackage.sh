#!/bin/sh


targetpath=/home/tom/temp/apex-checkout/
apex=apex
version=3.0.1

rm -fr $targetpath/$apex-$version
mkdir $targetpath
svn export https://gilbert.med.kuleuven.be/svn/apex3/tags/$VERSION $targetpath/$apex-$version
cd $targetpath
tar -czf ${apex}_$version.orig.tar.gz --exclude $apex-$version/debian $apex-$version
cd $apex-$version
debuild
