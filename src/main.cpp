/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <cstdio>
#include <cstdlib>
#include <random>
#include <sys/time.h>
#include "IndexFlat.hpp"
#include "IndexIVFFlat.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
// #include <faiss/index_io.h>
#include <map>
double elapsed() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

float* fvecs_read(const char* fname, size_t* d_out, size_t* n_out) {
    FILE* f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "could not open %s\n", fname);
        perror("");
        abort();
    }
    int d;
    fread(&d, 1, sizeof(int), f);
    assert((d > 0 && d < 1000000) || !"unreasonable dimension");
    fseek(f, 0, SEEK_SET);
    struct stat st;
    fstat(fileno(f), &st);
    size_t sz = st.st_size;
    assert(sz % ((d + 1) * 4) == 0 || !"weird file size");
    size_t n = sz / ((d + 1) * 4);

    *d_out = d;
    *n_out = n;
    float* x = new float[n * (d + 1)];
    size_t nr = fread(x, sizeof(float), n * (d + 1), f);
    assert(nr == n * (d + 1) || !"could not read whole file");

    // shift array to remove row headers
    for (size_t i = 0; i < n; i++)
        memmove(x + i * d, x + 1 + i * (d + 1), d * sizeof(*x));

    fclose(f);
    return x;
}
int* ivecs_read(const char* fname, size_t* d_out, size_t* n_out) {
    return (int*)fvecs_read(fname, d_out, n_out);
}

