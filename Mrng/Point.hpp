#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <map>
#include <bits/stdc++.h>

using namespace std;

//Class that stores for some point p the id of some near point and the distance that they have 
class near_point
{
    private:
        unsigned int near_point_id;
        double near_point_distance;
    public:
        void set_near_point_id (int i);
        unsigned int get_near_point_id ();
        void set_near_point_distance (double temp);
        double get_near_point_distance ();
        void print_near_point();
};

class Point 
{
    private:
		unsigned int id;
		vector<double> coordinates;
		unsigned int dimension;
        string f_string;
        //I only use the vector<near_point> neighbors when we have MRNG with cube and n nearest neighbors method
        vector<near_point> neighbors;
        //The two MRNG methods are using the L_p
        vector<near_point> L_p;
        //I only use the variables nearest_point_id and nearest_point_distance for the two methods
        //at the part we initialize the L_p for every point p
        unsigned int nearest_point_id;
        double nearest_point_distance;
    public:
        void set_id (unsigned int i);
        void set_dimension (unsigned int i);
        void set_coordinates (vector<double> image);    
        unsigned int get_dimension ();
        unsigned int get_id ();
        double get_i_coordinate (int i);
        void renew_f_string(string temp);
        void point_print();
        string get_Point_f_string();
        void print_point();
        Point get_Point();
        vector<double> get_coordinates (); 
        void set_i_coordinate(int i, double value);
        void set_nearest_point_id(unsigned int i);
        unsigned int get_nearest_point_id();
        void set_nearest_point_distance(double distance);
        double get_nearest_point_distance();
        void set_near_point_neighbors(near_point temp_near_point);
        void print_near_point_neighbors();
        unsigned int get_nearest_point_id_from_vector_neighbors(unsigned int i);
        double get_nearest_point_distance_from_vector_neighbors(unsigned int i);
        vector<near_point> &get_near_point();
        int sum_of_nearest_neighbors_from_point();
        void add_L_p_by_reference(near_point &n_p);
        void add_L_p_by_value(near_point n_p);
        vector<near_point> &get_L_p();
};
