#!/usr/bin/env sh

CAFFE=/cs/vml2/msibrahi/workspaces/caffe-lstm

GIT_PROJ_DIR=$CAFFE/examples/deep-activity-rec
DATASET_VIDEOS=/cs/vml2/msibrahi/Datasets/Greg-Volleyball/volleyball
DATASET_CONFIG=$GIT_PROJ_DIR/dataset-config
OUTPUT_DIR=$GIT_PROJ_DIR/ibrahim16-cvpr

TRAIN_SRC=trainval
TEST_SRC=test

WINDOW_NETWORK1=5
WINDOW_NETWORK2=10
STEP=1

GPU_ID=0
NETWORK1_HIDDEN=3000
NETWORK1_TRAIN_ITERS=15000

# Fusion Styles: Choose 0-7
# 0 => Conc / 1 group       1 => Max / 1 group        4 => Avg / 1 group        7 => sum / 1 group
# 2 => Max / 2 groups	    5 => Avg / 2 groups       3 => Max / 4 groups       6 => Avg / 4 groups    
FUSION_STYLE=2
FUSION_TRAIN_ITER=3493
FUSION_TEST_ITER=1337

VAR_FUSION_LAYERS_VAL="2 fc7 lstm1"
VAR_FUSION_LAYERS="FUSION_LAYERS"
declare "$VAR_FUSION_LAYERS=$VAR_FUSION_LAYERS_VAL"

NETWORK1_DIR=$OUTPUT_DIR/p1-network1
NETWORK1_MODEL_PATH=$NETWORK1_DIR/z_snapshot_iter_$NETWORK1_TRAIN_ITERS.caffemodel
NETWORK2_LEVELDB_FUSION_DIR=$OUTPUT_DIR/p2-ready-fuse
NETWORK2_EXTRACTION_NETOWRK_DIR=$OUTPUT_DIR/p3-extract-features-networks
NETWORK2_DIR=$OUTPUT_DIR/p4-network2

# Programs
EXE_P1_NETWORK1=exePhase1_2
EXE_P2_FUSE=exePhase1_2
EXE_P4_NETWORK2=exePhase3

###########################################################################
echo ------------------------------------------------------
echo 
echo "START processing script" "$0"
echo "OUTPUT Directory is " $OUTPUT_DIR
echo 
echo Doing path VALIDATIONS

## Some directories / files validation

[ -d $CAFFE ]             || echo Directory $CAFFE NOOOT exist
[ -d $OUTPUT_DIR ]        || echo Directory $OUTPUT_DIR NOOOT exist
[ -d $DATASET_VIDEOS ]    || echo Directory $DATASET_VIDEOS NOOOT exist
[ -d $DATASET_CONFIG ]    || echo Directory $DATASET_CONFIG NOOOT exist
[ -d $NETWORK1_DIR ]      || echo Directory $NETWORK1_DIR NOOOT exist
[ -d $NETWORK2_EXTRACTION_NETOWRK_DIR ] || echo Directory $NETWORK2_EXTRACTION_NETOWRK_DIR NOOOT exist
[ -d $NETWORK2_DIR ]      || echo Directory $NETWORK2_DIR NOOOT exist

echo READY...STEADY...Gooo ?
read -t 60

cd $CAFFE










###############
echo ------------------------------------------------------
echo Phase 2 - A - Generating Data to be Fused - $NETWORK2_LEVELDB_FUSION_DIR

mkdir -p $NETWORK2_LEVELDB_FUSION_DIR

# Clean if some previous wrong data
[ -d $NETWORK2_LEVELDB_FUSION_DIR/$TRAIN_SRC-leveldb ]  && [ ! -d $NETWORK2_LEVELDB_FUSION_DIR/$TEST_SRC-leveldb ] && rm -r $NETWORK2_LEVELDB_FUSION_DIR/$TRAIN_SRC-leveldb
[ ! -d $NETWORK2_LEVELDB_FUSION_DIR/$TRAIN_SRC-leveldb ]  && [ -d $NETWORK2_LEVELDB_FUSION_DIR/$TEST_SRC-leveldb ] && rm -r $NETWORK2_LEVELDB_FUSION_DIR/$TEST_SRC-leveldb

$GIT_PROJ_DIR/$EXE_P2_FUSE   \
  $DATASET_VIDEOS   \
  $DATASET_CONFIG   \
  $NETWORK2_LEVELDB_FUSION_DIR       $WINDOW_NETWORK2    $STEP  0  \
  2>&1 |  tee    \
  $NETWORK2_LEVELDB_FUSION_DIR/z_log_dataset_fuse.txt   

echo ========================



echo Phase 2 - B - Creating Mean File of Fused Data
echo "Computing image mean for dataset: " $TRAIN_SRC

$CAFFE/build/tools/compute_image_mean -backend=leveldb  $NETWORK2_LEVELDB_FUSION_DIR/$TRAIN_SRC-leveldb $NETWORK2_LEVELDB_FUSION_DIR/mean.binaryproto


read -t 10









###############
echo ------------------------------------------------------
echo 
echo DONE processing script "$0"
echo 
echo ------------------------------------------------------

