//
// Created by houri on 20.10.16.
//

#ifndef GBMOT_EVAL_H
#define GBMOT_EVAL_H

#include "../util/FileIO.h"
#include "../core/ObjectData2D.h"
#include "../../../../../usr/include/c++/4.8/string"
#include <vector>
#include"MOTA_MOTP.h"

#pragma once
#ifndef mota
#define mota

#include "sortingalgorthim.h" 
#include "BoxStruct.h"
#include <iostream>


// find the index for a certain detection form the id
int TRACK_ID_finder(info trackers, int traget);

// this function search the index of the target in the vector
int object_ID_finder(std::vector<std::vector<int>>& object_ID, int Traget, unsigned int col);

// find the difference between all mapped objects and all elements in a frame
std::vector<int> find_diff(const std::vector<int>& current, const std::vector<std::vector<int>>& mapping);

// forming assosiation between the ground truth and result in that frame
std::vector <std::vector<double>> greaddyassosiation(std::vector <std::vector<double>> score, double TH);

// calculate if two bboxes are intersecting
double Isect(double detbox[], double tbox[]);

//find the distance between two bboxes 
double mydistance(bbox BOX_INFO, bbox traget);

//Excute the evaluation and it is the main function
void MOTA_MOTP_CALC(const std::string& file_name_groundtruth, const std::string& file_name_Tracking, char delimitter, double Image_high, double Image_width, double threshold,bool GT_file_header,bool RE_file_header, const std::string& outputfiledir,bool detail);

//find the number of all objects in the ground truth file.  
double gt_objects_number(const std::vector<info>& groundtruth);



#endif //GBMOT_EVAL_H



