#!/bin/bash

XLPATH="/home/jorge/.xenomailab"
BLOCKPATH="$XLPATH/blocks"
WORKSPACE="/home/jorge/.xenomailab/workspace/Test"

trap 'rm -fr $tmpfiles' 1 2 3 15
