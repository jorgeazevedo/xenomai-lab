#!/bin/bash

echo "Initiating test"

# Prepare test
blocks="tick signal_generator sum Z display"
tmpfiles="tick.stdout tick.stderr signal_generator.stdout signal_generator.stderr sum.stdout sum.stderr Z.stdout Z.stderr result.log result.ok"
source configure.sh
./compileBlocks.sh "$blocks"
./initWorkspace.sh "$blocks"

# Configure block operation
cat <<EOF >$WORKSPACE/Z.conf
[Operation]
N=1

[Task]
Priority=50

EOF

# Run test
$BLOCKPATH/tick/tick -iroot1 -oroot2 Test/tick 1> tick.stdout 2> tick.stderr &
$BLOCKPATH/signal_generator/signal_generator -iroot2 -osignal0 Test/signal_generator 1> signal_generator.stdout 2> signal_generator.stderr &
$BLOCKPATH/sum/sum -isignal0,signal2 -osignal1 -pSumOut Test/sum 1> sum.stdout 2> sum.stderr &
$BLOCKPATH/Z/Z -isignal1 -osignal2 -pZOut Test/Z 1> Z.stdout 2> Z.stderr &
sleep 2
$BLOCKPATH/display/display -iSumOut,ZOut Test/display


sleep 2
killall tick
sleep 2

exit
