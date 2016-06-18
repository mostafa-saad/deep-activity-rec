#!/usr/bin/env sh

OUTDIR=examples/deep-activity-rec/ibrahim16-cvpr/p1-network1
GPU_ID=0

echo "Running Caffe using GPU" $GPU "In Directory " $OUTDIR

./build/tools/caffe train 2> $OUTDIR/z_trainval-test-log.txt \
  --solver $OUTDIR/trainval-test-solver.prototxt      --weights    models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel    --gpu $GPU_ID
