#include "MRNG_part.hpp"

void R::set_classR_point_id(unsigned int i)
{
	point_id = i;
}

unsigned int R::get_classR_point_id()
{
	return point_id;
}

void R::set_classR_point_checked_out()
{
	checked_out = 1;
}

int R::get_classR_point_checked_out()
{
	return checked_out;
}

void R::set_classR_distance_from_query(double distance)
{
	distance_from_query = distance;
}

double R::get_classR_distance_from_query()
{
	return distance_from_query;
}

void print_n_nearest_neighbors_of_point(Point &p)
{
	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	cout << "point with id " << p.get_id() << " has neighbors" << endl;
	for (unsigned int i = 0; i < p.get_near_point().size(); i++)
	{
		cout << "point id " << p.get_near_point().at(i).get_near_point_id() << endl;
	}
	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
}

void print_L_p_of_point(Point &p)
{
	cout << "----------------------------------------------------" << endl;
	cout << "point with id " << p.get_id() << " has L_p" << endl;
	for (unsigned int i = 0; i < p.get_L_p().size(); i++)
	{
		cout << "point id " << p.get_L_p().at(i).get_near_point_id() << endl;
		cout << "L_p  " << p.get_L_p().at(i).get_near_point_distance() << endl;
	}
	cout << "------------------------------------------------------" << endl;
}

void print_query_n_nearest_neighbors(Point &query, vector<R> &query_n_nearest_neighbors, Point &start_node)
{
	cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
	cout << "query id " << query.get_id() << endl;
	cout << "euclidean distance from starting node " << euclidean_distance(query, start_node) << endl;
	cout << "edqqqqq "<< query_n_nearest_neighbors.size() << endl;
	for (unsigned int i = 0; i < query_n_nearest_neighbors.size(); i++)
	{
		cout << "n nearest neighbor id " << query_n_nearest_neighbors.at(i).get_classR_point_id() << " ";
		cout << "n nearest neighbor distance " << query_n_nearest_neighbors.at(i).get_classR_distance_from_query() << endl;
	}
	cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
}

int checking_idpoint_inside_Rstructure(vector<R> &R_structure, unsigned int id_point)
{
	for (unsigned int i = 0; i < R_structure.size(); i++)
	{
		if (R_structure.at(i).get_classR_point_id() == id_point)
		{
			return 1;
		}
	}
	return 0;
}

bool compare_distance_from_query(R &r1, R &r2) {
            return (r1.get_classR_distance_from_query() < r2.get_classR_distance_from_query()); }

void generic_search_on_graph(vector<Point> &dataset, Point &start_node, Point &query,
	vector<R> &query_n_nearest_neighbors, int L, int N)
{
	unsigned int id = start_node.get_id();
	vector<R> R_structure;
	int i = 1;
	R temp_R_node;
	temp_R_node.set_classR_point_id(id);
	temp_R_node.set_classR_distance_from_query(euclidean_distance(dataset.at(id), query));
	R_structure.push_back(temp_R_node);
	while (i < L)
	{
		unsigned int p = 0;
		while ((p < R_structure.size()) && (R_structure.at(p).get_classR_point_checked_out() == 1))
		{
			p++;
		}
		if (p == R_structure.size())
		{
			break;
		}
		R_structure.at(p).set_classR_point_checked_out();
		vector<near_point> temp_point_L_p = dataset.at(p).get_L_p();
		for (unsigned int ii = 0; ii < temp_point_L_p.size(); ii++)
		{
			unsigned int temp_id = temp_point_L_p.at(ii).get_near_point_id();
			if (checking_idpoint_inside_Rstructure(R_structure, temp_id))
			{
				continue;
			}
			temp_R_node.set_classR_point_id(temp_id);
			temp_R_node.set_classR_distance_from_query(euclidean_distance(dataset.at(temp_id), query));
			R_structure.push_back(temp_R_node);
			i++;
		}
		sort(R_structure.begin(), R_structure.end(), compare_distance_from_query);
	}
	if (i < N)
	{
		N = i;
	}
	for (int i = 0; i < N; i++)
	{
		query_n_nearest_neighbors.push_back(R_structure.at(i));
	}
}

//centroid
void initialize_centroid(vector<Point> &dataset, Point &centroid)
{
	int dimension = dataset.at(0).get_dimension();
	centroid = dataset.at(0);

	centroid.set_id(dataset.size());

	for (unsigned int number = 1; number < dataset.size(); number++)
	{
		if (number % 1000 == 0)
		{
			cout << "[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]] " << dataset.at(number).get_id() << endl;
		}
		for (int i = 0; i < dimension; i++)
		{
			centroid.set_i_coordinate(i, (centroid.get_i_coordinate(i) *
					(number -1) + dataset.at(number).get_i_coordinate(i)) / number);
		}
	}
}

