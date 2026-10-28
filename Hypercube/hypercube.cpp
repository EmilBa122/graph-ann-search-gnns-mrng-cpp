#include <iostream>
#include <cmath>
#include <chrono>
#include <random>
#include <string>
#include <cstring>
#include <algorithm>
#include "hypercube.hpp"
#include "assert.h"
#include <bits/stdc++.h>
#include "../DistanceMetrics/DistanceMetrics.hpp"


Hypercube::Hypercube(const int buckets, double (*distance_fun)(const std::vector<double>&, const std::vector<double>&),
                const double& w_size, const size_t& v_vals_sz,
                const int& probes, const int& hash_fns_num, const int& M_p)
                :
                  window_size(w_size),
                  v_values_size(v_vals_sz), probes(probes), k(hash_fns_num),
                  M_points(M_p), distance_fn(distance_fun) // b = 2^d_hC = n
{
    this->buckets = buckets;
    table = new vector <Node> [buckets];

    // map points to f values
    //vector hash_array store f(h) map
    for (int i = 0; i < k; i++){
        map<int,int> m;
        hash_array.push_back(m);
    }

    //store h_value
    long long int M = 1;
    for (int i = 0; i < k; i++){
        hash_fun_vec.push_back(HashFunction(w_size, v_vals_sz, M));
    }


}

Hypercube::~Hypercube(){
    delete[] table;
}

void Hypercube::deleteItem(int index) // index = f
{
    // delete vector in table[index]

    if (table[index].empty() == 0){
        table[index].clear();
        table[index].shrink_to_fit();
    }

}

void Hypercube::insert(Node node) { // creates hash_array too
    string concat_result = "";
    random_device rd;
    mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0,1);


    for(int i = 0; i < k; i++){
        int hash_value = hash_fun_vec[i].get_value(node.data);
        // f(h(p)) not assigned
        if (hash_array[i].find(hash_value) == hash_array[i].end()) {
            hash_array[i].insert({hash_value, distribution(gen)});
        }
        concat_result += to_string(hash_array[i][hash_value]); //query binary tag
    }

    table[binaryStringToDec(concat_result)].push_back(node);
}

string Hypercube::get_querytag(vector<double> query_vector){
    string queryTag = "";
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::uniform_int_distribution<int> distribution(0,1);

    for (int i = 0; i < k; i++){

        int hash_value = hash_fun_vec[i].get_value(&query_vector);
        if (hash_array[i].find(hash_value) == hash_array[i].end()) {
            hash_array[i].insert({hash_value, distribution(generator)});
        }

        queryTag += to_string(hash_array[i][hash_value]);
    }

    return queryTag;
}


int binaryStringToDec(string binary){
    int value = 0;
    int indexCounter = 0;
    for(int i = binary.length()-1; i>=0; i--){

        if(binary[i]=='1'){
            value += pow(2, indexCounter);
        }
        indexCounter++;
    }

    return value;
}

vector<string> change_bits(const string bit_string,vector<vector<int>> combinations){
    vector<string> changed_vec;
    string changed_str;
    for (int i = 0; i < combinations.size(); i++){
         changed_str = bit_string;
         for (int j = 0; j < combinations[i].size(); j++){
                 int index = combinations[i][j];
                 if ( changed_str[index] == '1' ) changed_str[index] = '0';
                 else changed_str[index] = '1';
         }
         changed_vec.push_back(changed_str);
    }

    return changed_vec;
}

// Hamming distance
// first probe is the input string so we pass probes - 1
vector<vector<int>> Hypercube::comb(const string str, int probes)
{
    int N = str.length();
    int count = 0;
    vector<vector<int>> combinations;
    while (count < probes){
        for (int i = 1; i <= str.length(); i++){
                std::string bitmask(i,1); // i leading 1's
                bitmask.resize(N, 0); // N-K trailing 0's
                vector<int> permutation;

                // print integers and permute bitmask
                do {
                        for (int i = 0; i < N; ++i) // [0..N-1] integers
                        {
                                if (bitmask[i]) {
                                        permutation.push_back(i);
                                }

                        }
                        vector<vector<int>> temp;
                        temp.push_back(permutation);
                        // if probe is empty find another neighboring probe
                        if ( !table[binaryStringToDec(change_bits(str, temp)[0])].empty() )
                        {
                          combinations.push_back(permutation);
                          count++;
                        }
                        permutation.clear();
                        temp.clear();

                        if (count == probes) return combinations;

                } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
        }
    }

    return combinations;
}


// return neighbors index
vector<int> Hypercube::neighbors(string queryTag, int probes){
    vector<vector<int>> combinations = comb(queryTag, probes);
    vector<string> comb_vec_str = change_bits(queryTag, combinations);
    vector<int> probe_values;

    for (int i = 0; i < comb_vec_str.size(); i++){
        probe_values.push_back(binaryStringToDec(comb_vec_str[i]));
    }

    return probe_values;
}

void Hypercube::datasetInitialize(const Dataset& data){
    const int row_num = data.getRows();

    for(int i = 0; i < row_num; i++){
        const vector<double>* data_ptr = &data[i];
        int id_p;
        Node node(data_ptr, i, id_p, NULL);
        insert(node);
    }
}

// function to display hash table
void Hypercube::displayHypercube() {
    int count = 0;
    for (int i = 0; i < pow(2,k); i++) {
        if (table[i].size() > 0) cout << i <<": ";
        for (auto& x : table[i]) {
            for (int j = 0; j < (x.data)->size(); j++)
              if (j == 0) {cout << "id: " << x.data_id; count++;} // print point(maybe id)
            cout << endl;
        }
        if (table[i].size() > 0) cout << endl;
    }
}

