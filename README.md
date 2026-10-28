<h1 align="center">GNN and MRNG IMPLEMENTATION</h1>

## Assignment Description

The goal of the assignment is to **search** for the nearest neighbors of high-dimensional vectors efficiently using
**graph-based indexes** and to **compare** them with the LSH and Hypercube algorithms of assignment 1. The vectors are
images of handwritten digits from the **MNIST** dataset: each 28x28 image becomes a vector of 784 dimensions. All
distances are computed with the **Euclidean metric (L2)**.

-------------------------------

## Authors

**Apostolos Nikolaos Bonotis** <br>
**Emil Bantak** 

## Work distribution

### A part of the assignment (GNNS)

**Implemented by: Apostolos Nikolaos Bonotis**

Files:
- `GraphSearch/graph_search.cpp`
- `GraphSearch/graphknn.hpp`, `GraphSearch/graphknn.cpp`
- from his assignment 1: `LSH/`, `Hypercube/`, `Dataset/`, `DistanceMetrics/`, `Maths/`, `RangeQueryStructure/`

### B part of the assignment (Search-on-Graph with MRNG)

**Implemented by: Emil Bantak**

Files:
- `Mrng/Point.hpp`, `Mrng/Point.cpp`
- `Mrng/typical_functions.hpp`, `Mrng/typical_functions.cpp`
- `Mrng/cube_functions.hpp`, `Mrng/cube_functions.cpp`
- `Mrng/MRNG_part.hpp`, `Mrng/MRNG_part.cpp`

### C part of the assignment (Experiments)

**Joint work**

-------------------------------------------------------

### Part A: GNNS on a k-NN graph

Given a query image *q* and an integer *N*, the program `graph_search -m 1` returns approximately:
- the **nearest neighbor** of *q*
- the **N nearest neighbors** of *q*

The index is a **k-NN graph** (every image is connected to its **k** nearest images) that is built with **LSH**. It is
searched with **GNNS (Graph Nearest Neighbor Search)**, a greedy search with **R** random restarts, **T** greedy steps
and **E** expansions.

### Part B: Search-on-Graph on MRNG

Given a query image *q* and an integer *N*, the program `graph_search -m 2` returns approximately:
- the **nearest neighbor** of *q*
- the **N nearest neighbors** of *q*

The index is a **Monotonic Relative Neighborhood Graph (MRNG)** and it is searched with the **Search-on-Graph**
algorithm using a candidate pool of size **l**. Two versions of the MRNG are implemented: the normal MRNG of the slides
and the MRNG with the hypercube of assignment 1.

## How it works

### LSH (Locality-Sensitive Hashing)
Normal hash functions scatter similar items randomly. LSH does the opposite, it uses hash functions designed so that
nearby points are likely to land in the same bucket. Each point is projected onto random directions and cut into windows
of width `w`, combining `k` such hashes gives one hash table and we build `L` tables. To answer a query, we only compare
it against the points in its own buckets instead of the whole dataset, which is much faster at the cost of occasionally
missing the true nearest neighbor.

In this assignment LSH does not answer the queries of GNNS directly. It is used once, before the search, to build the
k-NN graph: every image of the dataset is given as a query to the LSH tables and its `k` approximate nearest neighbors
become its neighbors in the graph. The `lsh` program (the LSH implementation of assignment 1 of Apostolos Nikolaos
Bonotis, folder `LSH/`) is also used in the experiments of part C.

### Hypercube (Random Projection)
Each point is mapped to a corner (vertex) of a `k`-dimensional binary cube: every LSH hash value is randomly but
consistently turned into a 0 or 1 (the same value always gets the same bit), giving a bit string like `0110…`. Similar
points tend to get similar bit strings. To search, we look at the query's vertex first, then at neighboring vertices
with small Hamming distance (1 bit different, then 2, …), checking at most `M` points and `probes` vertices.

