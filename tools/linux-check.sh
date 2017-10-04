#!/bin/bash -e

cd "$(dirname $(readlink -f ${BASH_SOURCE[0]}))/.."

exec common/tools/linux-check.sh "$@"
