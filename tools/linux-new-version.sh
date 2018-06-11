#!/bin/bash -e

cd `dirname $0`/..

ROOTDIR=$(pwd)
VERSION_H="src/lib/apextools/version.h"
LAST_MODIFIED=$(stat -c "%Y" "$VERSION_H")

sensible-editor "$VERSION_H"

if [ "$LAST_MODIFIED" = "$(stat -c "%Y" "$VERSION_H")" ]; then
    echo "$VERSION_H not modified, aborting"
    exit 1
fi

echo "Updating schema versions"

APEX_SCHEMA_VERSION=$(cat "$VERSION_H" | grep '#define APEX_SCHEMA_VERSION' | cut -f 3 -d ' ' | sed 's/"//g')
NS="http://med.kuleuven.be/exporl/apex/$APEX_SCHEMA_VERSION/experiment"
SL="http://med.kuleuven.be/exporl/apex/$APEX_SCHEMA_VERSION/experiment https://exporl.med.kuleuven.be/apex/schemas/$APEX_SCHEMA_VERSION"

find data/schemas -name "*.xsd" -print -exec sed -ri 's#(targetNamespace="|xmlns:apex=")[^"]*#\1'"$NS"'#g' {} \+

find data/config -name "*.xml" -print -exec sed -ri 's#(xmlns:apex=")[^"]*#\1'"$NS"'#g' {} \+
find data/config -name "*.xml" -print -exec sed -ri 's#(schemaLocation=")[^"]*/([a-z]*.xsd)#\1'"$SL/\2"'#g' {} \+

find examples -name "*.xml" -o -name "*.apx" -print -exec sed -ri 's#(xmlns:apex=")[^"]*#\1'"$NS"'#g' {} \+
find examples -name "*.xml" -o -name "*.apx" -print -exec sed -ri 's#(schemaLocation=")[^"]*/([a-z]*.xsd)#\1'"$SL/\2"'#g' {} \+

sed -ri 's#( Version=")[^"]*#\1'"$APEX_SCHEMA_VERSION"'#g' tools/jenkins-windows-wix-main.wxs
sed -ri  's#(apex_)[0-9.x]*(msi)#\1'"$APEX_SCHEMA_VERSION"'.\2#g' tools/jenkins-windows-build.bat

"$ROOTDIR"/tools/apex-schema-documentation.sh "$@"
