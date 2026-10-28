#include <string>
#include <vector>


#include "../LSH/Hashing/Hash_table/LSH_Structure.hpp"
#include "../Hypercube/hypercube.hpp"
#include "../DistanceMetrics/DistanceMetrics.hpp"

class kNNGraph : public LSH_Structure {
private:
      int                 k;
      int                 l;
      int                 R;
      int                 N;
      int                 E;
      int                 T;
      int                 graph_size;
      LinkedList*         graph;
      Hypercube*          hq;

      const Dataset       data;

      double              (*distance_fn)(const std::vector<double>&, const std::vector<double>&);

public:
      kNNGraph(size_t k, size_t r, size_t l, size_t E, size_t N,
                      const size_t rows, const size_t& hash_tables_num, const int& table_size,
                      double (*distance_fun)(const std::vector<double>&, const std::vector<double>&),
                      const size_t& hash_fns_num, const double& w_size,
                      const size_t& v_vals_sz, const long long int& M, const Dataset& data);


      /*void kNNGraphHQ(const int buckets, double (*distance_fun)(const std::vector<double>&, const std::vector<double>&),
                      const double& w_size, const size_t& v_vals_sz,
                      const int& probes, const int& hash_fns_num, const int& M_p);



      Hypercube* datasetInitialize_hq(const Dataset& data);*/


      const void add() { graph_size++; }

      const void create_kNNGraph(int id, vector<NNQueryReturn> approx_k_neighbors);

      const vector<NNQueryReturn> GNNS(const vector<double>& query_vec);


      void evaluateQuerySet(const Dataset& data, const Dataset& query_pictures,
                              ofstream& outFile, const int& k);

      const void set_(int r,int N_,int exp,int t){
        R=r; this->N = N_; E = exp; T=t;
      }


      const void test(){
          LinkedList_Iterator neighbors = graph[0].begin();
          double dist;
          for (int i = 0; i < 20; i++){
              cout << distance_fn(data[0],data[neighbors.getID()]) << endl;
              neighbors++;
          }
          cout << "Tttttttttt" << endl;
          neighbors = graph[0].begin();
          for (int i = 0; i < 20; i++){
              cout << distance_fn(data[0],data[neighbors.getID()]) << endl;
              neighbors++;
          }
      }
};
