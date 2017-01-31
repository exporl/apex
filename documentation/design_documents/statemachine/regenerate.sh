#!/bin/bash

../../../bin/debug/statemachinevisualizer
for i in *.dot; do dot -Tpdf $i >${i%.dot}.pdf; done
