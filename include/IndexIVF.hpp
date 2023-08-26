#ifndef VINDEX_INDEX_IVF_HPP
#define VINDEX_INDEX_IVF_HPP
#include "Clustering.hpp"
#include "DirectMap.hpp"
#include "Index.hpp"
#include "InvertedLists.hpp"
#include "MetricType.hpp"

namespace vindex
{
  struct Level1Quantizer
  {
  public:
    Index *quantizer = nullptr;
    size_t nlist = 0;
    char quantizer_trains_alone = 0;
    bool own_fields = false;
    ClusteringParameters cp;
    Index *clustering_index = nullptr;

    void train_q1(size_t n, const float *x, bool verbose, MetricType metric_type);
    size_t coarse_code_size() const;
    void encode_listno(int64_t list_no, uint8_t *code) const;
    // int64_t decode_listno(const uint8_t *code) const;

    Level1Quantizer(Index *quantizer, size_t nlist);
    Level1Quantizer();
    ~Level1Quantizer();
  };
  struct SearchParametersIVF : SearchParameters
  {
    size_t nprobe = 1;    ///< number of probes at query time
    size_t max_codes = 0; ///< max nb of codes to visit to do a query
    SearchParameters *quantizer_params = nullptr;

    virtual ~SearchParametersIVF() {}
  };

  // the new convention puts the index type after SearchParameters
  using IVFSearchParameters = SearchParametersIVF;
  struct IndexIVFInterface : public Level1Quantizer
  {
  public:
    size_t nprobe = 1;    ///< number of probes at query time
    size_t max_codes = 0; ///< max nb of codes to visit to do a query

    explicit IndexIVFInterface(Index *quantizer = nullptr, size_t nlist = 0)
        : Level1Quantizer(quantizer, nlist) {}

    // virtual void search_preassigned(int64_t n, const float *x, int64_t k, const int64_t *assign, const float *centroid_dis, float *distances, int64_t *labels, bool store_pairs, const IVFSearchParameters *params = nullptr, IndexIVFStats *stats = nullptr) const = 0;
    // virtual void range_search_preassigned(int64_t nx, const float *x, float radius, const int64_t *keys, const float *coarse_dis, RangeSearchResult *result, bool store_pairs = false, const IVFSearchParameters *params = nullptr, IndexIVFStats *stats = nullptr) const = 0;

    virtual ~IndexIVFInterface() {}
  };
  struct IndexIVF : public Index, public IndexIVFInterface
  {
  public:
    InvertedLists *invlists = nullptr;
    bool own_invlists = false;
    size_t code_size = 0;
    // int parallel_mode = 0;
    // const int PARALLEL_MODE_NO_HEAP_INIT = 1024;
    DirectMap direct_map;
    bool by_residual = true;
    IndexIVF(Index *quantizer, size_t d, size_t nlist, size_t code_size, MetricType metric = METRIC_L2);
    IndexIVF();
    ~IndexIVF() override;

    void reset() override;
    void train(int64_t n, const float *x) override;
    void add(int64_t n, const float *x) override;
    void add_with_ids(int64_t n, const float *x, const int64_t *xids) override;
    virtual void add_core(int64_t n, const float *x, const int64_t *xids, const int64_t *precomputed_idx);
    virtual void encode_vectors(int64_t n, const float *x, const int64_t *list_nos, uint8_t *codes, bool include_listno = false) const = 0;

    // void add_sa_codes(int64_t n, const uint8_t *codes, const int64_t *xids);
    // virtual void train_encoder(int64_t n, const float *x, const int64_t *assign);
    // virtual int64_t train_encoder_num_vectors() const;
    void search_preassigned(int64_t n, const float *x, int64_t k, const int64_t *assign, const float *centroid_dis, float *distances, int64_t *labels, bool store_pairs, const IVFSearchParameters *params = nullptr, IndexIVFStats *stats = nullptr) const override;
    // void range_search_preassigned(int64_t nx, const float *x, float radius, const int64_t *keys, const float *coarse_dis, RangeSearchResult *result, bool store_pairs = false, const IVFSearchParameters *params = nullptr, IndexIVFStats *stats = nullptr) const override;
    void search(int64_t n, const float *x, int64_t k, float *distances, int64_t *labels, const SearchParameters *params = nullptr) const override;
    // void range_search(int64_t n, const float *x, float radius, RangeSearchResult *result, const SearchParameters *params = nullptr) const override;

    // virtual InvertedListScanner *get_InvertedListScanner(bool store_pairs = false, const IDSelector *sel = nullptr) const;
    // void reconstruct(int64_t key, float *recons) const override;
    // virtual void update_vectors(int nv, const int64_t *idx, const float *v);
    // void reconstruct_n(int64_t i0, int64_t ni, float *recons) const override;
    // void search_and_reconstruct(int64_t n, const float *x, int64_t k, float *distances, int64_t *labels, float *recons, const SearchParameters *params = nullptr) const override;

    // virtual void reconstruct_from_offset(int64_t list_no, int64_t offset, float *recons) const;
    // size_t remove_ids(const IDSelector &sel) override;
    // void check_compatible_for_merge(const Index &otherIndex) const override;
    // virtual void merge_from(Index &otherIndex, int64_t add_id) override;
    // virtual CodePacker *get_CodePacker() const;
    // virtual void copy_subset_to(IndexIVF &other, InvertedLists::subset_type_t subset_type, int64_t a1, int64_t a2) const;

    // size_t get_list_size(size_t list_no) const {
    //     return invlists->list_size(list_no);
    // }
    // bool check_ids_sorted() const;
    // void make_direct_map(bool new_maintain_direct_map = true);
    // void set_direct_map_type(DirectMap::Type type);
    // void replace_invlists(InvertedLists* il, bool own = false);
    // size_t sa_code_size() const override;
    // void sa_encode(int64_t n, const float* x, uint8_t* bytes) const override;
  };
  struct IndexIVFStats
  {
    size_t nq;                // nb of queries run
    size_t nlist;             // nb of inverted lists scanned
    size_t ndis;              // nb of distances computed
    size_t nheap_updates;     // nb of times the heap was updated
    double quantization_time; // time spent quantizing vectors (in ms)
    double search_time;       // time spent searching lists (in ms)

    IndexIVFStats()
    {
      reset();
    }
    void reset();
    void add(const IndexIVFStats &other);
  };
} // namespace vindex

#endif