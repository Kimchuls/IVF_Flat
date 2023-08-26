#include "IndexIVFFlat.hpp"

#include <cstring>

#include "VIndexAssert.hpp"
namespace vindex
{
    IndexIVFFlat::IndexIVFFlat(
        Index *quantizer,
        size_t d,
        size_t nlist,
        MetricType metric)
        : IndexIVF(quantizer, d, nlist, sizeof(float) * d, metric)
    {
        code_size = sizeof(float) * d;
        by_residual = false;
    }

    IndexIVFFlat::IndexIVFFlat()
    {
        by_residual = false;
    }
    void IndexIVFFlat::encode_vectors(
        int64_t n,
        const float *x,
        const int64_t *list_nos,
        uint8_t *codes,
        bool include_listnos) const
    {
        VINDEX_THROW_IF_NOT(!by_residual);
        if (!include_listnos)
        {
            memcpy(codes, x, code_size * n);
        }
        else
        {
            size_t coarse_size = coarse_code_size();
            for (size_t i = 0; i < n; i++)
            {
                int64_t list_no = list_nos[i];
                uint8_t *code = codes + i * (code_size + coarse_size);
                const float *xi = x + i * d;
                if (list_no >= 0)
                {
                    encode_listno(list_no, code);
                    memcpy(code + coarse_size, xi, code_size);
                }
                else
                {
                    memset(code, 0, code_size + coarse_size);
                }
            }
        }
    }
} // namespace vindex
