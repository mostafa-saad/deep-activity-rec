#!/usr/bin/env bash

ROOT_DIR=examples/deep-activity-rec/ibrahim16-cvpr-simple

# Phase 1 artifacts
rm -r $ROOT_DIR/p1-network1/test-leveldb
rm -r $ROOT_DIR/p1-network1/trainval-leveldb
rm $ROOT_DIR/p1-network1/mean.binaryproto
rm $ROOT_DIR/p1-network1/z_log_dataset_net1.txt
rm $ROOT_DIR/p1-network1/z_trainval-test-log.txt
rm $ROOT_DIR/p1-network1/z_snapshot_iter_*.caffemodel
rm $ROOT_DIR/p1-network1/z_snapshot_iter_*.solverstate

# Phasse 2
rm -r $ROOT_DIR/p2-ready-fuse

# Phasse 3 & 4
rm -r $ROOT_DIR/p4-network2/test-leveldb
rm -r $ROOT_DIR/p4-network2/trainval-leveldb
rm $ROOT_DIR/p4-network2/z_log_dataset_net2.txt
rm $ROOT_DIR/p4-network2/z_trainval-test-log.txt
rm $ROOT_DIR/p4-network2/z_trainval-test-window-evaluation-log-prop.txt
rm $ROOT_DIR/p4-network2/z_snapshot_iter_*.caffemodel
rm $ROOT_DIR/p4-network2/z_snapshot_iter_*.solverstate
