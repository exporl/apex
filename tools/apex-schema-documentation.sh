#!/bin/bash -e

cd "$(dirname $(readlink -f ${BASH_SOURCE[0]}))/.."

ROOTDIR=$(pwd)
SCHEMADIR=$ROOTDIR/data/schemas
OXYGEN=
OUTPUT="$ROOTDIR"/documentation/apex3-documentation-schemas
INDEX=false

parsecmd() {
    while [ $# -gt 0 ]; do
        #CMDSTART
        case "$1" in
        --oxygen) #
            # Oxygen home directory
            OXYGEN=${2%/}
            shift
            ;;
        -o|--output) #
            # Output directory
            OUTPUT=${2%/}
            shift
            ;;
        -i|--index) #
            # Generate markdown index page
            INDEX=true
            ;;
        -h|--help) #
            # this help
            echo "Usage: $0 [OPTION]..."
            printf "\nCommand line arguments:\n"
            sed -rn '/CMDSTART/,/CMDEND/{/\) \#|^ +# /{s/\)? #//g;s/^    //;p}}' "$0"
            exit 0
            ;;
        *)
            echo "Unknown parameter $1"
            exit 1
            ;;
        esac
        #CMDEND
        shift
    done
}

parsecmd "$@"

if [ -z "$OXYGEN" ]; then
    echo "Please specify Oxygen home directory"
    exit 1
fi
if [ ! -f "$OXYGEN/schemaDocumentation.sh" ]; then
    echo "Schema documentation tool not present in Oxygen home directory"
    exit 1
fi

if [ -z "$OUTPUT" ]; then
    OUTPUT=$SCHEMADIR
fi
if [ ! -d "$OUTPUT" ]; then
    mkdir -p $OUTPUT
fi

for SCHEMAPATH in $ROOTDIR/data/schemas/*.xsd; do
    SCHEMA=$(basename $SCHEMAPATH)
    "$OXYGEN"/schemaDocumentation.sh "$SCHEMAPATH" -format:html \
             -out:"$OUTPUT/${SCHEMA%.xsd}.html"
done

if [ $INDEX = true ]; then
    INDEXFILE="$OUTPUT/index.md"
    [ -f $INDEXFILE ] && rm $INDEXFILE
    touch $INDEXFILE

    echo "# Schema documentation" >> $INDEXFILE
    pushd $OUTPUT
    for X in `ls *.html | grep -vP "(ns|comp)"`; do
        echo "- [${X%.html}]($X)" >> $INDEXFILE
    done
    echo >> $INDEXFILE
    popd
fi
