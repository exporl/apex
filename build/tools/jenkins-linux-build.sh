#!/bin/sh -e

#rm -rf bin .build
rm -f *test-results.xml

if [ "$releasetype" = "release" ]; then
    qmake RELEASE=1
else
    qmake RELEASE=
fi
make qmake
make

mkdir bin/$releasetype/doc
git show --stat > bin/$releasetype/doc/commit.txt

echo "Running datatest"
bin/$releasetype/datatest -xunitxml | sed '$s/Aborted//' > datatest-results.xml || true
echo "Running spintest"
xvfb-run -a bin/$releasetype/spintest -xunitxml | sed '$s/Aborted//' > spintest-results.xml || true
echo "Running toolstest"
bin/$releasetype/toolstest -xunitxml | sed '$s/Aborted//' > toolstest-results.xml || true
echo "Running maintest"
xvfb-run -a bin/$releasetype/maintest -xunitxml | sed '$s/Aborted//' > maintest-results.xml || true
echo "Running writerstest"
#xvfb-run -a bin/$releasetype/writerstest -xunitxml > writerstest-results.xml || true

# crashes the testbench
#rm writerstest-results.xml 