In this assignment the hypercube is used in two places. The `cube` program (the hypercube implementation of assignment 1
of Apostolos Nikolaos Bonotis, folder `Hypercube/`) is used in the experiments of part C. Inside the MRNG with cube
(part B) the hypercube implementation of assignment 1 of Emil  Bantak (file `Mrng/cube_functions.cpp`) finds quickly
the `n` nearest neighbors of every image, where `n = (int)log(dataset size)`: the search starts from the image's own vertex and moves to
neighboring vertices in increasing Hamming distance, taking all the points of each vertex, until at least `n` points
have been checked and then it keeps the `n` closest ones.

### k-NN graph
The k-NN graph is a directed graph where every image is a node with arrows to its `k` nearest images. Building it
exactly would need the distance between every pair of images, so it is built approximately with LSH. The graph is stored
as an array of linked lists, where list `i` holds the neighbors of image `i` in increasing distance.

### GNNS (Graph Nearest Neighbor Search)
GNNS searches the k-NN graph greedily. It starts from a random image of the graph and looks at the first `E` neighbors
of the current node (`E` <= `k`). It moves to the neighbor that is closest to *q* and repeats for at most `T` greedy
steps, or until no neighbor is closer to *q* than the current node (a local minimum). Because a single walk can get
stuck in a local minimum, this is repeated from `R` random starting images (random restarts). Every image seen during
the walks is kept in a set with its distance to *q* and at the end the set is sorted and the `N` closest images are
returned.

### MRNG (Monotonic Relative Neighborhood Graph)
A k-NN graph gives every image many neighbors, so every greedy step checks many points and the search can stop at a
local minimum. MRNG builds a graph with fewer edges and a guarantee: from any node there is a **monotonic path** to any
other node, i.e. a path where every step gets closer to the target. For every image *p* its neighbors `L_p` are chosen
like this:
- the nearest image(s) to *p* are always neighbors
- every other candidate *r* becomes a neighbor unless there is already a neighbor *t* such that *pr* is the longest side
  of the triangle *prt*. In that case *t* is closer to both *p* and *r*, so *r* can be reached through *t* and the edge
  *pr* is not needed

The result is a graph with a small number of neighbors per node and short search paths. Using all the images as
candidates (the normal MRNG of the slides) costs O(n^2 logn + n^2*γ), where γ is the average number of neighbors per node.
That is why the MRNG with cube takes as candidates only the `n` nearest neighbors of *p* and of the points in `L_p`,
which are found with the hypercube. The points that are far from *p* are not checked at all, because for almost all of
them the edge *pr* would be the longest side of a triangle *prt* and they would never become neighbors of *p*. In this
way the MRNG with cube produces almost the same graph and search results as the MRNG of the slides, while it saves a
large number of operations/checks for points that are not important and so it is built much faster.

### Search-on-Graph
MRNG is searched with the Generic Search on Graph. The search always starts from the **navigating node**, after the
graph is built, the centroid (mean) of the dataset is computed once and the image closest to it (found by brute force)
becomes the start node. Then a candidate pool is kept, sorted by distance to *q*: each time the closest candidate that
has not been checked yet is expanded, its neighbors that are not already in the pool are added and the pool is sorted
again. The search stops when the pool has `l` candidates (or no unchecked candidate is left) and returns the first `N`.
A larger `l` gives better accuracy but a slower search, and `l` >= `N`.

### Why graphs?
LSH and the hypercube of assignment 1 are hashing indexes: the query is compared with the images that fall in the same
buckets. A graph index connects every image to images that are close to it, so the search can walk from node to node
towards *q* and compute distances only for the nodes on its path. The search time is about γ·λ, where γ is the average
number of neighbors per node and λ is the length of the search path: GNNS keeps γ = `k` and uses random restarts to
escape local minima, while MRNG lowers both γ and λ. The quality of every method is measured with the **MAF (Maximum
Approximation Factor)**: for every query the distance of the approximate nearest neighbor is divided by the distance of
the true nearest neighbor and the largest ratio over all queries is reported (1 means that the search was always exact).

---------------------------------------------------------------------------------

## Dataset

The programs use the two MNIST dataset image files (compressed with gzip):