//The below function check for the point p and every point that is inside on
//vector<near_point> &point_neighbors (= r) (on what is class near_point check Point.hpp file),
//the edge pr of the triangle prt (which t is all the point that belongs to the
//set L_p for this point p)
void MRNG_with_n_nearest_neighbors_triangle_exist_check(vector<near_point> &point_neighbors,
	vector<near_point> &point_L_p, vector<Point> &dataset, Point &p)
{
	for (unsigned int r = 0; r < point_neighbors.size(); r++)
	{
		int condition = 1;
		unsigned int r_id_point = point_neighbors.at(r).get_near_point_id();
		Point r_point = dataset.at(r_id_point).get_Point();
		double p_r_distance = point_neighbors.at(r).get_near_point_distance();
		for (unsigned int t = 0; t < point_L_p.size(); t++)
		{
			unsigned int t_id_point = point_L_p.at(t).get_near_point_id();
			Point t_point = dataset.at(t_id_point).get_Point();
			double t_p_distance = p.get_L_p().at(t).get_near_point_distance();
			double t_r_distance = euclidean_distance(t_point, r_point);
			if (p_r_distance > t_r_distance && p_r_distance > t_p_distance)
			{
				condition = 0;
				break;
			}
		}
		if (condition)
		{
			//add r in p -> L_p
			p.add_L_p_by_reference(point_neighbors.at(r));
		}
	}
}

//The below function check for the point p and every point in dataset that doesnt belongs to the L_p
//for this point p (= r) the edge pr of the triangle prt (which t is all the point that belongs to the
//set L_p for this point p)
void only_MRNG_triangle_exist_check(vector<Point> &dataset, Point &p)
{
	for (unsigned int r = 0; r < dataset.size(); r++)
	{
		int flag = 0;
		for (near_point np: p.get_L_p())
		{
			if (np.get_near_point_id() == dataset.at(r).get_id())
			{
				flag = 1;
				break;
			}
		}
		if (flag)
		{
			continue;
		}
		int condition = 1;
		unsigned int r_id_point = dataset.at(r).get_id();
		Point &r_point = dataset.at(r_id_point);
		double p_r_distance = euclidean_distance(p, r_point);
		for (unsigned int t = 0; t < p.get_L_p().size(); t++)
		{
			unsigned int t_id_point = p.get_L_p().at(t).get_near_point_id();
			Point t_point = dataset.at(t_id_point).get_Point();
			double t_p_distance = p.get_L_p().at(t).get_near_point_distance();
			double t_r_distance = euclidean_distance(t_point, r_point);
			if (p_r_distance > t_r_distance && p_r_distance > t_p_distance)
			{
				condition = 0;
				break;
			}
		}
		if (condition)
		{
			//add r in p -> L_p
			near_point temp;
			temp.set_near_point_id(r_id_point);
			temp.set_near_point_distance(p_r_distance);
			p.add_L_p_by_value(temp);
		}
	}
}

