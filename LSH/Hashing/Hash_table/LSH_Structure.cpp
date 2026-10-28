#include <algorithm>
#include <cassert>
#include <ctime>
#include <iostream> // FOR NOW
#include <set>

#include "./LSH_Structure.hpp"
#include "./Linked_list/linked_list.hpp"
#include "../../../DistanceMetrics/DistanceMetrics.hpp"
#include "../../../Maths/Maths.hpp"

using namespace std;

// Initializing LSH
LSH_Structure::LSH_Structure(const size_t& hash_tables_num, const int& table_size,
                double (*distance_fun)(const std::vector<double>&, const std::vector<double>&),
                const size_t& hash_fns_num, const double& w_size,
                const size_t& v_vals_sz, const long long int& M) :
                    hash_tables_number(hash_tables_num), table_size(table_size),
                    distance_fn(distance_fun), M(M)
{
    // Creating hash functions H
    for(int i = 0; i < hash_fns_num; i++){
        hash_functions.push_back(HashFunction(w_size, v_vals_sz, M));
    }

    hash_tables = new HashTable*[hash_tables_num];

    // Creating the hash table
    for(int i = 0; i < hash_tables_num; i++){
        hash_tables[i] = new HashTable(table_size, hash_fns_num, hash_functions,
                               hash_tables_number, w_size, v_vals_sz, M);
    }

}

void LSH_Structure::datasetInitialize(const Dataset& data)
{
    const int row_num = data.getRows();

    for(int i = 0; i < row_num; i++){
        const vector<double>* data_ptr = &data[i];
        add_vector(data_ptr, i);
    }

    /* It is used to check if the hashing is good
    for(int i = 0; i < hash_tables_number; i++){
        hash_tables[i]->printBucketSize();
        cout << endl;
    }
    */
}

bool isless(const NNQueryReturn& a, const NNQueryReturn& b){
    return a.dist_from_query < b.dist_from_query;
}

const vector<NNQueryReturn> LSH_Structure::kNearestNeighbors(const vector<double>& query_vec, const int& k){
    set<int>              vector_ids;
    set<long long int>    vector_ids_p;
    vector<NNQueryReturn> neighbors;

    int                   checked  = 0;
    bool                  to_break = false;

    for(int i = 0; i < hash_tables_number; i++){
        // Getting the neighbors that our hash function returned
        long long int id_q;
        LinkedList_Iterator cand_neighbors = hash_tables[i]->get_neighbors(query_vec, table_size, id_q);

        // If we do not see them again we just add them to a vector as a NNQueryReturn
        while(!cand_neighbors.isNull()){
            int cand_id = cand_neighbors.getID();
            long long int cand_id_p = cand_neighbors.getID_p();


            if(vector_ids.find(cand_id) == vector_ids.end() && cand_id_p == id_q){
                checked++;

                vector_ids.insert(cand_id);
                vector<double> point_data = *cand_neighbors.getData();
                double distance = distance_fn(query_vec, point_data);
                neighbors.push_back(NNQueryReturn(cand_neighbors.getData(),
                                                            distance, cand_id));
            }

            // This is an optional optimization.
            /*if(checked >= 10000 * hash_tables_number){
                to_break = true;
                break;
            }*/

            cand_neighbors++;
        }

        if(to_break) break;
    }

    if (neighbors.size() > 1) {
      sort(neighbors.begin(),neighbors.end());
    }

    vector<NNQueryReturn> final_neighbors;
    if(neighbors.size() > k){
      for (int i = 0; i < k; i++){
          final_neighbors.push_back(neighbors[i]);
      }
      return final_neighbors;
    }

    return neighbors;
}

const vector<int> LSH_Structure::rangeSearch(const vector<double>& query_vec,
                                            const double& radius)
{
    set<int> r_neighbors_ids;
    set<int> checked_ids;

    int      checked  = 0;
    bool     to_break = false;

    for(int i = 0; i < hash_tables_number; i++){
        // Getting the neighbors of the bucket returned
        long long int id_q;
        LinkedList_Iterator cand_neighbors = hash_tables[i]->get_neighbors(query_vec, table_size, id_q);

        while(!cand_neighbors.isNull()){
            // If we don't see that again and has distance < radius we append it
            if(checked_ids.find(cand_neighbors.getID()) == checked_ids.end()){
                checked_ids.insert(cand_neighbors.getID());
                checked++;

                if(distance_fn(query_vec, *cand_neighbors.getData()) < radius){
                    r_neighbors_ids.insert(cand_neighbors.getID());
                }
            }

            /*if(checked >= 10000 * hash_tables_number){
                to_break = true;
                break;
            }*/

            cand_neighbors++;
        }

        if(to_break) break;
    }

    return vector<int>(r_neighbors_ids.begin(), r_neighbors_ids.end());
}