| File | Contents |
|---|---|
| `train-images-idx3-ubyte.gz` | MNIST **training set**: 60,000 images of handwritten digits (28x28 pixels) |
| `t10k-images-idx3-ubyte.gz` | MNIST **test set**: 10,000 images of handwritten digits (28x28 pixels) |

The training set is given as the input file (`-d`) and the test set as the query file (`-q`). Each image is stored as
784 pixel values (0-255) and is converted into a vector of 784 dimensions: GNNS reads the files with the class
`Dataset` and MRNG reads them with `read_binary_file(…)`, which converts every image into a `Point` object

**How the code uses them:**
- **`graph_search -m 1` (GNNS)**: the training images are hashed into the `L` LSH hash tables and every training image
  is used as a query to the LSH tables to find its `k` nearest neighbors, which gives the k-NN graph (the program can
  optionally use only the first images of the training set). The first 200 test images are then used as queries and
  searched in the k-NN graph with GNNS
- **`graph_search -m 2` (MRNG)**: the first 30,000 training images are used to build the MRNG (normal or with cube) and
  to compute the navigating node. The first 20 test images are then used as queries and searched in the MRNG with
  Search-on-Graph
- **`lsh` and `cube`**: the programs of assignment 1 that are used in the experiments of part C. The training images are
  stored in the LSH hash tables or in the vertices of the hypercube and the test images are used as queries

In every program the training images are also scanned by the exhaustive search, which gives the true distances and
times of the queries for comparison (`distanceTrue`, `tAverageTrue`, MAF)

The programs read the **uncompressed** files, so decompress them first

---------------------------------------------------------------------------

## Compile

Run the command make in the terminal. To delete the files  
that the compiler creates run make clean.

## To run the program use

```
./graph_search [–d <input file>] [–q <query file>]
[–k <number of nearests neighbors in graph>(def. 30)]
[-R <Random gnn’s parameter> (def. 1)]
[–N <number of nearests>(def. 1)]
[-l <int, only for mrng>(def. 20)]
[-m <1 for gnn,2 for mrng>]
[-o <output file>]
```

where:

input file is the path to the training set file.  
query file is the path to the set file that we will use for queries.  
k are the neighbors of each node in graph.  
r is a random int used in gnn.  
output file is the path where we save the results of gnn or mrng.  
number of nearest is the number of nearest neighbors from the query that we will find.  
m is a decision variable to decide which algortihm to use(gnn or mrng).  
l is candidate’s size.

All parameters are optional.Default values are defined and if path is not given then the program will ask for path.

## GNNS

### create_kNNGraph

For the construction of the graph I use the linked list of assignment 1.  
I create a graph array of linked lists,where the index i of the array contains,  
the k neighbors of point i.Because the elements are inserted in the list  
at its start,the nearest neighbors must be put in increasing order.  
Also for the experiments I observed that if we also put in the graph  
neighbors with long distances in some cases it improves  
the gnn.

### GNNS

The set S_expanded is so that we do not examine the same node again if  
it has already been selected because of the random point.  
The S_comp is for checking if the point already exists in the  
set,it has the same points(id) as the S_ but I do not do the comparison  
with the S_ because the query object would have to be created which takes  
more time.  
The S_ stores the neighbors and the distance from the query,  
the id of the neighbor in the structure NNQuery.  
We expand every neighbor until we reach a local minimum  
or for T iterations.I ignore the neighbors that do not have other neighbors  
in graph[id].I sort the S_ and I return the N neighbors.

## MRNG

1\) Initially when you go to execute the b’ part of the assignment you will be asked to choose to run the MRNG method using the cube and n nearest neighbors for every point or the normal MRNG method. The normal MRNG method corresponds to the MRNG according to the slides of the course while the MRNG method with cube and n nearest neighbors for every point is the MRNG modified in the part of the candidates meaning that we will not take all the points of the space to compute their distance from a point p to initialize the L_p but we will divide the space according to the logic of the cube. For every point p we will find its n_nearest_neighbors. The number of the points that will be examined is L + and the remaining points that are in the bucket with the L point (the value of L is computed as (int)log(dataset.size()), where the latter declares the number of neighbors that every point will have) meaning we will start from the vertex that p belongs to and we will continue to its neighboring ones. After that we sort these points that we have taken so as L_p we will take the smallest one/s of these points so with high probability in these vertices of the cube there will be the point/s with the minimum distance from the point p. So in this way we avoid taking the distances dataset – {p} and then sorting them. Consequently we have initialized the L_p as with the normal MRNG that exists in the slides saving a large volume of operations/checks.