//MRNG_with_cube_main function is being used if the user chose the MRNG method using cube and
//n nearest neighbors for every point
//On why I modified the MRNG method from the slides to have this alternative method and what I accomplish
//with using this method check README file on paragraph B PART OF THE ASSIGNMENT the fist two bold reasons
void MRNG_with_cube_main(vector<Point> &dataset, int N, int l, string query_file)
{
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//			   Create and initialize the entity cube with the points of the dataset part              //
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int dimension = check_Points_Dimension(&dataset);

	vector<V_random_vector> v;
	vector<double> t;

	for (int i = 0; i < k_cube; i++)
	{
		v.push_back(V_random_vector(dimension));
		t.push_back(t_random_generator(w_cube));
		//v.at(i).print_v();
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//vector<int> h_p_i[k] has k tables vector<int> which in the position I of the k tables we refer to
	//the I point, so in the j table in the position I has the value of h_j_(p_Ι_) and using the
	//map<int, string> f and placing the k values of 0 and 1 for the I point from the I position
	//of every table we find which bucket we must put this point

	vector<int> h_p_i[k_cube];
	map <int, string> f;

	srand((unsigned) time(NULL));

	//move variable has the minimum value of some hpi, which later this value I will use as normalize
	//to have positive the h(p) i.e. the variable move will have certainly a large negative value
	//so the -move will be a large positive value
	double move = 0.0;
	//first calculate hps
	for (int g = 0; g < k_cube; g++)
	{
		for (unsigned int i = 0; i < dataset.size(); i++)
		{
			int hpi = h_p(&dataset.at(i), v.at(g), t.at(g), w_cube);
			if (move > hpi)
			{
				move = hpi;
			}
			h_p_i[g].push_back(hpi);
			//cout << h_p_i[g].at(i) << endl;
		}
	}

	//then normalize hps and calculate f
	for (int g = 0; g < k_cube; g++)
	{
		for (unsigned int i = 0; i < dataset.size(); i++)
		{
			h_p_i[g].at(i) -= (move - 1);
//			cout << h_p_i[g].at(i) << endl;
			f_i(&dataset.at(i), h_p_i[g].at(i), &f);
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int num_of_buckets = pow(2,k_cube);

	vector<Point> hyper_cube[num_of_buckets];
	int bucket;

	for (unsigned int i = 0; i < dataset.size(); i++)
	{
		string str_bin_bucket = dataset.at(i).get_Point_f_string();
		bucket = stoi(str_bin_bucket, 0, 2);
		//cout << bucket << endl;

		hyper_cube[bucket].push_back(dataset.at(i));
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
	cout << "=============================================================" << endl;
	//first we find for each point in the dataset the point/s with the nearest distance from it i.e.
	//we initialize the L_p for each point, and in the same time we find the n nearest neighbors for each point
	//using the logic of search in the cube method (i.e. using the function void search_cube(...))
	for (unsigned int i = 0; i < dataset.size(); i++)
	{
		if (i % 1000 == 0)
		{
			cout << "------------------------------ " << dataset.at(i).get_id() << endl;
		}
		//the function search_cube perform the logic of search in the cube method
		//specifically for every point in the dataset will find the n (= (int)log(dataset.size()))
		//nearest neighbors for this point, the fuction void search_cube(...) is being defined
		//in the file cube_function.cpp
		search_cube(dataset.at(i), hyper_cube, dataset, num_of_buckets, (int)log(dataset.size()));

		//We initialize the L_p for this point with the point/s that have the nearest distance from it
		dataset.at(i).set_nearest_point_id(dataset.at(i).get_nearest_point_id_from_vector_neighbors(0));
		dataset.at(i).set_nearest_point_distance(dataset.at(i).get_nearest_point_distance_from_vector_neighbors(0));

		double min_distance = dataset.at(i).get_near_point().at(0).get_near_point_distance();
		dataset.at(i).add_L_p_by_reference(dataset.at(i).get_near_point().at(0));
		int g = 1;
		double temp_distance = dataset.at(i).get_near_point().at(g).get_near_point_distance();
		while (min_distance == temp_distance)
		{
			dataset.at(i).add_L_p_by_reference(dataset.at(i).get_near_point().at(g));
			g++;
			temp_distance = dataset.at(i).get_near_point().at(g).get_near_point_distance();
		}
	}
	cout << "------------------------------ " << endl;
	//Now we have for each point the point/s with the nearest distance from it and the n nearest
	//neighbors from it
	//So for each point of the dataset we will call "two times" the function
	//MRNG_with_n_nearest_neighbors_triangle_exist_check (which is being defined in the same file we are
	//now above), "first" for the points that are neighbors for this point so the r = the n neighbors
	//of this point and L_p is the point/s with the nearest distance from it and "second"
	//for the neighbors of every point that belongs to the L_p of point p and for L_p
	//we take again the L_p of point p
	for (unsigned int p = 0; p < dataset.size(); p++)
	{
		//cout <<dataset.at(p).get_id() << " " << dataset.at(p).get_L_p().size() <<  endl;

		if (p % 1000 == 0)
		{
			cout << "------------------------------ " << dataset.at(p).get_id() << endl;
		}

		vector<near_point> point_neighbors = dataset.at(p).get_near_point();

		vector<near_point> point_L_p = dataset.at(p).get_L_p();//t e L_p_
		//first time
		//r = neighbors of point p
		//L_p = L_p of point p
		MRNG_with_n_nearest_neighbors_triangle_exist_check(point_neighbors, point_L_p, dataset, dataset.at(p));

		for (unsigned int t = 0; t < point_L_p.size(); t++)
		{
			//second time
			//r = neighbors of every point that belongs to the L_p for the point p
			//L_p = again the L_p of point p
			vector<near_point> t_point_neighbors = dataset.at(t).get_near_point();
			MRNG_with_n_nearest_neighbors_triangle_exist_check(t_point_neighbors, point_L_p, dataset, dataset.at(p));
		}
		//cout <<dataset.at(p).get_id() << " " << dataset.at(p).get_L_p().size() <<  endl;
	}
	cout << "{{{{{{{{{{{{{{{{{{{{{{{{{{}}}}}}}}}}}}}}}}}}}}}}}}}}" << endl;
	cout << "------------------------------ " << endl;
}

//only_MRNG_main function is being used if the user chose the normal MRNG method
//(i.e. the MRNG method from the slides)
void only_MRNG_main(vector<Point> &dataset, int N, int l, string query_file)
{
	cout << "=============================================================" << endl;
	//for start we find for each point in the dataset the point/s with the nearest distance from it
	//and after that we initialize the L_p for the above point
	for (unsigned int i = 0; i < dataset.size(); i++)
	{
		if (i % 1000 == 0)
		{
			cout << "------------------------------ " << dataset.at(i).get_id() << endl;
		}
		vector<unsigned int> min_id;
		double min_distance = 10000;
		for (unsigned int ii = 0; ii < dataset.size(); ii++)
		{
			if (i == ii)
				continue;
			double distance = euclidean_distance(dataset.at(i), dataset.at(ii));
			if (min_distance > distance)
			{
				min_distance = distance;
				min_id.clear();

				min_id.push_back(dataset.at(ii).get_id());
			}
			else if (min_distance == distance)
			{
				min_id.push_back(dataset.at(ii).get_id());
			}
		}

		dataset.at(i).set_nearest_point_id(min_id.at(0));
		dataset.at(i).set_nearest_point_distance(min_distance);

		for (unsigned int ii = 0; ii < min_id.size(); ii++)
		{
			near_point temp;
			temp.set_near_point_id(min_id.at(ii));
			temp.set_near_point_distance(min_distance);
			dataset.at(i).add_L_p_by_value(temp);
		}
	}
	cout << "------------------------------ " << endl;
	//And now for each point of the dataset we call the function only_MRNG_triangle_exist_check (which
	//is being defined in the same file we are now above)
	for (unsigned int p = 0; p < dataset.size(); p++)
	{
		//cout <<dataset.at(p).get_id() << " " << dataset.at(p).get_L_p().size() <<  endl;

		if (p % 1000 == 0)
		{
			cout << "------------------------------ " << dataset.at(p).get_id() << endl;
		}

		only_MRNG_triangle_exist_check(dataset, dataset.at(p));

		//cout <<dataset.at(p).get_id() << " " << dataset.at(p).get_L_p().size() <<  endl;
	}
	cout << "{{{{{{{{{{{{{{{{{{{{{{{{{{}}}}}}}}}}}}}}}}}}}}}}}}}}" << endl;
	cout << "------------------------------ " << endl;
}

void output_text_file_write(ofstream* myfile, Point &query, vector<R> &real_nearest_neighbor,
	vector<R> &query_n_nearest_neighbors, float exhausted_time, float mrng_time, unsigned int N)
{

	*myfile << "Query: " << query.get_id() << endl;

	for (unsigned int i = 0; i < query_n_nearest_neighbors.size(); i++)
	{
		*myfile << "Nearest neighbor-" << to_string(i) << ": " << query_n_nearest_neighbors.at(i).get_classR_point_id()
			<< endl;
		*myfile << "distanceApproximate: " << query_n_nearest_neighbors.at(i).get_classR_distance_from_query()
			<< endl;
	}

	for (unsigned int i = 0; i < real_nearest_neighbor.size(); i++)
	{
		if (i == N)
		{
			break;
		}
		*myfile << "Nearest neighbor-" << to_string(i) << ": " << real_nearest_neighbor.at(i).get_classR_point_id()
			<< endl;
		*myfile << "distanceTrue: " << real_nearest_neighbor.at(i).get_classR_distance_from_query() << endl;
	}

	*myfile << "tAverageApproximate: " << mrng_time << endl;
	*myfile << "tAverageTrue: " << exhausted_time << endl;
	*myfile << "MAF: " << query_n_nearest_neighbors.at(0).get_classR_distance_from_query() /
		real_nearest_neighbor.at(0).get_classR_distance_from_query() << endl;
	*myfile << endl;
}

//The "main" fuction for the MRNG part
void MRNG_main(vector<Point> &dataset, int N, int l, string query_file, string output_text_file)
{
	ofstream myfile(output_text_file);

	if (!myfile)
	{
		cout << "Cannot open file " << output_text_file << endl;
		exit(0);
	}

	myfile << "MRNG Results" << endl;

	//Choose what MRNG method you want to run
	string method;
	cout << "You want MRNG method using cube and n nearest neighbors for every point\n";
	cout << "Type y / n" << endl;
	cin >> method;
	if ((method == "y") || (method == "yes") || (method == "Y") || (method == "Yes"))
	{
		//If you chose the MRNG method using cube and n nearest neighbors for every point
		//you go to the below function
		MRNG_with_cube_main(dataset, N, l, query_file);
	}
	else if ((method == "n") || (method == "no") || (method == "N") || (method == "No"))
	{
		cout << "You want the normal MRNG method\n";
		cout << "Type y / n" << endl;
		cin >> method;
		if ((method == "y") || (method == "yes") || (method == "Y") || (method == "Yes"))
		{
			//Else if you chose the normal MRNG method (i.e. the MRNG method from the slides)
			//you go to the below function
			only_MRNG_main(dataset, N, l, query_file);
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}

//Note: from here and now we manage the two types of MRNG the same way regardless the type of
//MRNG method we chose at the beginning
//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//					initialize_centroid	&& compose_navigating_node								  //
//									   part											             //
//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
	Point centroid;
	//initialize_centroid function is being defined above on the same file we are now
	initialize_centroid(dataset, centroid);
	//centroid.print_point();
	unsigned int min_id;
	double min_distance = 10000;
	for (unsigned int i = 0; i < dataset.size(); i++)
	{
		double distance = euclidean_distance(centroid, dataset.at(i));
		if (min_distance > distance)
		{
			min_distance = distance;
			min_id = dataset.at(i).get_id();
		}
	}
	Point &navigating_node = dataset.at(min_id);//start node p
 	//return;
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
//								 query file part											//
/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
	cout << "Reading the query file..." << endl;

	vector<Point> query;

	//query_file = "t10k-images.idx3-ubyte";

	query = read_binary_file(query_file, 20);

	//return;
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
	vector<double> average_error_point;
	//For every query we do the below
	for (unsigned int i = 0; i < query.size(); i++)
	{
		clock_t time_req_mrng;
		clock_t time_req_exhausted;

		//real nearest neighbor with exhaustive search method part

		vector<R> real_nearest_neighbor;

		time_req_exhausted = clock();
		double min = 10000;
		unsigned int min_id;
		for (unsigned int ii = 0; ii < dataset.size(); ii++)
		{
			double temp_distance = euclidean_distance(query.at(i), dataset.at(ii));
			if (min > temp_distance)
			{
				min = temp_distance;
				min_id = dataset.at(ii).get_id();
				R temp_R;
				temp_R.set_classR_distance_from_query(min);
				temp_R.set_classR_point_id(min_id);
				real_nearest_neighbor.push_back(temp_R);
			}
		}

		sort(real_nearest_neighbor.begin(), real_nearest_neighbor.end(), compare_distance_from_query);

		time_req_exhausted = clock() - time_req_exhausted;
		float exhausted_time = (float)time_req_exhausted/CLOCKS_PER_SEC;


		//generic search on graph method part

		vector<R> query_n_nearest_neighbors;
		cout << "////////////////////////////////////////////////////////////////////////" << endl;
		time_req_mrng = clock();
		//generic_search_on_graph function is being defined above on the same file we are now
		generic_search_on_graph(dataset, navigating_node, query.at(i), query_n_nearest_neighbors, l, N);
		time_req_mrng = clock() - time_req_mrng;
		float mrng_time = (float)time_req_mrng/CLOCKS_PER_SEC;


		average_error_point.push_back(query_n_nearest_neighbors.at(0).get_classR_distance_from_query()
			/ real_nearest_neighbor.at(0).get_classR_distance_from_query());


		//cout << "navigating_node id " << navigating_node.get_id() << " distance from query " <<
		//	euclidean_distance(navigating_node, query.at(i)) << endl;
		//cout << "real nearest neighbor id " << real_nearest_neighbor.at(0).get_classR_point_id() <<
		//	" distance from query " << real_nearest_neighbor.at(0).get_classR_distance_from_query() << endl;
		//print_query_n_nearest_neighbors(query.at(i), query_n_nearest_neighbors, navigating_node);
		output_text_file_write(&myfile, query.at(i), real_nearest_neighbor, query_n_nearest_neighbors,
			exhausted_time, mrng_time, N);
	}

	double final_Average_error = 0;
	for (unsigned int i = 0; i < average_error_point.size(); i++)
	{
		final_Average_error = average_error_point.at(i) + final_Average_error;
	}
	final_Average_error = final_Average_error / query.size();

	//cout << "final_Average_error " << final_Average_error << endl;

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
}
