#ifndef __AMPLIFIED_HASH_FN__
#define __AMPLIFIED_HASH_FN__

#include <string>
#include <vector>

#include "../H_function/h_function.hpp"

using namespace std;

class AmplifiedHashFunction{
private:
    // HashFunction --> Hi
    vector<const HashFunction*> hash_functions;
    vector<int>                 r_values;

    const size_t                hash_functions_num;
    const long long int         M;


public:
    AmplifiedHashFunction(const size_t& hash_fns_num, const vector<HashFunction>& h_functions,
                  const double& w_size, const size_t& v_vals_sz,const long long int& M);

    const int get_value(const std::vector<double>* query_vector, const int& table_size, long long int& ID_p);

    ~AmplifiedHashFunction(){};
};

#endif
