#include "cube_functions.hpp"

//Code for random generator from
//https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
int random_generator()
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> dist_int(2, 6);

	int w = dist_int(gen);
	return w;
}

double t_random_generator(int w)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<double> dist(0, w);

	double t = dist(gen);
	return t;
}

int flip_a_coin()
{
	int random = rand() % 2;
//	cout << random << endl;
	return random;
}

//Code for random number from normal distribution
//https://stackoverflow.com/questions/60721093/random-number-from-normal-distribution-in-c
double get_Normal_Distribution()
{
	static unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	static default_random_engine generator(seed);
	
	static normal_distribution<double> normalDist(0, 1);
	
	return normalDist(generator);
}

//Returns dot product of two vectors normalized by w
double dot_product_w (Point p, V_random_vector v, int w)
{
	double sum = 0;                             
	for (unsigned int i = 0; i < p.get_dimension(); i++)
	{
		sum = sum + (unsigned int) p.get_i_coordinate(i) *  v.get_i_coordinate(i) / w;
	}
//	cout << sum << endl;
	return sum;
}

//Returns dot product of two vectors
double dot_product (Point p, V_random_vector v)
{
	double sum = 0;                             
	for (unsigned int i = 0; i < p.get_dimension(); i++)
	{
		sum = sum + (unsigned int) p.get_i_coordinate(i) *  v.get_i_coordinate(i);
	}
//	cout << sum << endl;
	return sum;
}

int h_p(Point *p, V_random_vector v, double t, int w)
{
	//Because the dot product it is very large, I do "/w" below 
	int h_p = floor((dot_product(*p, v) + t ) / w ); 
	return h_p;
}

//The below function checks if it has do map the key value that has the variable h_p with 0 or 1
//and if no it add this to the map and refreshes for some Point the f_string that has
void f_i(Point *p, int h_p, map <int, string>* f)
{
	map<int, string>::iterator it;
	it = f->find(h_p);
	if (it == f->end())
	{
		string temp = to_string(flip_a_coin());
		f->insert(pair<int, string>(h_p, temp));
		p->renew_f_string(temp);
	}
	else
	{
		p->renew_f_string(it->second);
	}
}

//Original source but I modified some things
//https://dev.to/ggorantala/hamming-distance-kcm
int hamming_distance(int x, int y)
{
    int x_or = x ^ y; // keep different bits, they will be represented as 1

    int dist = 0; // hamming_distance
    while(x_or > 0)
    { // check all bits
        if((x_or | 1) == x_or) // if true, means the leftmost bit is 1, hence different
            dist++;
        x_or = x_or >> 1;
    }
    return dist;
}

double euclidean_distance(Point x, Point y)
{
	double sum = 0.0;
	for (unsigned  int i = 0; i < x.get_dimension(); i++)
	{
		sum = sum + pow(x.get_i_coordinate(i) - y.get_i_coordinate(i), 2);
	}
	//cout << "sum " << sum << endl;
	return sqrt(sum);
}

void V_random_vector::set_random_vector_coordinates ()
{
	for (unsigned int i = 0; i < dimension; i++)
	{
		v.push_back(get_Normal_Distribution());
//		cout << v.at(i) << endl;
	}
}

double V_random_vector::get_i_coordinate (int i)
{
	return v.at(i);
}

V_random_vector::V_random_vector(unsigned int dimension)
{
	this->dimension = dimension;
	set_random_vector_coordinates();
}

void V_random_vector::print_v()
{
	for (unsigned int i = 0; i < dimension; i++)
	{
		cout << v.at(i) << " " ;
	}
	cout << endl;
}

bool compare_distance(near_point &np1, near_point &np2) { 
            return (np1.get_near_point_distance() < np2.get_near_point_distance()); }

//The below function performs the logic of search in the cube method
void search_cube(Point &p, vector<Point> *hyper_cube, vector<Point> &dataset, int  num_of_buckets, unsigned int N)
{
    int bucket;

	vector<int> neighbours;
	
	string str_bin_bucket = p.get_Point_f_string();
	bucket = stoi(str_bin_bucket, 0, 2);

	neighbours.push_back(bucket);
	int remain_probes = num_of_buckets - 1;
	int dist;
	int flag = 1;
	int current_distance = 1;
	//process which we calculate and store sorted the hamming distance from the value of the
	//variable bucket compared with the values of zeros and ones that represent every bucket we have
	while (flag == 1)
	{
		for (int g = 0; g < num_of_buckets ; g++)
		{
			dist = hamming_distance(g, bucket);
			if (remain_probes > 0)
			{
				if (dist == current_distance)
				{
					neighbours.push_back(g);
					remain_probes--;
				}
			}
			else
				flag = 0;
//				cout << "hamming_distance = " << i << " - " <<  bucket_x << "=" << dist << endl;
		}
		current_distance++;
	}

	double distance;
	int position = 0;
	remain_probes = num_of_buckets;
	unsigned int L = 0;
	vector<near_point> near_neighbors;
	near_point temp_near_point;
	//And for this point p we will store temporarily the distance it has from 
	//(N + all the points that are on the same bucket with the N point) points
	while (remain_probes > 0)
	{
		unsigned int j = 0;
		int current_index = neighbours.at(position);
		while (j < hyper_cube[current_index].size())
		{
			unsigned int point_id = hyper_cube[current_index].at(j).get_id();
			if (p.get_id() == point_id)
			{
				j++;
				continue;
			}
			distance = euclidean_distance(p, hyper_cube[current_index].at(j));
			temp_near_point.set_near_point_id(point_id);
			temp_near_point.set_near_point_distance(distance);
			near_neighbors.push_back(temp_near_point);
			L++;
			j++;
		}
		if (L >= N)
		{
			break;
		}
		remain_probes--;
		position++;
	}

	sort(near_neighbors.begin(), near_neighbors.end(), compare_distance);
	//And from these points that we had temporarily store and sort we will keep
	//the first N point for the point p 
	for (unsigned int i = 0; i < near_neighbors.size(); i++)
	{
		if (i == N)
		{
			break;
		}
		p.set_near_point_neighbors(near_neighbors.at(i));
	}
}
