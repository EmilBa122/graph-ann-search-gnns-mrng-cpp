#include "Point.hpp"

void near_point::set_near_point_id (int i)
{
	near_point_id = i;
}

unsigned int near_point::get_near_point_id ()
{
	return near_point_id;
}

void near_point::set_near_point_distance (double temp)
{
	near_point_distance = temp;
}

double near_point::get_near_point_distance ()
{
	return near_point_distance;
}

void near_point::print_near_point()
{
    cout << near_point_id << " " << near_point_distance << endl;
}

void Point::set_id (unsigned int i)
{
	id = i;
}

void Point::set_dimension (unsigned int i)
{
	dimension = i;
}

void Point::set_coordinates (vector<double> image)
{
    for (unsigned int i = 0; i < image.size(); i++)
    {
        coordinates.push_back(image.at(i));
    }
}

unsigned int Point::get_dimension ()
{
    return dimension;
}

unsigned int Point::get_id ()
{
    return id;
}

double Point::get_i_coordinate (int i)
{
    return coordinates.at(i);
}

void Point::renew_f_string(string temp)
{
    f_string = f_string + temp;
}

void Point::point_print()
{
    cout << f_string << endl;
}

string Point::get_Point_f_string()
{
    return f_string;
}

void Point::print_point()
{
    for (unsigned int i = 0; i < dimension; i++)
    {
        cout << coordinates.at(i) << " ";
    }
    cout << endl;
}

Point Point::get_Point()
{
    return *this;
}

vector<double> Point::get_coordinates ()
{
    return coordinates;
} 

void Point::set_i_coordinate(int i, double value)
{
    coordinates.at(i) = value;
}

void Point::set_nearest_point_id(unsigned int i)
{
    nearest_point_id = i;
}

unsigned int Point::get_nearest_point_id()
{
    return nearest_point_id;
}

void Point::set_nearest_point_distance(double distance)
{
    nearest_point_distance = distance; 
}

double Point::get_nearest_point_distance()
{
    return nearest_point_distance;
}

void Point::set_near_point_neighbors(near_point temp_near_point)
{
    neighbors.push_back(temp_near_point);
}

void Point::print_near_point_neighbors()
{
    for (unsigned int i = 0; i < neighbors.size(); i++)
    {
        neighbors.at(i).print_near_point();
    }
}

unsigned int Point::get_nearest_point_id_from_vector_neighbors(unsigned int i)
{
    return neighbors.at(i).get_near_point_id();
}

double Point::get_nearest_point_distance_from_vector_neighbors(unsigned int i)
{
    return neighbors.at(i).get_near_point_distance();
}

vector<near_point> &Point::get_near_point()
{
    return neighbors;
}

int Point::sum_of_nearest_neighbors_from_point()
{
    return neighbors.size();
}

void Point::add_L_p_by_reference(near_point &n_p)
{
    L_p.push_back(n_p);
}

void Point::add_L_p_by_value(near_point n_p)
{
    L_p.push_back(n_p);
}

vector<near_point> &Point::get_L_p()
{
    return L_p;
}
