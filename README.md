## [A Hierarchical Deep Temporal Model for Group Activity Recognition. Mostafa S. Ibrahim, Srikanth Muralidharan, Zhiwei Deng, Arash Vahdat, Greg Mori.  IEEE Computer Vision and Pattern Recognition 2016](http://www.cs.sfu.ca/~mori/research/papers/ibrahim-cvpr16.pdf)

## History
* The first version of this work is accepted at CVPR 2016.
* An extended work is submitted to TPAMI (under review). [Jounral Paper Link](http://arxiv.org/pdf/1607.02643v1.pdf).
* This journal version builds on the previous version to include the following:
  * We have collected an expanded Volleyball dataset that is 3 times larger than CVPR submission.
  * We conducted further analysis of experimental results and included comparisons to an additional set of baseline methods.
  * We implemented a variant of our approach to perform spatial pooling strategies over people.
* The provided dataset is the expanded version. Please use and compare againt this version.

## Abstract
In group activity recognition, the temporal dynamics of the whole activity can be inferred based on the dynamics of the individual people representing the activity. We build a deep model to capture these dynamics based on LSTM models. To make use of these observations, we present a **2-stage deep temporal model for the group activity recognition** problem.  In our model, a LSTM model is designed to represent **action dynamics of individual people** in a sequence and another LSTM model is designed to **aggregate person-level information** for whole activity understanding.  We evaluate our model over two datasets: the Collective Activity Dataset and a new volleyball dataset.

## Model
<img src="https://github.com/mostafa-saad/deep-activity-rec/blob/master/img/fig1.png" alt="Figure 1" height="400" >

**Figure 1**: High level figure for group activity recognition via a hierarchical model. Each person in a scene is modeled using a temporal model that captures his/her dynamics, these models are integrated into a higher-level model that captures scene-level activity.

<img src="https://github.com/mostafa-saad/deep-activity-rec/blob/master/img/fig2-b.png" alt="Figure 2" height="400" >

**Figure 2**: Detailed figure for the model. Given tracklets of K-players, we feed each tracklet in a CNN, followed by a person LSTM layer to represent each player's action. We then pool over all people's temporal features in the scene. The output of the pooling layer is feed to the second LSTM network to identify the whole teams activity.

<img src="https://github.com/mostafa-saad/deep-activity-rec/blob/master/img/fig3.jpg" alt="Figure 3" height="400" >

**Figure 3**: Previous basic mode drops spatial information. In updated model, 2-group pooling to capture spatial arrangements of players.

## Dataset
### [Download Link](http://vml.cs.sfu.ca/wp-content/uploads/volleyballdataset/volleyball.zip)

We collected a new dataset using publicly available **YouTube volleyball** videos. We annotated **4830 frames** that were handpicked from **55 videos** with 9 player action labels and 8 team activity labels. 

<img src="https://github.com/mostafa-saad/deep-activity-rec/blob/master/img/dataset1.jpg" alt="Figure 3" height="400" >

**Figure 3**: A frame labeled as Left Spike and bounding boxes around each team players is annotated in the dataset.


<img src="https://github.com/mostafa-saad/deep-activity-rec/blob/master/img/dataset2.jpg" alt="Figure 4" height="400" >

**Figure 4**: For each visible player, an action label is annotaed.

We used 3493 frames for training, and the remaining 1337 frames for testing. One video is either in training or testing. The list of action and activity labels and related statistics are tabulated in following tables

|Group Activity Class|No. of Instances|
|---|---|
|Right set|644|
|Right spike|623|
|Right pass|801|
|Right winpoint|295|
|Left winpoint|367|
|Left pass|826|
|Left spike|642|
|Left set|633|


|Action Classes|No. of Instances|
|---|---|
|Waiting|3601|
|Setting|1332|
|Digging|2333|
|Falling|1241||
|Spiking|1216|
|Blocking|2458|
|Jumping|341|
|Moving|5121|
|Standing|38696|

**Further information**:
* The dataset is 55 videos. Each video has a directory for it with sequntial IDs (0, 1...54)
 * **Train Videos**: 1 3 6 7 10 13 15 16 18 22 23 31 32 36 38 39 40 41 42 48 50 52 53 54
 * **Validation Videos**: 0 2 8 12 17 19 24 26 27 28 30 33 46 49 51
 * **Test Videos**: 4 5 9 11 14 20 21 25 29 34 35 37 43 44 45 47
* Inside each video directory, a set of directories corresponds to annotated frames (e.g. volleyball/39/29885)
  * Video 39, frame ID 29885
* Each frame directory has 41 images (20 images before target frame, **target frame**, 20 frames after target frame)
  * E.g. for frame ID: 29885 => Window = {29865, 29866.....29885, 29886....29905}
  * You can use such window as your temporal window. 
  * Scences change in fast manner in volleyball, hence frames beyond that window shouldn't represent belong to target frame most of time. 
  * In our work, we used 5 before and 4 after frames.
* Each video directory has annotations.txt file that contains selected frames annotations.
* Each annotation line in format: {Frame ID} {Frame Activity Class} {Player Annotation}  {Player Annotation} ...
  * Player Annotation corresponds to a tight bounding box surrounds each player
* Each {Player Annotation} in format: {Action Class} X Y W H

## Installation
* There are 2 internal projects: a simple one for sake of validation and other one, the real pipeline.
* Download and Install [Dlib library](http://dlib.net/).
* Download and Install [Caffe-LSTM library](https://github.com/junhyukoh/caffe-lstm). 
  * Assume your download disk path is $lstm_path
* cd $lstm_path/examples
* git clone https://github.com/mostafa-saad/deep-activity-rec.git
* Open makefile at examples/deep-activity-rec
  * Update path locations of variables CAFFE_LSTM_DIR and DLIB_DIR
  * Update the INCS_DIRS and LIBS_DIRS (based on your environment)
* Open examples/deep-activity-rec/ibrahim16-cvpr-simple/script-simple.sh
  * Update path variable for CAFFE
* cd examples/deep-activity-rec
* Compile code: make all
* cd ../..
* Run: examples/deep-activity-rec/ibrahim16-cvpr-simple/script-simple.sh
 * Make sure top concole lines don't complaint about "NOT exist directory".
 * You may validate overall console processing with file script-simple-expected-log.txt
 * If so, fix it, use script-clean.sh, run script-simple.sh.
* The code process is multiple staged outlined in a script file.
  * Processng should end with simple accuracy table, all of it almost zeros. 
  * The key point, see the console log and make sure no problems. 
  * Otherwise, read the script and try to get the different phases and read logs to get the errors.
  * Every sub-directory under ibrahim16-cvpr-simple has 1 or more logs.
  * Directory p4-network2 should have the final model and accuracy table.
* If everything is ok, we can proceed with actual pipeline.
* Download dataset to path deep-activity-rec/volleyball
 * Same directory structure as given deep-activity-rec/volleyball-simple
* Whatever steps/changes you did for ibrahim16-cvpr-simple, do it for ibrahim16-cvpr.
* Run: examples/deep-activity-rec/ibrahim16-cvpr/script.sh
* GPU/CPU note
  * The script.sh has 2 heavy processing phases that needs CPU not GPU
  * One can instead run following 2 scripts in parallel on CPU: script-p1-data.sh and script-p2-data-fuse.sh
  * Then Run on GPU following script: script-p1-train-p3-p4.sh
  * The overall of these 3 scripts = main script

## Experiments

<img src="https://github.com/mostafa-saad/deep-activity-rec/blob/master/img/table-ac.png" alt="Figure 5" height="300" >

**Table 1**: Comparison of the team activity recognition performance of baselines against our model evaluated on the Volleyball Dataset. Experiments are using 2 group styles with max pool strategy. Last 3 entries comparison against Improved Dense Trajectories approach.

## License and Citation

Source code is released under the **BSD 2-Clause license**

Please cite our model in your publications if it helps your research:

    @inproceedings{msibrahi16deepactivity,
      author    = {Mostafa S. Ibrahim and Srikanth Muralidharan and Zhiwei Deng and Arash Vahdat and Greg Mori},
      title     = {A Hierarchical Deep Temporal Model for Group Activity Recognition.},
      booktitle = {Computer Vision and Pattern Recognition (CVPR)},
      year      = {2016}
    }

    @inproceedings{msibrahi16deepactivity,
      author    = {Mostafa S. Ibrahim and Srikanth Muralidharan and Zhiwei Deng and Arash Vahdat and Greg Mori},
      title     = {Hierarchical Deep Temporal Models for Group Activity Recognition.},
      journal   = {arXiv preprint arXiv:1607.02643},
      year      = {2016}
    }

## CVPR 16 Poster
* You can find our CVPR 2016 poster [here](https://github.com/mostafa-saad/deep-activity-rec/blob/master/extra/poster.pdf).

<img src="https://github.com/mostafa-saad/deep-activity-rec/blob/master/extra/poster.jpg" alt="Poster" height="400" >

Mostafa on left and Srikanth on right while presenting the poster.


