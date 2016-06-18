#!/usr/bin/env sh

OUTDIR=examples/deep-activity-rec/ibrahim16-cvpr-simple/p4-network2
GPU_ID=0

echo "Running Caffe using GPU" $GPU "In Directory " $OUTDIR

./build/tools/caffe train 2> $OUTDIR/z_trainval-test-log.txt \
  --solver $OUTDIR/trainval-test-solver.prototxt  --gpu $GPU_ID
