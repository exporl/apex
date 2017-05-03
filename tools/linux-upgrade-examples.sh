#!/bin/bash -e

cd "$(dirname ${BASH_SOURCE[0]})/.."

UPGRADER=bin/debug/apexupgrader

"$UPGRADER" -i data/config/*.xml
find examples -iname '*.apx' -exec "$UPGRADER" -i {} \;
