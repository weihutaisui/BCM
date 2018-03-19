#ifndef BITS_PER_LONG_H
#define BITS_PER_LONG_H

/* Number of bits in a word of type `long int'.  */
#ifdef LONG_MAX

#if LONG_MAX == 2147483647
#define BITS_PER_LONG 32
#else
/* Safe assumption.  */
#define BITS_PER_LONG 64
#endif

#elif defined(__LONG_MAX__)

#if __LONG_MAX__ == 2147483647
#define BITS_PER_LONG 32
#else
/* Safe assumption.  */
#define BITS_PER_LONG 64
#endif

#else /* !LONG_MAX && !__LONG_MAX__ */

#include <bits/wordsize.h>
#if __WORDSIZE == 64
#define BITS_PER_LONG 64
#else
#define BITS_PER_LONG 32
#endif
#endif /* LONG_MAX */

#endif /* BITS_PER_LONG_H */