**2) Another modification that has been made to the MRNG of the slides is that I also use the n nearest neighbors in the part of the triangles. Specifically in the MRNG from the slides we take every point r that belongs to R – L_p and every point t that belongs to L_p and we check if the edge pr is not the largest in the triangle prt so as r we do not need to take all the R – L_p points because for the vast majority of them the edge pr will certainly be the largest in the triangle prt, but we will take as r the n neighboring points of p and of t respectively that are between them because in most cases for these points their edge pr will not be the largest in the triangle prt. So instead of taking as r the whole R – L_p we take as r all the n neighboring points of p and respectively of t that are between them and so with this modification we produce the same results as in the MRNG of the slides saving a large number of operations/checks for points that are not important.**

## Design choices and Explanation of the Implementation of the exercise

### 1) Files Point.hpp/.cpp

**The most elementary object on which the assignment is implemented is the class Point, where this class represents a point**. In the private part we have an unsigned int id which is the id of a point, for its coordinates we have the vector\<double> coordinates, and for the number of the dimension in which the point lies we have the variable unsigned int dimension.  
**<u>ATTENTION</u>  if we want to run the MRNG method using the cube and n nearest neighbors** then we have the string f_string that has the number of the bucket where the point will go with the method of the cube and **we have a vector\<near_point> neighbors where we will store the n neighboring points for this point.**  
[For the neighboring points and for the L_p of a <u>point</u> I have made in the same file the class near_point which has stored the id of a point according to the variable unsigned int near_point_id and the distance of the latter from the <u>point</u> according to the variable double near_point_distance]. Having written this **for both MRNG methods I have a vector\<near_point> L_p that corresponds to the L_p of the theory** and an unsigned int nearest_point_id which keeps the id of the nearest point to the specific point and a double nearest_point_distance that keeps the distance of these two points. In the public part of the class Point I have a series of simple mutator and accessor functions so that I can interact with the variables that are in the private part of the class.

### 2) Files typical_functions.hpp/.cpp

I have the function void read_command(…) that reads and stores the parameters in the case that the user gives them. Also I have the function vector\<Point> read_binary_file(…) which reads data from a binary_file that we pass as an argument and returns a structure vector\<Point> that corresponds to the whole space on which we will work.

### 3) Files MRNG_part.hpp/.cpp

In the file MRNG_part.cpp the basic parts of the two MRNG methods are implemented. The function void MRNG_main(…) is the “main” function for the B’ part of the assignment. Initially the user will be asked which of the two MRNG methods he wants to run.

**If he wants the MRNG of the slides he must choose the normal MRNG method** so in this case we will **go to the function void only_MRNG_main(…) [which is in the same file]**. The function void only_MRNG_main(…) implements the theory on page 17 of the pdf. Specifically for every <u>point</u> of the dataset it initially finds the nearest point/s and does push_back of them to the vector\<near_point> L_p of the specific <u>point</u>. Then for every point of the dataset it calls the function only_MRNG_triangle_exist_check(…). The function void only_MRNG_triangle_exist_check(  vector\<Point> &dataset, Point &p) that is implemented in the same file where we are now initially receives as arguments a <u>point p</u> and the whole dataset and for this point p it checks with every single point r of the dataset if the edge pr is smaller than every t point, where every t point belongs to the structure vector\<near_point> L_p for the <u>specific point p</u>. If yes then it does push_back of the point r to the L_p of the <u>specific point p</u>. So after this function we return to the function only_MRNG_main(…) knowing for the specific point the L_p and we do this for all the points of the dataset.

