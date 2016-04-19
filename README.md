## [Paper Link](http://www.cs.sfu.ca/~mori/research/papers/ibrahim-cvpr16.pdf)
* A Hierarchical Deep Temporal Model for **Group Activity Recognition**
* **Authors**: Mostafa S. Ibrahim, Srikanth Muralidharan, Zhiwei Deng, Arash Vahdat, Greg Mori

## Abstract
In group activity recognition, the temporal dynamics of the whole activity can be inferred based on the dynamics of the individual people representing the activity. We build a deep model to capture these dynamics based on LSTM models. To make use of these observations, we present a **2-stage deep temporal model for the group activity recognition** problem.  In our model, a LSTM model is designed to represent **action dynamics of individual people** in a sequence and another LSTM model is designed to **aggregate person-level information** for whole activity understanding.  We evaluate our model over two datasets: the Collective Activity Dataset and a new volleyball dataset.

## Model
<img src="https://github.com/mostafa-saad/deep-activity-rec/blob/master/img/fig1.png" alt="Figure 1" height="400" >

**Figure 1**: High level figure for group activity recognition via a hierarchical model. Each person in a scene is modeled using a temporal model that captures his/her dynamics, these models are integrated into a higher-level model that captures scene-level activity.

<img src="https://github.com/mostafa-saad/deep-activity-rec/blob/master/img/fig2.png" alt="Figure 1" height="400" >

**Figure 2**: Detailed figure for the model. Given tracklets of K-players, we feed each tracklet in a CNN, followed by a person LSTM layer to represent each player's action. We then pool over all people's temporal features in the scene. The output of the pooling layer is feed to the second LSTM network to identify the whole teams activity.

## Dataset
### [Download Link](http://www.sfu.ca/~msibrahi/)

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
TODO

## License and Citation

Source code is released under the **BSD 2-Clause license**

Please cite model in your publications if it helps your research:

    @inproceedings{msibrahi16deepactivity,
      author    = {Ibrahim M. S. and M. Srikanth and Deng Z. and A. Vahdat and G. Mori,},
      title     = {A Hierarchical Deep Temporal Model for Group Activity Recognition.},
      booktitle = {Computer Vision and Pattern Recognition (CVPR)},
      year      = {2016}
    }


<!---

---
layout: page
title: A title...oh
order: 7
---


Commented!



[![Build Status](https://travis-ci.org/BVLC/caffe.svg?branch=master)](https://travis-ci.org/BVLC/caffe)
[![License](https://img.shields.io/badge/license-BSD-blue.svg)](LICENSE)



## header 1

### like header 2

<table>
   <tr>
  	<td class="col-thirty"><strong>Code</strong></td>
  	<td>data1</td>
  </tr>
   <tr>
  	<td class="col-thirty"><strong>Format</strong></td>
  	<td> data 2</td>
  </tr>
</table>

Another table

|| **Bold** |
|---|---|
|**Embedding Status Images**         |[![Build Status](https://ci.swift.org/job/oss-swift-incremental-RA-osx/badge/icon)](https://docs.travis-ci.com/user/status-images/)|

##### Hmmm
**See Here Tabed**

    git clone https://github.com/test-test
    git checkout release


**[Some URL](http://brew.sh/)**


For C++ developers:
```c++
int main()
{
    return 0;
}
```

[![](https://img.shields.io/github/tag/alexhultman/uWebSockets.svg)]()

-->

