#!/bin/bash

APEXEXECUTABLE=
ACTION=test
REFERENCEURL=https://github.com/exporl/apex-test-reference
CLEAN=false
SKIP=false
VERBOSE=false
NORESULTS=false
JOBS=4
TIMEOUT=800
EXPERIMENTS=
TAPFILE=
EXTRAPARAMS=
CONTINUOUSCOMPARE=

parsecmd() {
    while [ $# -gt 0 ]; do
        #CMDSTART
        case "$1" in
        --executable) # [EXECUTABLE]
            # the apex executable to use; the default is the corresponding debug build
            APEXEXECUTABLE=$2
            shift
            ;;
        -a|--action) # [test|store]
            # either test against preexisting results or store the results for later testing
            ACTION=$2
            shift
            ;;
        --reference) # [URL]
            # download a custom reference tarball
            REFERENCEURL=$2
            shift
            ;;
        -c|--clean) #
            # clean all temporary and reference files
            CLEAN=true
            ;;
        -s|--skip) #
            # remember successful experiments and skip them on the next run
            SKIP=true
            ;;
        -v|--verbose) #
            # show the apex output of unsuccessful tests
            VERBOSE=true
            ;;
        --noresults) #
            # never check the result files
            NORESULTS=true
            ;;
        -t|--timeout) # [SECONDS]
            # use a custom timeout instead of the standard 800s
            TIMEOUT=$2
            shift
            ;;
        -e|--experiment) # [FILE:options]
            # only test specific experiments
            EXPERIMENTS="$(echo "$EXPERIMENTS"; echo "$2")"
            shift
            ;;
        -o|--output) # [FILE]
            # TAP output file name
            TAPFILE=$2
            shift
            ;;
        -j|--jobs) #[NUMBER]
            # number of tests to run in parallel
            JOBS=$2
            shift
            ;;
        -p|--parameters) # [STRING]
            # specify additional parameters to be passed to the executable
            EXTRAPARAMS=$2
            shift
            ;;
        -h|--help) #
            # this help
            printf "Usage: $0 [OPTION]...\n\n"
            printf "Test apex experiments against prexisting results.\n\n"
            printf "Command line arguments:\n"
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

split() {
    local temp="$1"
    while [[ "$#" -gt 1 ]]; do
        shift
        export -n "$1"="${temp%%:*}"
        if [[ "$temp" =~ : ]]; then
            temp="${temp#*:}"
        else
            temp=""
        fi
    done
}

prereq() {
    if ! which $1 &> /dev/null; then
        redmsg "Error: Prerequisite $1 not found"
        exit 2
    fi
}

runapex() {
    local EXPERIMENTSUBDIR="$(dirname $1)"
    local EXPERIMENTFILENAME="$(basename $1)"
    local EXPERIMENTBASENAME=${EXPERIMENTFILENAME%%.*}
    local RESULTSPATH="$EXPERIMENTSUBDIR/$EXPERIMENTBASENAME-results.apr"

    local APEXPARAMS="--record --virtual-soundcard --deterministic --autoanswer --autostart --autosaveresults --exitafter --resultsfile $RESULTSPATH $EXTRAPARAMS"
    brownmsg "Running $APEXEXECUTABLE" $APEXPARAMS "$1"
    # allow Ctrl-C interrupt by relaying any SIGINT to the process group of timeout, and exiting the script
    trap 'kill -INT -$pid; exit' INT
    timeout "$TIMEOUT" xvfb-run -a -s "-screen 0 1280x1024x24" -e "$1.xvfb-error-log" "$APEXEXECUTABLE" $APEXPARAMS "$1" &> "$1.out.log" &
    pid=$!
    wait $pid
    trap - INT
}

downloadreference() {
    if [[ ! -f "$2/LICENSE" ]]; then
        brownmsg "Fetching reference from $1"
        git clone --depth 1 $1 $2
    else
        pushd $2
        git fetch --depth 1
        git checkout origin/master
        popd
    fi
}

savereference() {
    brownmsg "Saving test results in $ZIPFILE, run the following to upload"
    tar -czf $ZIPFILE -C "$RESULTSROOT" ./

    CPCMD="scp $ZIPFILE gilbert.med.kuleuven.be:/var/www/apex/"
    brownmsg "$CPCMD ? [Y]"
    read -n 1 -i y RESPONSE
    if [[ "$RESPONSE" == "y" ]]; then
        $CPCMD
    fi
}

failtest() {
    redmsg "$1"
    TEST_STATUS="$TEST_STATUS $2-failed"
}

oktest() {
    brownmsg "$1"
    TEST_STATUS="$TEST_STATUS $2-ok"
}

skiptest() {
    brownmsg "$1"
    TEST_STATUS="$TEST_STATUS $2-skipped"
}

