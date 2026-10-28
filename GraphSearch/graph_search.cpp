#include <cstdbool>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>

#include <string>
#include <vector>
#include <stdexcept>

#include "../Dataset/Dataset.hpp"
#include "../DistanceMetrics/DistanceMetrics.hpp"
#include "../LSH/Hashing/Hash_table/LSH_Structure.hpp"
#include "../Hypercube/hypercube.hpp"
#include "../Mrng/MRNG_part.hpp"
#include "./graphknn.hpp"




#define WINDOW_SIZE 15000 //10000 works better in terms of error and time, 15000 even better results but slower
                         // 7500 faster but increases error

using namespace std;


bool tryParse(std::string& input, int& output) {
    try{
        output = std::stoi(input);
        if (output <= 0){
            cout << "must be a positive integer" << endl;
            return false;
        }
    } catch (std::invalid_argument) {
        return false;
    }
    return true;
}

void readPosInt(string& term_output, string& input, int& output){
    cout << term_output;
    cin >> input;
      while(!tryParse(input, output)){
          cout << term_output;
          cin >> input;
      }
}

int main(int argc, char* argv[]){
    int    k                 = 50;   // -k
    bool   knn_given         = false;

    int    l                 = 20;   // -l
    bool   candidates_given  = false;

    int    nearest_neighbors = 1;   // -N
    bool   nns_given         = false;

    int    exp               = 30; // -E
    bool   expantion_given   = false;

    int    r                 = 1;  // -R
    bool   random_num_given  = false;

    int    m                 = 1;  // -m
    bool   m_num_given       = false;

    char*  input_file_c_str  = NULL;
    char*  query_file_c_str  = NULL;
    char*  output_file_c_str = NULL;

    //////////////////////// Checking the arguments ///////////////////////////

    if((argc - 1) % 2 == 1){
        cout << "Usage: ./graph_search  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
        cout << "              [-E  <int>]  [-R  <int>]  [-Ν <number of nearest>] [-l <int, only for search graph>]" << endl;
        cout << "                                                      [-m <1 for GNNS, 2 for MRNG> -o <output_file>]" << endl;
        return 1;
    }

    // For convenience I will handle argv
    // elements as char* instead of STL string
    for(int i = 1; i < argc; i=i+2){
        if(!strcmp(argv[i], "-d")){
            if(input_file_c_str != NULL){
                cout << "Usage: ./graph_search  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-E  <int>]  [-R  <int>]  [-Ν <number of nearest>] [-l <int, only for search graph>]" << endl;
                cout << "                                                      [-m <1 for GNNS, 2 for MRNG> -o <output_file>]" << endl;

                return 1;
            }

            input_file_c_str = argv[i+1];
        } else if(!strcmp(argv[i], "-q")){
            if(query_file_c_str != NULL){
                cout << "Usage: ./graph_search  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-E  <int>]  [-R  <int>]  [-Ν <number of nearest>] [-l <int, only for search graph>]" << endl;
                cout << "                                                      -m <1 for GNNS, 2 for MRNG> -o <output_file>" << endl;

                return 1;
            }

            query_file_c_str = argv[i+1];
        } else if(!strcmp(argv[i], "-o")){
            if(output_file_c_str != NULL){
                cout << "Usage: ./graph_search  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-E  <int>]  [-R  <int>]  [-Ν <number of nearest>] [-l <int, only for search graph>]" << endl;
                cout << "                                                      -m <1 for GNNS, 2 for MRNG> -o <output_file>" << endl;

                return 1;
            }

            output_file_c_str = argv[i+1];
        } else if(!strcmp(argv[i], "-k")){
            if(knn_given){
                cout << "Usage: ./graph_search  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-E  <int>]  [-R  <int>]  [-Ν <number of nearest>] [-l <int, only for search graph>]" << endl;
                cout << "                                                      -m <1 for GNNS, 2 for MRNG> -o <output_file>" << endl;

                return 1;
            }

            knn_given = true;
            k         = atoi(argv[i+1]);
        } else if(!strcmp(argv[i], "-l")){
            if(candidates_given){
                cout << "Usage: ./graph_search  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-E  <int>]  [-R  <int>]  [-Ν <number of nearest>] [-l <int, only for search graph>]" << endl;
                cout << "                                                      -m <1 for GNNS, 2 for MRNG> -o <output_file>" << endl;

                return 1;
            }

            candidates_given = true;
            l                = atoi(argv[i+1]);
        } else if(!strcmp(argv[i], "-N")){
            if(nns_given){
                cout << "Usage: ./graph_search  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-E  <int>]  [-R  <int>]  [-Ν <number of nearest>] [-l <int, only for search graph>]" << endl;
                cout << "                                                      -m <1 for GNNS, 2 for MRNG> -o <output_file>" << endl;

                return 1;
            }

            nns_given         = true;
            nearest_neighbors = atoi(argv[i+1]);
        } else if(!strcmp(argv[i], "-R")){
            if(random_num_given){
                cout << "Usage: ./graph_search  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-E  <int>]  [-R  <int>]  [-Ν <number of nearest>] [-l <int, only for search graph>]" << endl;
                cout << "                                                      -m <1 for GNNS, 2 for MRNG> -o <output_file>" << endl;

                return 1;
            }

            random_num_given = true;
            r                = atof(argv[i+1]);
        } else if(!strcmp(argv[i], "-E")){
              if(expantion_given){
                  cout << "Usage: ./graph_search  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                  cout << "              [-E  <int>]  [-R  <int>]  [-Ν <number of nearest>] [-l <int, only for search graph>]" << endl;
                  cout << "                                                      -m <1 for GNNS, 2 for MRNG> -o <output_file>" << endl;

                  return 1;
              }

              expantion_given = true;
              exp             = atof(argv[i+1]);
        } else if(!strcmp(argv[i], "-m")){
              if(m_num_given){
                  cout << "Usage: ./graph_search  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                  cout << "              [-E  <int>]  [-R  <int>]  [-Ν <number of nearest>] [-l <int, only for search graph>]" << endl;
                  cout << "                                                      -m <1 for GNNS, 2 for MRNG> -o <output_file>" << endl;

                  return 1;
              }

              m_num_given = true;
              m           = atof(argv[i+1]);
        } else{
            cout << "Usage: ./graph_search  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
            cout << "              [-E  <int>]  [-R  <int>]  [-Ν <number of nearest>] [-l <int, only for search graph>]" << endl;
            cout << "                                                      -m <1 for GNNS, 2 for MRNG> -o <output_file>" << endl;

            return 1;
        }

      if(i == argc-2){
            if(m == 1 && candidates_given == true){
                cout << "Usage: ./graph_search  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-E  <int>]  [-R  <int>]  [-Ν <number of nearest>] [-l <int, only for search graph>]" << endl;
                cout << "                                                      -m <1 for GNNS, 2 for MRNG> -o <output_file>" << endl;

                return 1;
            }
            if(m == 2 && candidates_given != true){
                cout << "Usage: ./graph_search  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-E  <int>]  [-R  <int>]  [-Ν <number of nearest>] [-l <int, only for search graph>]" << endl;
                cout << "                                                      -m <1 for GNNS, 2 for MRNG> -o <output_file>" << endl;

                return 1;
            }
        }
    }

    string input_file_path;
    string query_file_path;
    string output_file_path;

    // Checking the files
    if(input_file_c_str == NULL){
        cout << "Give the path of the input file: " << endl;
        cin  >> input_file_path;
    }else{
        input_file_path = (const char*) input_file_c_str;
    }

    if(query_file_c_str == NULL){
        cout << "Give the path of the query file: " << endl;
        cin  >> query_file_path;
    }else{
        query_file_path = (const char*) query_file_c_str;
    }


    if (m == 1){
        string rerun_response = "y";


        ifstream input_file_stream(input_file_path, std::ios::binary);

        if(!input_file_stream.is_open()){
            cout << "Cannot open input file " << input_file_path << endl;
            return 1;
        }

        ifstream query_file_stream(query_file_path, std::ios::binary);

        if(!query_file_stream.is_open()){
            cout << "Cannot open query file " << query_file_path << endl;
            return 1;
        }

        Dataset pictures(input_file_stream);
        Dataset query_pictures(query_file_stream);


        string opt = "y";
        while(true){
            cout << "resize dataset y or n: ";
            cin >> opt;
            while (opt.compare("y") && opt.compare("n")){
                cout << "resize response must be: y or n ";
                cin >> opt;
            }

            if (!opt.compare("y")){
                int size ;
                string size_;
                string term_output = "dataset size: ";
                readPosInt(term_output,size_,size);
                pictures.resize_(size);
            }
            break;
        }


        cout << "rows " << pictures.getRows() << endl;

        double  window_size  = 15000;
        int     dimensions   = pictures.getColumns();

        const int hash_tables = 8; //    L 8 14 14  18 18 18 18 (8,6) (14,8) (6,4)
        const int hash_functions = 6; // h 6 8  12  12 8  10 14
        cout << "Window Size "  << window_size << endl;

        long long int M = (long long int) pow(2,12)-5;


        kNNGraph knn_graph(k, r, l, exp, nearest_neighbors, pictures.getRows(), hash_tables,
                        pictures.getRows() / 8, EuclideanDistance, hash_functions, window_size, dimensions, M, pictures);


        // calls lsh.datasetInitialize
        knn_graph.datasetInitialize(pictures);

        input_file_stream.close();
        //ofstream outFile("./test");


        for (int i = 0; i < pictures.getRows(); i++){
            const vector<NNQueryReturn> approx_k_neighbors     = knn_graph.kNearestNeighbors(pictures[i], k);
            /*const vector<double> true_k_nearest_dists = pictures.getKNearestDistances(EuclideanDistance, pictures[i], k);
            for(int i = 0; i < k; i++){
                if(i < approx_k_neighbors.size()){

                    outFile << "Nearest neighbor-" << i+1 << ":" << approx_k_neighbors[i].neighbor_id << endl;
                    outFile << "distanceLSH: " << approx_k_neighbors[i].dist_from_query << endl;
                } else{
                    //not_found++;
                    outFile << "Nearest neighbor-" << i+1 << ":" << "-" << endl;
                    outFile << "distanceLSH: " << "-" << endl;
                }

                outFile << "distanceTrue: " << true_k_nearest_dists[i] << endl;
            }
            outFile << endl;*/
            knn_graph.add();
            knn_graph.create_kNNGraph(i, approx_k_neighbors);
            if (i % 1000 == 0 && i != 0) cout << i << " points of graph created" << endl;
        }


        if(output_file_c_str == NULL){
            cout << "Give the path of the output file: " << endl;
            cin  >> output_file_path;
        } else{
            output_file_path = (const char*) output_file_c_str;
        }

        ofstream output_file_stream(output_file_path);

        if(!output_file_stream.is_open()){
            query_file_stream.close();
            cout << "Cannot open output file " << output_file_path << endl;
            return 1;
        }

        opt = "y";
        string exp_, r_, t_;
        int t;
        while(!opt.compare("y")){
                string term_output = "E: ";
                readPosInt(term_output,exp_,exp);

                term_output = "R: ";
                readPosInt(term_output,r_,r);

                term_output = "T: ";
                readPosInt(term_output,t_,t);

                knn_graph.set_(r,nearest_neighbors,exp,t);
                knn_graph.evaluateQuerySet(pictures, query_pictures, output_file_stream, nearest_neighbors);
                cout << "rerun: y or n: ";
                cin >> opt; // Get user input from the keyboard
                while (opt.compare("y") && opt.compare("n")){
                    cout << "rerun response must be: y or n ";
                    cin >> opt;
                }
          }

          output_file_stream.close();
          query_file_stream.close();

          input_file_c_str  = NULL;
          query_file_c_str  = NULL;
          output_file_c_str = NULL;
      }
      else if (m==2){
     //############################################################//
     //                              Mrng                         //
     //###########################################################//
          vector<Point> dataset = read_binary_file(input_file_c_str, 30000);

          MRNG_main(dataset, nearest_neighbors, l, query_file_c_str, output_file_c_str);
      }

}
