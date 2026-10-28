#include <iostream>
#include <cassert>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <experimental/random>

#include "AmplifiedHashFunction.hpp"
#include "../../../Maths/Maths.hpp"

using namespace std;

// Just creates its hash functions.
AmplifiedHashFunction::AmplifiedHashFunction(const size_t& hash_fns_num,
            const vector<HashFunction>& h_functions,
            const double& w_size, const size_t& v_vals_sz,
            const long long int& M) : hash_functions_num(hash_fns_num), M(M)
{
    random_device rd; // obtain a random number from hardware
    mt19937 gen(rd()); // seed the generator
    uniform_int_distribution<> distr(1, pow(2,31)-1); // define the range,or rand_max
    uniform_int_distribution<> dist(1, hash_fns_num); // define the range

    for (int i = 0; i < hash_fns_num; i++){
            // choose h randomly
            hash_functions.push_back(&h_functions[dist(gen)-1]); // index
    }

    for(int i = 0; i < hash_fns_num; i++){
        // r values are 32-bit integers
        r_values.push_back(distr(gen));
    }
}
/*long long int sum = 0;
for(int i = 0; i < hash_functions_num; i++){
    // avoid overflow
    sum += ((long long int) (hash_functions[i]->get_value(query_vector) % M) * (r_values[i] % M)) % M;
}*/
// return ID(p)
const int AmplifiedHashFunction::get_value(const vector<double>* query_vector, const int& table_size, long long int& ID_p)
{
    long long int sum = 0;
    // for M = 2^32-5 we get best results but its slower(half time than trute force)
    for(int i = 0; i < hash_functions_num; i++){
      // avoid overflow
      long long int A = (long long int) hash_functions[i]->get_value(query_vector) % M;
      long long int B = (long long int) r_values[i] % M;
      sum += A * B % M;

    }
    // auto cast
    ID_p = sum;

    return (sum % M) % table_size;
}
