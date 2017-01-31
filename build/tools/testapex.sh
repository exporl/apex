#!/bin/sh

# APEX 3 testbench

cd `dirname $0`/../..

echo "[Validating examples]"

for i in `find examples/ -name '*.xml' -o -name '*.apx'`; do
    r=`xmllint --noout --schema data/schemas/experiment.xsd "$i" 2>&1 >/dev/null`;
    if [ ! $? -eq 0 ] ; then
	echo $r
    fi
done
