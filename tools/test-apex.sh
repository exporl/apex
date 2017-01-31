#!/bin/bash

function fail
{
    exit 1
}

function usage
{
    echo Usage: $1 apex-reference-dir apex-to-test-dir [tests-to-run...]
    fail
}

function error
{
    echo Error: $1
    fail
}

function warning
{
    echo Warning: $1
}

function prereq
{
    if ! run_silent which $1
    then
        error "Prerequisite $1 not found"
    fi
}

function run_silent
{
    $@ &> /dev/null
}

function run_apex
{
    echo  $1 --record --virtual-soundcard --deterministic --autoanswer --autostart --autosaveresults --exitafter $2
#    run_silent
    timeout 300 xvfb-run -a -e "$2.xvfb-error-log" $1 --record --dummy --deterministic --autoanswer --autostart --autosaveresults --exitafter $2 >"$2.out.log" 2>&1
    #return $?
}

function compare_wav
{
    #when comparing two wav files directly, there is always one byte different
    #at a fixed place somewhere in the wav header. no idea why but just comparing
    #the raw data fixes this and should be enough.
    RAW_FIRST=$1.raw
    RAW_SECOND=$2.raw

    run_silent sox $1 $RAW_FIRST
    run_silent sox $2 $RAW_SECOND

    cmp -s $RAW_FIRST $RAW_SECOND
    SUCCESS=$?

    rm $RAW_FIRST $RAW_SECOND

    return $SUCCESS
}


function compare_results
{
    REF=$1
    TEST=$2

    bin/debug/resultschecker $REF $TEST
    RET=$?

    if [ $RET -ne 0 ]; then
        echo bin/debug/resultschecker $REF $TEST
    fi

    return $RET


    REFTEMP=ref-temp.apr
    TESTTEMP=test-temp.apr

    grep -v startdate $REF | grep -v enddate | grep -v responsetime |grep -v duration >$REFTEMP
    grep -v startdate $TEST | grep -v enddate | grep -v responsetime |grep -v duration >$TESTTEMP

    diff -w $REFTEMP $TESTTEMP
    RET=$?
    rm $REFTEMP $TESTTEMP

    return $RET
}

function get_reference
{
    if [ ! -f $OUT_DIR/$ZIPFILE ];
    then
        echo "File $OUT_DIR/$ZIPFILE not found, fetching"
        wget -O $OUT_DIR/$ZIPFILE https://gilbert.med.kuleuven.be/apex/$ZIPFILE
    fi
    tar xfz $OUT_DIR/$ZIPFILE -C $OUT_DIR/reference
}

function save_reference
{
    echo "Saving test results in $ZIPFILE, run the following to upload"
    tar -czf $ZIPFILE -C $OUT_DIR/to_test ./

    CPCMD="scp $ZIPFILE gilbert.med.kuleuven.be:/var/www/apex/"
    echo "$CPDMD ? [Y]"
    read -n 1 -i y RESPONSE
    if [ "$RESPONSE" == "y" ]; then
        $CPMD
    fi
}

