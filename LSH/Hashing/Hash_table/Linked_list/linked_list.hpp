#ifndef H__LINKED_LIST__
#define H__LINKED_LIST__

#include <cstdbool>
#include <iostream>
#include <vector>
#include <cassert>

#include "../../../../Maths/Maths.hpp"


// Simple Linked List implementation
struct Node{
    Node*                      next;

    const std::vector<double>* data;
    const int                  data_id;    // ID is used in order to print the nearest neighbor
    const long long int        ID_p;


    Node(const std::vector<double>* vec, const int id, const int id_p, Node* nxt):
                data(vec), data_id(id), ID_p(id_p), next(nxt) {}

    ~Node(){}
};

class LinkedList_Iterator{
private:
    Node* nodePtr;
    Node* first;

public:
    LinkedList_Iterator(Node* node): nodePtr(node){}


    const std::vector<double>* getData() const{return (nodePtr->data);}
    const std::vector<double>& getData2() const{return *(nodePtr->data);}
    const int getID() const { return nodePtr->data_id; }
    const long long int getID_p() const { return nodePtr->ID_p; }
    const bool isNull() const { return nodePtr == NULL; }


    LinkedList_Iterator& operator++(){
        nodePtr = nodePtr->next;
        return *this;
    }

    LinkedList_Iterator* operator++(int other)
    {
        LinkedList_Iterator* temp = this;
        ++*this;
        return temp;
    }

};

class LinkedList{
private:
    size_t size;

    Node* first;
    Node* first_node;

public:
    LinkedList(): size(0), first_node(NULL) {}

    LinkedList_Iterator begin(){return LinkedList_Iterator(first_node);}

    size_t getSize() const { return size; }

    // Adding to the start of the list
    void add(const std::vector<double>* vec, const int id, const int id_p){
        size++;
        first_node = new Node(vec, id, id_p, first_node);
    }

    void add(const std::vector<double>* vec, const int id){
        size++;
        first_node = new Node(vec, id, id, first_node);
    }

/*    void print_list(const vector<double>& query_vec, double  (*distance_fn)(const std::vector<double>&, const std::vector<double>&)){
        LinkedList_Iterator iter = begin();
        int i = 0;
        double prev = -1;
        while (!iter.isNull() && i < 20){
            i++;
            cout << distance_fn(query_vec, iter.getData2()) << endl;
            if (distance_fn(query_vec, iter.getData2()) < prev){cout << "yes" << endl;}
            prev = distance_fn(query_vec, iter.getData2());
            iter++;
        }
    }*/


    ~LinkedList(){
        Node* curr_node = first_node;

        while(curr_node != NULL){
            Node* tmp = curr_node;
            curr_node = curr_node->next;

            delete tmp;
        }
    }
};

#endif
