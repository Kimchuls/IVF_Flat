#ifndef VINDEX_INDEX_HPP
#define VINDEX_INDEX_HPP
#include <cstdio>
#include <iostream>
#include "MetricType.hpp"
#include "IDSelector.hpp"
namespace vindex
{
  class Index
  {
  public:
    int get_dim();
    int64_t get_total();
    bool get_is_trained();
    explicit Index(int64_t dim = 0, MetricType metric = METRIC_L2)
        : dim(dim), total(0), verbose(false), is_trained(false), metric_type(metric) {}
    virtual ~Index(){}
    /* training on float x[n*dim] */
    virtual void train(int64_t n, const float *x);
    /*add float x[n*dim] with label total ... total+n-1*/
    virtual void add(int64_t n, const float *x) = 0;
    /*add float x[n*dim] with label xids[n]*/
    virtual void add_with_ids(int64_t n, const float *x, const int64_t *xids);
    /**query n vectors x[n*dim] to the index,
     * return at most k vectors (or padded -1s)
     * as output labels[n*k] and distance[n*k]
     */
    virtual void search(int64_t n, const float *x, int64_t k, float *distances,
                        int64_t *labels) const = 0; // TODO:check for SearchParameters

    /** query n vectors x[n*dim] to the index,
     * return all vectors with distance LESS than radius
     */
    // virtual void range_search(int64_t n, const float *x, float radius, RangeSearchResult *result)const;

    /**return the indexes of the k vectors closest to the query x
     *
     */
     virtual void assign(int64_t n, const float* x, int64_t* labels, int64_t k=1)const;

    /// remove all elememts from db
    virtual void reset() = 0;

    /// remove IDs from index
    virtual size_t remove_ids(const IDSelector& sel);

    /// Reconstruct a stored vector
    virtual void reconstruct(int64_t id, float* vector)const;

    /// reconstruct several stroed vectors
    // virtual void reconstruct_batch(int64_t n, const int64_t* ids, float* vectors)const;

    /// reconstruct vectors left to left+n-1
    // virtual void reconstruct_n(int64_t left, int64_t n, float* vectors)const;

    // reconstruct the stored vectors(n,k,dim) for the search results
    // virtual void search_and_reconstruct(int64_t n, const float *x, int64_t k, float *distances, int64_t *labels, float *vectors) const;

    /** Computes a residual vector after indexing encoding.
     *
     * The residual vector is the difference between a vector and the
     * reconstruction that can be decoded from its representation in
     * the index. The residual can be used for multiple-stage indexing
     * methods, like IndexIVF's methods.
     *
     * @param x           input vector, size d
     * @param residual    output residual vector, size d
     * @param key         encoded index, as returned by search and assign
     */
    virtual void compute_residual(const float *x, float *residual, int64_t key)
        const;

    // virtual void compute_residual_n(int64_t n, const float *xs, float *residuals, const int64_t *keys) const;

    // virtual DistanceComputer* get_distance_computer() const;
    // virtual size_t sa_code_size() const;
    // virtual void sa_encode(idx_t n, const float* x, uint8_t* bytes) const;
    // virtual void sa_decode(idx_t n, const uint8_t* bytes, float* x) const;
    // virtual void merge_from(Index& otherIndex, idx_t add_id = 0);
    // virtual void check_compatible_for_merge(const Index& otherIndex) const;

    int dim;                // vector dimension
    int64_t total;          // total indexed vectors
    bool verbose;           // verbosity level
    bool is_trained;        // index not need training, or is trained
    MetricType metric_type; // type of metric this index uses for search
    // float metric_arg;
  };
} // namespace vindex

#endif