#!/bin/sh
cd ~/data/apex/apex/src
svn add *cpp *h

for i in *; do svn add $i/*.cpp; done;
for i in *; do svn add $i/*.h; done;

svn commit
