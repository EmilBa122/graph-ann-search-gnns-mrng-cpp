#include "./hash_table.hpp"

extern int WINDOW_SIZE;

using namespace std;

HashTable::HashTable(const size_t& bucks_num, const size_t& hash_fns_num,
            const vector<HashFunction>& h_functions,
            const size_t& L, const double& w_size, const size_t& v_vals_sz,
            const int& M)
            : buckets_num(bucks_num), L(L),
              hashFunction(hash_fns_num, h_functions, w_size, v_vals_sz, M)
{
    buckets = new LinkedList[buckets_num];
}

// Getting the corresponding bucket
LinkedList_Iterator HashTable::get_neighbors(const vector<double>& query_vector, const int& table_size,
                                        long long int& ID_q)
{
    int index = hashFunction.get_value(&query_vector, table_size, ID_q);

    return buckets[index].begin();
}

void HashTable::add_vector(const std::vector<double>* vec, const int& id, const int& table_size)
{
    long long int ID_p;
    int index = hashFunction.get_value(vec, table_size, ID_p);

    buckets[index].add(vec, id, ID_p);
}
