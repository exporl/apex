#!/bin/bash -e

cd `dirname $0`/..

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

find data/schemas -name "*.xsd" -print -exec sed -ri 's#(targetNamespace="|xmlns:apex=")[^"]*#\1'"$NS"'#g' {} \+
