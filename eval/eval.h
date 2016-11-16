//
// Created by houri on 20.10.16.
//

#ifndef GBMOT_EVAL_H
#define GBMOT_EVAL_H

#include "../util/FileIO.h"
#include "../core/ObjectData2D.h"
#include "../../../../../usr/include/c++/4.8/string"
#include <vector>
#



namespace Eval {


    class eval {
    private:
        double rateFN;      //rate of false negative
        double rateTP;      //rate of true positive
        double rateFP;      //rate of false positive
        double FN;          //amount of false negative detection in the resultant file
        double TP;          //amount of true positive detection in the resultant file
        double FP;          //amount of false positive detection in the resultant file
        double IDswitch;    // amount of switches between different object
        double MOTA;        // the value of MOTA
        double MOTP;        // the value of MOTP


    public :


        eval calculatemotq (std::Vector3d &result,std::Vector3d &groundtruth, const std::string &file_name,std::Vector3d &distance, char delimiter);
        // this helps to calculate the parameters that determine the performance of the tracking algo
        // Value is a Vector2d contains the ground truth data and Value is the result










    };


}





























#endif //GBMOT_EVAL_H



