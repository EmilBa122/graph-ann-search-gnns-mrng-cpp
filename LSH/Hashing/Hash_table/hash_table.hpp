#ifndef __HASH_TABLE__
#define __HASH_TABLE__

#include <iostream>
#include <vector>

#include "./Linked_list/linked_list.hpp"
#include "../G_function/AmplifiedHashFunction.hpp"

// It will be a static hash table
// It represents one of the many hash tables of LSH_Structure.
class HashTable{
private:
    const size_t          L;
    const size_t          buckets_num;
    LinkedList*           buckets;
    AmplifiedHashFunction hashFunction;
    const vector<HashFunction>          h_functions;

public:
    HashTable(const size_t& buckets_num, const size_t& hash_fns_num,
            const vector<HashFunction>& hash_functions,
            const size_t& L,
            const double& w_size, const size_t& v_vals_sz,
            const int& M);

    LinkedList_Iterator get_neighbors(const std::vector<double>&, const int& table_size, long long int& ID_q);
    void                add_vector(const std::vector<double>*, const int&, const int&);

    void printBucketSize() const{
        for(size_t i = 0; i < buckets_num; i++){
            std::cout << buckets[i].getSize() << std::endl;
        }
    }

    ~HashTable(){
        delete[] buckets;
    };
};

#endif
