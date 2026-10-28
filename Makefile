COMPILER     = g++
FLAGS        = -g -g3 -Wpedantic -O3
LSH_OBJS     = AmplifiedHashFunction.o h_function.o hash_table.o LSH_Structure.o Dataset.o DistanceMetrics.o Maths.o lsh.o
CUBE_OBJS  	 = AmplifiedHashFunction.o h_function.o hash_table.o LSH_Structure.o Dataset.o DistanceMetrics.o Maths.o hypercube.o cube.o
GNN_OBJS     = AmplifiedHashFunction.o h_function.o hash_table.o LSH_Structure.o Dataset.o DistanceMetrics.o Maths.o graphknn.o graph_search.o #hypercube.o
OBJS_GM			 = Point.o typical_functions.o cube_functions.o MRNG_part.o AmplifiedHashFunction.o h_function.o hash_table.o LSH_Structure.o Dataset.o DistanceMetrics.o Maths.o graphknn.o graph_search.o


#FLAGS_MRNG   = -std=c++11 -g -c -O3

all: graph_search lsh cube

cube: $(CUBE_OBJS)
				$(COMPILER) $(FLAGS) $(CUBE_OBJS) -o cube

lsh: $(LSH_OBJS)
				$(COMPILER) $(FLAGS) $(LSH_OBJS) -o lsh

graph_search: $(OBJS_GM)
								$(COMPILER) $(FLAGS) $(OBJS_GM) -o graph_search

# create/compile the individual files >>separately<<
Point.o: ./Mrng/Point.cpp ./Mrng/Point.hpp
	$(COMPILER) $(FLAGS) -c ./Mrng/Point.cpp

typical_functions.o: ./Mrng/typical_functions.cpp ./Mrng/typical_functions.hpp ./Mrng/Point.hpp
	$(COMPILER) $(FLAGS) -c ./Mrng/typical_functions.cpp

cube_functions.o: ./Mrng/cube_functions.cpp ./Mrng/cube_functions.hpp ./Mrng/Point.hpp
	$(COMPILER) $(FLAGS) -c ./Mrng/cube_functions.cpp

MRNG_part.o: ./Mrng/MRNG_part.cpp ./Mrng/MRNG_part.hpp ./Mrng/typical_functions.hpp ./Mrng/cube_functions.hpp
	$(COMPILER) $(FLAGS) -c ./Mrng/MRNG_part.cpp


graph_search.o: ./GraphSearch/graph_search.cpp ./Mrng/MRNG_part.hpp ./Mrng/typical_functions.hpp
				$(COMPILER) $(FLAGS) -c ./GraphSearch/graph_search.cpp

graphknn.o: ./GraphSearch/graphknn.cpp
				$(COMPILER) $(FLAGS) -c ./GraphSearch/graphknn.cpp

AmplifiedHashFunction.o: ./LSH/Hashing/G_function/AmplifiedHashFunction.cpp
				$(COMPILER) $(FLAGS) -c ./LSH/Hashing/G_function/AmplifiedHashFunction.cpp

h_function.o: ./LSH/Hashing/H_function/h_function.cpp
				$(COMPILER) $(FLAGS) -c ./LSH/Hashing/H_function/h_function.cpp

hash_table.o: ./LSH/Hashing/Hash_table/hash_table.cpp
				$(COMPILER) $(FLAGS) -c ./LSH/Hashing/Hash_table/hash_table.cpp

LSH_Structure.o: ./LSH/Hashing/Hash_table/LSH_Structure.cpp
				$(COMPILER) $(FLAGS) -c ./LSH/Hashing/Hash_table/LSH_Structure.cpp

Dataset.o: ./Dataset/Dataset.cpp
				$(COMPILER) $(FLAGS) -c ./Dataset/Dataset.cpp

DistanceMetrics.o: ./DistanceMetrics/DistanceMetrics.cpp
				$(COMPILER) $(FLAGS) -c ./DistanceMetrics/DistanceMetrics.cpp

Maths.o: ./Maths/Maths.cpp
				$(COMPILER) $(FLAGS) -c ./Maths/Maths.cpp

lsh.o: ./LSH/lsh.cpp
				$(COMPILER) $(FLAGS) -c ./LSH/lsh.cpp

hypercube.o: ./Hypercube/hypercube.cpp
				$(COMPILER) $(FLAGS) -c ./Hypercube/hypercube.cpp

cube.o: ./Hypercube/cube.cpp
				$(COMPILER) $(FLAGS) -c ./Hypercube/cube.cpp

.PHONY: clean

clean:
				rm -rf $(GNN_OBJS) $(LSH_OBJS) $(CLUSTER_OBJS) $(CUBE_OBJS) $(OBJS_GM) cube cluster graph_search lsh
