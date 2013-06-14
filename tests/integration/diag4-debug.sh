#!/bin/bash

echo "Initiating test"

# Prepare test
blocks="tick signal_generator sum Z debug"
tmpfiles="tick.stdout tick.stderr signal_generator.stdout signal_generator.stderr sum.stdout sum.stderr Z.stdout Z.stderr result.log result.ok"
source configure.sh
./compileBlocks.sh "$blocks"
./initWorkspace.sh "$blocks"

# Configure block operation
cat <<EOF >$WORKSPACE/Z.conf
[Operation]
N=3

[Task]
Priority=50

EOF

# Run test
$BLOCKPATH/tick/tick -iroot1 -oroot2,root3 Test/tick 1> tick.stdout 2> tick.stderr &
$BLOCKPATH/signal_generator/signal_generator -iroot2 -osignal0 Test/signal_generator 1> signal_generator.stdout 2> signal_generator.stderr &
$BLOCKPATH/sum/sum -isignal0,signal2 -osignal1 Test/sum 1> sum.stdout 2> sum.stderr &
$BLOCKPATH/Z/Z -isignal1 -osignal2 Test/Z 1> Z.stdout 2> Z.stderr &
$BLOCKPATH/debug/debug -iroot3 Test/debug

echo "Shutting down"
sleep 2
killall tick
sleep 2

# Clean up
./cleanupWorkspace.sh "$tmpfiles"