parsestatus() {
    if [[ "$TEST_STATUS" =~ -failed ]]; then
        TEST_STATUS="not ok $1 -$TEST_STATUS"
        TEST_SUCCEEDED=false
    elif [[ "$TEST_STATUS" =~ "# SKIP" ]]; then
        TEST_STATUS="ok $1 -$TEST_STATUS"
        TEST_SUCCEEDED=true
    else
        [[ "$SKIP" == "true" && "$NORESULTS" == "false" ]] && echo "$TEST_STATUS" > "$SKIPFILE"
        TEST_STATUS="ok $1 -$TEST_STATUS"
        TEST_SUCCEEDED=true
    fi
    echo "$TEST_STATUS" >> "$TAPFILE"
}

runtest() {
    bluemsg "Testing $1"

    local EXPERIMENTROOT="$REFERENCEROOT"
    [[ "$2" =~ new ]] && EXPERIMENTROOT="$APEXROOT/examples"
    local EXPERIMENTSUBDIR="$(dirname $1)"
    local EXPERIMENTFILENAME="$(basename $1)"
    local EXPERIMENTBASENAME=${EXPERIMENTFILENAME%%.*}
    local REFERENCEDIR="$REFERENCEROOT/$EXPERIMENTSUBDIR"
    local RESULTSDIR="$RESULTSROOT/$EXPERIMENTSUBDIR"

    SKIPFILE="$EXPERIMENTROOT/$1.skipped"
    TEST_STATUS=

    if [[ ! -f "$EXPERIMENTROOT/$1" ]]; then
        failtest "Experiment not found" "load"
        parsestatus "$1"
        return
    fi

    mkdir -p "$RESULTSDIR"
    # subshell to limit the nullglob to the copy command
    (shopt -s nullglob; cp "$EXPERIMENTROOT/$EXPERIMENTSUBDIR/"{"$EXPERIMENTFILENAME",*.js} "$RESULTSDIR")

    if [[ "$SKIP" == "false" || "$ACTION" == "store" || "$SPECIFICEXPERIMENTS" == "true" ]]; then
        rm -f "$SKIPFILE"
    else
        if [[ -f "$SKIPFILE" ]]; then
            greenmsg "Test skipped"
            TEST_STATUS="$(cat "$SKIPFILE") # SKIP"
            parsestatus "$1"
            return
        fi
    fi

    runapex "$RESULTSDIR/$EXPERIMENTFILENAME"

    [[ "$ACTION" == "store" ]] && return

    local TESTPARTFAILED=false
    local REFERENCEWAVS="$(find "$REFERENCEDIR" -regex "$REFERENCEDIR/$EXPERIMENTBASENAME-[0-9]+.wav" | sort)"
    local RESULTSWAVS="$(find "$RESULTSDIR" -regex "$RESULTSDIR/$EXPERIMENTBASENAME-[0-9]+.wav" | sort)"
    brownmsg "Checking wav output ($(wc -l <<< "$REFERENCEWAVS") files)"
    if [ $(wc -l <<< "$REFERENCEWAVS") -eq $(wc -l <<< "$RESULTSWAVS") ]; then
        if [[ "$2" =~ continuoussilence ]]; then
            RESULT=$("$APEXROOT"/tools/linux-experimenttest-audio-compare-functions.py \
                                compareexpectingcontinuoussilence \
                                "$REFERENCEWAVS" "$RESULTSWAVS")
        elif [[ "$2" =~ continuous ]]; then
            RESULT=$("$APEXROOT"/tools/linux-experimenttest-audio-compare-functions.py \
                                continuous "$REFERENCEWAVS" "$RESULTSWAVS")
        elif [ "$USEBERTHA" = "true" ]; then
            RESULT=$("$APEXROOT"/tools/linux-experimenttest-audio-compare-functions.py \
                                filewisetruncate "$REFERENCEWAVS" "$RESULTSWAVS")
        else
            RESULT=$("$APEXROOT"/tools/linux-experimenttest-audio-compare-functions.py \
                                filewise "$REFERENCEWAVS" "$RESULTSWAVS")
        fi
        if [[ "$RESULT" != "True" ]]; then
            TESTPARTFAILED=true
            failtest "wav files differ" "wav"
        fi
    else
        failtest "Different number of WAV files" "wav-count"
        TESTPARTFAILED=true
    fi
    if [[ "$TESTPARTFAILED" == "false" ]]; then
        oktest "WAV files identical" "wav"
    fi

    if grep --quiet 'xsi:type="apex:\(L34\|Coh\)DeviceType"' $RESULTSDIR/$EXPERIMENTFILENAME; then
        brownmsg "Checking Cochlear output"
        if ! diff -w $REFERENCEDIR/L34output.xml $RESULTSDIR/cohoutput.xml; then
            failtest "Different Cochlear stimulus" "coh"
        else
            oktest "Cochlear stimulus identical" "coh"
        fi
    fi

    if [[ "$2" =~ noresults || "$NORESULTS" == "true" ]]; then
        skiptest "Checking results output: skipped" "results"
    else
        brownmsg "Checking results output"
        if ! $APEXROOT/bin/debug/resultschecker "$REFERENCEDIR/$EXPERIMENTBASENAME-results.apr" \
            "$RESULTSDIR/$EXPERIMENTBASENAME-results.apr"; then
            failtest "Different results output" "results"
            echo "$REFERENCEDIR/$EXPERIMENTBASENAME-results.apr"
            echo "$RESULTSDIR/$EXPERIMENTBASENAME-results.apr"
        else
            oktest "Results output identical" "results"
        fi
    fi

    parsestatus "$1"
    if [[ "$TEST_SUCCEEDED" == "true" ]]; then
        greenmsg "Test successful"
    elif [[ "$VERBOSE" == "true" ]]; then
        brownmsg "Apex output:"
        cat "$RESULTSDIR/$EXPERIMENTFILENAME.out.log"
    fi
}

