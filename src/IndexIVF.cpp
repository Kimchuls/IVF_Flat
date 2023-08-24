#include "IndexIVF.hpp"
#include "VIndexAssert.hpp"

namespace vindex
{
  Level1Quantizer::Level1Quantizer(Index *quantizer, size_t nlist)
      : quantizer(quantizer), nlist(nlist)
  {
    cp.niter = 10;
  }
  Level1Quantizer::Level1Quantizer() {}

  Level1Quantizer::~Level1Quantizer()
  {
    if (own_fields)
    {
      delete quantizer;
    }
  }

  void Level1Quantizer::train_q1(size_t n, const float *x, bool verbose, MetricType metric_type)
  {
    size_t dim = quantizer->dim;
    if (quantizer->is_trained && (quantizer->total == nlist))
    {
      if (verbose)
        printf("IVF quantizer does not need training.\n");
    }
    else if (quantizer_trains_alone == 1)
    {
      if (verbose)
        printf("IVF quantizer trains alone...\n");
      quantizer->train(n, x);
      quantizer->verbose = verbose;
      VINDEX_THROW_IF_NOT_MSG(quantizer->total == nlist, "nlist not consistent with quantizer size");
    }
    else if (quantizer_trains_alone == 0)
    {
      if (verbose)
        printf("Training level-1 quantizer on %zd vectors in %zdD\n", n, dim);
    }
  }
} // namespace vindex
