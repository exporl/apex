#!/bin/sh

# update the apex matlab toolbox (only on Tom's computer :( )

TARGETPATH=/home/tom/data/apex/apex/amt/scratch
rm -fr $TARGETPATH
sh /home/tom/matlab/apex/makeAMT.sh $TARGETPATH