function do_tests
{
    echo "Running test $1"

    DIRECTORY=$REF_OUT_DIR
    if [ $NEW -eq 1 ]; then
        DIRECTORY=$APEX_ROOT/"examples"
    fi

    ORIGINAL_TEST_FILE=$DIRECTORY/$1

    EXPERIMENT_DIR=`dirname $1`
    EXPERIMENT_FILE=`basename $1`
    REF_DIR=$REF_OUT_DIR/$EXPERIMENT_DIR
    TEST_DIR=$TEST_OUT_DIR/$EXPERIMENT_DIR
    mkdir -p $TEST_DIR
    if [ ! -f "$ORIGINAL_TEST_FILE" ];
    then
        echo "File $ORIGINAL_TEST_FILE not found";
        return
    fi
    cp $ORIGINAL_TEST_FILE $TEST_DIR
    cp $DIRECTORY/$EXPERIMENT_DIR/*.js $TEST_DIR
    REF_FILE=$REF_DIR/$EXPERIMENT_FILE
    TEST_FILE=$TEST_DIR/$EXPERIMENT_FILE

    echo "-> Running the Apex to test"
    run_apex $APEX_ROOT/$APEX_EXECUTABLE $TEST_FILE

    if [ "$ACTION" == "STORE" ]
    then
        return;
    fi

    echo "-> Comparing output"
    TEST_FAILED=false

#    echo "-> Checking whether every file produced by the reference is also produced by the test"

    EXPERIMENT_NAME=${EXPERIMENT_FILE%.*}

    for WAV in `cd $REF_DIR; ls $EXPERIMENT_NAME-[[:digit:]]*.wav`
    do
        if [ ! -f $TEST_DIR/$WAV ]
        then
            echo "  -> Reference produced $WAV but the test did not"
            TEST_FAILED=true
        fi
    done

#    echo "-> Checking whether every file produced by the test is also produced by the reference"

    for WAV in `ls $TEST_DIR/$EXPERIMENT_NAME-[[:digit:]]*.wav`
    do
        if [ ! -f $REF_DIR/$(basename $WAV) ]
        then
            echo "  -> Test produced $WAV but the reference did not"
            TEST_FAILED=true
        fi
    done

#    echo "-> Comparing wav files"

    for WAV in `cd $REF_DIR; ls $EXPERIMENT_NAME-[[:digit:]]*.wav`
    do
        if [ -f $TEST_DIR/$WAV ]
        then
#            echo -n "$WAV"

            compare_wav $REF_DIR/$WAV $TEST_DIR/$WAV
            if [ $? -ne 0 ]
            then
                echo $REF_DIR/$WAV NOT EQUAL TO $TEST_DIR/$WAV
#               echo -n " (NOT EQUAL),"
                TEST_FAILED=true
            fi
        fi
    done

#   Check if the experiment is a L34 experiment and act accordingly
    if grep --quiet 'xsi:type="apex:L34DeviceType"' $TEST_DIR/$EXPERIMENT_FILE; then
        diff -w $REF_DIR/L34output.xml $TEST_DIR/L34output.xml
        if [ $? -ne 0 ]; then
            TEST_FAILED=true
        fi
    fi

#    echo "-> Comparing results files"

    if [ $SKIP_RESULTS -ne 1 ]; then
        compare_results $REF_DIR/$EXPERIMENT_NAME-results.apr $TEST_DIR/$EXPERIMENT_NAME-results.apr
        if [ $? -ne 0 ]; then
            echo $REF_DIR/$EXPERIMENT_NAME-results.apr NOT EQUIVALENT TO $TEST_DIR/$EXPERIMENT_NAME-results.apr
            TEST_FAILED=true
        fi
    fi

    if $TEST_FAILED
    then
        echo "Test $1 failed"
        FAILED_TESTS=(${FAILED_TESTS[@]} $1)
    else
        echo "Test $1 successful"
        SUCCESSFUL_TESTS=(${SUCCESSFUL_TESTS[@]} $1)
    fi
}

#start of script
prereq sox
prereq xvfb-run

#if [ $# -lt 2 ] || [ ! -d $1 ] || [ ! -d $2 ]
#then
#    usage $0
#fi

if [ $# -eq 1 -a "$1" == "--store" ]
then
    ACTION=STORE
else
    ACTION=TEST
fi

#constants

APEX_ROOT="$( cd -P "$( dirname "${BASH_SOURCE[0]}")"/../..  && pwd )"

echo "Using APEX_ROOT = $APEX_ROOT"

APEX_EXECUTABLE=bin/debug/apex
APEX_EXAMPLES_DIR=examples
APEX_STIMULI_DIR_NAME=stimuli
APEX_STIMULI_DIR=$APEX_EXAMPLES_DIR/$APEX_STIMULI_DIR_NAME

#APEX_REF_ROOT=`cd $1; pwd`
#APEX_TEST_ROOT=`cd $2; pwd`

REF_NAME=reference
TEST_NAME=to_test
OUT_DIR=$APEX_ROOT/.build/test
REF_OUT_DIR=$OUT_DIR/$REF_NAME
TEST_OUT_DIR=$OUT_DIR/$TEST_NAME

ZIPFILE='apex_reference_output.tgz'

if [ ! -e "$APEX_ROOT/$APEX_EXECUTABLE" ]; then
    echo "APEX binary not found: " $APEX_ROOT/$APEX_EXECUTABLE
    exit;
fi

echo "Creating output directory $OUT_DIR"
rm -rf $TEST_OUT_DIR
mkdir -p $REF_OUT_DIR $TEST_OUT_DIR

#ln -s $APEX_REF_ROOT/$APEX_STIMULI_DIR $REF_OUT_DIR/$APEX_STIMULI_DIR_NAME
ln -s  $APEX_ROOT/$APEX_STIMULI_DIR $TEST_OUT_DIR/

get_reference

echo -e "Starting tests\n"
SKIP_RESULTS=0
NEW=1

if [ $# -ge 1 -a "$ACTION" != "STORE" ]
then
    #tests specified on command line
#    CMD_LINE=($@)
#    for EXPERIMENT in ${CMD_LINE[@]:2}
    for EXPERIMENT in $@
    do
        do_tests $EXPERIMENT
        echo
    done
else
    #read files from tests.txt
    while read FILE
    do
        #skip empty lines
        [ -z "$FILE" ] && continue

        #skip comments
        [[ $FILE == \#* ]] && continue

        ARR=(${FILE//;/ })

        if [ ${#ARR[@]} -eq 3 ]; then
            SKIP_RESULTS=1
            NEW=1
        else
            if [ ${#ARR[@]} -eq 2 ]; then
                if [ ${ARR[1]} == "new" ]; then
                    SKIP_RESULTS=0
                    NEW=1
                else
                    SKIP_RESULTS=1
                    NEW=0
                fi
            else
                SKIP_RESULTS=0
                NEW=0
            fi
        fi

        do_tests $FILE
    done <`dirname $BASH_SOURCE`/experiments-to-test.txt
fi


if [ "$ACTION" == "STORE" ]
then
    save_reference;
else

    echo "Tests done"

    echo -e "\nTest summary:"
    echo "-> Successful tests(${#SUCCESSFUL_TESTS[@]}): ${SUCCESSFUL_TESTS[@]}"
    echo "-> Failed tests(${#FAILED_TESTS[@]}): ${FAILED_TESTS[@]}"
    echo "-> Skipped tests(${#SKIPPED_TESTS[@]}): ${SKIPPED_TESTS[@]}"

    echo -e "\nAll done, good bye!"

fi

