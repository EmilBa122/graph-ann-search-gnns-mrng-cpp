#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvla"

#include "typical_functions.hpp"
#include "cube_functions.hpp"
#include <cmath>
#include <fstream>
#include <ctime>

//Things we need for, to create the cube if we have MRNG with cube and n nearest neighbors method
#define w_cube 6
#define k_cube 16
#define probes_cube 6

//I use the below class for the part of the Generic Search on Graph
//The below class for some query point stores the id of a point from the dataset and the distance that
//they have these two points
class R
{
    private:
        unsigned int point_id;
        int checked_out = 0;
        double distance_from_query;
    public:
        void set_classR_point_id(unsigned int i);
        unsigned int get_classR_point_id();
        void set_classR_point_checked_out();
        int get_classR_point_checked_out();
        void set_classR_distance_from_query(double distance);
        double get_classR_distance_from_query();
};


//The "main" fuction for the MRNG part
void MRNG_main(vector<Point> &dataset, int N, int l, string query_file, string output_text_file);