So returning to the function MRNG_main(…) we now know for every point of the dataset its L_p. Next according to the theory page 19 of the pdf I compute the centroid of the dataset with the function void initialize_centroid(…) [which is implemented in the same file above] and I find navigating_node again according to page 19 of the theory pdf. Then I read and store the query as vector\<Point> query. At this point in the file MRNG_part.hpp I have defined the class R that we will use in the part of the Generic Search on Graph page 6 of the theory pdf. So the class R in its private part has an unsigned int point_id where the id of a point of the dataset is stored and a double distance_from_query where the distance of the point id from a query is stored. In the public part of the class R I have the corresponding mutators/accessors to interact with the variables that are in the private part of the class. We go back to the function MRNG_main(…) at the point where we have read and stored the query. Now for every single query we first compute the n nearest neighbors through the exhaustive search. Then we define a vector\<R> query_n_nearest_neighbors where we will store the n_nearest_neighbors of the query according to the generic search on graph page 6 of the theory pdf. Where the generic search on graph is implemented with the function void generic_search_on_graph(…) [it is in the same file further above] always according to the theory of the generic search on graph page 6 pdf.

**For the MRNG with cube and n nearest neighbors for every point in the function void MRNG_main(…) from the line of initialize_centroid and below the same logic as the normal MRNG is followed. The only thing that changes is that instead of calling from the function MRNG_main(…) where we are now the function only_MRNG_main(…) we call the function MRNG_with_cube_main(…)** and we use the files cube_functions.hpp/.cpp for the creation and initialization of the cube. Specifically we go to the **function void MRNG_with_cube_main(…) [it is in the same file we have been in so far further above]** and by first making the cube and placing the points of the dataset in the various buckets at the vertices of the cube **we use the function void search_cube(…) [it is in the file cube_functions.cpp] to find the (int)log(dataset.sixe()) number of neighbors of a point p.** So inside the function starting from the bucket where the point p is we take all the points of the bucket and we compute for every point its distance from the point p. We check if we have passed or reached the number of neighbors that we have given as an argument to the function, if not we go to the next vertex of the cube and we repeat the process, otherwise we sort the distances of the points that we have checked and we keep the N (= (int)log(dataset.sixe())) points as neighbors for this point p. We do this for all the points of the dataset. Then I follow the same logic as in the normal MRNG i.e. I call the function void MRNG_n_nearest_neighbors_triangle_exist(…) for the point p which essentially does the same work as the void only_MRNG_triangle_exist_check(…) just modified at the point that instead of checking all the points r that belong to the dataset – L_p it first checks for every single neighbor r of the point p if the distance pr is smaller than the respective distance pt for every point t of the L_p. When this function finishes we go back to the MRNG_with_cube_main(…) and we call again the MRNG_n_nearest_neighbors_triangle_exist(…) but this time as r we will take the neighbors of every t point of the L_p of the p point and we will check if the distance pr is smaller than the respective distance pt for every point t of the L_p. So in this way we avoid a huge volume of operations/checks for non important points and we achieve the same result as the normal MRNG.  
And then when the function MRNG_with_cube_main(…) finishes we return to the function MRNG_main(… ) and we continue normally as if we had the normal MRNG that I explained above.

## Experiments

- dataset size: 5000  
  GNN:  
  optimal values  
  (E,R,T)=(15,15,15), MAF = 1.4, AverageError = 1,03  
  total time for 200 queries 0.5 sec  
  time per query 0.004  
  Lsh:  
  optimal values  
  w = 20k, k = 6, L = 8 , M = 2^15-5  
  total time = 0.28 for 200 queries , time per query: 0.002  
  MAF = 1.67, Aer = 1.08

  I observe that, the lsh is slightly better than the gnn for a small dataset,  
  in time and accuracy.However, if the time of the gnn increases(x2 of the lsh)  
  then it has better results than the lsh(MAF: 1.4, Aer: 1.04) and  
  it is 2 times faster than the brute force while the optimal lsh within the same  
  time cannot achieve this accuracy.

  Hypercube:  
  optimal values  
  M=1k,p=10,k=10,w=15k  
  total time: 0.23, MAF: 1.60, Aer: 1.07

  M=2k,p=20,k=10,w=15k  
  total time: 0.5, MAF: 1.35, Aer: 1.02

  the Hypercube on a small dataset is better than the lsh and the gnn  
  in accuracy and in time and in the same times as the gnn it achieves  
  better results in contrast to the lsh.

  We can reduce the time of the gnn ,(E,R,T)=(15,15,15), with low  
  average error(1.05,better than the lsh) but the MAF will increase = 2.1.  
  As the R grows the MAF decreases and the optimal value of T  
  is about15.

