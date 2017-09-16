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

double  gt_objects_number(const std::vector<info>& groundtruth)
{
	double	counter = 0;
	for (size_t i = 0; i < groundtruth.size(); i++)
	{
		counter = counter + groundtruth.at(i).num_of_object;
	}
	return counter;
}

std::vector<int> find_diff(const std::vector<int>& current, const std::vector<std::vector<int>>& mapping) // to find difference between two matrices
{
	std::vector<int> Matched_index;
	std::vector<int> element;
	std::vector<int> tempo;
	for (unsigned int i = 0; i < mapping.size(); i++)
	{
		tempo.push_back(mapping.at(i).at(0));
	}
	for (unsigned int j = 0; j < current.size(); j++)
	{
		if (find_index(current.at(j), tempo) == -1)
			element.push_back(current.at(j));
		else
			Matched_index.push_back(j);
	}

	return element;
}

int object_ID_finder(std::vector<std::vector<int>>& object_ID, int Traget, unsigned int col)// Linear search the elelement between a target_index and 
{
	for (unsigned int  i = 0; i < object_ID.size(); i++)
	{
		if (Traget == object_ID.at(i).at(col))
			return int(i);
	}
	return -1;
}

int TRACK_ID_finder(info trackers, int traget) //search a certian ID_of the target and then return where it is in a certain frame
{
	for (unsigned int i = 0; i < trackers.boxes_info.size(); i++)
	{
		if (traget == trackers.boxes_info.at(i).object_ID)
			return int (i);
	}
	return -1;
}

std::vector <std::vector<double>> greaddyassosiation(std::vector <std::vector<double>> score, double TH)// assosiate a certain frame 
{
	unsigned int T = getsize(score);
	unsigned int D = getsizecoloumn(score);
	double currentscore = maxvlaue(score);
	unsigned int I = Max_index_row(score);
	unsigned int J = Max_index_coloumn(score);
	std::vector <std::vector<double>>Asso;


	for (unsigned int i = 0; i < T; i++)
	{
		std::vector<double> tempo;
		for (unsigned int j = 0; j < D; j++)
		{

			tempo.push_back(0);
		}
		Asso.push_back(tempo);
	}

	while (currentscore != 0)
	{
		score.at(I).at(J) = 0;
		if (currentscore > TH)
		{
			for (unsigned int i = 0; i < D; i++)
				score.at(I).at(i) = 0;
			for (unsigned int i = 0; i < T; i++)
				score.at(i).at(J) = 0;
			Asso.at(I).at(J) = 1;

		}
		currentscore = maxvlaue(score);
		I = Max_index_row(score);
		J = Max_index_coloumn(score);

	}

	return Asso;

}

double Isect(double detbox[], double tbox[])
{
	double leftA = detbox[0];
	double bottomA = detbox[1];
	double rightA = leftA + detbox[2];
	double topA = bottomA + detbox[3];

	double leftB = tbox[0];
	double bottomB = tbox[1];
	double rightB = leftB + tbox[2];
	double topB = bottomB + tbox[3];

	double left = std::max(leftA, leftB);
	double right = std::min(rightA, rightB);
	double bottom = std::max(bottomA, bottomB);
	double top = std::min(topA, topB);

	if (left < right && bottom < top)
	{
		double interSection = (right - left) * (top - bottom);
		return interSection;
	}
	return 0;

}


double mydistance(bbox BOX_INFO, bbox traget)
{
	double xb = BOX_INFO.xtl;
	double yb = BOX_INFO.ytl;
	//double bw = 2*(BOX_INFO.width );
	//double bh = 2*(BOX_INFO.hight );
	double bw = 2 * (BOX_INFO.width - BOX_INFO.xtl);
	double bh = 2 * (BOX_INFO.hight - BOX_INFO.ytl);
	double debox[4] = { xb,yb,bw,bh };
	//

	double xt = traget.xtl;
	double yt = traget.ytl;
	double tw = traget.width;
	double th = traget.hight;
	double tbox[4] = { xt,yt,tw,th };

	// the function will be written later 
	double intersection = Isect(debox, tbox);

	double UNIOUN = (tw*th) + (bw*bh) - intersection;
	double dist;
	if (UNIOUN == 0)
		dist = 0;
	else
		dist = intersection / UNIOUN;
	return dist;


}


