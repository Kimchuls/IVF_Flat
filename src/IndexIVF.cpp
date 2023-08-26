#include "IndexIVF.hpp"

#include <inttypes.h>
#include <memory>
#include <omp.h>

#include "IndexFlat.hpp"
#include "utils.hpp"
#include "VIndexAssert.hpp"


namespace vindex
{
  /************************************
   * Level1Quantizer Implementation
   *************************************/
  Level1Quantizer::Level1Quantizer(Index *quantizer, size_t nlist)
      : quantizer(quantizer), nlist(nlist)
  {
    cp.niter = 10;
  }
  Level1Quantizer::Level1Quantizer() {}

  size_t Level1Quantizer::coarse_code_size() const
  {
    size_t nl = nlist - 1;
    size_t nbyte = 0;
    while (nl > 0)
    {
      nbyte++;
      nl >>= 8;
    }
    return nbyte;
  }
  void Level1Quantizer::encode_listno(int64_t list_no, uint8_t *code) const
  {
    // little endian
    size_t nl = nlist - 1;
    while (nl > 0)
    {
      *code++ = list_no & 0xff;
      list_no >>= 8;
      nl >>= 8;
    }
  }

  Level1Quantizer::~Level1Quantizer()
  {
    if (own_fields)
      delete quantizer;
  }

  void Level1Quantizer::train_q1(size_t n, const float *x, bool verbose, MetricType metric_type)
  {
    size_t d = quantizer->d;
    if (quantizer->is_trained && (quantizer->ntotal == nlist))
    {
      if (verbose)
        printf("IVF quantizer does not need training.\n");
    }
    else if (quantizer_trains_alone == 1)
    {
      if (verbose)
        printf("IVF quantizer trains alone...\n");
      quantizer->train(n, x); // TODO:check where is the real train
      quantizer->verbose = verbose;
      VINDEX_THROW_IF_NOT_MSG(quantizer->ntotal == nlist, "nlist not consistent with quantizer size");
    }
    else if (quantizer_trains_alone == 0)
    {
      if (verbose)
        printf("Training level-1 quantizer on %zd vectors in %zdD\n", n, d);
      Clustering clus(d, nlist, cp);
      quantizer->reset();
      if (clustering_index)
      {
        printf("DEBUG: IndexIVF.cpp line 46\n");
        // clus.train(n, x, *clustering_index);
        // quantizer->add(nlist, clus.centroids.data());
      }
      else
      {
        clus.train(n, x, *quantizer);
      }
      quantizer->is_trained = true;
    }
    else if (quantizer_trains_alone == 2)
    {
      if (verbose)
      {
        printf("Training L2 quantizer on %zd vectors in %zdD%s\n",
               n,
               d,
               clustering_index ? "(user provided index)" : "");
      }
      // also accept spherical centroids because in that case
      // L2 and IP are equivalent
      VINDEX_THROW_IF_NOT(
          metric_type == METRIC_L2 ||
          (metric_type == METRIC_INNER_PRODUCT && cp.spherical));

      Clustering clus(d, nlist, cp);
      if (!clustering_index)
      {
        IndexFlatL2 assigner(d);
        clus.train(n, x, assigner);
      }
      else
      {
        clus.train(n, x, *clustering_index);
      }
      if (verbose)
      {
        printf("Adding centroids to quantizer\n");
      }
      if (!quantizer->is_trained)
      {
        if (verbose)
        {
          printf("But training it first on centroids table...\n");
        }
        quantizer->train(nlist, clus.centroids.data());
      }
      quantizer->add(nlist, clus.centroids.data());
    }
  }

  /************************************
   * IndexIVF Implementation
   *************************************/
  IndexIVF::IndexIVF(Index *quantizer, size_t d, size_t nlist,
                     size_t code_size, MetricType metric)
      : Index(d, metric), IndexIVFInterface(quantizer, nlist),
        invlists(new ArrayInvertedLists(nlist, code_size)), own_invlists(true),
        code_size(code_size)
  {
    VINDEX_THROW_IF_NOT(d == quantizer->d);
    is_trained = quantizer->is_trained && (quantizer->ntotal == nlist);
    if (metric_type == METRIC_INNER_PRODUCT)
      cp.spherical = true;
  }

  IndexIVF::IndexIVF() {}
  IndexIVF::~IndexIVF()
  {
    if (own_invlists)
      delete invlists;
  }

  void IndexIVF::add(int64_t n, const float *x)
  {
    add_with_ids(n, x, nullptr);
  }
  void IndexIVF::add_with_ids(int64_t n, const float *x, const int64_t *xids)
  {
    std::unique_ptr<int64_t[]> coarse_idx(new int64_t[n]);
    quantizer->assign(n, x, coarse_idx.get());
    add_core(n, x, xids, coarse_idx.get());
  }

