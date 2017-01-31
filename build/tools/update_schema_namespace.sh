#!/bin/sh

# Update schema namespace to current version number

cd `dirname $0`


NS="http://med.kuleuven.be/exporl/apex/`cat ../../VERSION_SCHEMA`/experiment";
NSC="http://med.kuleuven.be/exporl/apex/`cat ../../VERSION_SCHEMA`/config";
GIL="https://gilbert.med.kuleuven.be/apex/schemas/`cat ../../VERSION_SCHEMA`/experiment.xsd";
GILC="https://gilbert.med.kuleuven.be/apex/schemas/`cat ../../VERSION_SCHEMA`/apexconfig.xsd";

RS="http://med.kuleuven.be/exporl/apex/result";

for i in `find ../../data/schemas -name "*.xsd"`; do
echo $i
    if [ "$i" = "../../data/schemas/apexconfig.xsd" ]; then
	sed -i "s|\\(targetNamespace=\"\\)[^\"]*|\\1$NSC|" "$i"
	sed -i "s|\\(xmlns:apex=\"\\)[^\"]*|\\1$NSC|" $i
    else
        sed -i "s|\\(targetNamespace=\"\\)[^\"]*|\\1$NS|" "$i"
	sed -i "s|\\(xmlns:apex=\"\\)[^\"]*|\\1$NS|" "$i"
    fi
    sed -i 's/\r//g'  "$i"
done;


for i in `find ../../examples -name "*.xml" -or -name "*.apx"`; do
    echo $i
    sed -i "s|\\(schemaLocation=\"\\)[^\"]*|\\1$NS $GIL|" "$i"
    sed -i "s|\\(xmlns:apex=\"\\)[^\"]*|\\1$NS|" "$i"
done

for i in `find ../../data/xslt -name "*.xsl"`; do
    echo $i
    sed -i "s|\\(xmlns:apex=\"\\)[^\"]*|\\1$RS|" "$i"
done


CF="../../data/config/apexconfig.xml";
sed -i "s|\\(schemaLocation=\"\\)[^\"]*|\\1$NSC $GILC|" $CF
sed -i "s|\\(xmlns:apex=\"\\)[^\"]*|\\1$NSC|" $CF
