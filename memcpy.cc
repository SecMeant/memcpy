#define INLINE __attribute__((always_inline))
#define NOINLINE __attribute__((noinline, noclone))

#ifdef __cplusplus
#define restrict __restrict__
#endif

using u64 = unsigned long long int;
using u32 = unsigned int;
using u8  = unsigned char;

static void INLINE memcpy_base_(
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
void memcpy_spec_(
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

#define xstr(a) str(a)
#define str(a) #a

#define CASE_MEMCPY_SPEC_(N) \
    case N: \
        return memcpy_spec_<N>(dst, src);

extern "C" void hlz_memcpy(
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
    default:
        return memcpy_base_(dst, src, size);
    }
}