  void IndexIVF::add_core(
      int64_t n,
      const float *x,
      const int64_t *xids,
      const int64_t *coarse_idx)
  {
    // do some blocking to avoid excessive allocs
    int64_t bs = 65536;
    if (n > bs)
    {
      for (int64_t i0 = 0; i0 < n; i0 += bs)
      {
        int64_t i1 = std::min(n, i0 + bs);
        if (verbose)
        {
          printf("   IndexIVF::add_with_ids %" PRId64 ":%" PRId64 "\n",
                 i0,
                 i1);
        }
        add_core(
            i1 - i0,
            x + i0 * d,
            xids ? xids + i0 : nullptr,
            coarse_idx + i0);
      }
      return;
    }
    VINDEX_THROW_IF_NOT(coarse_idx);
    VINDEX_THROW_IF_NOT(is_trained);
    direct_map.check_can_add(xids);

    size_t nadd = 0, nminus1 = 0;

    for (size_t i = 0; i < n; i++)
    {
      if (coarse_idx[i] < 0)
        nminus1++;
    }

    std::unique_ptr<uint8_t[]> flat_codes(new uint8_t[n * code_size]);
    encode_vectors(n, x, coarse_idx, flat_codes.get());

    DirectMapAdd dm_adder(direct_map, n, xids);

#pragma omp parallel reduction(+ : nadd)
    {
      int nt = omp_get_num_threads();
      int rank = omp_get_thread_num();

      // each thread takes care of a subset of lists
      for (size_t i = 0; i < n; i++)
      {
        int64_t list_no = coarse_idx[i];
        if (list_no >= 0 && list_no % nt == rank)
        {
          int64_t id = xids ? xids[i] : ntotal + i;
          size_t ofs = invlists->add_entry(
              list_no, id, flat_codes.get() + i * code_size);

          dm_adder.add(i, list_no, ofs);

          nadd++;
        }
        else if (rank == 0 && list_no == -1)
        {
          dm_adder.add(i, -1, 0);
        }
      }
    }

    if (verbose)
    {
      printf("    added %zd / %" PRId64 " vectors (%zd -1s)\n",
             nadd,
             n,
             nminus1);
    }

    ntotal += n;
  }

  void IndexIVF::search(
      int64_t n,
      const float *x,
      int64_t k,
      float *distances,
      int64_t *labels,
      const SearchParameters *params_in) const
  {
    VINDEX_THROW_IF_NOT(k > 0);
    const IVFSearchParameters *params = nullptr;
    if (params_in)
    {
      params = dynamic_cast<const IVFSearchParameters *>(params_in);
      VINDEX_THROW_IF_NOT_MSG(params, "IndexIVF params have incorrect type");
    }
    const size_t nprobe =
        std::min(nlist, params ? params->nprobe : this->nprobe);
    VINDEX_THROW_IF_NOT(nprobe > 0);

    // search function for a subset of queries
    auto sub_search_func = [this, k, nprobe, params](
                               int64_t n,
                               const float *x,
                               float *distances,
                               int64_t *labels,
                               IndexIVFStats *ivf_stats)
    {
      std::unique_ptr<int64_t[]> idx(new int64_t[n * nprobe]);
      std::unique_ptr<float[]> coarse_dis(new float[n * nprobe]);

      double t0 = getmillisecs();
      quantizer->search(
          n,
          x,
          nprobe,
          coarse_dis.get(),
          idx.get(),
          params ? params->quantizer_params : nullptr);

      double t1 = getmillisecs();
      invlists->prefetch_lists(idx.get(), n * nprobe);

      search_preassigned(
          n,
          x,
          k,
          idx.get(),
          coarse_dis.get(),
          distances,
          labels,
          false,
          params,
          ivf_stats);
      double t2 = getmillisecs();
      ivf_stats->quantization_time += t1 - t0;
      ivf_stats->search_time += t2 - t0;
    };

    if ((parallel_mode & ~PARALLEL_MODE_NO_HEAP_INIT) == 0)
    {
      int nt = std::min(omp_get_max_threads(), int(n));
      std::vector<IndexIVFStats> stats(nt);
      std::mutex exception_mutex;
      std::string exception_string;

#pragma omp parallel for if (nt > 1)
      for (int64_t slice = 0; slice < nt; slice++)
      {
        IndexIVFStats local_stats;
        int64_t i0 = n * slice / nt;
        int64_t i1 = n * (slice + 1) / nt;
        if (i1 > i0)
        {
          try
          {
            sub_search_func(
                i1 - i0,
                x + i0 * d,
                distances + i0 * k,
                labels + i0 * k,
                &stats[slice]);
          }
          catch (const std::exception &e)
          {
            std::lock_guard<std::mutex> lock(exception_mutex);
            exception_string = e.what();
          }
        }
      }

      if (!exception_string.empty())
      {
        FAISS_THROW_MSG(exception_string.c_str());
      }

      // collect stats
      for (int64_t slice = 0; slice < nt; slice++)
      {
        indexIVF_stats.add(stats[slice]);
      }
    }
    else
    {
      // handle paralellization at level below (or don't run in parallel at
      // all)
      sub_search_func(n, x, distances, labels, &indexIVF_stats);
    }
  }
  void IndexIVF::train(int64_t n, const float *x)
  {
    if (verbose)
      printf("Training level-1 quantizer\n");
    train_q1(n, x, verbose, metric_type);
    // if (verbose)
    //   printf("Training IVF residual\n");

    // int64_t max_nt = train_encoder_num_vectors();
    // if (max_nt <= 0)
    //   max_nt = (size_t)1 << 35;

    // TransformedVectors tv(
    //     x, fvecs_maybe_subsample(d, (size_t *)&n, max_nt, x, verbose));

    // if (by_residual)
    // {
    //   std::vector<int64_t> assign(n);
    //   quantizer->assign(n, tv.x, assign.data());

    //   std::vector<float> residuals(n * d);
    //   quantizer->compute_residual_n(n, tv.x, residuals.data(), assign.data());

    //   train_encoder(n, residuals.data(), assign.data());
    // }
    // else
    // {
    //   train_encoder(n, tv.x, nullptr);
    // }

    // is_trained = true;
  }

  void IndexIVF::reset()
  {
    direct_map.clear();
    invlists->reset();
    ntotal = 0;
  }
} // namespace vindex
