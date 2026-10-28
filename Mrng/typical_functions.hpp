#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <arpa/inet.h>
#include <vector>

#include "Point.hpp"

using namespace std;

void read_command (int argc, char **argv, int &k, int &E, int &R, int &N, int &l, int &m,
		string &input, string &query, string &output);
vector<Point> read_binary_file (string binary_file, int sum);
unsigned int check_Points_Dimension (vector<Point> *dataset);
