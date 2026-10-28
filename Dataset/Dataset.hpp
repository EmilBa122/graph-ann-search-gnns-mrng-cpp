#ifndef __DATASET__
#define __DATASET__

#include <fstream>
#include <vector>

// This structure is used just to hold some vectors.
class Dataset {
private:
    std::vector<std::vector<double> > pictures;

public:
    Dataset(std::ifstream&);

    int    getRows() const    { return pictures.size(); }
    int    getColumns() const { return pictures[0].size(); }


    std::vector<double> getKNearestDistances(double (*dist_fn)(const std::vector<double>&, const std::vector<double>&),
                                            const std::vector<double>&, const int&) const;

    const std::vector<double>& operator[](const int& i) const {
        return pictures[i];
    }

    void resize_(int size){
         pictures.resize(size);
    }


    ~Dataset(){}
};

#endif
