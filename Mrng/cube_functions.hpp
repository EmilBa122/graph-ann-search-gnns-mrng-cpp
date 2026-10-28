#pragma once

#include <iostream>
#include <string>
#include <random>
#include <cstdlib>
#include <chrono>
#include <map>
#include <cmath>

#include "Point.hpp"

using namespace std;

//The class V_random_vector represent the normal (Gaussian) distribution
class V_random_vector
{
    private:
        unsigned int dimension;
        vector<double> v;
        void set_random_vector_coordinates ();
    public:
        V_random_vector(unsigned int dimension);
        void print_v();
        double get_i_coordinate (int i);
};

int random_generator();
double t_random_generator(int w);
double get_Normal_Distribution();
int h_p(Point *p, V_random_vector v, double t, int w);
void f_i(Point *p, int hp, map <int, string>* f);
double euclidean_distance(Point x, Point y);
int hamming_distance(int x, int y);
//The below function performs the logic of search in the cube method
void search_cube(Point &p, vector<Point> *hyper_cube, vector<Point> &dataset, int  num_of_buckets, unsigned int l);
