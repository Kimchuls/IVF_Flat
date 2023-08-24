#ifndef VINDEX_INDEX_IVF_HPP
#define VINDEX_INDEX_IVF_HPP
#include "Index.hpp"
#include "Clustering.hpp"
#include "MetricType.hpp"
namespace vindex
{
  class Level1Quantizer
  {
  public:
    Index *quantizer = nullptr; // quantizer that maps vectors to inverted lists
    size_t nlist = 0;           // number of inverted lists
    /**
     * = 0: use the quantizer as index in a kmeans training
     * = 1: just pass on the training set to the train() of the quantizer
     * = 2: kmeans training on a flat index + add the centroids to the quantizer
     */
    char quantizer_trains_alone = 0;
    bool own_fields = false; ///< whether object owns the quantizer
    ClusteringParameters cp; ///< to override default clustering params
    /// to override index used during clustering
    Index *clustering_index = nullptr;

    void train_q1(size_t n, const float *x, bool verbose, MetricType metric_type);
    // /// compute the number of bytes required to store list ids
    // size_t coarse_code_size() const;
    // void encode_listno(idx_t list_no, uint8_t *code) const;
    // idx_t decode_listno(const uint8_t *code) const;

    Level1Quantizer(Index *quantizer, size_t nlist);
    Level1Quantizer();
    ~Level1Quantizer();
  };
  class IndexIVF : Index
  {
  };
} // namespace vindex

#endif