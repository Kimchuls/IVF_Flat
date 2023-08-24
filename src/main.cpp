#include <iostream>
#include "IndexIVFFlat.hpp"
// int main(int argc, char *argv[])
int main()
{
    int d = 128;      // dimension
    int nb = 1000000; // database size
    int nq = 10000;   // nb of queries
    int nlist = 1000;
    int k = 100;
    vindex::IndexFlatL2 quantizer(d); // the other index
    // vindex::IndexIVFFlat index(&quantizer, d, nlist);
}
