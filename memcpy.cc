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
template<u64 DataSize_>
INLINE void memcpy_spec_(
          void * restrict dst_,
    const void * restrict src_
) {
    constexpr auto DataSize = DataSize_;
    auto * restrict dst = reinterpret_cast<u8*>(dst_);
    auto * restrict src = reinterpret_cast<const u8*>(src_);

    for (u32 i = 0u; i < DataSize; ++i)
        dst[i] = src[i];
}

template<>
INLINE void memcpy_spec_<100'000'000UL>(
          void * restrict dst_,
    const void * restrict src_
) {
    u64 size = 100'000'000UL;

    const u64 misalign = 0x40UL - ((u64)dst_ & 0x3fULL);

          u8 * restrict dst_u8 = (      u8 * restrict) dst_;
    const u8 * restrict src_u8 = (const u8 * restrict) src_;

    memcpy_base_(dst_, src_, misalign);
    dst_u8 += misalign;
    src_u8 += misalign;
    size   -= misalign;

    const u64 num_elems = size / (sizeof(__m256i)*4);
    const u64 num_rest  = size % (sizeof(__m256i)*4);

    auto * restrict dst = reinterpret_cast<__m256i*>(dst_u8);
    auto * restrict src = reinterpret_cast<const __m256i*>(src_u8);
    __m256i d0, d1, d2, d3;

#define VECTOR_LOAD(addr) _mm256_loadu_si256(addr)
#define VECTOR_STORE(addr, data) _mm256_stream_si256(addr, data)

    u32 i = 0;
#pragma clang loop unroll(disable)
    while (i < num_elems) {
	d0 = VECTOR_LOAD(src+0);
	d1 = VECTOR_LOAD(src+1);
	d2 = VECTOR_LOAD(src+2);
	d3 = VECTOR_LOAD(src+3);

	VECTOR_STORE(dst+0, d0);
	VECTOR_STORE(dst+1, d1);
	VECTOR_STORE(dst+2, d2);
	VECTOR_STORE(dst+3, d3);

	dst += 4;
	src += 4;
	++i;
    }

    memcpy_base_(dst, src, num_rest);
}

template<u64 DataSize_>
INLINE void memcpy_spec_unrolled_(
          void * restrict dst_,
    const void * restrict src_
) {
    constexpr auto DataSize = DataSize_;
    auto * restrict dst = reinterpret_cast<u8*>(dst_);
    auto * restrict src = reinterpret_cast<const u8*>(src_);

#pragma GCC unroll 64
    for (u32 i = 0u; i < DataSize; ++i)
        dst[i] = src[i];
}

#if 0
template<>
INLINE void memcpy_spec_<1024*1024>(
          void * restrict dst_,
    const void * restrict src_
) {
    constexpr u32 DataSize = 1024*1024;
    constexpr u32 NumElems = DataSize / sizeof(__m256i);
    auto * restrict dst = reinterpret_cast<__m256i*>(dst_);
    auto * restrict src = reinterpret_cast<const __m256i*>(src_);

    for (u32 i = 0u; i < NumElems; ++i) {
        const auto d = _mm256_loadu_si256(src);
        _mm256_storeu_si256(dst, d);
        ++dst;
        ++src;
    }
}
#endif

#define CASE_MEMCPY_SPEC_(N) \
    case (N): \
        return memcpy_spec_<(N)>(dst, src);

#define CASE_MEMCPY_SPEC_UNROLLED_(N) \
    case (N): \
        return memcpy_spec_unrolled_<(N)>(dst, src);

static void hedgy_memcpy_(
          void * restrict dst,
    const void * restrict src,
    u64 size
) {
    switch (size) {
        CASE_MEMCPY_SPEC_(1UL)
        CASE_MEMCPY_SPEC_(10UL)
        CASE_MEMCPY_SPEC_(100UL)
        CASE_MEMCPY_SPEC_(1000UL)
        CASE_MEMCPY_SPEC_(10000UL)
        CASE_MEMCPY_SPEC_(100000UL)
        CASE_MEMCPY_SPEC_(1000000UL)
        CASE_MEMCPY_SPEC_(10000000UL)
        CASE_MEMCPY_SPEC_(100000000UL)
    default:
        return memcpy_base_(dst, src, size);
    }
}

static void hedgy_memcpy_unrolled_(
          void * restrict dst,
    const void * restrict src,
    u64 size
) {
    switch (size) {
        CASE_MEMCPY_SPEC_UNROLLED_(1UL)
        CASE_MEMCPY_SPEC_UNROLLED_(10UL)
        CASE_MEMCPY_SPEC_UNROLLED_(100UL)
        CASE_MEMCPY_SPEC_UNROLLED_(1000UL)
        CASE_MEMCPY_SPEC_UNROLLED_(10000UL)
        CASE_MEMCPY_SPEC_UNROLLED_(100000UL)
        CASE_MEMCPY_SPEC_UNROLLED_(1000000UL)
        CASE_MEMCPY_SPEC_UNROLLED_(10000000UL)
    default:
        return memcpy_base_(dst, src, size);
    }
}

extern "C" void* hedgy0_memcpy(
          void * restrict dst,
    const void * restrict src,
    u64 size
) {
    hedgy_memcpy_(dst, src, size);
    return dst;
}

extern "C" void* hedgy1_memcpy(
          void * restrict dst,
    const void * restrict src,
    u64 size
) {
    hedgy_memcpy_unrolled_(dst, src, size);
    return dst;
}
