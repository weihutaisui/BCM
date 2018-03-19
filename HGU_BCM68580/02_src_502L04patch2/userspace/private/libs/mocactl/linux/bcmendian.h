#ifndef __BCMENDIAN_H__
#define __BCMENDIAN_H__

#if (__BYTE_ORDER == __LITTLE_ENDIAN) && (!defined(__EMU_HOST_20__))
#define BE64(x) \
( \
        ((uint64_t)( \
                (((uint64_t)(x) & (uint64_t)0x00000000000000ffULL) << 56) | \
                (((uint64_t)(x) & (uint64_t)0x000000000000ff00ULL) << 40) | \
                (((uint64_t)(x) & (uint64_t)0x0000000000ff0000ULL) << 24) | \
                (((uint64_t)(x) & (uint64_t)0x00000000ff000000ULL) << 8) | \
                (((uint64_t)(x) & (uint64_t)0x000000ff00000000ULL) >> 8) | \
                (((uint64_t)(x) & (uint64_t)0x0000ff0000000000ULL) >> 24) | \
                (((uint64_t)(x) & (uint64_t)0x00ff000000000000ULL) >> 40) | \
                (((uint64_t)(x) & (uint64_t)0xff00000000000000ULL) >> 56) )) \
)

#define BE32(x) \
    (\
            (((x) & 0x000000ffUL) << 24) | \
            (((x) & 0x0000ff00UL) <<  8) | \
            (((x) & 0x00ff0000UL) >>  8) | \
            (((x) & 0xff000000UL) >> 24) \
    )
    
#define BE16(x) \
    ( \
            (((x) & 0x00ffUL) << 8) | \
            (((x) & 0xff00UL) >>  8) \
    )
 
#else
#define BE64(x)         (x)
#define BE32(x)         (x)
#define BE16(x)         (x)
#endif

#endif
