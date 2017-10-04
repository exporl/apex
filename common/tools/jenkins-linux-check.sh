#!/bin/bash -e

cd "$(dirname ${BASH_SOURCE[0]})/../.."

common/tools/linux-check.sh "$@" --continue