void MOTA_MOTP_CALC(const std::string& file_name_groundtruth, const std::string& file_name_Tracking, char delimitter, double Image_high, double Image_width, double threshold,bool GT_file_header, bool RE_file_header, const std::string& outputfiledir, bool detail)
{
	//const std::string& file_name_groundtruth: Directory of the ground truth file 
	//const std::string& file_name_Tracking: Directory of the tracker output 
	//char delimitter: the delimiter for both ground truth and result file
	//double Image_high: image hight for normalized output of the tracker
	//double Image_width: image width for normalized output of the tracker
	//double threshold: Thershold for matching both an object from ground_truth and a hypothsise from the tracker
	//bool GT_file_header: If the ground truth file has a header
	//bool RE_file_header: If the result file has a header
	//const std::string& outputfiledir: the directory of the output file of the evalaution
	//bool detail: If the output file outputs details about the evaluation process 


	std::vector <info> GT = Readingfromfile(file_name_groundtruth, delimitter, GT_file_header);
	std::vector <info> RE = Readingfromfile(file_name_Tracking, delimitter, GT_file_header);
	Box_data_converter(GT, RE, Image_width, Image_high);
	std::ofstream outputfile;
	outputfile.open(outputfiledir);
	outputfile << "Results"<<"\n";
	
	
	// Intial values;
	double idswitch = 0;
	double truepos = 0;
	double falseneg = 0;
	double falsepos = 0;

	// distances for MOTA calculation
	double distances = 0;

	std::vector <std::vector<int>> premapping;// for detecting idsw 
	premapping.clear();
	std::vector <std::vector<int>> mapping; // for detecting the idsw
	mapping.clear();
	double gt = gt_objects_number(GT);

	unsigned int frame_length;
	if (GT.size() < RE.size())
	{
		frame_length = GT.size();
	}
	else
	{
		frame_length = RE.size();
	}
	unsigned int starting_point = 0;
	//finding the starting frame that both tracker and ground truth share
	for (unsigned  i = 0; i < GT.size(); i++)
	{
		if (GT.at(i).frame_id == RE.at(0).frame_id)
		{
			starting_point = i; break;
		}
	}
	//finding all the errors when the tracker is not operating 
	for (unsigned i = 0; i < starting_point; i++)
	{
		falseneg = falseneg +double( GT.at(i).num_of_object);
	}
	// resudial errors after the tracker stops tracking but the ground truth is not finished yet
	for (unsigned i = starting_point+frame_length; i < GT.size(); i++)
	{
		falseneg = falseneg+ double(GT.at(i).num_of_object);
	}
	std::cout << "\n" << "The evlauation will start from the frame: " << GT.at(starting_point).frame_id << "\n";
	GT.erase(GT.begin(), GT.begin() + starting_point);
	

	// teporory parameters for doing the calcuations 
	info bboxes;
	info tracker_ID_DATA;
	//Tmp Counter
	double idswitchTmp = 0;
	double trueposTmp = 0;
	double	falsenegTmp = 0;
	double falseposTmp = 0;
	std::vector <std::vector<double>> Asso;
	std::vector <std::vector<double>> Cost;
	// end of the counter
	std::vector <std::vector<double>> score;
	std::vector <std::vector<int>> indexObj;


	for (unsigned int  i = 0; i < frame_length; i++)
	{

		tracker_ID_DATA = RE.at(i);// from the result file
		bboxes = GT.at(i);// from the grond truth

		idswitchTmp = 0;
		trueposTmp = 0;
		falsenegTmp = 0;
		falseposTmp = 0;
		Asso.clear();
		Cost.clear();

		// Compute current mapping procedure.
		score.clear();
		indexObj.clear();


		// Getting the distance matrix Tracks vs Annotations
		std::vector <int>currentAllLabel;

		for (unsigned int j = 0; j < bboxes.boxes_info.size(); j++)
		{
			currentAllLabel.push_back(bboxes.boxes_info.at(j).object_ID);
			std::vector<double> tempo_score;
			std::vector<int> tempo_ID;
			for (unsigned int L = 0; L < tracker_ID_DATA.boxes_info.size(); L++)
			{

				double distance = mydistance(bboxes.boxes_info.at(j), tracker_ID_DATA.boxes_info.at(L));
				tempo_score.push_back(distance);
				tempo_ID.push_back(bboxes.boxes_info.at(j).object_ID);

				//score.at(j).at(L) = distance;
				//indexObj.at(j).at(L) = bboxes.boxes_info.at(j).object_ID;
			}
			score.push_back(tempo_score);
			indexObj.push_back(tempo_ID);
		}
		//should be checked again;
		Asso = greaddyassosiation(score, threshold);

		// Compute current mapping (between tracks hyp. and annotations).

		//I CHECKED TILL HERE and every thing is fine till now.
		std::vector<std::vector <int>> mapping;
		mapping.clear();

		for (unsigned int r = 0; r < Asso.size(); r++)
		{
			std::vector<int> temop_asso;
			for (unsigned int r1 = 0; r1 < Asso.at(0).size(); r1++)
			{
				if (Asso[r][r1] == 1)
				{
					int obj = bboxes.boxes_info.at(r).object_ID;
					int Ttrack = tracker_ID_DATA.boxes_info.at(r1).object_ID;
					temop_asso.push_back(obj);
					temop_asso.push_back(Ttrack);
				}
			}
			if (temop_asso.empty())
				continue;
			mapping.push_back(temop_asso);
		}//checking if there is mapping.

		// if length(mapping)  > 0 && length(premapping) > 0
		if ((mapping.size() > 0) && (premapping.size() > 0))
		{
			for (unsigned int o = 0; o < mapping.size(); o++)
			{
				int idx;
				idx = find_index(mapping.at(o).at(0), premapping, 0);
				if (idx == -1)
					goto trueposition;
				//continue;
			// h = find(idxTracks == mapping(o,2));
			//int idx = find_index(mapping.at(o).at(0), premapping.at(egal).at(0));
				if (mapping[o][1] != premapping[idx][1])
				{
					// if contraditcs count as ID switch
					idswitch++;
					idswitchTmp++;
					if (detail==true)
					{
						outputfile << "IDSWITCH at frame:" << GT.at(i).frame_id<<"\n";
					}

				}
				else
				{
					trueposition:
					truepos++;
					if (detail == true)
					{
						outputfile << "true positive at frame:" << GT.at(i).frame_id << "\n";
					}
					int h = TRACK_ID_finder(tracker_ID_DATA, mapping.at(o).at(1));
					if (h == -1)
						continue;
					//h = find(idxTracks == mapping(o, 2));
					int idxo = object_ID_finder(indexObj, mapping[o][0], h);
					if (idxo == -1 || h == -1)
						continue;
					//distances = distances + score[idxo][h]; this was the original
					distances = distances + score[idxo][h];
					trueposTmp++;
				}
			}
		}
		else if (mapping.size() > 0)
		{
			for (unsigned int o = 0; o < mapping.size(); o++)
			{
				//count as TP and evaluate the MOTP.
				if (detail == true)
				{
					outputfile << "true position at frame:" << GT.at(i).frame_id << "\n";
				}
				truepos++;
				int h = TRACK_ID_finder(tracker_ID_DATA, mapping.at(o).at(1));
				//h = find(idxTracks == mapping(o, 2));
				int idxo = object_ID_finder(indexObj, mapping[o][0], h);
				if (idxo == -1 || h == -1)
					continue;
				distances = distances + score[idxo][h];// this was was the original
				//distances = distances + score[h][idxo];
				trueposTmp++;
			}

		}

		else if (premapping.size() > 0)
		{
			mapping = premapping;
		}
		else
		{
			std::cout << "\n"<<"from first_frame and there is no mapping till now<<"<<"\n";
		}
		

		for (unsigned int k = 0; k < Asso.size(); k++)//here sum the row summation Check false negative (unmapped annotated obj. up to a threshold).
		{
			double sum = 0;
			for (unsigned int M = 0; M < Asso.at(k).size(); M++)
			{
				sum = sum + Asso.at(k).at(M);
			}

			if ((sum == 0))
			{
				if (detail == true)
				{
					outputfile << "false negative at frame:" << GT.at(i).frame_id << "\n";
				}
				falseneg++;
				falsenegTmp++;
			}
		}


		if (Asso.empty())
		{
			for (unsigned int k = 0; k < bboxes.boxes_info.size(); k++)
			{
				if (detail == true)
				{
					outputfile << "false negative at frame:" << GT.at(i).frame_id << "\n";
				}
				falseneg = falseneg + 1;
				falsenegTmp = falsenegTmp + 1;
			}
		}




		for (unsigned int n = 0; n < Asso.at(0).size(); n++)//number of column  summation of column  %% Check false positive (unmapped tracker hyp. up to a threshold).
		{

			double sum1 = 0;
			for (unsigned int L = 0; L < Asso.size(); L++)
			{
				sum1 = sum1 + Asso.at(L).at(n);
			}
			if (sum1 == 0)
			{
				// Check false positive(unmapped tracker hyp.up to a threshold).
				if (detail == true)
				{
					outputfile << "false negative at frame:" << GT.at(i).frame_id << "\n";
				}
				falsepos = falsepos + 1;
				falseposTmp = falseposTmp + 1;
			}
		}
		//get unmapped object
	// from here i should optimize the code to find the errors 

		std::vector<int> unmapped;
		unmapped.clear();
		if (!mapping.empty())
		{
			unmapped = find_diff(currentAllLabel, mapping);

		}

		for (unsigned int unmap = 0; unmap < unmapped.size(); unmap++)
		{
			if (!premapping.empty())
			{
				int idxunmap = find_index(unmapped.at(unmap), mapping, 0);
				if (idxunmap == -1)
					continue;//ignore the errors value 
				mapping.push_back(premapping.at(idxunmap));
			}
		}
		premapping = mapping;
		std::cout << "end of the loop: " << i << std::endl;
	}//end of the for loop main one



	std::cout << std::endl;
	std::cout << "Total number of objects gt: " << gt << std::endl;
	std::cout<<"False Negative: " << falseneg << std::endl;
	
	std::cout <<"True positive: " << truepos << std::endl;
	std::cout <<"False positive: "<< falsepos << std::endl;
	
	std::cout <<"IDSW: " << idswitch << std::endl;
	
	std::cout<<"MOTP: "<< distances /(truepos)<<"\n";
	std::cout << "MOTA: " << 1 - ((idswitch + falsepos + falseneg) / gt)<<"\n";
	outputfile << "Summary of the result" << "\n";
	outputfile << "rateFN: 	rateTP:	 rateFP:	TP:		FN:		FP:		IDSW  "<<"\n";
	outputfile << falseneg / gt << " : " << truepos / gt << " : " << falsepos / gt << " : " << truepos << " : " << falseneg << " : " << falsepos << " : " << idswitch << "\n";
	outputfile << "MOTP:= " << distances / (truepos) <<"\n" <<"MOTA:= " << 1 - ((idswitch + falsepos + falseneg) / gt) << "\n";

	outputfile.close();
}




#endif //GBMOT_EVAL_H