- dataset size: 10k

  GNN:  
  optimal values  
  (E,R,T)=(30, 5, 20), MAF = 1.9, AverageError = 1.08  
  total time for 200 queries 0.48 sec  
  (E,R,T)=(50 5, 25), MAF = 1.57, AverageError = 1.06  
  total time for 200 queries 0.6 sec  
  (E,R,T)=(25, 20, 15), MAF = 1.4, AverageError = 1.03  
  total time for 200 queries 0.77 sec

  Lsh:  
  optimal values  
  w = 15k, k = 6, L = 8 , M = 2^15-5  
  total time = 0.72 for 200 queries  
  MAF = 1.45, Aer = 1.07

  I observe that,as the E increases decreasing the R and increasing  
  the T we get good times.

  The lsh is slightly slower than the gnn but it has a smaller MAF,  
  if the MAF of the gnn increases a little,then the gnn is faster  
  than the lsh with better Aer and MAF.That is in the same time  
  as the lsh(we do not run for the fastest time but for a  
  good time and better accuracy than the one we have  
  in the fastest lsh and gnn)the gnn is better than the  
  lsh.

  Therefore we observe an improvement of the gnn as the dataset increased.

- Dataset size: 20k

  GNN:  
  optimal values  
  (E,R,T)=(25, 10, 20), MAF = 1.43, AverageError = 1.04  
  total time for 200 queries 0.90 sec  
  (E,R,T)=(40, 15, 20), MAF = 1.28, AverageError = 1.01  
  total time for 200 queries 2.25 sec

  Lsh:  
  optimal values  
  w = 20k, k = 6, L = 8 , M = 2^15-5  
  total time = 1.58 for 200 queries  
  MAF = 1.57, Aer = 1.05

  I observe that, the gnn is  about 2 times faster than the lsh  
  and it has a smaller MAF and Aerr.Moreover,if the time  
  of the gnn increases(x1.5 of the lsh time) then the results are very good  
  (MAF = 1.28, AverageError = 1.01, total time = 2.25 sec) and  
  considerably faster than brute force.

  Hypercube:  
  optimal values  
  M=3k,p=20,k=10,w=15k  
  total time: 0.85, MAF: 1.46, Aer: 1.067

  M=10k,p=40,k=12,w=20k  
  total time: 2.35, MAF: 1.37, Aer: 1.04

  The hypercube is slightly faster(0.85 vs 0.90) than the gnn but  
  as we see in the values above the gnn is better in terms of  
  the accuracy.Moreover if we look at the MAF and Aerr of the optimal  
  hypercube for the time(2.35) and of the optimal gnn for the time(2.25)  
  we see that the hypercube does not have as good accuracy at this time  
  as the gnn.

I conclude that for large datasets out of the gnn,lsh,hypercube,  
the best algorithm is the gnn and for smaller ones the hypercube.

Mrng:  
As the datatet increases the time of the mrng increases slightly,  
for 15k it is 0.0002 per query and for 30k it is 0.0003.  
Therefore it is better in time than the gnn.lsh,hypercube,  
but it has a larger MAF(1.8).Increasing the l we have greater  
accuracy. By choosing a good hypercube for the initialization,also  
the results are improved.I observe that, with a moderate  
hypercube for the initialization,the Mrng greatly improves the  
accuracy.  
Moreover for a small dataset the hypercube remains better.  
Also the results in the mrng with cube and in the normal mrng  
in most cases do not have any notable deviation.
