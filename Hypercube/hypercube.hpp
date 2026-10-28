#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include <iostream>
#include <vector>
#include <map>
#include "../RangeQueryStructure/rangeQueryStructure.hpp"
#include "../Maths/Maths.hpp"
#include "../LSH/Hashing/Hash_table/LSH_Structure.hpp"
#include "../LSH/Hashing/H_function/h_function.hpp"
#include "../LSH/Hashing/Hash_table/Linked_list/linked_list.hpp"

using namespace std;

class Hypercube : public rangeQueryStructure
{
    int buckets;    // No. of buckets
    int probes;
    int M_points;
    int k;
    vector<map<int, int>>  hash_array;

    const double           window_size;
    const size_t           v_values_size;

    // Pointer to an array containing buckets
    vector<Node> *table;
    vector<HashFunction> hash_fun_vec;

    double                 (*distance_fn)(const std::vector<double>&, const std::vector<double>&);
public:
    Hypercube(int, double (*distance_fun)(const std::vector<double>&, const std::vector<double>&),
                    const double&, const size_t&,
                    const int&, const int&, const int&);  // Constructor

    ~Hypercube();
    // inserts a key into hash table
    void insert(Node query_vector);

    // deletes a key from hash table
    void deleteItem(int index);

    vector<vector<int>> comb(string str, int probes);

    string get_querytag(vector<double>);

    void datasetInitialize(const Dataset&);

    vector<int> neighbors(string, int);

    void displayHypercube();

    void displayNeighbors(int* N);

    const vector<int> rangeSearch(const vector<double>&, const double&);

    vector<NNQueryReturn> kNearestNeighbors(const vector<double>&, const int&);

    void evaluateQuerySet(const Dataset&, ifstream&,
                            ofstream&, const int&, const double&);
};


int binaryStringToDec(string);

#endif //HYPERCUBE_H
