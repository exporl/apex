#!/bin/bash -e

function compare_audio() {
    # result file should be longer than the 1 second silence
    if [ $(echo $(soxi -D $2 2>/dev/null)'>'"1.0" | bc) = "0" ]; then
        echo "False"
        return
    fi
    # find the first 2 consecutive values in the resultsdata and compare
    python - <<EOF
import numpy as np;
import soundfile as sf;
referencedata, sr = sf.read("$1")
resultsdata, sr2 = sf.read("$2")
col1 = referencedata[:,0]
col2 = resultsdata[:,0]
indicesOfFirst = np.where(col2 == col1[0])[0]
indicesOfSecond = np.where(col2 == col1[1])[0]
indexOfFirst = 0
for i in range(min(indicesOfFirst.size, indicesOfSecond.size) - 1):
         if indicesOfFirst[i] + 1 == indicesOfSecond[i]:
            indexOfFirst = indicesOfFirst[i]
            break
maxlength = min(col1.size, col2.size)
print np.allclose(col1[:maxlength - indexOfFirst], col2[indexOfFirst:maxlength])
EOF
}

export -f compare_audio

tools/jenkins-linux-build.sh "$@"

tools/linux-upgrade-examples.sh

tools/linux-experimenttests.sh --audio-compare-function compare_audio \
                               --experiment "regression/continuous-sine.apx"
