#!/usr/bin/env sh
# This script converts the vollyball data into leveldb format.

OUTDIR=examples/deep-activity-rec/ibrahim16-cvpr/p1-network1

echo "Computing image mean for trainval dataset: " $OUTDIR

./build/tools/compute_image_mean -backend=leveldb $OUTDIR/trainval-leveldb $OUTDIR/mean.binaryproto

echo "Done."