if [[ -t 1 ]]; then
    RED=$(tput setaf 1)
    GREEN=$(tput setaf 2)
    BROWN=$(tput setaf 3)
    BLUE=$(tput setaf 4)
    NORMAL=$(tput sgr0)
fi

redmsg() {
    echo "$RED$@$NORMAL"
}

greenmsg() {
    echo "$GREEN$@$NORMAL"
}

brownmsg() {
    echo "$BROWN$@$NORMAL"
}

bluemsg() {
    echo "$BLUE$@$NORMAL"
}

parsecmd "$@"

APEXROOT="$(cd "$(dirname $(readlink -f ${BASH_SOURCE[0]}))/.." && pwd)"
OUTPUTDIR="$APEXROOT/.build/test"
REFERENCEROOT="$OUTPUTDIR/reference"
RESULTSROOT="$OUTPUTDIR/results"

if [[ -z "$TAPFILE" ]]; then
    TAPFILE="$APEXROOT/experiments.tap"
fi
rm -f "$TAPFILE"

if [[ -z "$APEXEXECUTABLE" ]]; then
    APEXEXECUTABLE="$APEXROOT/bin/debug/apex"
fi

if [[ -z "$EXPERIMENTS" ]]; then
    EXPERIMENTS="$(cat "$APEXROOT/tools/experiments-to-test.txt")"
    SPECIFICEXPERIMENTS=false
else
    SPECIFICEXPERIMENTS=true
fi
EXPERIMENTS="$(echo "$EXPERIMENTS" | grep -vE '^(\s*$|#)')"
EXPERIMENTCOUNT="$(echo "$EXPERIMENTS" | wc -l)"

echo "APEX executable: $APEXEXECUTABLE"
echo "Action: $ACTION"
echo "Reference URL: $REFERENCEURL"
echo "Clean: $CLEAN"
echo "Skip: $SKIP"
echo "Verbose: $VERBOSE"
echo "No results file checks: $NORESULTS"
echo "Timeout: $TIMEOUT seconds"
echo "APEX root: $APEXROOT"
echo "Reference directory: $REFERENCEROOT"
echo "Results directory: $RESULTSROOT"
echo

brownmsg "Checking dependencies"
prereq sox
prereq xvfb-run
prereq "$APEXEXECUTABLE"

brownmsg "Preparing temporary directory"
if [[ "$CLEAN" == "true" ]]; then
    echo "Cleaning output directory"
    rm -rf "$OUTPUTDIR"
fi
rm -rf "$RESULTSROOT"
mkdir -p "$RESULTSROOT"
ln -s "$APEXROOT/examples/stimuli" "$RESULTSROOT/stimuli"
echo "1..$EXPERIMENTCOUNT" > "$TAPFILE"

brownmsg "Preparing reference"
mkdir -p "$REFERENCEROOT"
downloadreference "$REFERENCEURL" "$REFERENCEROOT"

brownmsg "Running tests ($EXPERIMENTCOUNT)"

trytest() {
    split "$1" NAME OPTIONS
    [[ ! -z "$NAME" ]] && runtest "$NAME" "$OPTIONS"
}

USEBERTHA=false
if [[ ! "$EXTRAPARAMS" =~ "--disable-bertha" ]]; then
    USEBERTHA=true
fi

# exports so the parallel runtest can access them
export REFERENCEROOT RESULTSROOT APEXROOT TIMEOUT APEXEXECUTABLE OUTPUTDIR TAPFILE SKIP NORESULTS EXTRAPARAMS AUDIO_COMPARE_FUNCTION USEBERTHA
export -f runtest parsestatus skiptest oktest failtest redmsg greenmsg brownmsg bluemsg runapex trytest split
echo $EXPERIMENTS | xargs -d ' ' -n 1 -I % -P $JOBS bash -c 'trytest %'


if [[ "$ACTION" == "store" ]]; then
    savereference
else
    bluemsg "Test summary ($TAPFILE):"
    FAILED=0
    SKIPPED=0
    SUCCEEDED=0
    {
        read && echo $REPLY
        while read; do
            if [[ "$REPLY" =~ -failed ]]; then
                redmsg "$REPLY"
                FAILED=$(($FAILED + 1))
            elif [[ "$REPLY" =~ "# SKIP" ]]; then
                brownmsg "$REPLY"
                SKIPPED=$(($SKIPPED + 1))
            else
                greenmsg "$REPLY"
                SUCCEEDED=$(($SUCCEEDED + 1))
            fi
        done
    } <<< "$(cat $TAPFILE)"
    echo "Failed: $FAILED"
    echo "Skipped: $SKIPPED"
    echo "Success: $SUCCEEDED"
fi
