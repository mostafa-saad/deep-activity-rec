#!/usr/bin/env sh


OUTDIR=examples/deep-activity-rec/ibrahim16-cvpr-simple/p1-network1
GPU_ID=0
ITER=15000

echo "Resuming Caffe using GPU" $GPU "In Directory " $OUTDIR "Starting from iteration " $ITER

./build/tools/caffe train 2> $OUTDIR/z_trainval-test-log-resume.txt \
  --solver $OUTDIR/trainval-test-solver.prototxt    --snapshot=$OUTDIR/z_snapshot_iter_$ITER.solverstate  --gpu $GPU_ID
