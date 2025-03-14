#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/random.h>

using u64 = unsigned long long int;
using u32 = unsigned int;

#ifdef __cplusplus
#define restrict __restrict__
#endif

extern "C" void* hedgy0_memcpy(
          void * restrict dst,
    const void * restrict src,
    u64 size
);

extern "C" void* hedgy1_memcpy(
          void * restrict dst,
    const void * restrict src,
    u64 size
);

int main(void)
{
    u32 buffer_size;
    void *pbuffer0, *pbuffer1;

    if (getrandom(&buffer_size, sizeof(buffer_size), 0) != sizeof(buffer_size)) {
        fprintf(stderr, "%s: Failed to read random bytes\n", "buffer_size");
        return 1;
    }

    /* Set buffer_size within range <1KB; 16MB + 1KB>. */
    buffer_size %= (1024 * 1024 * 16);
    buffer_size += 1024;
    buffer_size = 100'000'000;

    pbuffer0 = malloc(buffer_size);
    if (!pbuffer0) {
        fprintf(stderr, "%s: Failed to allocate memory %u\n", "pbuffer0", buffer_size);
        return 1;
    }

    pbuffer1 = malloc(buffer_size);
    if (!pbuffer1) {
        fprintf(stderr, "%s: Failed to allocate memory %u\n", "pbuffer1", buffer_size);
        return 1;
    }

    if (getrandom(pbuffer1, buffer_size, 0) != buffer_size) {
        fprintf(stderr, "%s: Failed to read random bytes\n", "pbuffer1");
        return 1;
    }

    hedgy0_memcpy(pbuffer0, pbuffer1, buffer_size);

    if (memcmp(pbuffer0, pbuffer1, buffer_size)) {
        fprintf(stderr, "memcmp failed %d\n", 0);
        return 1;
    }

    memset(pbuffer0, 0, buffer_size);

    hedgy1_memcpy(pbuffer0, pbuffer1, buffer_size);

    if (memcmp(pbuffer0, pbuffer1, buffer_size)) {
        fprintf(stderr, "memcmp failed %d\n", 1);
        return 1;
    }

    puts("OK");
    return 0;
}