void load_data(char* filename, float*& data, int num, int dim) {
  std::ifstream in(filename, std::ios::binary); //open file in binary
  if (!in.is_open()) {
    std::cout << "open file error" << std::endl;
    exit(-1);
  }
  data = new float[(size_t)num * (size_t)dim];
  in.seekg(0, std::ios::beg);   //shift to start
//       printf("shift successfully\n");
  for (size_t i = 0; i < num; i++) {
    in.seekg(4, std::ios::cur); //right shift 4 bytes

for(int j=0;j<dim;j++)
    in.read((char*)(data + i*dim+j), 4 );       //load data     
   //in.read((char*)(data + i*dim), dim );   
}
}
  /*
  int c=0;
  for(size_t i = 0; i < num * dim; i++) {	//output vector by line
    if((i+1) % dim == 0 ) {
      std::cout << (float)data[i];
      std::cout<< std::endl<<c;
      c+=1;
      std::cout << "\n\n";
    }
    else{
    std::cout << (float)data[i];
    std::cout << " ";
    }
  
  }
  in.close();
}

*/
int main() {
    double t0 = elapsed();

    int d = 128;      // dimension
    int nb = 1000000; // database size
    int nq = 10000;  // nb of queries

    //std::mt19937 rng;
    //std::uniform_real_distribution<> distrib;

    
    char* base_filepath="/home/jin467/playground/IVF_Flat/sift/sift_base.fvecs";
    
    //load base dataset in method1
    float* xb = new float[d * nb];
    printf("[%.3f s] loading dataset of vectors \n",elapsed() - t0);
    load_data(base_filepath, xb, nb, d);
     printf("[%.3f s] loaded a %d vectors in %d dimension \n",elapsed() - t0, nb, d);
    
     //load base dataset in method2
    // float* xb = new float[d * nb];
    // size_t dd; //dimension
    // size_t nt; //the number of vectors
    //     printf("[%.3f s] loading dataset of vectors in method 2 \n",elapsed() - t0);
    // //xb=fvecs_read(base_filepath, &dd, &nt);
    //      printf("[%.3f s] loaded a %d vectors in %d dimension in method 2 \n",
    //            elapsed() - t0, nt, dd);

    /* generate base vector
    for (int i = 0; i < nb; i++) {
        for (int j = 0; j < d; j++)
            xb[d * i + j] = distrib(rng);
        xb[d * i] += i / 1000.;
    }*/
    float* xq = new float[d * nq];
    char* query_filepath="/home/jin467/playground/IVF_Flat/sift/sift_query.fvecs";
    size_t dd2; //dimension
    size_t nt2; //the number of query
    printf("[%.3f s] loading queryset of vectors \n",
               elapsed() - t0);
    xq=fvecs_read(query_filepath, &dd2, &nt2);
     printf("[%.3f s] loaded a %d vectors in %d dimension \n",
               elapsed() - t0, nq, d);

    /* generate query vector
    for (int i = 0; i < nq; i++) {
        for (int j = 0; j < d; j++)
            xq[d * i + j] = distrib(rng);
        xq[d * i] += i / 1000.;
    }*/
    size_t kk;                // nb of results per query in the GT
    int64_t* gt; // nq * kk matrix of ground-truth nearest-neighbors



    {
        printf("[%.3f s] Loading ground truth for %ld queries\n",
               elapsed() - t0,
               nq);

        // load ground-truth and convert int to long
        size_t nq2;
        int* gt_int = ivecs_read("/home/jin467/playground/IVF_Flat/sift/sift_groundtruth.ivecs", &kk, &nq2);
        assert(nq2 == nq || !"incorrect nb of ground truth entries");

        gt = new int64_t[kk * nq];
        for (int i = 0; i < kk * nq; i++) {
            gt[i] = gt_int[i];
        }
        delete[] gt_int;
    }
        srand((int)time(0)); 
        std::vector<float> trainvecs(nb/100 * d);
        for (int i = 0; i < nb/100; i++) {
        int rng=(rand() % (nb+1));; //rng=random number in nb

        for (int j = 0; j < d; j++){
            //printf(" setting %d vector's %d data, trianvecs[%d]=xb[%d] \n",i,j,d * i + j,rng * d + j);
            trainvecs[d * i + j] = xb[rng * d + j];
        }
    }

    int nlist = 1000;
    int k = 100;
    vindex::IndexFlatL2 quantizer(d); // the other index
    vindex::IndexIVFFlat index(&quantizer, d, nlist);
    printf("[%.3f s] start training \n",
               elapsed() - t0);
    index.train(nb/100, trainvecs.data());
    printf("[%.3f s] finish training \n",
               elapsed() - t0);
    printf("[%.3f s] start adding \n",
               elapsed() - t0);
    index.add(nb, xb);
    printf("[%.3f s] finish adding \n",
               elapsed() - t0);
    
    { // I/O demo
        //const char* outfilename = "/home/zhan4404/dataset/sift/indexivfflat_trained.faissindex";
        //printf("[%.3f s] use 1%% baseset as pre-trained index and store it to %s\n",
        //       elapsed() - t0,
        //       outfilename);
        //write_index(&index, outfilename);
    }
    /*
    { // sanity check
        int64_t* I = new int64_t[k * 5];
        float* D = new float[k * 5];

        index.search(5, xb, k, D, I);

        printf("I=\n");
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < k; j++)
                printf("%5zd ", I[i * k + j]);
            printf("\n");
        }

        printf("D=\n");
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < k; j++)
                printf("%7g ", D[i * k + j]);
            printf("\n");
        }

        delete[] I;
        delete[] D;
    }
    */
    { // search xq
        int64_t* I = new int64_t[k * nq];
        float* D = new float[k * nq];
        index.nprobe = 20;
        /*int64_t* tmpI = new int64_t[1];
        float* tmpD = new float[1];
        index.nprobe = 20;
        printf("[%.3f s] start searching \n",
               elapsed() - t0);
        for(int c=0;c<nq;c++){
            index.search(1, xq[c], k, tmpI, tmpD);
        }*/
        index.search(nq, xq, k, D, I);
        printf("[%.3f s] finish searching \n",
               elapsed() - t0);
        /*
        printf("I=\n");
        for (int i = nq - 5; i < nq; i++) {
            for (int j = 0; j < k; j++)
                printf("%5zd ", I[i * k + j]);
            printf("\n");
        }*/
        //compute recall
        printf("[%.3f s] Compute recalls\n", elapsed() - t0);

        // evaluate result by hand.
        int n_1 = 0, n_10 = 0, n_100 = 0;
        for (int i = 0; i < nq; i++) {
            int gt_nn = gt[i * k];
            for (int j = 0; j < k; j++) {
                //printf("gt_nn=%d,I=%lld \n",gt_nn,I[i * k + j]);
                if (I[i * k + j] == gt_nn) {
                    if (j < 1)
                        n_1++;
                    if (j < 10)
                        n_10++;
                    if (j < 100)
                        n_100++;
                }
            }
        }
        printf("R@1 = %.4f\n", n_1 / float(nq));
        printf("R@10 = %.4f\n", n_10 / float(nq));
        printf("R@100 = %.4f\n", n_100 / float(nq));

        //calculate the intersection between result and groundtruth:
        {
        int n2_100=0;
        for (int i = 0; i < nq; i++) {
            std::map<float, int> umap;
            for (int j = 0; j < k; j++) {              
                umap.insert({gt[i*k+j], 0});
            }
            for (int l = 0; l < k; l++) {
                
                if (umap.find(I[i*k+l])!= umap.end()){
                    n2_100++;                 
                }
            }
            umap.clear();

        }
        printf("Intersection R@100 = %.4f\n", n2_100 / float(nq*k));
        }
        delete[] I;
        delete[] D;
    }

    delete[] xb;
    delete[] xq;

    return 0;
}
