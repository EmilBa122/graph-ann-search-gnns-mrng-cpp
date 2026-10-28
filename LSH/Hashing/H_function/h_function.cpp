#include <cassert>
#include <cmath>

#include <iostream>

#include <random>
#include <vector>
#include <algorithm>

#include "./h_function.hpp"
#include "../../../Maths/Maths.hpp"

using namespace std;

HashFunction::HashFunction(const double& w_size, const size_t& v_vals_sz,
                const long long int& M) : window_size(w_size),
                v_values_size(v_vals_sz), M(M)
{
    random_device          rd;  //Will be used to obtain a seed for the random number engine
    mt19937                gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_real_distribution<> dist(0.0,(double) w_size - 1.0); // because t_values are real

    default_random_engine gen_;
    normal_distribution<double> dis(0.0,1.0);
    t_value = (double) dist(gen_);
    // Initializing the v_i values, d dimensions
    for(int i = 0; i < v_vals_sz;i++){
        v_values.push_back(dis(gen));
    }

    // normalization of v
    double min = *min_element(v_values.begin(), v_values.end());
    for(int i = 0; i < v_vals_sz;i++){
        v_values[i] += abs(min);
    }
}

int HashFunction::get_value(const vector<double>* query_vector) const
{
    assert(query_vector->size() == v_values_size);

    // calculating h_value
    double h_sum = 0.0;
    for(int i = 0; i < v_values_size; i++){
        h_sum += ((*query_vector)[i] * v_values[i]);
    }
    int h = (int) floor((h_sum + t_value) / window_size);

    return h;
}
