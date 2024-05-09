#ifndef _CONFIG_

#define _ZETA_OPTIMIZE_

// Host Details
#define _ZETA_LITTLE_ENDIAN_ 0x00
#define _ZETA_BIG_ENDIAN_ 0x01

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define _ZETA_BYTE_ORDER_ _ZETA_LITTLE_ENDIAN_
#else
#define _ZETA_BYTE_ORDER_ _ZETA_BIG_ENDIAN_
#endif

#if defined(__amd64) || defined(__amd64__)
#define _ZETA_HOST_CPU_AMD_ 1
#endif

#if defined(__x86_64)
#define _ZETA_HOST_CPU_x86_64_ARCH_ 1
#endif

#if defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
#define _ZETA_HOST_OS_LINUX_ 1
#endif

#if defined(_WIN64)
#define _ZETA_HOST_OS_WINDOWS_ 1
#endif

// Utilities
#ifndef surelyT

#define surelyT(x) __builtin_expect(!!(x), 1) // tell the compiler that the expression x is most likely to be true
#define surelyF(x) __builtin_expect(!!(x), 0) // tell the compiler that the expression x is most likely to be false

#endif

/*Based on optimizations, this may be set*/
#if defined(_ZETA_OPTIMIZE_)
#define _ZETA_ALWAYS_INLINE_ __attribute__((always_inline))
#else
#define _ZETA_ALWAYS_INLINE_ static inline
#endif

#define _ZETA_NO_DISCARD_ [[nodiscard]]
#define _ZETA_NO_THROW_ __attribute__((no_throw))
#define _ZETA_NO_RETURN_ __attribute__((no_return))
#define _ZETA_NO_NULL_ __attribute__((nonnull))

#if __SIZEOF_LONG__ == __SIZEOF_LONG_LONG__
#define LONG long
#else
#define LONG long long
#endif

#define INTERNAL_FUNC static // for a function that is localized to a module only
#define INTERNAL_VAR static  // for a variable that is localized to a module only
#define LOCAL static         // any static variable inside a function

#endif
