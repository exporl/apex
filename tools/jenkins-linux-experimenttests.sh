#!/bin/bash -e

tools/jenkins-linux-build.sh "$@"

tools/linux-upgrade-examples.sh

tools/linux-experimenttests.sh --parameters "--disable-bertha"
