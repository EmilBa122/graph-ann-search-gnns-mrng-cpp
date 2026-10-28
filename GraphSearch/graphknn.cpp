#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <set>
#include <algorithm>
#include <ctime>
#include <experimental/random>


#include "./graphknn.hpp"


using namespace std;



kNNGraph::kNNGraph(size_t k, size_t r, size_t l, size_t E, size_t N,
                const size_t rows, const size_t& hash_tables_num, const int& table_size,
                double (*distance_fun)(const std::vector<double>&, const std::vector<double>&),
                const size_t& hash_fns_num, const double& w_size,
                const size_t& v_vals_sz, const long long int& M, const Dataset& data)
                : k(k), R(r), l(l), E(E), N(N), distance_fn(distance_fun), data(data),
                  LSH_Structure(hash_tables_num, table_size, EuclideanDistance,
                                hash_fns_num, w_size, v_vals_sz, M)
{
      graph = new LinkedList[rows];
      T = 20;
      graph_size = 0;
}


const void kNNGraph::create_kNNGraph(int id, const vector<NNQueryReturn> approx_k_neighbors){
      // linked list adds item in start of list
      for (int i = approx_k_neighbors.size()-1; i >= 0; i--){
          //cout << approx_k_neighbors[i].dist_from_query << endl;
          if (approx_k_neighbors[i].dist_from_query != 0) graph[id].add(approx_k_neighbors[i].neighbor_vec,
                                                                            approx_k_neighbors[i].neighbor_id);
      }
}

const vector<NNQueryReturn> kNNGraph::GNNS(const vector<double>& query_vec){
      random_device rd; // obtain a random number from hardware
      mt19937 gen(rd()); // seed the generator
      uniform_int_distribution<> distr(0, graph_size-1); // define the range,or rand_max

      //cout << "size: " << graph_size << endl;

      vector<NNQueryReturn> nearest_neighbors;
      set<int> S_comp; // use for already checked
      set<int> S_expanded;
      auto cmp = [](NNQueryReturn a, NNQueryReturn b) { return a.dist_from_query < b.dist_from_query; };
      set<NNQueryReturn, decltype(cmp)> S_(cmp);
      //set<NNQueryReturn>  S_;
      double distance;

      for (int r = 0; r < R; r++){
          int random_point = distr(gen);
          while (graph[random_point].begin().isNull() || S_expanded.find(random_point) != S_expanded.end()){
              random_point = distr(gen);
          }
          //cout << "random_point: " << random_point << endl;
          LinkedList_Iterator neighbors = graph[random_point].begin();

          double current_node_dist = distance_fn(query_vec, data[random_point]);


          S_expanded.insert(random_point);
          S_.insert(NNQueryReturn(neighbors.getData(),
                                    current_node_dist, random_point));
          S_comp.insert(random_point);

          double min_dist = current_node_dist;
          int min_element = random_point;
          int min_first = 1;
          int point = random_point;
          for (int t = 0; t < T; t++){
              bool flag = false;

              for (int n = 0; n < E && !neighbors.isNull(); n++){
                  int id = neighbors.getID();

                  distance = distance_fn(query_vec, neighbors.getData2());

                  if (S_comp.find(id) == S_comp.end()){
                      S_comp.insert(id);

                      NNQueryReturn query = NNQueryReturn(neighbors.getData(),
                                                            distance, id);
                      S_.insert(query);
                  }


                  neighbors++;
                  if (distance < current_node_dist && graph[id].getSize() > 0 || min_first == 1){
                     flag = true;
                     min_dist = distance;
                     min_element = id;
                     min_first = 0;
                   }
              }

              if (flag == false){
                  break;
              }
              else{
                  neighbors = graph[min_element].begin();
                  point = min_element;
                  current_node_dist = min_dist;
              }
          }
      }

      for (auto it = S_.begin(); it != S_.end(); ++it){
          nearest_neighbors.push_back(*it);
      }


      if (!nearest_neighbors.empty()) sort(nearest_neighbors.begin(), nearest_neighbors.end());

      vector<NNQueryReturn> final_neighbors;
      if(nearest_neighbors.size() > k){
        for (int i = 0; i < k; i++){
            final_neighbors.push_back(nearest_neighbors[i]);
        }
        return final_neighbors;
      }

      //nearest_neighbors.resize(k);
      return nearest_neighbors;

}



void kNNGraph::evaluateQuerySet(const Dataset& data, const Dataset& query_pictures,
                        ofstream& outFile, const int& k)
{
    double total_gnn_time  = 0.0;
    double total_true_time = 0.0;

    double max_error = 0.0;
    double avg_error = 0.0;
    int not_found = 0;
    int how_many  = 0;

    int rows = query_pictures.getRows();
    int cols = query_pictures.getColumns();

    // Reading the query vector
    double total_time = 0.0;
    for(int i=0; i < 200/*query_pictures.getRows() /*!querySetFile.eof()*/; ++i)
    {

        clock_t start, end;

        // Finding approximante kNN, real kNN and range query
        start = clock();
        const vector<NNQueryReturn> approx_k_neighbors     = GNNS(query_pictures[i]);
        end   = clock();

        total_gnn_time = ((double) end - start) / ((double) CLOCKS_PER_SEC);

        start = clock();
        const vector<double>        true_k_nearest_dists = data.getKNearestDistances(distance_fn, query_pictures[i], k);
        end   = clock();

        total_true_time = ((double) end - start) / ((double) CLOCKS_PER_SEC);

        // Doing the necessary writes to output file
        outFile << "Query: " << i+1 << endl;

        for(int i = 0; i < k; i++){
            if(i < approx_k_neighbors.size()){
                assert(approx_k_neighbors[i].dist_from_query >= true_k_nearest_dists[i]);

                outFile << "Nearest neighbor-" << i+1 << ":" << approx_k_neighbors[i].neighbor_id << endl;
                outFile << "distanceGNN: " << approx_k_neighbors[i].dist_from_query << endl;
            } else{
                not_found++;
                outFile << "Nearest neighbor-" << i+1 << ":" << "-" << endl;
                outFile << "distanceGNN: " << "-" << endl;
            }

            outFile << "distanceTrue: " << true_k_nearest_dists[i] << endl;
        }

        outFile << "tGNNS: "  << total_gnn_time  << endl;
        outFile << "tTrue: " << total_true_time << endl;

        outFile << endl;

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
        total_time += total_gnn_time;
    }


    cout << "Max Error Factor " << max_error << endl;
    cout << "Average Error " << avg_error / ((double) how_many) << endl;
    cout << "Not found nearest neighbors " << not_found << endl;
    cout << "Total gnn time " << total_time << endl;

}
