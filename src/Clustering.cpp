#include "Clustering.hpp"

namespace vindex
{
  ClusteringParameters::ClusteringParameters()
        : niter(25),
          nredo(1),
          verbose(false),
          spherical(false),
          int_centroids(false),
          update_index(false),
          frozen_centroids(false),
          min_points_per_centroid(39),
          max_points_per_centroid(256),
          seed(1234),
          decode_block_size(32768) {}
  Clustering::Clustering(int d, int k)
  {
  }
  Clustering::Clustering(int d, int k, const ClusteringParameters &cp)
  {
  }
  void Clustering::train(int64_t n, const float *x, Index &index, const float *x_weight)
  {
  }
  void Clustering::post_process_centroids()
  {
        if (spherical) {
        fvec_renorm_L2(d, k, centroids.data());
    }

    if (int_centroids) {
        for (size_t i = 0; i < centroids.size(); i++)
            centroids[i] = roundf(centroids[i]);
    }
  }
} // namespace vindex
