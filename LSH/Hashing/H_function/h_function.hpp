#ifndef __HASH_FUNCTION__
#define __HASH_FUNCTION__

#include <vector>

class HashFunction{
private:
    const double           window_size;
    const size_t           v_values_size;
    const long long int    M;
    std::vector<double>    v_values;
    int                    t_value;

public:
    HashFunction(const double&,const size_t&,const long long int&);
    int get_value(const std::vector<double>* query_vector) const;
    ~HashFunction(){};
};

#endif
