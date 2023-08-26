#include <iostream>
#include "IndexIVFFlat.hpp"
#include "IndexFlat.hpp"
#include <random>
#include <vector>
// int main(int argc, char *argv[])
int main()
{
    int d = 128;      // dimension
    int nb = 1000; // database size
    int nq = 100;   // nb of queries
    int nlist = 10;
    int k = 100;
    vindex::IndexFlatL2 quantizer(d); // the other index
    vindex::IndexIVFFlat index(&quantizer, d, nlist);

    // srand(time(0));
    std::default_random_engine rand_gen;
    std::uniform_int_distribution<> distrib(0, 5);
    float *xb = new float[d*nb];
    for(int i=0;i<d*nb;i++){
        xb[i]=distrib(rand_gen);
    }
    index.train(nb,xb);
    return 0;
}