void Hypercube::displayNeighbors(int* N){
    for (int i = 0; i < k; i++){
        if (table[N[i]].empty() == 0){
            cout<<N[i]<<endl;
            cout<<"Neighbors:" << endl; // id
            for (auto& x : table[N[i]] ) {
                for (int j = 0; j < (x.data)->size(); j++)
                   if (j == 0 ) cout << " " << x.data_id; // print point(maybe id)
                cout << endl;
            }
            cout << endl;
        }
    }
}

vector<NNQueryReturn> Hypercube::kNearestNeighbors(const vector<double>& query_vec,const int& nearest_neighbors){
    vector<NNQueryReturn> dist_id;

    vector<int> probe_values;
    // hamming distance to find all neighbors probes
    probe_values = neighbors(get_querytag(query_vec), probes-1);
    //insert queryTag index, its the first probe we must check
    probe_values.insert(probe_values.begin(),binaryStringToDec(get_querytag(query_vec)));
    //NNQueryReturn dist_node;
    int counter = 0;
    int flag = 0;

    for (int i = 0; i < probe_values.size(); i++){
        for (auto& x : table[probe_values[i]]){
            counter++; // not empty probe has been checked
            NNQueryReturn dist_node = NNQueryReturn(x.data, distance_fn(query_vec, *(x.data)), x.data_id);
            //dist_node.neighbor_vec = x.data;
            //dist_node.dist_from_query = distance_fn(query_vec, *(x.data));
            //dist_node.neighbor_id = x.data_id;
            dist_id.push_back(dist_node);

            if (counter == M_points /*&& nearest_neighbors == 1*/){ //visit another neighbor,M_points
                counter = 0;
                flag = 1;
                break;
            }
        }
        if (flag == 1) break;
    }

    if (dist_id.size() > 0) sort(dist_id.begin(), dist_id.end());
    /*if(dist_id.size() > nearest_neighbors)
            dist_id.resize(nearest_neighbors);
    return dist_id;*/

    vector<NNQueryReturn> final_dist_id;
    if(dist_id.size() > k){
      for (int i = 0; i < k; i++){
          final_dist_id.push_back(dist_id[i]);
      }
      return final_dist_id;
    }
        //neighbors.resize(k);

    return dist_id;
}

const vector<int> Hypercube::rangeSearch(const vector<double>& query_vec,
                                            const double& radius)
{
    vector<int> in_range;
    vector<int> probe_values;
    probe_values = neighbors(get_querytag(query_vec), probes-1);
    probe_values.insert(probe_values.begin(),binaryStringToDec(get_querytag(query_vec)));
    int counter = 0;
    int flag = 0;

    for (int i = 0; i < probe_values.size(); i++){
        for (auto& x : table[probe_values[i]]){
            counter++;
            if (distance_fn(query_vec, *(x.data)) < radius){
                in_range.push_back(x.data_id);
            }

            if (counter == M_points /*&& nearest_neighbors == 1*/){ //visit another neighbor,visit M_points
                counter = 0;
                flag = 1;
                break;
            }

        }
        if (flag == 1) break;
    }

    //delete N;
    sort(in_range.begin(), in_range.end());
    return in_range;
}


void Hypercube::evaluateQuerySet(const Dataset& data, ifstream& querySetFile,
                        ofstream& outFile, const int& k, const double& radius){
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

    double total_hc_time  = 0.0;
    double total_true_time = 0.0;

    double max_error = 0.0;
    double avg_error = 0.0;
    int not_found = 0;
    int how_many = 0;

    double total_time = 0.0;
    for(int i=0; i < 200/*!querySetFile.eof()*/ ; ++i)
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

        start = clock();
        const vector<NNQueryReturn> approx_k_neighbors = kNearestNeighbors(img_vector, k);
        end   = clock();

        total_hc_time = ((double) end - start) / ((double) CLOCKS_PER_SEC);

        const vector<int>           range_query_res = rangeSearch(img_vector, radius);

        start = clock();
        const vector<double>        true_k_nearest_dists = data.getKNearestDistances(distance_fn, img_vector, k);
        end   = clock();

        total_true_time = ((double) end - start) / ((double) CLOCKS_PER_SEC);

        outFile << "Query: " << i+1 << endl;

        for(int i = 0; i < k; i++){
            if(i < approx_k_neighbors.size()){
                assert(approx_k_neighbors[i].dist_from_query >= true_k_nearest_dists[i]);

                outFile << "Nearest neighbor-" << i+1 << ":" << approx_k_neighbors[i].neighbor_id << endl;
                outFile << "distanceHC: " << approx_k_neighbors[i].dist_from_query << endl;
            } else{
                not_found++;
                outFile << "Nearest neighbor-" << i+1 << ":" << "-" << endl;
                outFile << "distanceHC: " << "-" << endl;
            }

            outFile << "distanceTrue: " << true_k_nearest_dists[i] << endl;
        }

        outFile << "tHC: "  << total_hc_time  << endl;
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

        if(approx_k_neighbors.size() && (true_k_nearest_dists[0] != 0)){
            curr_error = approx_k_neighbors[0].dist_from_query / true_k_nearest_dists[0];
            avg_error += curr_error;
            how_many++;
        }

        if(curr_error > max_error){
            max_error = curr_error;
        }
        total_time += total_hc_time;
    }

    cout << "Max Error Factor " << max_error << endl;
    cout << "Average Error " << avg_error / ((double) how_many) << endl;
    cout << "Not found nearest neighbors " << not_found << endl;
    cout << "Total hq time " << total_time << endl;
}