// Solving the queries from query set file
void LSH_Structure::evaluateQuerySet(const Dataset& data, ifstream& querySetFile,
                        ofstream& outFile, const int& k, const double& radius)
{
    int length = 4, int_value, magic_number;
    int numberOfImages, rows, cols;
    char* buffer = new char[length];

    int i = 0;
    while (i < 4){
    //first 4 values are 4 bytes each
    querySetFile.read(buffer,4);

    int* buffer2 = (int*) buffer; // read the 4 bytes as int
    int int_value = reverseInt(*buffer2);

    if (i == 0) {magic_number = int_value;}
    else if (i == 1) {numberOfImages = int_value;}
    else if (i == 2) {rows = int_value;}
    else {cols = int_value;}
    i++;
    }

    double total_lsh_time  = 0.0;
    double total_true_time = 0.0;

    double max_error = 0.0;
    double avg_error = 0.0;
    int not_found = 0;
    int how_many  = 0;

    //buffer = (char*) realloc(buffer,1);
    // Reading the query vector
    double total_time = 0.0;
    for(int i=0; i < 200 /*!querySetFile.eof()*/; ++i)
    {
        vector<double> img_vector;
        assert(img_vector.size() == 0);

        for(int r = 0; r < rows ; ++r)
        {
            for(int c = 0; c < cols; ++c)
            {
                querySetFile.read(buffer,1);
                unsigned char* buffer2 = (unsigned char*) buffer;
                unsigned char pixel = buffer2[0];
                img_vector.push_back((double) pixel);
            }
        }

        clock_t start, end;

        // Finding approximante kNN, real kNN and range query
        start = clock();
        const vector<NNQueryReturn> approx_k_neighbors     = kNearestNeighbors(img_vector, k);
        end   = clock();

        total_lsh_time = ((double) end - start) / ((double) CLOCKS_PER_SEC);

        const vector<int>           range_query_res = rangeSearch(img_vector, radius);

        start = clock();
        const vector<double>        true_k_nearest_dists = data.getKNearestDistances(distance_fn, img_vector, k);
        end   = clock();

        total_true_time = ((double) end - start) / ((double) CLOCKS_PER_SEC);

        // Doing the necessary writes to output file
        outFile << "Query: " << i+1 << endl;

        for(int i = 0; i < k; i++){
            if(i < approx_k_neighbors.size()){
                assert(approx_k_neighbors[i].dist_from_query >= true_k_nearest_dists[i]);

                outFile << "Nearest neighbor-" << i+1 << ":" << approx_k_neighbors[i].neighbor_id << endl;
                outFile << "distanceLSH: " << approx_k_neighbors[i].dist_from_query << endl;
            } else{
                not_found++;
                outFile << "Nearest neighbor-" << i+1 << ":" << "-" << endl;
                outFile << "distanceLSH: " << "-" << endl;
            }

            outFile << "distanceTrue: " << true_k_nearest_dists[i] << endl;
        }

        outFile << "tLSH: "  << total_lsh_time  << endl;
        outFile << "tTrue: " << total_true_time << endl;
        outFile << "R-near neighbors:" << endl;

        for(int i = 0; i < range_query_res.size(); i++){
            outFile << range_query_res[i] << endl;
        }

        outFile << endl;

        assert(img_vector.size() == rows * cols);

        if(i % 100 == 0)
            cout << i << endl;

        double curr_error = 0.0;

        // avg error = 1 means no error
        if(approx_k_neighbors.size() && (true_k_nearest_dists[0] != 0)){
            curr_error = approx_k_neighbors[0].dist_from_query / true_k_nearest_dists[0];
            avg_error += curr_error;
            how_many++;
        }

        if(curr_error > max_error){
            max_error = curr_error;
        }
        total_time += total_lsh_time;
    }


    cout << "Max Error Factor " << max_error << endl;
    cout << "Average Error " << avg_error / ((double) how_many) << endl;
    cout << "Not found nearest neighbors " << not_found << endl;
    cout << "Total lsh time " << total_time << endl;

    delete[] buffer;
}
