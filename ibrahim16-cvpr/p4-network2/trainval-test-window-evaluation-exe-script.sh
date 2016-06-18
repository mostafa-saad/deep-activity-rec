#!/usr/bin/env sh

OUTDIR=examples/deep-activity-rec/ibrahim16-cvpr/p4-network2
WINDOW=10
GPU_ID=0

TEST_EXAMPLES=1337
ITER=20000
LAYER=prop


examples/deep-activity-rec/exePhase4  \
 $WINDOW \
 GPU $GPU_ID \
 $OUTDIR/z_snapshot_iter_$ITER.caffemodel \
 $OUTDIR/trainval-test-window-evaluation-network.prototxt \
 $LAYER \
 $TEST_EXAMPLES \
 2>&1 |  tee   $OUTDIR/z_trainval-test-window-evaluation-log-prop.txt 
