#include"C:\Users\mhdho\Documents\Visual Studio 2015\Projects\Filereading\Filereading\sortingalgorthim.h"


unsigned int getsize(std::vector<std::vector<double>>& myvector)
{
	unsigned int sizerow;
	 return sizerow = myvector.size();
}

unsigned int getsizecoloumn(std::vector<std::vector<double>>& myvector)
{

	unsigned int sizecoloumn;
	return	sizecoloumn =	myvector[0].size();


}

double maxvlaue(std::vector<std::vector<double>>& myvector)
{
	double max = myvector[0][0];
	for (unsigned int i = 0; i < myvector.size(); i++)
	{
		for (unsigned int j = 0; j < myvector[i].size(); j++)
		{
			if (max <= myvector[i][j])
			{
				max = myvector[i][j];
			}

		}
	}

	return max;
}

unsigned int Max_index_row(std::vector<std::vector<double>>& myvector)
{
	double max = myvector[0][0];
	unsigned int counter = 0;
	for (unsigned int i = 0; i < myvector.size(); i++)
	{
		for (unsigned int j = 0; j < myvector[i].size(); j++)
		{
			if (max <= myvector[i][j])
			{
				max = myvector[i][j];
			}

		}
	}
	for (unsigned int i = 0; i < myvector.size(); i++)
	{
		for (unsigned int j = 0; j < myvector[i].size(); j++)
		{
			
			if (max == myvector[i][j])
				return i;
		}
	}
	return -1;
}



unsigned int Max_index_coloumn(std::vector<std::vector<double>>& myvector)
{
	double max = myvector[0][0];
	unsigned int counter = 0;
	for (unsigned int i = 0; i < myvector.size(); i++)
	{
		for (unsigned int j = 0; j < myvector[i].size(); j++)
		{
			if (max <= myvector[i][j])
			{
				max = myvector[i][j];
			}

		}
	}
	for (unsigned int i = 0; i < myvector.size(); i++)
	{
		for (unsigned int j = 0; j < myvector[i].size(); j++)
		{

			if (max == myvector[i][j])
				return j;
		}
	}
	return -1;
}


 /*int find_index(int target, std::vector<int>& premapping)
{
	unsigned int end_term = premapping.size();
	for (unsigned int i = 0; i < end_term; i++)
	{
		if (target == premapping.at(i))
			return i;
	}
	return -1;
}
*/

 int find_index(double target, std::vector<std::vector<double>>& vector2d, int option)
{	// this function is find the index and return it 	
	// row to be ignored 
	 int counter = 0;
	if (option == 0)
	{
		for (unsigned int i = 0; i < vector2d.size(); i++)
		{
			if (target == vector2d.at(i).at(0))
			return counter;
			counter++;
		}
	}
	else if (option == 1)
	{
		for (unsigned int i = 0; i < vector2d.at(0).size(); i++)
		{
			if (target == vector2d.at(0).at(i))
			return counter;
			counter++;
		}

	}
	return -1;
}

 int find_index(int target, std::vector<std::vector<int>>& vector2d, int option)
 {	// this function is find the index and return it 	
	// row to be ignored 
	 int counter = 0;
	 if (option == 0)
	 {
		 for (unsigned int i = 0; i < vector2d.size(); i++)
		 {
			 if (target == vector2d.at(i).at(0))
			 return counter;
			 counter++;
		 }
	 }
	 else if (option == 1)
	 {
		 for (unsigned int i = 0; i < vector2d.at(0).size(); i++)
		 {
			 if (target == vector2d.at(0).at(i))
			 return counter;
			 counter++;
		 }

	 }
	 return -1;
 }

 double sum_row(std::vector<std::vector<double>>& vector2d, int row_num)
 {
	 double sum = 0;
	 for (unsigned int i = 0; i < vector2d.at(row_num).size(); i++)
	 {
		 sum = sum + vector2d.at(row_num).at(i);
	 }
	 return sum;
 }


 double sum_col(std::vector<std::vector<double>>& vector2d, int col_num)
 {
	 double sum = 0;
	 for (unsigned int i = 0; i < vector2d.size(); i++)
	 {
		 sum = sum + vector2d.at(i).at(col_num);
	 }
	 return sum;
 }



 

 int find_index(int target, std::vector<int>& vector1d)
 {
	 int counter = 0;
	 for (unsigned int i = 0; i < vector1d.size(); i++)
	 {
		 if (target == vector1d.at(i))
			 return counter;
		 counter++;
	 }
	 return -1;
 
 }
