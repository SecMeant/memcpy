#include <immintrin.h>

#define INLINE __attribute__((always_inline)) inline
#define NOINLINE __attribute__((noinline, noclone))

#ifdef __cplusplus
#define restrict __restrict__
#endif

using u64 = unsigned long long int;
using u32 = unsigned int;
using u8  = unsigned char;

INLINE static void memcpy_base_(
          void * restrict dst_,
    const void * restrict src_,
    u64 size
) {
    auto * restrict dst = reinterpret_cast<u8*>(dst_);
    auto * restrict src = reinterpret_cast<const u8*>(src_);

    while (size--) {
        *dst = *src;
        ++dst;
        ++src;
    }
}

/*
 * Specialized version of memcpy for given data size.
 */
template<u64 DataSize>
INLINE void memcpy_spec_(
          void * restrict dst_,
    const void * restrict src_
) {
    auto * restrict dst = reinterpret_cast<u8*>(dst_);
    auto * restrict src = reinterpret_cast<const u8*>(src_);

    for (u32 i = 0u; i < DataSize; ++i) {
        dst[i] = src[i];
        ++dst;
        ++src;
    }
}

template<>
INLINE void memcpy_spec_<1024*1024>(
          void * restrict dst_,
    const void * restrict src_
) {
    constexpr u32 DataSize = 1024*1024;
    constexpr u32 NumElems = DataSize / sizeof(__m256i);
    auto * restrict dst = reinterpret_cast<__m256i*>(dst_);
    auto * restrict src = reinterpret_cast<const __m256i*>(src_);

#pragma GCC unroll 64
    for (u32 i = 0u; i < NumElems; ++i) {
        const auto d = _mm256_loadu_si256(src);
        _mm256_storeu_si256(dst, d);
        ++dst;
        ++src;
    }
}

#define CASE_MEMCPY_SPEC_(N) \
    case (N): \
        return memcpy_spec_<(N)>(dst, src);

static void hlz_memcpy_(
          void * restrict dst,
    const void * restrict src,
    u64 size
) {
    switch (size) {
        CASE_MEMCPY_SPEC_(1)
        CASE_MEMCPY_SPEC_(10)
        CASE_MEMCPY_SPEC_(100)
        CASE_MEMCPY_SPEC_(1000)
        CASE_MEMCPY_SPEC_(10000)
        CASE_MEMCPY_SPEC_(100000)
        CASE_MEMCPY_SPEC_(1000000)
        CASE_MEMCPY_SPEC_(10000000)
        CASE_MEMCPY_SPEC_(1024*1024)
    default:
        return memcpy_base_(dst, src, size);
    }
}

extern "C" void* hlz_memcpy(
          void * restrict dst,
    const void * restrict src,
    u64 size
) {
    hlz_memcpy_(dst, src, size);
    return dst;
}
