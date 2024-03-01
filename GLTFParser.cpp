/*****************************************************************
*                                                                *
*    Purpose:                                                    *
*        Simple and efficient parser for GLTF format             *
*        allows you to import 3d mesh, material and scene        *
*    Author:                                                     *
*        Anilcan Gulkaya 2023 anilcangulkaya7@gmail.com          *
*    Restrictions:                                               *
*        No extension support.                                   *
*    License:                                                    *
*        No License whatsoever do Whatever you want.             *
*                                                                *
*****************************************************************/

#ifndef AX_MALLOC
    #define AX_MALLOC(size) ((void*)new char[size])
    #define AX_CALLOC(size) ((void*)new char[size]{})
    #define AX_FREE(ptr)    (delete[] (char*)(ptr) )
#endif

#ifndef __cplusplus
extern "C" {
#endif
/*****************************************************************
*                           ASTL COMMON                          *
*****************************************************************/

#ifndef ASTL_COMMON
#define ASTL_COMMON

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <float.h>

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;

typedef uint8_t  uchar;
typedef uint16_t ushort;
typedef uint32_t uint;

#ifdef AX_EXPORT
	#define AX_API __declspec(dllexport)
#else
	#define AX_API __declspec(dllimport)
#endif

#ifdef _MSC_VER
# // do nothing	it already has __forceinline
#elif __CLANG__
#       define __forceinline [[clang::always_inline]] 
#elif __GNUC__
#ifndef __forceinline
#       define __forceinline inline __attribute__((always_inline))
#endif
#endif

#ifdef _MSC_VER
#   include <intrin.h>
#	define VECTORCALL __vectorcall
#elif __CLANG__
#   define VECTORCALL [[clang::vectorcall]] 
#elif __GNUC__
#   define VECTORCALL  
#endif

#if defined(__GNUC__)
#    define AX_PACK(decl) decl __attribute__((__packed__))
#elif defined(_MSC_VER)
#    define AX_PACK(decl) __pragma(pack(push, 1)) decl __pragma(pack(pop))
#else
#error you should define pack function
#endif

#if defined(__GNUC__) || defined(__MINGW32__)
    #define RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RESTRICT __restrict
#else
    #define RESTRICT
#endif

#ifndef AXGLOBALCONST
#	if _MSC_VER
#		define AXGLOBALCONST extern const __declspec(selectany)
#	elif defined(__GNUC__) && !defined(__MINGW32__)
#		define AXGLOBALCONST extern const __attribute__((weak))
#   else 
#       define AXGLOBALCONST extern const 
#	endif
#endif

#if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
#    define AX_LIKELY(x) __builtin_expect(x, 1)  
#    define AX_UNLIKELY(x) __builtin_expect(x, 0)
#else
#    define AX_LIKELY(x) (x)   
#    define AX_UNLIKELY(x) (x) 
#endif

// https://nullprogram.com/blog/2022/06/26/
#if defined(_DEBUG) || defined(Debug)
#  if __GNUC__
#    define ASSERT(c) if (!(c)) __builtin_trap()
#  elif _MSC_VER
#    define ASSERT(c) if (!(c)) __debugbreak()
#  else
#    define ASSERT(c) if (!(c)) *(volatile int *)0 = 0
#  endif
#else
#  define ASSERT(c)
#endif

#if defined(__has_builtin)
#   define AX_COMPILER_HAS_BUILTIN(x) __has_builtin(x)
#else
#   define AX_COMPILER_HAS_BUILTIN(x) 0
#endif

#if AX_COMPILER_HAS_BUILTIN(__builtin_assume)
#   define AX_ASSUME(x) __builtin_assume(x)
#elif defined(_MSC_VER)
#   define AX_ASSUME(x) __assume(x)
#else
#   define AX_ASSUME(x) (void)(x)
#endif

#if AX_COMPILER_HAS_BUILTIN(__builtin_unreachable)
#   define AX_UNREACHABLE() __builtin_unreachable()
#elif _MSC_VER
#   define AX_UNREACHABLE() __assume(0)
#else
#   define AX_UNREACHABLE() 
#endif

#if AX_COMPILER_HAS_BUILTIN(__builtin_prefetch)
#   define AX_PREFETCH(x) __builtin_prefetch(x)
#elif defined(_MSC_VER)
#   define AX_PREFETCH(x) _mm_prefetch(x, _MM_HINT_T0)
#else
#   define AX_PREFETCH(x)
#endif

// https://gist.github.com/boxmein/7d8e5fae7febafc5851e
// https://en.wikipedia.org/wiki/CPUID
// example usage:
// void get_cpu_model(char *cpu_model) { // return example: "AMD Ryzen R5 1600"
//     int* cpumdl = (int*)cpu_model;
//     AX_CPUID(0x80000002, cpumdl); cpumdl += 4;
//     AX_CPUID(0x80000003, cpumdl); cpumdl += 4;
//     AX_CPUID(0x80000004, cpumdl); 
// }
// int arr[4];
// AX_CPUID(1, arr);
// int numCores = (arr[1] >> 16) & 0xff; // virtual cores included
#if defined(__clang__) || defined(__GNUC__)
//#   include <cpuid.h>
//#   define AX_CPUID(num, regs)       __cpuid(num, regs[0], regs[1], regs[2], regs[3])
//#   define AX_CPUID2(num, sub, regs) __cpuid_count(num, sub, regs[0], regs[1], regs[2], regs[3])
#   define AX_CPUID(num, regs)       
#else
#   define AX_CPUID(num, regs) __cpuid(regs, num)
#endif

/* Architecture Detection */
// detection code from mini audio
// you can define AX_NO_SSE2 or AX_NO_AVX2 in order to disable this extensions

#if defined(__x86_64__) || defined(_M_X64)
#   define AX_X64
#elif defined(__i386) || defined(_M_IX86)
#   define AX_X86
#elif defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC) || __arm__ || __aarch64__
    #define AX_ARM
#endif

#if defined(AX_ARM)
    #if defined(_MSC_VER) && !defined(__clang__) && (defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC) || defined(__aarch64__))
        #include <arm64_neon.h>
    #else
        #include <arm_neon.h>
    #endif
#endif

// write AX_NO_SSE2 or AX_NO_AVX2 to disable vector instructions

/* Intrinsics Support */
#if (defined(AX_X64) || defined(AX_X86)) && !defined(AX_ARM)
    #if defined(_MSC_VER) && !defined(__clang__)
        #if _MSC_VER >= 1400 && !defined(AX_NO_SSE2)   /* 2005 */
            #define AX_SUPPORT_SSE
        #endif
        #if _MSC_VER >= 1700 && !defined(AX_NO_AVX2)   /* 2012 */
            #define AX_SUPPORT_AVX2
        #endif
    #else
        #if defined(__SSE2__) && !defined(AX_NO_SSE2)
            #define AX_SUPPORT_SSE
        #endif
        #if defined(__AVX2__) && !defined(AX_NO_AVX2)
            #define AX_SUPPORT_AVX2
        #endif
    #endif
    
    /* If at this point we still haven't determined compiler support for the intrinsics just fall back to __has_include. */
    #if !defined(__GNUC__) && !defined(__clang__) && defined(__has_include)
        #if !defined(AX_SUPPORT_SSE)   && !defined(AX_NO_SSE2)   && __has_include(<emmintrin.h>)
            #define AX_SUPPORT_SSE
        #endif
        #if !defined(AX_SUPPORT_AVX2)   && !defined(AX_NO_AVX2)   && __has_include(<immintrin.h>)
            #define AX_SUPPORT_AVX2
        #endif
    #endif

    #if defined(AX_SUPPORT_AVX2) || defined(AX_SUPPORT_AVX)
        #include <immintrin.h>

    #elif defined(AX_SUPPORT_SSE)
        #include <emmintrin.h>
    #endif
#endif

#ifndef AX_NO_UNROLL
    #if defined(__clang__)
    #   define AX_NO_UNROLL _Pragma("clang loop unroll(disable)") _Pragma("clang loop vectorize(disable)")
    #elif defined(__GNUC__) >= 8
    #   define AX_NO_UNROLL _Pragma("GCC unroll 0")
    #elif defined(_MSC_VER)
    #   define AX_NO_UNROLL __pragma(loop(no_vector))
    #else
    #   define AX_NO_UNROLL
    #endif
#endif

#if defined(__clang__) || defined(__GNUC__)
    #define AX_CPP_VERSION __cplusplus
    #define AX_CPP14 201402L
    #define AX_CPP17 201703L
    #define AX_CPP20 202002L
#elif defined(_MSC_VER)
    #define AX_CPP_VERSION _MSC_VER
    #define AX_CPP14 1900
    #define AX_CPP17 1910
    #define AX_CPP20 1920
#endif

#if AX_CPP_VERSION < AX_CPP14
    // below c++ 14 does not support constexpr functions
    #define __constexpr
#else
    #define __constexpr constexpr
#endif

#if AX_CPP_VERSION >= AX_CPP17
#   define if_constexpr if constexpr
#else
#   define if_constexpr if
#endif

#if AX_CPP_VERSION < AX_CPP14
    // below c++ 14 does not support constexpr
    #define __const const
#else
    #define __const constexpr
#endif

#ifdef _MSC_VER
    #define SmallMemCpy(dst, src, size) __movsb((unsigned char*)(dst), (unsigned char*)(src), size);
#else
    #define SmallMemCpy(dst, src, size) __builtin_memcpy(dst, src, size);
#endif

#ifdef _MSC_VER
    #define SmallMemSet(dst, val, size) __stosb((unsigned char*)(dst), val, size);
#else
    #define SmallMemSet(dst, val, size) __builtin_memset(dst, val, size);
#endif

#define MemsetZero(dst, size) SmallMemSet(dst, 0, size)

#if defined(_MSC_VER) && !defined(__clang__)
#if defined(_M_IX86) //< The __unaligned modifier isn't valid for the x86 platform.
    #define UnalignedLoad64(ptr) *((uint64_t*)(ptr))
#else
    #define UnalignedLoad64(ptr) *((__unaligned uint64_t*)(ptr))
#endif
#else
    __forceinline uint64_t UnalignedLoad64(void const* ptr) {
        __attribute__((aligned(1))) uint64_t const *result = (uint64_t const *)ptr;
        return *result;
    }
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#if defined(_M_IX86) //< The __unaligned modifier isn't valid for the x86 platform.
    #define UnalignedLoad32(ptr) *((uint32_t*)(ptr))
#else
    #define UnalignedLoad32(ptr) *((__unaligned uint32_t*)(ptr))
#endif
#else
    __forceinline uint64_t UnalignedLoad32(void const* ptr) {
        __attribute__((aligned(1))) uint32_t const *result = (uint32_t const *)ptr;
        return *result;
    }
#endif

#define UnalignedLoadWord(x) (sizeof(unsigned long) == 8 ? UnalignedLoad64(x) : UnalignedLoad32(x))

// #define AX_USE_NAMESPACE
#ifdef AX_USE_NAMESPACE
#   define AX_NAMESPACE namespace ax {
#   define AX_END_NAMESPACE }
#else
#   define AX_NAMESPACE
#   define AX_END_NAMESPACE
#endif

AX_NAMESPACE

// change it as is mobile maybe?
inline constexpr bool IsAndroid()
{
#ifdef __ANDROID__
    return true;
#else
    return false;
#endif
}

template<typename T, int N>
__constexpr int ArraySize(const T (&)[N]) { return N; }

#if defined(_MSC_VER)     /* Visual Studio */
    #define AX_BSWAP32(x) _byteswap_ulong(x)
#elif (defined (__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ >= 403)) \
|| (defined(__clang__) && __has_builtin(__builtin_bswap32))
    #define AX_BSWAP32(x) __builtin_bswap32(x)
#else
inline uint32_t AX_BSWAP32(uint32_t x) {
    return ((in << 24) & 0xff000000 ) |
           ((in <<  8) & 0x00ff0000 ) |
           ((in >>  8) & 0x0000ff00 ) |
           ((in >> 24) & 0x000000ff );
}
#endif

#if defined(_MSC_VER) 
    #define ByteSwap32(x) _byteswap_uint64(x)
#elif (defined (__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ >= 403)) \
|| (defined(__clang__) && __has_builtin(__builtin_bswap32))
    #define ByteSwap64(x) __builtin_bswap64(x)
#else
inline uint64_t ByteSwap(uint64_t x) {
    return ((x << 56) & 0xff00000000000000ULL) |
           ((x << 40) & 0x00ff000000000000ULL) |
           ((x << 24) & 0x0000ff0000000000ULL) |
           ((x << 8)  & 0x000000ff00000000ULL) |
           ((x >> 8)  & 0x00000000ff000000ULL) |
           ((x >> 24) & 0x0000000000ff0000ULL) |
           ((x >> 40) & 0x000000000000ff00ULL) |
           ((x >> 56) & 0x00000000000000ffULL);
}
#endif

#define ByteSwapWord(x) (sizeof(ulong) == 8 ? ByteSwap64(x) : ByteSwap32(x))

// according to intel intrinsic, popcnt instruction is 3 cycle (equal to mulps, addps) 
// throughput is even double of mulps and addps which is 1.0 (%100)
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html

#if defined(__ARM_NEON__)
    #define PopCount32(x) vcnt_u8((int8x8_t)x)
#elif defined(AX_SUPPORT_SSE)
	#define PopCount32(x) _mm_popcnt_u32(x)
    #define PopCount64(x) _mm_popcnt_u64(x)
#elif defined(__GNUC__) || !defined(__MINGW32__)
	#define PopCount32(x) __builtin_popcount(x)
	#define PopCount64(x) __builtin_popcountl(x)
#else

inline uint32_t PopCount32(uint32_t x) {
	x =  x - ((x >> 1) & 0x55555555);        // add pairs of bits
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);  // quads
	x = (x + (x >> 4)) & 0x0F0F0F0F;        // groups of 8
	return (x * 0x01010101) >> 24;          // horizontal sum of bytes	
}

// standard popcount; from wikipedia
inline uint64_t PopCount64(uint64_t x) {
	x -= ((x >> 1) & 0x5555555555555555ull);
	x = (x & 0x3333333333333333ull) + (x >> 2 & 0x3333333333333333ull);
	return ((x + (x >> 4)) & 0xf0f0f0f0f0f0f0full) * 0x101010101010101ull >> 56;
}
#endif

#ifdef _MSC_VER
    #define TrailingZeroCount32(x) _tzcnt_u32(x)
    #define TrailingZeroCount64(x) _tzcnt_u64(x)
#elif defined(__GNUC__) || !defined(__MINGW32__)
    #define TrailingZeroCount32(x) __builtin_ctz(x)
    #define TrailingZeroCount64(x) __builtin_ctzll(x)
#else
    #define TrailingZeroCount32(x) PopCount64((x & -x) - 1u)
    #define TrailingZeroCount64(x) PopCount64((x & -x) - 1ull)
#endif

#define TrailingZeroCountWord(x) (sizeof(ulong) == 8 ? TrailingZeroCount64(x) : TrailingZeroCount32(x))

#ifdef _MSC_VER
	#define LeadingZeroCount32(x) _lzcnt_u32(x)
	#define LeadingZeroCount64(x) _lzcnt_u64(x)
#elif defined(__GNUC__) || !defined(__MINGW32__)
	#define LeadingZeroCount32(x) __builtin_clz(x)
	#define LeadingZeroCount64(x) __builtin_clzll(x)
#else
template<typename T> inline T LeadingZeroCount64(T x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
    if (sizeof(T) == 8) x |= (x >> 32); 
	return (sizeof(T) * 8) - PopCount(x);
}
#endif

#define LeadingZeroCountWord(x) (sizeof(ulong) == 8 ? LeadingZeroCount64(x) : LeadingZeroCount32(x))

template<typename T>
__forceinline __constexpr T NextSetBit(T* bits)
{
    *bits &= ~T(1);
    T tz = sizeof(T) == 8 ? (T)TrailingZeroCount64(*bits) : (T)TrailingZeroCount32(*bits);
    *bits >>= tz;
    return tz;
}

#define EnumHasBit(_enum, bit) !!(_enum & bit)

template<typename To, typename From>
__forceinline __constexpr To BitCast(const From& _Val) 
{
#if AX_CPP_VERSION < AX_CPP17
  return *reinterpret_cast<const To*>(&_Val);
#else
  return __builtin_bit_cast(To, _Val);
#endif
}

#ifndef MIN
    #if AX_CPP_VERSION >= AX_CPP17
    template<typename T> __forceinline __constexpr T MIN(T a, T b) { return a < b ? a : b; }
    template<typename T> __forceinline __constexpr T MAX(T a, T b) { return a > b ? a : b; }
    #else
    // using macro if less than 17 because we want this to be constexpr
    #   ifndef MIN
    #       define MIN(a, b) ((a) < (b) ? (a) : (b))
    #       define MAX(a, b) ((a) > (b) ? (a) : (b))
    #   endif
    #endif
#endif

template<typename T> __forceinline __constexpr T Clamp(T x, T a, T b) { return MAX(a, MIN(b, x)); }

__forceinline __constexpr int64_t Abs(int64_t x) 
{
    return x & ~(1ull << 63ull);
}

__forceinline __constexpr int Abs(int x)
{
    return x & ~(1 << 31);
}

__forceinline __constexpr float Abs(float x)
{
    int ix = BitCast<int>(x) & 0x7FFFFFFF; // every bit except sign mask
    return BitCast<float>(ix);
}

__forceinline __constexpr double Abs(double x)
{
    uint64_t  ix = BitCast<uint64_t >(x) & (~(1ull << 63ull));// every bit except sign mask
    return BitCast<double>(ix);
}

template<typename T> __forceinline __constexpr 
bool IsPowerOfTwo(T x) { return (x != 0) && ((x & (x - 1)) == 0); }

__forceinline __constexpr int NextPowerOf2(int x)
{
    x--;
    x |= x >> 1; x |= x >> 2; x |= x >> 4;
    x |= x >> 8; x |= x >> 16;
    return ++x;
}

__forceinline __constexpr int64_t NextPowerOf2(int64_t x)
{
    x--;
    x |= x >> 1; x |= x >> 2;  x |= x >> 4;
    x |= x >> 8; x |= x >> 16; x |= x >> 32;
    return ++x;
}

// maybe we should move this to Algorithms.hpp
template<typename T>
inline uint PointerDistance(const T* begin, const T* end)
{
    return uint((char*)end - (char*)begin) / sizeof(T);
}

inline __constexpr int CalculateArrayGrowth(int _size)
{
    const int addition = _size >> 1;
    if (AX_UNLIKELY(_size > (INT32_MAX - addition))) {
        return INT32_MAX; // growth would overflow
    }
    return _size + addition; // growth is sufficient
}

typedef unsigned long long unsignedll;

inline int StringLength(char const* s)
{
    char const* p = s;
    unsignedll m = 0x7efefefefefefeff, n = ~m, i;

    for (; (unsignedll)p & (sizeof(unsignedll) - 1); p++) {
        if (!*p) {
            return (int)(unsignedll)(p - s);
        }
    }

    for (;;) {

        i = *(unsignedll const*)p;

        if (!(((i + m) ^ ~i) & n)) {
            p += sizeof(unsignedll);

        } else {

            for (i = sizeof(unsignedll); i; p++, i--) {
                if (!*p) {
                    return (int)(unsignedll)(p - s);
                }
            }
        }
    }
}
#endif // ASTL_COMMON

/*****************************************************************
*                               IO                               *
*****************************************************************/

#include <stdio.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <io.h>
    #include <direct.h>
    #define F_OK 0
    #define access _access
#else 
    #include <unistd.h>
    #include <sys/types.h>
    #include <fcntl.h>
    #define _mkdir mkdir
    #define _fileno fileno
    #define _filelengthi64 filelength
#endif

#if defined __WIN32__ || defined _WIN32 || defined _Windows
    #if !defined S_ISDIR
            #define S_ISDIR(m) (((m) & _S_IFDIR) == _S_IFDIR)
    #endif
#endif

#ifdef __ANDROID__
    #include <android/asset_manager.h>
    #include <game-activity/native_app_glue/android_native_app_glue.h>

    extern android_app* g_android_app;
#endif

struct ScopedFILE {
    FILE* file;
    ScopedFILE(FILE* _file) : file(_file) {}
    ~ScopedFILE() {
        fclose(file); 
    }
};

// these functions works fine with file and folders
inline bool FileExist(const char* file) {
#ifdef __ANDROID__
    AAsset* asset = AAssetManager_open(g_android_app->activity->assetManager, file, 0);
    AAsset_close(asset);
    return asset != nullptr;
#else
    return access(file, F_OK) == 0;
#endif
}

inline uint64_t FileSize(const char* file) {
#ifdef __ANDROID__
    AAsset* asset = AAssetManager_open(g_android_app->activity->assetManager, file, 0);
    if (asset != nullptr) {
        off64_t sz = AAsset_getLength64(asset);
        AAsset_close(asset);
        return sz;
    }
    return 0;
#elif defined(_WIN32) || defined(__linux__)
    struct stat sb;
    if (stat(file, &sb) == 0) return 0;
    return sb.st_size;
#else
    ScopedFILE f = fopen(file, "rb");
    return _filelengthi64(fileno(f.file));
#endif
}

inline bool RenameFile(const char* oldFile, const char* newFile) {
    return rename(oldFile, newFile) != 0;
}

inline bool CreateFolder(const char* folderName) {
#ifdef __ANDROID__
    __builtin_trap();
    return false;
#else
    return _mkdir(folderName
               #ifndef _WIN32
               , 0777
               #endif 
               ) == 0;
#endif
}

inline bool IsDirectory(const char* path) {
  struct stat file_info; 
  return stat(path, &file_info) == 0 && (S_ISDIR(file_info.st_mode));
}

enum AOpenFlag_ {
    AOpenFlag_Read, 
    AOpenFlag_Write
};
typedef int AOpenFlag;

#ifdef __ANDROID__
struct AFile {
    AAsset* asset;
};

inline AFile AFileOpen(const char* fileName, AOpenFlag flag) {
    return { AAssetManager_open(g_android_app->activity->assetManager, fileName, 0) };
}

inline void AFileRead(void* dst, uint64_t size, AFile file) {
    AAsset_read(file.asset, dst, size);
}

inline void AFileSeekBegin(long size, AFile file) {
    AAsset_seek(file.asset, 0, SEEK_SET);
}

inline void AFileSeek(long offset, AFile file) {
    AAsset_seek(file.asset, offset, SEEK_CUR);
}

inline void AFileWrite(const void* src, uint64_t size, AFile file)
{ }

inline void AFileClose(AFile file) {
    AAsset_close(file.asset);
}

inline bool AFileExist(AFile file) {
    return file.asset != nullptr;
}

inline uint64_t AFileSize(AFile file) {
    return AAsset_getLength(file.asset);
}

#else

struct AFile {
    FILE* file;
};

inline AFile AFileOpen(const char* fileName, AOpenFlag flag) {
    FILE* file;
    const char* modes[2] = { "rb", "wb" };
#ifdef _MSC_VER
    fopen_s(&file, fileName, modes[flag]);
#else
    file = fopen(fileName, modes[flag]);
#endif
    AFile afile;
    afile.file = file;
    return afile;
}

inline void AFileRead(void* dst, uint64_t size, AFile file) {
    fread(dst, 1, size, file.file);
}

inline void AFileWrite(const void* src, uint64_t size, AFile file) { 
    fwrite(src, 1, size, file.file);
}

inline void AFileSeekBegin(AFile file) {
    fseek(file.file, 0, SEEK_SET);
}

inline void AFileSeek(long offset, AFile file) {
    fseek(file.file, offset, SEEK_CUR);
}

inline void AFileClose(AFile file) {
    fclose(file.file);
}

inline bool AFileExist(AFile file) { 
    return file.file != nullptr;
}


inline uint64_t AFileSize(AFile file) {
#if defined(_WIN32)
    return _filelengthi64(_fileno(file.file));
#elif defined(__ANDROID__)
    return AAsset_getLength(file.asset);
#else
    if (fseek(file.file, 0, SEEK_END) != 0) 
        return 0; // Or handle the error as appropriate

    long fileSize = ftell(file.file);
    if (fileSize == -1) 
        return 0; // Or handle the error as appropriate
    return (uint64_t)fileSize;
#endif  
}

#endif

inline char* ReadAllFile(const char* fileName, char* buffer = 0) {
    AFile file = AFileOpen(fileName, AOpenFlag_Read);
    uint64_t fileSize = AFileSize(file);
    if (buffer == nullptr) 
        buffer = (char*)AX_CALLOC(fileSize); // +1 for null terminator
    AFileRead(buffer, fileSize, file);
    AFileClose(file);
    return buffer;
}

inline void FreeAllText(char* text) {
    AX_FREE(text);
}

inline constexpr bool IsNumber(char a) { return a <= '9' && a >= '0'; };
inline constexpr bool IsLower(char a)  { return a >= 'a' && a <= 'z'; };
inline constexpr bool IsUpper(char a)  { return a >= 'A' && a <= 'Z'; };
inline constexpr bool ToLower(char a)  { return a < 'a' ? a + ('A' - 'a') : a; }
inline constexpr bool ToUpper(char a)  { return a > 'Z' ? a - 'a' + 'A' : a; }
// is alphabetical character?
inline constexpr bool IsChar(char a) { return IsUpper(a) || IsLower(a); };
inline constexpr bool IsWhitespace(char c) { return c <= ' '; }

template<typename T>
inline constexpr void Swap(T& a, T& b)
{
    T temp = (T&&)a;
    a = (T&&)b;
    b = (T&&)temp;
}

// String to number functions
inline int ParseNumber(const char*& ptr)
{
    const char* curr = ptr;
    while (*curr && (*curr != '-' && !IsNumber(*curr))) 
        curr++; // skip whitespace
    
    int val = 0l;
    bool negative = false;
    
    if (*curr == '-') 
        curr++, negative = true;
    
    while (*curr > '\n' && IsNumber(*curr))
      val = val * 10 + (*curr++ - '0');
    
    ptr = curr;
    return negative ? -val : val;
}

inline int ParsePositiveNumber(const char*& ptr)
{
    const char* curr = ptr;
    while (*curr && !IsNumber(*curr))
        curr++; // skip whitespace

    int val = 0;
    while (*curr > '\n' && IsNumber(*curr))
        val = val * 10 + (*curr++ - '0');
    ptr = curr;
    return val;
}

inline bool IsParsable(const char* curr)
{   // additional checks
	if (*curr == 0 || *curr == '\n') return false;
	if (!IsNumber(*curr) || *curr != '-') return false;
	return true;
}

inline float ParseFloat(const char*& text)
{
    const int MAX_POWER = 20;
    const double POWER_10_POS[MAX_POWER] =
    {
        1.0e0,  1.0e1,  1.0e2,  1.0e3,  1.0e4,  1.0e5,  1.0e6,  1.0e7,  1.0e8,  1.0e9,
        1.0e10, 1.0e11, 1.0e12, 1.0e13, 1.0e14, 1.0e15, 1.0e16, 1.0e17, 1.0e18, 1.0e19,
    };

    const double POWER_10_NEG[MAX_POWER] =
    {
        1.0e0,   1.0e-1,  1.0e-2,  1.0e-3,  1.0e-4,  1.0e-5,  1.0e-6,  1.0e-7,  1.0e-8,  1.0e-9,
        1.0e-10, 1.0e-11, 1.0e-12, 1.0e-13, 1.0e-14, 1.0e-15, 1.0e-16, 1.0e-17, 1.0e-18, 1.0e-19,
    };

    const char* ptr = text;
    while (!IsNumber(*ptr) && *ptr != '-') ptr++;
	
    double sign = 1.0;
    if(*ptr == '-')
        sign = -1.0, ptr++; 

    double num = 0.0;

    while (IsNumber(*ptr)) 
        num = 10.0 * num + (double)(*ptr++ - '0');

    if (*ptr == '.') ptr++;

    double fra = 0.0, div = 1.0;

    while (IsNumber(*ptr) && div < 1.0e9) // 1e8 is 1 and 8 zero 1000000000
        fra = 10.0f * fra + (double)(*ptr++ - '0'), div *= 10.0f;

    num += fra / div;

    while (IsNumber(*ptr)) ptr++;

    if (*ptr == 'e' || *ptr == 'E') // exponent
    {
        ptr++;
        const double* powers;

        switch (*ptr)
        {
            case '+': powers = POWER_10_POS; ptr++; break;
            case '-': powers = POWER_10_NEG; ptr++; break;
            default:  powers = POWER_10_POS;        break;
        }

        int eval = 0;
        while (IsNumber(*ptr))
            eval = 10 * eval + (*ptr++ - '0');

        num *= (eval >= MAX_POWER) ? 0.0 : powers[eval];
    }

    text = ptr;
    return (float)(sign * num);
}

#ifndef AX_NO_UNROLL
    #if defined(__clang__)
    #   define AX_NO_UNROLL _Pragma("clang loop unroll(disable)") _Pragma("clang loop vectorize(disable)")
    #elif defined(__GNUC__) >= 8
    #   define AX_NO_UNROLL _Pragma("GCC unroll 0")
    #elif defined(_MSC_VER)
    #   define AX_NO_UNROLL __pragma(loop(no_vector))
    #else
    #   define AX_NO_UNROLL
    #endif  
#endif

inline bool StartsWith(const char*& curr, const char* str)
{
    const char* currStart = curr;
    while (IsWhitespace(*curr)) curr++;
    if (*curr != *str) return false;
    while (*str && *curr++ == *str++);
    bool isEqual = *str == 0;
    if (!isEqual) curr = currStart;
    return isEqual;
}

template<typename T> inline void Fill(T* begin, T* end, const T& val)
{
    while (begin < end) *begin++ = val;
}

template<typename T> inline void FillN(T* arr, T val, int n)
{
    for (int i = 0; i < n; ++i) arr[i] = val;
}

template<typename T> inline bool Contains(const T* arr, const T& val, int n)
{
    for (int i = 0; i < n; ++i) 
        if (arr[i] == val) return true;
    return false;
}

// returns the index if found, otherwise returns -1
template<typename T> inline int IndexOf(const T* arr, const T& val, int n)
{
    for (int i = 0; i < n; ++i) 
        if (arr[i] == val) return i;
    return -1;
}

// returns number of val contained in arr
template<typename T> inline int CountIf(const T* arr, const T& val, int n)
{
    int count = 0;
    for (int i = 0; i < n; ++i) 
        count += arr[i] == val;
    return count;
}

template<typename T> inline void Copy(T* dst, const T* src, int n)
{
    for (int i = 0; i < n; ++i)
        dst[i] = src[i];
}

template<typename T> inline void MoveArray(T* dst, T* src, int n)
{
  for (int i = 0; i < n; ++i)
        dst[i] = (T&&)src[i];
}

template<typename T> inline void ClearN(T* src, int n)
{
    for (int i = 0; i < n; ++i)
        src[i].~T();
}

template<typename T> inline void ConstructN(T* src, int n)
{
    T def{};
    for (int i = 0; i < n; ++i) 
        src[i] = def;
}

/****************************************************************
*                             Memory                            *
*****************************************************************/

template<typename T> struct RemoveRef      { typedef T Type; };
template<typename T> struct RemoveRef<T&>  { typedef T Type; };
template<typename T> struct RemoveRef<T&&> { typedef T Type; };
template<typename T> struct RemovePtr      { typedef T Type; };
template<typename T> struct RemovePtr<T*>  { typedef T Type; };

template<typename T>
__forceinline typename RemoveRef<T>::Type&& Move(T&& obj)
{
  typedef typename RemoveRef<T>::Type CastType;
  return (CastType&&)obj;
}

template<typename T>
__forceinline T&& Forward(typename RemoveRef<T>::Type& obj) { return (T&&)obj; }

template<typename T>
__forceinline T&& Forward(typename RemoveRef<T>::Type&& obj) { return (T&&)obj; }

template<class T, class U = T>
__forceinline __constexpr T Exchange(T& obj, U&& new_value)
{
  T old_value = Move(obj);
  obj = Forward<U>(new_value);
  return old_value;
}

// Aligned memory codes taken from Game Engine Architecture book by Jason Gregory

// Shift the given address upwards if/as necessary to// ensure it is aligned to the given number of bytes.
inline uint64_t AlignAddress(uint64_t addr, uint64_t align)
{
    const uint64_t  mask = align - 1;
    ASSERT((align & mask) == 0); // pwr of 2
    return (addr + mask) & ~mask;
}

// Shift the given pointer upwards if/as necessary to// ensure it is aligned to the given number of bytes.
template<typename T>
inline T* AlignPointer(T* ptr, uint64_t align)
{
    const uint64_t  addr = (uint64_t )ptr;
    const uint64_t  addrAligned = AlignAddress(addr, align);
    return (T*)addrAligned;
}

// Aligned allocation function. IMPORTANT: 'align'// must be a power of 2 (typically 4, 8 or 16).
inline void* AllocAligned(uint64_t bytes, uint64_t align)
{
    // Allocate 'align' more bytes than we need.
    uint64_t  actualBytes = bytes + align;
    // Allocate unaligned block.
    uint8* pRawMem = (uint8*)AX_MALLOC(actualBytes);
    // Align the block. If no alignment occurred,// shift it up the full 'align' bytes so we// always have room to store the shift.
    uint8* pAlignedMem = AlignPointer(pRawMem, align);
    
    if (pAlignedMem == pRawMem)
        pAlignedMem += align;
    // Determine the shift, and store it.// (This works for up to 256-byte alignment.)
    uint8 shift = (uint8)(pAlignedMem - pRawMem);
    ASSERT(shift > 0 && shift <= 256);
    pAlignedMem[-1] = (uint8)(shift & 0xFF);
    return pAlignedMem;
}

inline void FreeAligned(void* pMem)
{
    ASSERT(pMem);
    // Convert to U8 pointer.
    uint8* pAlignedMem = (uint8*)pMem;
    // Extract the shift.
    uint64_t  shift = pAlignedMem[-1];
    if (shift == 0)
        shift = 256;
    // Back up to the actual allocated address,
    uint8* pRawMem = pAlignedMem - shift;
    AX_FREE(pRawMem);
}

// if you want memmove it is here with simd version: https://hackmd.io/@AndybnA/0410

inline void MemSetAligned64(void* RESTRICT dst, unsigned char val, uint64_t sizeInBytes)
{
    // we want an offset because the while loop below iterates over 4 uint64_t at a time
    const uint64_t * end = (uint64_t *)((char*)dst + (sizeInBytes >> 3));
    uint64_t * dp = (uint64_t *)dst;
#ifdef _MSC_VER
    uint64_t   d4;
    __stosb((unsigned char*)&d4, val, 8);
#else
    uint64_t   d4 = val; d4 |= d4 << 8ull; d4 |= d4 << 16ull; d4 |= d4 << 32ull;
#endif

    while (dp < end)
    {
        dp[0] = dp[1] = dp[2] = dp[3] = d4;
        dp += 4;
    }
   
    switch (sizeInBytes & 7)
    {
        case 7: *dp++ = d4;
        case 6: *dp++ = d4;
        case 5: *dp++ = d4;
        case 4: *dp++ = d4;
        case 3: *dp++ = d4;
        case 2: *dp++ = d4;
        case 1: *dp   = d4;
    };
}

inline void MemSet32(uint32_t* RESTRICT dst, uint32_t val, uint32_t numValues)
{
    for (uint32_t i = 0; i < numValues; i++)
        dst[i] = val;
}

inline void MemSet64(uint64_t* RESTRICT dst, uint64_t val, uint32_t numValues)
{
    for (uint32_t i = 0; i < numValues; i++)
        dst[i] = val;
}

inline void MemSetAligned32(uint32_t* RESTRICT dst, unsigned char val, uint64_t sizeInBytes)
{
    const uint32_t* end = (uint32_t*)((char*)dst + (sizeInBytes >> 2));
    uint32_t* dp = (uint32_t*)dst;
#ifdef _MSC_VER
    uint32_t  d4;
    __stosb((unsigned char*)&d4, val, 4);
#else
    uint32_t  d4 = val; d4 |= d4 << 8ull; d4 |= d4 << 16ull;
#endif
    while (dp < end)
    {
        dp[0] = dp[1] = dp[2] = dp[3] = d4;
        dp += 4;
    }
    
    switch (sizeInBytes & 3)
    {
        case 3: *dp++ = d4;
        case 2: *dp++ = d4;
        case 1: *dp   = d4;
    };
}

// use size for struct/class types such as Vector3 and Matrix4, 
// leave as zero size constant for big arrays or unknown size arrays
template<int alignment = 0>
inline void MemSet(void* dst, unsigned char val, uint64_t sizeInBytes)
{
    if_constexpr (alignment == 8)
        MemSetAligned64(dst, val, sizeInBytes);
    else if (alignment == 4)
        MemSetAligned32((uint32_t*)dst, val, sizeInBytes);
    else
    {
        uint64_t  uptr = (uint64_t )dst;
        if (!(uptr & 7) && uptr >= 8) MemSetAligned64(dst, val, sizeInBytes);
        else if (!(uptr & 3) && uptr >= 4)  MemSetAligned32((uint32_t*)dst, val, sizeInBytes);
        else
        {
            unsigned char* dp = (unsigned char*)dst;
            while (sizeInBytes--)
                *dp++ = val;
        }
    }
}

inline void MemCpyAligned64(void* dst, const void* RESTRICT src, uint64_t sizeInBytes)
{
    uint64_t *       dp  = (uint64_t *)dst;
    const uint64_t * sp  = (const uint64_t *)src;
    const uint64_t * end = (const uint64_t *)((char*)src) + (sizeInBytes >> 3);
        
    while (sp < end)
    {
        dp[0] = sp[0];
        dp[1] = sp[1];
        dp[2] = sp[2];
        dp[3] = sp[3];
        dp += 4, sp += 4;
    }

    SmallMemCpy(dp, sp, sizeInBytes & 7);
}

inline void MemCpyAligned32(uint32_t* dst, const uint32_t* RESTRICT src, uint64_t sizeInBytes)
{
    uint32_t*       dp  = (uint32_t*)dst;
    const uint32_t* sp  = (const uint32_t*)src;
    const uint32_t* end = (const uint32_t*)((char*)src) + (sizeInBytes >> 2);
        
    while (sp < end)
    {
        dp[0] = sp[0];
        dp[1] = sp[1];
        dp[2] = sp[2];
        dp[3] = sp[3];
        dp += 4, sp += 4;
    }
    
    switch (sizeInBytes & 3)
    {
        case 3: *dp++ = *sp++;
        case 2: *dp++ = *sp++;
        case 1: *dp++ = *sp++;
    };
}

// use size for structs and classes such as Vector3 and Matrix4,
// and use MemCpy for big arrays or unknown size arrays
template<int alignment = 0>
inline void MemCpy(void* dst, const void* RESTRICT src, uint64_t sizeInBytes)
{
    if_constexpr (alignment == 8)
        MemCpyAligned64(dst, src, sizeInBytes);
    else if (alignment == 4)
        MemCpyAligned32((uint32_t*)dst, (const uint32_t*)src, sizeInBytes);
    else
    {
        const char* cend = (char*)((char*)src + sizeInBytes);
        const char* scp = (const char*)src;
        char* dcp = (char*)dst;
        
        while (scp < cend) *dcp++ = *scp++;
    }
}

template<typename T>
struct MallocAllocator
{
    static const bool IsPod = true;
    static const int InitialSize = 512 / MIN((int)sizeof(T), 128);

    T* Allocate(int count) const 
    {
        T* ptr = (T*)AllocAligned(sizeof(T) * count, alignof(T));
        MemsetZero(ptr, sizeof(T) * count);
        return ptr;
    }

    T* AllocateUninitialized(int count) const 
    {
        return (T*)AllocAligned(sizeof(T) * count, alignof(T));
    }
      
    void Deallocate(T* ptr, int count) const 
    {
        FreeAligned(ptr);
    }
      
    T* Reallocate(T* ptr, int oldCount, int count) const
    {
        T* old = ptr;
        T* nev = AllocateUninitialized(count);
        MemCpy<alignof(T)>(nev, old, MIN(oldCount, count) * sizeof(T));
        Deallocate(old, oldCount);
        return nev;
    }
};

template<typename T>
struct FixedSizeGrowableAllocator
{
    static const bool IsPOD = false;
    
    static __constexpr int InitialSize()
    {
        return NextPowerOf2(512 / MIN((int)sizeof(T), 128));
    } 

    struct Fragment
    {
        Fragment* next;
        T* ptr;
        int64_t   size; // used like a index until we fill the fragment
    };

    int currentCapacity = 0;
    Fragment* base = nullptr;
    Fragment* current = nullptr;

    FixedSizeGrowableAllocator(int initialSize)
    {
        // WARNING initial size must be power of two
        ASSERT((initialSize & (initialSize - 1)) == 0);
        currentCapacity = initialSize;
        base = (Fragment*)AllocAligned(sizeof(Fragment), alignof(Fragment));
        MemsetZero(base, sizeof(Fragment));
        current = base;
        base->next = nullptr;
        base->ptr = (T*)AllocAligned(sizeof(T) * initialSize, alignof(T));
        base->size = 0;
    }

    FixedSizeGrowableAllocator() : FixedSizeGrowableAllocator(InitialSize())
    { }

    ~FixedSizeGrowableAllocator()
    {
        if (!base) return;
        while (base)
        {
            FreeAligned(base->ptr);
            Fragment* oldBase = base;
            base = base->next;
            FreeAligned(oldBase);
        }
    }
    
    // move constructor
    FixedSizeGrowableAllocator(FixedSizeGrowableAllocator&& other)
    {
        currentCapacity = other.currentCapacity;
        base = other.base;
        current = other.current;
        other.currentCapacity = 0;
        other.base = other.current = nullptr;
    }

    // copy constructor.
    FixedSizeGrowableAllocator(const FixedSizeGrowableAllocator& other)
    {
        if (!other.base) return;

        int64_t totalSize = 0l;
        Fragment* start = other.base;

        while (start)
        {
            totalSize += start->size;
            start = start->next;
        }

        currentCapacity = NextPowerOf2(totalSize);
        // even though other contains multiple fragments we will fit all data into one fragment
        base = (Fragment*)AllocAligned(sizeof(Fragment), alignof(Fragment)) ;
        MemsetZero(base, sizeof(Fragment));
        base->next = nullptr;
        base->ptr = (T*)AllocAligned(sizeof(T) * currentCapacity, alignof(T));
        base->size = totalSize;
        current = base;

        // copy other's memory to ours
        T* curr = base->ptr;
        start = other.base;
        while (start)
        {
            Copy(curr, start->ptr, start->size);
            curr += start->size;
            start = start->next;
        }
    }

    void* TakeOwnership()
    {
        void* result = base;
        base = nullptr;
        return result;
    }

    void CheckFixGrow(int count)
    {
        if (current->size + count >= currentCapacity)
        {
            while (currentCapacity < current->size + count)
                currentCapacity <<= 1;

            current->next = (Fragment*)AllocAligned(sizeof(Fragment), alignof(Fragment));
            MemsetZero(current->next, sizeof(Fragment));
            current = current->next;
            current->next = nullptr;
            current->ptr = (T*)AllocAligned(sizeof(T) * currentCapacity, alignof(T));
            current->size = 0;
        }
    }

    T* Allocate(int count)
    {
        CheckFixGrow(count);
        T* ptr = current->ptr + current->size;
        current->size += count;
        T def{};
        for (int i = 0; i < count; i++)
            ptr[i] = def;
        return ptr;
    }

    T* AllocateUninitialized(int count)
    {
        CheckFixGrow(count);
        T* ptr = current->ptr + current->size;
        current->size += count;
        return ptr;
    }

    void Deallocate(T* ptr, int count)
    {
        for (int i = 0; i < count; i++)
            ptr[i].~T();
    }

    T* Reallocate(T* ptr, int oldCount, int count)
    {
        Deallocate(ptr, oldCount);
        return Allocate(count);
    }
};

/********************************************************************************
*    Purpose:                                                                   *
*        Simple Dynamic array data structure.                                   *
*    Author:                                                                    *
*        Anilcan Gulkaya 2023 anilcangulkaya7@gmail.com github @benanil         *
********************************************************************************/

template<typename ValueT,
         typename AllocatorT = MallocAllocator<ValueT>>
struct Array
{
    using Iterator = ValueT*;
    using ConstIterator = const ValueT*;
	static const int InitialSize = AllocatorT::InitialSize;
       
	// don't change this variables if you don't know what are you doing
	ValueT* arr    = nullptr;
	int m_count    = 0;
	int m_capacity = 0;
	AllocatorT allocator{};

	Array() : arr(nullptr), m_count(0), m_capacity(0), allocator()
	{ }

	~Array()
	{
		Clear();
	}

	explicit Array(const AllocatorT& _allocator) : arr(nullptr), m_count(0), m_capacity(0), allocator(_allocator)
	{ }

	explicit Array(Array const& other) : Array(other.Data(), other.Size()) {}

	explicit Array(int _capacity) : m_capacity(_capacity)
	{
		if (_capacity == 0) return;
		arr = allocator.AllocateUninitialized(m_capacity);
	}

	Array(int _capacity, int _count)
		: m_capacity(_capacity), m_count(_count)
	{
		arr = allocator.Allocate(m_capacity);
	}

	Array(int _count, const ValueT& val)
		: m_capacity(_count + (_count / 2)), m_count(_count)
	{
		arr = allocator.AllocateUninitialized(m_capacity);
		FillN(arr, val, m_count);
	}

	Array(Iterator _begin, Iterator _end)
	{
		m_count = PointerDistance(_begin, _end);
		m_capacity = m_count + (m_count / 2);
		arr = allocator.AllocateUninitialized(m_capacity);
		MoveArray(arr, _begin, m_count);
	}

	Array(ConstIterator _begin, ConstIterator _end)
	{
		m_count = PointerDistance(_begin, _end);
		m_capacity = m_count + (m_count / 2);
		arr = allocator.AllocateUninitialized(m_capacity);
		Copy(arr, _begin, m_count);
	}

	Array(ConstIterator _begin, int _size)
		: m_count(_size), m_capacity(_size + (_size / 2))
	{
		arr = allocator.AllocateUninitialized(m_capacity);
		Copy(arr, _begin, _size);
	}

	Array& operator = (Array const& other)
	{
		if (&other != this)
		{
			GrowIfNecessary(other.Size());
			Copy(arr, other.arr, other.Size());
			m_count = other.Size();
		}
		return *this;
	}

	Array& operator = (Array&& other)
	{
		if (&other != this)
		{
			if (arr != nullptr)
				allocator.Deallocate(arr, m_capacity);
			arr = other.arr;
			allocator = other.allocator;
			m_count = other.Size();
			m_capacity = other.m_capacity;
			other.m_capacity = 0;
			other.m_count = 0;
			other.arr = nullptr;
		}
		return *this;
	}

	bool operator == (const Array& other) const
	{
		if (&other == this) return true;
		if (other.Size() != Size()) return false;

		for (int i = 0; i < other.Size(); ++i)
		{
			if (arr[i] != other.arr[i])
				return false;
		}
		return true;
	}

	bool operator != (const Array& other) const
	{
		return !(other == *this);
	}

	ValueT& operator[](int index)
	{
		ASSERT(index >= 0 && index < m_count);
		return arr[index];
	}

	ValueT& GetUnchecked(int index)
	{
		return arr[index];
	}
	
	const ValueT& GetUnchecked(int index) const
	{
		return arr[index];
	}

	const ValueT& operator[](int index) const
	{
		ASSERT(index >= 0 && index < m_count);
		return arr[index];
	}

	Array  operator + (const ValueT& other) const { Array _new = *this; _new.Add(other); return _new; }

	Array& operator += (const ValueT& type) { Add(type);                           return *this; }

	Array& operator += (const Array& other) { Add(other.cbegin(), other.Size());   return *this; }

	// you should know what are you doing if you want to use this function
	// takes the ownership of the data, which means you need to make deallocation 
	ValueT* TakeOwnership()
	{
		m_capacity = m_count = 0;
		ValueT *res = arr;
		arr = nullptr;
		return res;
	}

	void Add(const ValueT& value)
	{
		GrowIfNecessary(m_count + 1);
		arr[m_count++] = value;
	}

	void Add(ConstIterator _begin, int _size)
	{
		ASSERT(m_count < INT32_MAX - _size); // array max size reached
		GrowIfNecessary(m_count + _size);
		for (int i = 0; i < _size; i++)
		{
			arr[m_count++] = *_begin++;
		}
	}

	void AddUninitialized(int _size)
	{
		ASSERT(m_count < INT32_MAX - _size); // array max size reached
		GrowIfNecessary(m_count + _size);
		m_count += _size;
	}

	void Add(ConstIterator _begin, ConstIterator _end)
	{
		Add(_begin, PointerDistance(_begin, _end));
	}

	void Insert(int index, const ValueT& value)
	{
		OpenSpace(index, 1);
		arr[index] = value;
	}

	void Insert(ConstIterator _it, const ValueT* _begin, const ValueT* _end)
	{
		Insert(PointerDistance(begin(), _it), _begin, _end);
	}

	void Insert(int index, const ValueT* _begin, const ValueT* _end)
	{
		uint size = PointerDistance(_begin, _end);
		OpenSpace(index, size);
		Copy(arr, _begin, size);
	}

	void InsertUnordered(int index, const ValueT& value)
	{
		GrowIfNecessary(m_count + 1);
		arr[m_count++] = (ValueT&&)arr[index];
		arr[index] = value;
	}
	
	typedef bool(*RemoveFunc)(const ValueT& val);

	// returns number of elements removed
	int RemoveAll(RemoveFunc match)
	{
		int freeIndex = 0;   // the first free slot in items array

		// Find the first item which needs to be removed.
		while (freeIndex < m_count && match(arr[freeIndex]) == false) freeIndex++;
		if (freeIndex >= m_count) return 0;

		int current = freeIndex + 1;
		while (current < m_count)
		{
			// Find the first item which needs to be kept.
			while (current < m_count && match(arr[current]) == true) current++;

			if (current < m_count)
			{
				// copy item to the free slot.
				arr[freeIndex++] = Forward<ValueT>(arr[current++]);
			}
		}

		int numCleared = m_count - freeIndex;
		ClearN(arr + freeIndex, numCleared);
		m_count = freeIndex;
		return numCleared; // removed item count
	}

	template<typename... Args>
	ValueT& EmplaceAt(int index, Args&&... args)
	{
		OpenSpace(index, 1);
		arr[index].~ValueT();
		ValueT val(Forward<Args>(args)...);
		arr[index] = (ValueT&&)val;
		return arr[index];
	}

	template<typename... Args>
	ValueT& EmplaceBack(Args&&... args)
	{
		GrowIfNecessary(m_count + 1);
		ValueT val(Forward<Args>(args)...);
		arr[m_count] = (ValueT&&)val;
		return arr[m_count++];
	}

	void Clear()
	{
		if (arr != nullptr)
		{
			allocator.Deallocate(arr, m_capacity);
			m_count = 0;
			m_capacity = 0;
			arr = nullptr;
		}
	}

	void RemoveAt(Iterator _it, int _count = 1)
	{
		RemoveSpace(PointerDistance(begin(), _it), _count);
	}

	void RemoveAt(int _index, int _count = 1)
	{
		RemoveSpace(_index, _count);
	}

	void RemoveBack() { RemoveSpace(m_count - 1, 1); }

	void Remove(const ValueT& val)
	{
		int index = IndexOf(arr, val, m_count);
		if (index != -1)
		{
			RemoveSpace(index, 1);
		}
	}

	// faster than remove
	void RemoveUnordered(int index)
	{
		arr[index].~ValueT();
		Swap(arr[index], arr[--m_count]);
	}

	void Reserve(int _size)
	{
		if (_size > m_capacity)
		{
			GrowIfNecessary(_size);
		}
	}

	void Resize(int _size)
	{
		if (_size > m_capacity)
		{
			if (arr) // array can be nullptr (first initialization)
				arr = allocator.Reallocate(arr, m_capacity, _size);
			else
				arr = allocator.Allocate(_size);
			m_capacity = _size;
		}
		m_count    = _size;
	}

	// you can use this function after removing elements in the array, this will reduct the amount of memory used
	void ShrinkToFit()
	{
		int newCapacity = CalculateArrayGrowth(m_count);
		if (m_capacity > newCapacity)
		{
			arr = allocator.Reallocate(arr, m_capacity, newCapacity);
		}
	}

	const ValueT& Back()     const { return arr[m_count - 1]; }
	ValueT&       Back()           { return arr[m_count - 1]; }
	void          PopBack()        { arr[--m_count].~ValueT(); }
	void          PushBack(const ValueT& val) { Add(val); }

	const ValueT& Front()    const { return arr[0]; }
	      ValueT& Front()          { return arr[0]; }
	void          PopFront()       { RemoveAt(0); }

	int           Size()     const { return m_count; }
	int           Capacity() const { return m_capacity; }
	ValueT*       Data()           { return arr; }
	const ValueT* Data()     const { return arr; }
	bool          Empty()    const { return m_count == 0; }

	ConstIterator cbegin()   const { return arr; }
	ConstIterator cend()     const { return arr + m_count; }
	ConstIterator end()      const { return arr + m_count; }
	ConstIterator begin()    const { return arr; }
	Iterator      begin() { return arr; }
	Iterator      end() { return arr + m_count; }

#ifdef ASTL_STL_COMPATIBLE
	using iterator = ValueT*;
	using const_iterator = const ValueT*;
	void erase(Iterator _it, int _count = 1) { Remove(_it, _count); }
	void insert(int index, const ValueT& value) { Insert(index, value); }
	void push_back(const ValueT& value) { Add(value); }
	template<typename... Args>
	ValueT& emplace_back(Args&&... args)
	{
		return EmplaceBack(Forward<Args>(args)...);
	}
	int size()     const { return m_count; }
	int capacity() const { return m_capacity; }
	ValueT* data() { return arr; }
	const ValueT* data() const { return arr; }
	bool empty() const { return size == 0; }
	const ValueT& back() const { return arr[m_count - 1]; }
	ValueT& back() { return arr[m_count - 1]; }
	ValueT pop_back() { return arr[--m_count]; }
	void resize(int _size) { Resize(_size); }
	void shrink_to_fit() { ShrinkToFit(); }
#endif

	void GrowIfNecessary(int _size)
	{
		ASSERT(_size <= INT32_MAX);
		
		if (AX_UNLIKELY(_size > m_capacity))
		{
			int oldCapacity = m_capacity;
			m_capacity = _size <= InitialSize ? InitialSize : CalculateArrayGrowth(_size);

			if (arr) // array can be nullptr (first initialization)
				arr = allocator.Reallocate(arr, oldCapacity, m_capacity);
			else
				arr = allocator.Allocate(m_capacity);
		}
	}

	void OpenSpace(int _index, int _count)
	{
		long i = m_count + _count;
		int  j = m_count;
		ASSERT(i <= INT32_MAX);
		
		GrowIfNecessary((int)i);

		while (j >= _index)
		{
			arr[--i] = (ValueT&&) arr[--j];
		}
		m_count += _count;
	}

	void RemoveSpace(int _index, int _count)
	{
		ASSERT((_index + _count) <= m_count);
		int newSize = MAX(m_count - _count, 0);

		int i = _index;
		int j = _index + _count;

		// *******i****j***   < opens space with incrementing both of the pointers (two pointer algorithm)
		while (j < m_count)
		{
			arr[i++] = (ValueT&&) arr[j++];
		}

		if_constexpr (!AllocatorT::IsPod)
		{
			for (int i = newSize; i < m_count; ++i)
			{
				arr[i].~ValueT();
			}
		}
		m_count = newSize;
	}
};

inline void SBFree(void* a) { if (a) FreeAligned(((int*)a)-2); }

inline int SBCount(void* a) { if (!a) return 0; return *((int*)a - 1); }

template<typename T>
inline void SBPush(T*& b, const T& v)
{
	T* a = b;
	constexpr int twoIntSize = sizeof(int) + sizeof(int);
	if (a == nullptr)
	{
		a = (T*)AllocAligned(sizeof(T) * 16 + twoIntSize, alignof(T));
		((int*)a)[0] = 16;
		((int*)a)[1] = 0;
		a = (T*)((int*)a+2);
	}

	int* arr = ((int*)a);
	int size = arr[-1], capacity = arr[-2];
	if (size + 1 >= capacity)
	{
		int* old = arr-2;
		int newCapacity = CalculateArrayGrowth(capacity);
		a = (T*)AllocAligned(newCapacity * sizeof(T) + twoIntSize, alignof(T));
		MemCpy<alignof(T)>(a, old, capacity * sizeof(T) + twoIntSize);
		FreeAligned(old);
		a = (T*)((int*)a+2);
		arr = ((int*)a);
		arr[-2] = newCapacity;
	}
	SmallMemCpy(a + size, &v, sizeof(T));
	((int*)a)[-1] = size + 1;
	b = a;
}

__forceinline __constexpr float SqrtConstexpr(float a) {
    // from: Jack W. Cerenshaw's math toolkit for real time development book: page 63 Listing 4.3
    // I've removed some of the branches. slightly slower than sqrtf
    // double version is also here: https://gist.github.com/benanil/9d1668c0befb24263e27bd04dfa2e90f#file-mathfunctionswithoutstl-c-L230
    const double A = 0.417319242, B = 0.5901788532;
    union { double fp; struct { unsigned lo, hi; }; } x {};
    if (a <= 0.001) return 0.0f;
    x.fp = (double)a;
    // grab the exponent
    unsigned expo = (x.hi >> 20u) - 0x3fe;
    x.hi &= 0x000fffffu;
    x.hi += 0x3fe00000u;
    // get square root of normalized number
    double root = A + B * x.fp;
    root = 0.5 * (x.fp / root + root); // you can even remove this haha if you do that you might want to reduce this: 0.414213562
    root = 0.5 * (x.fp / root + root);
    // root = 0.5 * (x.fp / root + root); // iterate 3 times probably overkill
    // now rebuild the result
    x.fp = root;
    bool isOdd = expo & 1;
    expo += isOdd;
    x.fp *= 1.0 + (isOdd * 0.414213562);
    expo = (expo + (expo < 0u)) >> 1;
    // put it back
    expo += 0x3feu;
    x.hi &= 0x000fffffu;
    x.hi += expo << 20u;
    return (float)x.fp;
}

__forceinline float Sqrt(float a) {
#ifdef AX_SUPPORT_SSE
	return _mm_cvtss_f32(_mm_sqrt_ps(_mm_set_ps1(a)));
#elif defined(__clang__)
	return __builtin_sqrt(a);
#else
	return SqrtConstexpr(a);
#endif
}

inline void QuaternionFromMatrix(float* Orientation, const float* m) {
    int i, j, k = 0;
    const int numCol = 4;
    float root, trace = m[0*numCol+0] + m[1 * numCol + 1] + m[2 * numCol + 2];
    
    if (trace > 0.0f)
    {
        root = Sqrt(trace + 1.0f);
        Orientation[3] = 0.5f * root;
        root = 0.5f / root;
        Orientation[0] = root * (m[1 * numCol + 2] - m[2 * numCol + 1]);
        Orientation[1] = root * (m[2 * numCol + 0] - m[0 * numCol + 2]);
        Orientation[2] = root * (m[0 * numCol + 1] - m[1 * numCol + 0]);
    }
    else
    {
        static const int Next[3] = { 1, 2, 0 };
        i = 0;
        i += m[1 * numCol + 1] > m[0 * numCol + 0]; // if (M.m[1][1] > M.m[0][0]) i = 1
        if (m[2 * numCol + 2] > m[i * numCol + i]) i = 2;
        j = Next[i];
        k = Next[j];
        
        root = Sqrt(m[i * numCol + i] - m[j * numCol + j] - m[k * numCol + k] + 1.0f);
        
        Orientation[i] = 0.5f * root;
        root = 0.5f / root;
        Orientation[j] = root * (m[i * numCol + j] + m[j * numCol + i]);
        Orientation[k] = root * (m[i * numCol + k] + m[k * numCol + i]);
        Orientation[3] = root * (m[j * numCol + k] - m[k*numCol+j]);
    } 
}

#if defined(AX_SUPPORT_SSE) && !defined(AX_ARM)
/*//////////////////////////////////////////////////////////////////////////*/
/*                                 SSE                                      */
/*//////////////////////////////////////////////////////////////////////////*/

typedef __m128 vec_t;
typedef __m128 veci_t;

#define VecZero()           _mm_setzero_ps()
#define VecOne()            _mm_set1_ps(1.0f)
#define VecNegativeOne()    _mm_setr_ps( -1.0f, -1.0f, -1.0f, -1.0f)
#define VecSet1(x)          _mm_set1_ps(x)
#define VecSet(x, y, z, w)  _mm_set_ps(x, y, z, w)  /* -> {w, z, y, x} */
#define VecSetR(x, y, z, w) _mm_setr_ps(x, y, z, w) /* -> {x, y, z, w} */
#define VecLoad(x)          _mm_loadu_ps(x)
#define VecLoadA(x)         _mm_load_ps(x)
#define Vec3Load(x)         VecSetW(_mm_loadu_ps(x), 0.0f)

#define VecStore(ptr, x)       _mm_store_ps(ptr, x)
#define VecFromInt(x, y, z, w) _mm_castsi128_ps(_mm_setr_epi32(x, y, z, w))
#define VecFromInt1(x)         _mm_castsi128_ps(_mm_set1_epi32(x))
#define VecToInt(x) x
#define VecFromVeci(x) x
#define MakeShuffleMask(x,y,z,w)     (x | (y<<2) | (z<<4) | (w<<6)) /* internal use only */

// Get Set
#define VecSplatX(V1) _mm_permute_ps(V1, MakeShuffleMask(0, 0, 0, 0))
#define VecSplatY(V1) _mm_permute_ps(V1, MakeShuffleMask(1, 1, 1, 1))
#define VecSplatZ(V1) _mm_permute_ps(V1, MakeShuffleMask(2, 2, 2, 2))
#define VecSplatW(V1) _mm_permute_ps(V1, MakeShuffleMask(3, 3, 3, 3))

#define VecGetX(v) _mm_cvtss_f32(v)
#define VecGetY(v) _mm_cvtss_f32(VecSplatY(v))
#define VecGetZ(v) _mm_cvtss_f32(VecSplatZ(v))
#define VecGetW(v) _mm_cvtss_f32(VecSplatW(v))

#define VecSetX(v, x) _mm_move_ss(v, _mm_set_ss(x))
#define VecSetY(v, y) _mm_insert_ps(v, _mm_set_ss(y), 0x10)
#define VecSetZ(v, z) _mm_insert_ps(v, _mm_set_ss(z), 0x20)
#define VecSetW(v, w) _mm_insert_ps(v, _mm_set_ss(w), 0x30)

#define VecAddf(a, b) _mm_add_ps(a, VecSet1(b))
#define VecSubf(a, b) _mm_sub_ps(a, VecSet1(b))
#define VecMulf(a, b) _mm_mul_ps(a, VecSet1(b))
#define VecDivf(a, b) _mm_div_ps(a, VecSet1(b))

#define VecFmadLane(a, b, c, l) _mm_fmadd_ps(a, _mm_permute_ps(b, MakeShuffleMask(l, l, l, l)), c)

#define VecFmad(a, b, c) _mm_fmadd_ps(a, b, c)
#define VecHadd(a, b) _mm_hadd_ps(a, b)

#define VecRcp(a) _mm_rcp_ps(a)
#define VecSqrt(a) _mm_sqrt_ps(a)

// Vector Math
#define VecDot(a, b)  _mm_dp_ps(a, b, 0xff)
#define VecDotf(a, b) _mm_cvtss_f32(_mm_dp_ps(a, b, 0xff))
#define VecNorm(v)    _mm_mul_ps(_mm_rsqrt_ps(_mm_dp_ps(v, v, 0xff)), v);
#define VecLenf(v)    _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(v, v, 0xff)))
#define VecLen(v)     VecSqrt(_mm_dp_ps(v, v, 0xff))

#define Vec3Dot(a, b)  _mm_dp_ps(a, b, 0x7f)
#define Vec3Dotf(a, b) _mm_cvtss_f32(_mm_dp_ps(a, b, 0x7f))
#define Vec3Norm(v)    _mm_mul_ps(_mm_rsqrt_ps(_mm_dp_ps(v, v, 0x7f)), v)
#define Vec3Lenf(v)    _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(v, v, 0x7f)))
#define Vec3Len(v)     _mm_sqrt_ps(_mm_dp_ps(v, v, 0x7f))

// return (vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffle(vec1, vec2, x, y, z, w)   _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(x,y,z,w))
#define VecShuffleR(vec1, vec2, x, y, z, w)  _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(w,z,y,x))

#elif defined(AX_ARM)
/*//////////////////////////////////////////////////////////////////////////*/
/*                                 NEON                                     */
/*//////////////////////////////////////////////////////////////////////////*/
#include <arm_neon.h>
typedef float32x4_t vec_t;
typedef uint32x4_t veci_t;

#define VecZero()           vdupq_n_f32(0.0f)
#define VecOne()            vdupq_n_f32(1.0f)
#define VecNegativeOne()    vdupq_n_f32(-1.0f)
#define VecSet1(x)          vdupq_n_f32(x)
#define VecSet(x, y, z, w)  ARMCreateVec(w, z, y, x) /* -> {w, z, y, x} */
#define VecSetR(x, y, z, w) ARMCreateVec(x, y, z, w) /* -> {x, y, z, w} */
#define VecLoad(x)          vld1q_f32(x)
#define VecLoadA(x)         vld1q_f32(x)
#define Vec3Load(x)         ARMVector3Load(x)

#define VecStore(ptr, x)        vst1q_f32(ptr, x)
#define VecFromInt1(x)          vdupq_n_s32(x)
#define VecFromInt(x, y, z, w)  ARMCreateVecI(x, y, z, w)
#define VecToInt(x) x

#define VecFromVeci(x) x

#define VecGetX(v) vgetq_lane_f32(v, 0)
#define VecGetY(v) vgetq_lane_f32(v, 1)
#define VecGetZ(v) vgetq_lane_f32(v, 2)
#define VecGetW(v) vgetq_lane_f32(v, 3)

#define VecSetX(v, x) ((v) = vsetq_lane_f32(x, v, 0))
#define VecSetY(v, y) ((v) = vsetq_lane_f32(y, v, 1))
#define VecSetZ(v, z) ((v) = vsetq_lane_f32(z, v, 2))
#define VecSetW(v, w) ((v) = vsetq_lane_f32(w, v, 3))

// Arithmetic
#define VecAdd(a, b) vaddq_f32(a, b)
#define VecSub(a, b) vsubq_f32(a, b)
#define VecMul(a, b) vmulq_f32(a, b)
#define VecDiv(a, b) ARMVectorDevide(a, b)
                                                     
#define VecAddf(a, b) vaddq_f32(a, VecSet1(b))
#define VecSubf(a, b) vsubq_f32(a, VecSet1(b))
#define VecMulf(a, b) vmulq_f32(a, VecSet1(b))
#define VecDivf(a, b) ARMVectorDevide(a, VecSet1(b))

// Vector Math
#define VecDot(a, b)  ARMVectorDot(a, b)
#define VecDotf(a, b) VecGetX(ARMVectorDot(a, b))
#define VecNorm(v)    ARMVectorNorm(v)
#define VecLenf(v)    VecGetX(ARMVectorLength(v))
#define VecLen(v)     ARMVectorLength(v)

#define Vec3Dot(a, b)  ARMVector3Dot(a, b)
#define Vec3Dotf(a, b) VecGetX(ARMVector3Dot(a, b))
#define Vec3Norm(v)    ARMVector3Norm(v)
#define Vec3Lenf(v)    VecGetX(ARMVector3Length(v))
#define Vec3Len(v)     ARMVector3Length(v)

// Swizzling Masking
#define VecShuffle(vec1, vec2, x, y, z, w)  ARMVectorShuffle<x, y, z, w>(vec1, vec2)
#define VecShuffleR(vec1, vec2, x, y, z, w) ARMVectorShuffle<w, z, y, x>(vec1, vec2)

__forceinline vec_t ARMVector3Load(float* src)
{
    return vcombine_f32(vld1_f32(src), vld1_lane_f32(src + 2, vdup_n_f32(0), 0));
}

__forceinline vec_t ARMCreateVec(float x, float y, float z, float w) {
    float32x2_t V0 = vcreate_f32((uint64_t)__builtin_bit_cast(uint, x) | ((uint64)__builtin_bit_cast(uint, y) << 32));
    float32x2_t V1 = vcreate_f32((uint64_t)__builtin_bit_cast(uint, z) | ((uint64)__builtin_bit_cast(uint, w) << 32));
    return vcombine_f32(V0, V1);
}

__forceinline veci_t ARMCreateVecI(uint x, uint y, uint z, uint w) {
    uint32x2_t V0 = vcreate_s32(((uint64_t)x) | (((uint64_t)y) << 32));
    uint32x2_t V1 = vcreate_s32(((uint64_t)z) | (((uint64_t)w) << 32));
    return vcombine_u32(V0, V1);
}

__forceinline vec_t ARMVector3Dot(vec_t a, vec_t b) {
    float32x4_t vTemp = vmulq_f32(a, b);
    float32x2_t v1 = vget_low_f32(vTemp);
    float32x2_t v2 = vget_high_f32(vTemp);
    v1 = vpadd_f32(v1, v1);
    v2 = vdup_lane_f32(v2, 0);
    v1 = vadd_f32(v1, v2);
    return vcombine_f32(v1, v1);
}

__forceinline vec_t ARMVector3Length(vec_t v) {
    float32x4_t vTemp = vmulq_f32(v, v);
    float32x2_t v1 = vget_low_f32(vTemp);
    float32x2_t v2 = vget_high_f32(vTemp);
    v1 = vpadd_f32(v1, v1);
    v2 = vdup_lane_f32(v2, 0);
    v1 = vadd_f32(v1, v2);
    const float32x2_t zero = vdup_n_f32(0);
    uint32x2_t VEqualsZero = vceq_f32(v1, zero);
    float32x2_t Result = vrsqrte_f32(v1);
    Result = vmul_f32(v1, Result);
    Result = vbsl_f32(VEqualsZero, zero, Result);
    return vcombine_f32(Result, Result);
}

__forceinline vec_t ARMVectorLength(vec_t v) {
    float32x4_t vTemp = vmulq_f32(v, v);
    float32x2_t v1 = vget_low_f32(vTemp);
    float32x2_t v2 = vget_high_f32(vTemp);
    v1 = vadd_f32(v1, v2);
    v1 = vpadd_f32(v1, v1);
    const float32x2_t zero = vdup_n_f32(0);
    uint32x2_t VEqualsZero = vceq_f32(v1, zero);
    float32x2_t Result = vrsqrte_f32(v1);
    Result = vmul_f32(v1, Result);
    Result = vbsl_f32(VEqualsZero, zero, Result);
    return vcombine_f32(Result, Result);
}

__forceinline vec_t ARMVectorDot(vec_t a, vec_t b) {
    float32x4_t vTemp = vmulq_f32(a, b);
    float32x2_t v1 = vget_low_f32(vTemp);
    float32x2_t v2 = vget_high_f32(vTemp);
    v1 = vadd_f32(v1, v2);
    v1 = vpadd_f32(v1, v1);
    return vcombine_f32(v1, v1);
}

template<int E0, int E1, int E2, int E3>
__forceinline vec_t ARMVectorSwizzle(vec_t v)
{
  float a = vgetq_lane_f32(v, E0); 
  float b = vgetq_lane_f32(v, E1); 
  float c = vgetq_lane_f32(v, E2); 
  float d = vgetq_lane_f32(v, E3); 
  return VecSetR(a, b, c, d); 
}

template<int E0, int E1, int E2, int E3>
__forceinline vec_t ARMVectorShuffle(vec_t v0, vec_t v1)
{
  float a = vgetq_lane_f32(v0, E0);
  float b = vgetq_lane_f32(v0, E1);
  float c = vgetq_lane_f32(v1, E2);
  float d = vgetq_lane_f32(v1, E3);
  return VecSetR(a, b, c, d);
}

#else // AX_SUPPORT_SSE
/*//////////////////////////////////////////////////////////////////////////*/
/*                                 No Vector                                */
/*//////////////////////////////////////////////////////////////////////////*/

struct vec_t {
    float x, y, z, w;
          float& operator[](int i)       { return ((float*)this)[i]; }
    const float  operator[](int i) const { return ((float*)this)[i]; }
};

__forceinline vec_t MakeVec4(float x, float y, float z, float w) { return vec_t { x, y, z, w }; }
__forceinline vec_t MakeVec4(float x) { return vec_t { x, x, x, x }; }
__forceinline vec_t MakeVec4(const float* x) { return vec_t { x[0], x[1], x[2], x[3] }; }

#define VecZero()           MakeVec4(0.0f)
#define VecOne()            MakeVec4(1.0f)
#define VecNegativeOne()    MakeVec4(-1.0f, -1.0f, -1.0f, -1.0f)
#define VecSet1(x)          MakeVec4(x)
#define VecSet(x, y, z, w)  MakeVec4(w, z, y, x)
#define VecSetR(x, y, z, w) MakeVec4(x, y, z, w)
#define VecLoad(x)          MakeVec4(x)
#define VecLoadA(x)         MakeVec4(x)

#define VecStore(ptr, a)       SmallMemCpy(ptr, &a.x, 4 * 4);
#define VecFromInt1(x)         MakeVec4i(x)
#define VecFromInt(x, y, z, w) MakeVec4i(x, y, z, w)
#define VecToInt(x)    BitCast<veci_t>(x)
#define VecFromVeci(x) BitCast<vec_t>(x)

// Getters setters
#define VecGetX(v) v.x
#define VecGetY(v) v.y
#define VecGetZ(v) v.z
#define VecGetW(v) v.w

#define VecSplatX(V1) MakeVec4(V1.x)
#define VecSplatY(V1) MakeVec4(V1.y)
#define VecSplatZ(V1) MakeVec4(V1.z)
#define VecSplatW(V1) MakeVec4(V1.w)

#define VecSetX(v, val) v.x = val
#define VecSetY(v, val) v.y = val
#define VecSetZ(v, val) v.z = val
#define VecSetW(v, val) v.w = val

// return (vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffle(vec1, vec2, x, y, z, w)  MakeVec4(vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffleR(vec1, vec2, x, y, z, w) MakeVec4(vec1[w], vec1[z], vec2[y], vec2[x])

#define VecMulf(a, b) MakeVec4(a.x * b, a.y * b, a.z * b, a.w * b)

#define VecDotf(a, b)  (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w)
#define VecDot(a, b)   MakeVec4(a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w)
#define VecNorm(v)     VecDiv(v, Vec3Len(v))
#define VecLenf(v)     Sqrt(VecDot(v, v))
#define VecLen(v)      MakeVec4(Sqrt(VecDot(v, v)))

#define Vec3Dot(a, b)  (a.x * b.x + a.y * b.y + a.z * b.z)
#define Vec3Norm(v)    VecDiv(v, Vec3Len(v))
#define Vec3Lenf(v)    Sqrt(Vec3Dot(v, v))
#define Vec3Len(v)     MakeVec4(Sqrt(Vec3Dot(v, v)))

#define VecSqrt(a)     MakeVec4(Sqrt(a.x), Sqrt(a.y), Sqrt(a.z), Sqrt(a.w))

#endif

__forceinline void VECTORCALL Vec3Store(float* f, vec_t v)
{
    #if defined(__ARM_NEON__) || defined(_M_ARM64)
    vst1_f32(f, vget_low_f32(v));
    vst1q_lane_f32(f + 2, v, 2);
    #else
    f[0] = VecGetX(v);
    f[1] = VecGetY(v);
    f[2] = VecGetZ(v);
    #endif
}

struct alignas(16) Matrix4
{
    union
    {
        struct { vec_t r[4]; };
        struct { float m[4][4]; };
    };
    
    static vec_t VECTORCALL ExtractScaleV(const Matrix4 matrix) 
    {
    	return VecSetR(Vec3Lenf(matrix.r[0]), Vec3Lenf(matrix.r[2]), Vec3Lenf(matrix.r[1]), 0.0f);
    }

    static Matrix4 VECTORCALL Transpose(Matrix4 M)
    {
        Matrix4 mResult;
        #ifdef AX_ARM
        float32x4x2_t P0 = vzipq_f32(M.r[0], M.r[2]);
        float32x4x2_t P1 = vzipq_f32(M.r[1], M.r[3]);
        float32x4x2_t T0 = vzipq_f32(P0.val[0], P1.val[0]);
        float32x4x2_t T1 = vzipq_f32(P0.val[1], P1.val[1]);
        mResult.r[0] = T0.val[0];
        mResult.r[1] = T0.val[1];
        mResult.r[2] = T1.val[0];
        mResult.r[3] = T1.val[1];
        #else
        const vec_t vTemp1 = VecShuffleR(M.r[0], M.r[1], 1, 0, 1, 0);
        const vec_t vTemp3 = VecShuffleR(M.r[0], M.r[1], 3, 2, 3, 2);
        const vec_t vTemp2 = VecShuffleR(M.r[2], M.r[3], 1, 0, 1, 0);
        const vec_t vTemp4 = VecShuffleR(M.r[2], M.r[3], 3, 2, 3, 2);
        mResult.r[0] = VecShuffleR(vTemp1, vTemp2, 2, 0, 2, 0);
        mResult.r[1] = VecShuffleR(vTemp1, vTemp2, 3, 1, 3, 1);
        mResult.r[2] = VecShuffleR(vTemp3, vTemp4, 2, 0, 2, 0);
        mResult.r[3] = VecShuffleR(vTemp3, vTemp4, 3, 1, 3, 1);
        #endif
        return mResult;
    }
};


#define __private static
#define __public 

struct GLTFAccessor
{
    int bufferView;
    int componentType; // GraphicType
    int count;
    int byteOffset;
    int type; // 1 = SCALAR, 2 = VEC2, 3 = VEC3, 4 = VEC4, mat4
};

struct GLTFBufferView
{
    int buffer;
    int byteOffset;
    int byteLength;
    int target;
    int byteStride;
};

typedef FixedSizeGrowableAllocator<char> AStringAllocator;
typedef FixedSizeGrowableAllocator<int>  AIntAllocator;

#define StrCMP16(_str, _otr) (sizeof(_otr) <= 9 ? StrCmp8(_str, _otr, sizeof(_otr)) : \
                                                  StrCmp16(_str, _otr, sizeof(_otr))) 
 
bool StrCmp8(const char* RESTRICT a, const char* b, uint64_t n)
{
    uint64_t al, bl;
    uint64_t mask = ~0ull >> (64 - ((n-1) * 8));
    al = UnalignedLoad64(a);
    bl = UnalignedLoad64(b);
    return ((al ^ bl) & mask) == 0;
}

bool StrCmp16(const char* RESTRICT a, const char* b, uint64_t bSize)
{
    bool equal = StrCmp8(a, b, 9);
    equal &= StrCmp8(a + 8, b + 8, bSize - 8);
    return equal;
}

inline const char* SkipUntill(const char* curr, char character)
{
    AX_NO_UNROLL while (*curr != character) curr++;
    return curr;
}

inline const char* SkipAfter(const char* curr, char character)
{
    AX_NO_UNROLL while (*curr++ != character);
    return curr;
}

__private const char* CopyStringInQuotes(char*& str, const char* curr, AStringAllocator& stringAllocator)
{
    while (*curr != '"') curr++; // find quote
    curr++; // skip "
    // get length in quotes
    const char* quote = curr;
    while (*quote != '"') quote++;
    int len = (int)(quote - curr);
    char* alloc = stringAllocator.AllocateUninitialized(len + 16);
    str = alloc;
    SmallMemCpy(alloc, curr, len);
    alloc += len;
    curr  += len;
    *alloc = '\0'; // null terminate
    return ++curr;// skip quote
}

__private const char* GetStringInQuotes(char* str, const char* curr)
{
    ++curr; // skip quote " 
    AX_NO_UNROLL while (*curr != '"') *str++ = *curr++;
    *str = '\0'; // null terminate
    return ++curr;
}

__private const char* HashStringInQuotes(uint64_t* hash, const char* curr)
{
    ++curr; // skip quote " 
    uint64_t h = 0ull;
    uint64_t shift = 0;
    AX_NO_UNROLL while (*curr != '"' && shift < 64) { h |= uint64_t(*curr++) << shift; shift += 8; }
    *hash = h;
    return ++curr;
}

// most 8 characters
constexpr uint64_t AHashString8(const char* curr)
{
    uint64_t h = 0ull;
    uint64_t shift = 0;

    while (*curr != '\0') 
    { 
        h |= uint64_t(*curr++) << shift; 
        shift += 8; 
    }
    return h;
}

__private const char* SkipToNextNode(const char* curr, char open, char close)
{
    int balance = 1;
    // skip to first open brackets
    AX_NO_UNROLL while (*curr++ != open);

    while (*curr && balance > 0)
    {
        balance += *curr == open;
        balance -= *curr++ == close;
    }
    return curr;
}

__private const char* ParseFloat16(const char*& curr, short& flt)
{
    flt = (short)(ParseFloat(curr) * 400.0f);
    return curr;
}

__private const char* ParseAccessors(const char* curr, Array<GLTFAccessor>& accessorArray)
{
    GLTFAccessor accessor{};
    curr += 10; // skip accessors"
    // read each accessor
    while (true)
    {
        // search for name
        while (*curr && *curr != '"')
        {
            if (*curr == '}') // next accessor
            {
                accessorArray.Add(accessor);
                MemsetZero(&accessor, sizeof(GLTFAccessor));
            }

            if (*curr == ']') // end all accessors
            return ++curr;
            curr++;
        }
        ASSERT(*curr != '\0' && "parsing accessors failed probably you forget to close brackets!");
        curr++;
        if (StrCMP16(curr, "bufferView"))         accessor.bufferView = ParsePositiveNumber(curr);
        else if (StrCMP16(curr, "byteOffset"))    accessor.byteOffset = ParsePositiveNumber(curr);
        else if (StrCMP16(curr, "componentType")) accessor.componentType = ParsePositiveNumber(curr) - 0x1400; // GL_BYTE 
        else if (StrCMP16(curr, "count"))         accessor.count = ParsePositiveNumber(curr);
        else if (StrCMP16(curr, "name"))
        {
            curr += sizeof("name'"); // we don't need accessor's name
            int numQuotes = 0;
            // skip two quotes
            while (numQuotes < 2)
                numQuotes += *curr++ == '"';
        }
        else if (StrCMP16(curr, "type"))
        {
            curr += sizeof("type'"); // skip type
            curr = SkipUntill(curr, '"');
            uint64_t hash;
            curr = HashStringInQuotes(&hash, curr);
            
            switch (hash)
            {   case AHashString8("SCALAR"): accessor.type = 1; break;
                case AHashString8("VEC2"):   accessor.type = 2; break;
                case AHashString8("VEC3"):   accessor.type = 3; break;
                case AHashString8("VEC4"):   accessor.type = 4; break;
                case AHashString8("MAT4"):   accessor.type = 16;break;
                default: ASSERT(0 && "Unknown accessor type");
            };
        }
        else if (StrCMP16(curr, "min")) curr = SkipToNextNode(curr, '[', ']'); // skip min and max
        else if (StrCMP16(curr, "max")) curr = SkipToNextNode(curr, '[', ']');
        else if (StrCMP16(curr, "normalized")) curr = SkipAfter(curr, '"');
        else
        {
            ASSERT(0 && "unknown accessor var");
            return (const char*)AError_UNKNOWN_ACCESSOR_VAR;
        }
    }
}

__private const char* ParseBufferViews(const char* curr, Array<GLTFBufferView>& bufferViews)
{
    GLTFBufferView bufferView{};
    curr += sizeof("bufferViews'");

    // read each buffer view
    while (true)
    {
        // search for name
        while (*curr && *curr != '"')
        {
            if (*curr == '}') // next buffer view
            {
                bufferViews.Add(bufferView);
                MemsetZero(&bufferView, sizeof(GLTFBufferView));
            }
            if (*curr++ == ']') return curr; // end all buffer views
        }
        ASSERT(*curr != '0' && "buffer view parse failed, probably you forgot to close brackets!");

        uint64_t hash;
        curr = HashStringInQuotes(&hash, curr);

        switch (hash)
        {
            case AHashString8("buffer"):    bufferView.buffer = ParsePositiveNumber(++curr); break; 
            case AHashString8("byteOffs"):  bufferView.byteOffset = ParsePositiveNumber(++curr); break; 
            case AHashString8("byteLeng"):  bufferView.byteLength = ParsePositiveNumber(++curr); break; 
            case AHashString8("byteStri"):  bufferView.byteStride = ParsePositiveNumber(++curr); break; 
            case AHashString8("target"):    bufferView.target = ParsePositiveNumber(++curr); break;
            case AHashString8("name"):  {
                int numQuote = 0;
                while (numQuote < 2)
                    numQuote += *curr++ == '"';
                break;
            }
            default: {
                ASSERT(0 && "UNKNOWN buffer view value!");
                return (const char*)AError_UNKNOWN_BUFFER_VIEW_VAR;
            }
        };
    }
}

static void DecodeBase64(char *dst, const char *src, size_t src_length)
{
    struct Base64Table
    {
        uint8_t table[256] = { 0 };
        constexpr Base64Table()
        {
            for (char c = 'A'; c <= 'Z'; c++) table[c] = (uint8_t)(c - 'A');
            for (char c = 'a'; c <= 'z'; c++) table[c] = (uint8_t)(26 + (c - 'a'));
            for (char c = '0'; c <= '9'; c++) table[c] = (uint8_t)(52 + (c - '0'));
            table['+'] = 62;
            table['/'] = 63;
        }
    };
    
    static constexpr Base64Table table{};
    
    for (uint64_t i = 0; i + 4 <= src_length; i += 4) {
        uint32_t a = table.table[src[i + 0]];
        uint32_t b = table.table[src[i + 1]];
        uint32_t c = table.table[src[i + 2]];
        uint32_t d = table.table[src[i + 3]];
           
        dst[0] = (char)(a << 2 | b >> 4);
        dst[1] = (char)(b << 4 | c >> 2);
        dst[2] = (char)(c << 6 | d);
        dst += 3;
    }
}

__private const char* ParseBuffers(const char* curr, const char* path, Array<GLTFBuffer>& bufferArray)
{
    GLTFBuffer buffer{};
    curr += sizeof("buffers'"); // skip buffers"
    char binFilePath[256]={0};
    char* endOfWorkDir = binFilePath;
    int binPathlen = StringLength(path);
    SmallMemCpy(binFilePath, path, binPathlen);
    endOfWorkDir = binFilePath + binPathlen;
    
    // remove bla.gltf
    while (binFilePath[binPathlen - 1] != '/' && binFilePath[binPathlen - 1] != '\\')
        binFilePath[--binPathlen] = '\0', endOfWorkDir--;

    // read each buffer
    while (true)
    {
        // search for name
        while (*curr && *curr != '"')
        {
            if (*curr == '}') // next buffer
            {
                bufferArray.Add(buffer);
                MemsetZero(&buffer, sizeof(GLTFBuffer));
                MemsetZero(endOfWorkDir, sizeof(binFilePath) - (size_t)(endOfWorkDir - binFilePath));
            }

            if (*curr++ == ']') return curr; // end all buffers
        }
        ASSERT(*curr && "parsing buffers failed, probably you forgot to close braces");
        curr++;
        if (StrCMP16(curr, "uri")) // is uri
        {
            curr += sizeof("uri'"); // skip uri": 
            while (*curr != '"') curr++;
            if (StartsWith(curr, "\"data:"))
            {
                curr = SkipAfter(curr, ',');
                uint64_t base64Size = 0;
                while (curr[base64Size] != '\"') {
                    base64Size++;
                }
                buffer.uri = AX_MALLOC(base64Size);
                DecodeBase64((char*)buffer.uri, curr, base64Size);
                curr += base64Size + 1;
            }
            else
            {
                curr = GetStringInQuotes(endOfWorkDir, curr);
                buffer.uri = ReadAllFile(binFilePath);
                ASSERT(buffer.uri && "uri is not exist");
                if (!buffer.uri) return (const char*)AError_BIN_NOT_EXIST;
            }
        }
        else if (StrCMP16(curr, "byteLength"))
        {
            buffer.byteLength = ParsePositiveNumber(++curr);
        }
        else
        {
            ASSERT(0 && "Unknown buffer variable! byteLength or uri excepted.");
            return (const char*)AError_BUFFER_PARSE_FAIL;
        }
    }
}

// write paths to path buffer, buffer is seperated by null terminators
__private const char* ParseImages(const char* curr, const char* path, Array<AImage>& images, AStringAllocator& stringAllocator)
{
    curr = SkipUntill(curr, '[');
    curr++;
    
    int pathLen = StringLength(path);
    while (path[pathLen-1] != '/') pathLen--;

    AImage image{};
    // read each buffer
    while (true)
    {
        // search for name
        while (*curr && *curr != '"')
            if (*curr++ == ']')
                return curr; // end all images
        
        ASSERT(*curr != '\0' && "parse images failed probably you forgot to close brackets");

        curr++;
        bool isUri = StrCMP16(curr, "uri");

        // mimeType and name is not supported
        if (isUri)
        {
            curr += 4;
            curr = SkipAfter(curr, '"');
            int uriSize = 0;
            while (curr[uriSize] != '"') uriSize++;

            image.path = stringAllocator.Allocate(uriSize + pathLen + 16);
            SmallMemCpy(image.path, path, pathLen);
            SmallMemCpy(image.path + pathLen, curr, uriSize);
            image.path[uriSize + pathLen] = '\0';
            curr += uriSize;
            images.PushBack(image);
        }
    }
    return nullptr;
}

__private const char* ParseTextures(const char* curr, Array<ATexture>& textures, AStringAllocator& stringAllocator)
{
    curr += sizeof("textures'");
    ATexture texture{};

    // read each buffer
    while (true)
    {
        // search for name
        while (*curr && *curr != '"')
        {
            if (*curr == '}') // next buffer view
            {
                textures.Add(texture);
                MemsetZero(&texture, sizeof(ATexture));
            }

            if (*curr == ']') // end all buffer views
            return ++curr;
            curr++;
        }
        ASSERT(*curr != '\0' && "parse images failed probably you forgot to close brackets");
        curr++;
        if (StrCMP16(curr, "sampler")) 
        {
            texture.sampler = ParsePositiveNumber(++curr);
        }
        else if (StrCMP16(curr, "source"))
        {
            texture.source = ParsePositiveNumber(++curr);
        }
        else if (StrCMP16(curr, "name"))
        {
            curr = CopyStringInQuotes(texture.name, curr + 5, stringAllocator);
        }
        else {
            ASSERT(0 && "Unknown buffer variable! sampler, source or name excepted.");
            return (const char*)AError_UNKNOWN_TEXTURE_VAR;
        }
    }
}

__private const char* ParseAttributes(const char* curr, APrimitive* primitive)
{
    curr += sizeof("attributes'");

    while (true)
    {
        while (*curr != '"')
        if (*curr++ == '}') return curr;
        
        curr++; // skip "
        unsigned maskBefore = primitive->attributes;
        if      (StrCMP16(curr, "POSITION"))   { primitive->attributes |= AAttribType_POSITION; }
        else if (StrCMP16(curr, "NORMAL"))     { primitive->attributes |= AAttribType_NORMAL; }
        else if (StrCMP16(curr, "TEXCOORD_0")) { primitive->attributes |= AAttribType_TEXCOORD_0; }
        else if (StrCMP16(curr, "TANGENT"))    { primitive->attributes |= AAttribType_TANGENT; }
        else if (StrCMP16(curr, "TEXCOORD_1")) { primitive->attributes |= AAttribType_TEXCOORD_1; }
        else if (StrCMP16(curr, "JOINTS_0"))   { primitive->attributes |= AAttribType_JOINTS; }
        else if (StrCMP16(curr, "WEIGHTS_0"))  { primitive->attributes |= AAttribType_WEIGHTS; }
        else if (StrCMP16(curr, "TEXCOORD_"))  { curr = SkipAfter(curr, '"'); continue; } // < NO more than two texture coords
        else { ASSERT(0 && "attribute variable unknown!"); return (const char*)AError_UNKNOWN_ATTRIB; }

        curr = SkipUntill(curr, '"'); curr++;// skip quote because attribute in double quotes
        // using bitmask will help us to order attributes correctly(sort) Position, Normal, TexCoord
        unsigned newIndex = TrailingZeroCount32(maskBefore ^ primitive->attributes);
        primitive->vertexAttribs[newIndex] = (void*)(uint64_t)ParsePositiveNumber(curr);
    }
}

__private const char* ParseMeshes(const char* curr, Array<AMesh>& meshes, AStringAllocator& stringAllocator)
{
    char text[64]{};
    curr += sizeof("meshes'"); // skip meshes" 
    AMesh mesh{};
    MemsetZero(&mesh, sizeof(AMesh));

    // parse all meshes
    while (true)
    {
        while (*curr != '"')
        {
            if (*curr == '}') 
            {
                meshes.Add(mesh);
                MemsetZero(&mesh, sizeof(AMesh));
            }
            if (*curr++ == ']') return curr; // end of meshes
        }
        curr = GetStringInQuotes(text, curr);
        
        if (StrCMP16(text, "name")) {
            curr = CopyStringInQuotes(mesh.name, curr, stringAllocator); 
            continue; 
        }
        else if (!StrCMP16(text, "primitives")) { 
            ASSERT(0 && "only primitives and name allowed"); 
            return (const char*)AError_UNKNOWN_MESH_VAR; 
        }

        APrimitive primitive{};  
        primitive.material = -1;
        // parse primitives
        while (true)
        {
            while (*curr != '"')
            {
                if (*curr == '}')
                {
                    SBPush(mesh.primitives, primitive);
                    MemsetZero(&primitive, sizeof(APrimitive));
                    mesh.numPrimitives++;
                    primitive.material = -1;
                }

                if (*curr++ == ']') goto end_primitives; // this is end of primitive list
            }
            curr++;
            
            if      (StrCMP16(curr, "attributes")) { curr = ParseAttributes(curr, &primitive); }
            else if (StrCMP16(curr, "indices"))    { primitive.indiceIndex = ParsePositiveNumber(curr); }
            else if (StrCMP16(curr, "mode"))       { primitive.mode        = ParsePositiveNumber(curr); }
            else if (StrCMP16(curr, "material"))   { primitive.material    = ParsePositiveNumber(curr); }
            else { ASSERT(0); return (const char*)AError_UNKNOWN_MESH_PRIMITIVE_VAR; }
        }
        end_primitives:{}
        curr++; // skip ]
    }
    return nullptr;
}

struct IntPtrPair { int numElements, *ptr; };

static IntPtrPair ParseIntArray(const char*& cr, AIntAllocator& intAllocator)
{
    const char* curr = cr;
    IntPtrPair result={};
    // find how many elements there are:
    while (!IsNumber(*curr)) curr++;
    
    const char* begin = curr;
    result.numElements = 1;
    while (true)
    {
        result.numElements += *curr == ',';
        if (*curr++ == ']') break;
    }
    curr = begin;
    result.ptr = intAllocator.AllocateUninitialized(result.numElements);
    result.numElements = 0;
    
    while (*curr != ']')
    {
        if (IsNumber(*curr))
        {
            result.ptr[result.numElements] = ParsePositiveNumber(curr);
            result.numElements++;
        }
        curr++;
    }
    cr = curr;
    return result;
}

__private const char* ParseNodes(const char* curr,
                                 Array<ANode>& nodes,
                                 AStringAllocator& stringAllocator,
                                 FixedSizeGrowableAllocator<int>& intAllocator, float scale)
{
    curr = SkipUntill(curr, '[');
    curr++;
    ANode node{};
    node.rotation[3] = 1.0f;
    node.scale[0] = node.scale[1] = node.scale[2] = scale; 
    node.index = -1;
    
    // read each node
    while (true)
    {
        // search for name
        while (*curr && *curr != '"')
        {
            if (*curr == '}')
            {
                nodes.Add(node);
                MemsetZero(&node, sizeof(ANode));
                node.rotation[3] = 1.0f;
                node.scale[0] = node.scale[1] = node.scale[2] = scale;
                node.index = -1;
            }
            if (*curr++ == ']') return curr; // end all nodes
        }
        ASSERT(*curr != '\0' && "parsing nodes not possible, probably forgot to close brackets!");
        curr++; // skips the "
        
        // mesh, name, children, matrix, translation, rotation, scale
        if      (StrCMP16(curr, "mesh"))   { node.type = 0; node.index = ParsePositiveNumber(curr); continue; } // don't want to skip ] that's why continue
        else if (StrCMP16(curr, "camera")) { node.type = 1; node.index = ParsePositiveNumber(curr); continue; } // don't want to skip ] that's why continue
        else if (StrCMP16(curr, "children"))
        {
            IntPtrPair result = ParseIntArray(curr, intAllocator);
            node.numChildren = result.numElements;
            node.children = result.ptr;
        }
        else if (StrCMP16(curr, "matrix"))
        {
            Matrix4 m;
            float* matrix = (float*)&m;
            
            for (int i = 0; i < 16; i++)
                matrix[i] = ParseFloat(curr);
            
            m = Matrix4::Transpose(m);
            node.translation[0] = matrix[12];
            node.translation[1] = matrix[13];
            node.translation[2] = matrix[14];
            QuaternionFromMatrix(node.rotation, matrix);

            vec_t v = VecMulf(Matrix4::ExtractScaleV(m), scale);
            Vec3Store(node.scale, v);
        }
        else if (StrCMP16(curr, "translation"))
        {
            node.translation[0] = ParseFloat(curr);
            node.translation[1] = ParseFloat(curr);
            node.translation[2] = ParseFloat(curr);
        }
        else if (StrCMP16(curr, "rotation"))
        {
            node.rotation[0] = ParseFloat(curr);
            node.rotation[1] = ParseFloat(curr);
            node.rotation[2] = ParseFloat(curr);
            node.rotation[3] = ParseFloat(curr);
        }
        else if (StrCMP16(curr, "scale"))
        {
            node.scale[0] = ParseFloat(curr) * scale;
            node.scale[1] = ParseFloat(curr) * scale;
            node.scale[2] = ParseFloat(curr) * scale;
        }
        else if (StrCMP16(curr, "name"))
        {
            curr = CopyStringInQuotes(node.name, curr + 5, stringAllocator);
            continue; 
        }
        else if (StrCMP16(curr, "skin"))
        {
            node.skin = ParsePositiveNumber(curr);
            continue; // continue because we don't want to skip ] and it is not exist
        }
        else
        {
            ASSERT(0 && "Unknown node variable");
            return (const char*)AError_UNKNOWN_NODE_VAR;
        }

        curr = SkipUntill(curr, ']');
        curr++;
    }
    return nullptr;
}

__private const char* ParseCameras(const char* curr, Array<ACamera>& cameras, AStringAllocator& stringAllocator)
{
    curr += sizeof("camera'");
    char text[64]{};
    ACamera camera{};
    // parse all meshes
    while (true)
    {
        while (*curr != '"')
        {
            if (*curr == '}') 
            {
                cameras.Add(camera);
                MemsetZero(&camera, sizeof(ACamera));
            }
            if (*curr++ == ']') return curr; // end of cameras
        }
        curr = GetStringInQuotes(text, curr);
        
        if (StrCMP16(text, "name")) {
            curr = CopyStringInQuotes(camera.name, curr, stringAllocator); 
            continue; 
        }
        if (StrCMP16(text, "type")) {
            curr = SkipUntill(curr, '"');
            curr++;
            camera.type = *curr == 'p'; // 0 orthographic 1 perspective 
            curr = SkipUntill(curr, '"');
            curr++;
            continue; 
        }
        else if (!StrCMP16(text, "orthographic") && !StrCMP16(text, "perspective")) { 
            ASSERT(0 && "unknown camera variable"); 
            return (const char*)AError_UNKNOWN_CAMERA_VAR; 
        }

        // parse primitives
        while (true)
        {
            while (*curr != '"')
            if (*curr++ == '}')  goto end_properties; // this is end of camera variables
            
            curr++;
            if      (StrCMP16(curr, "zfar"))        { camera.zFar        = ParseFloat(curr); }
            else if (StrCMP16(curr, "znear"))       { camera.zNear       = ParseFloat(curr); }
            else if (StrCMP16(curr, "aspectRatio")) { camera.aspectRatio = ParseFloat(curr); }
            else if (StrCMP16(curr, "yfov"))        { camera.yFov        = ParseFloat(curr); }
            else if (StrCMP16(curr, "xmag"))        { camera.xmag        = ParseFloat(curr); }
            else if (StrCMP16(curr, "ymag"))        { camera.ymag        = ParseFloat(curr); }
            else { ASSERT(0); return (const char*)AError_UNKNOWN_CAMERA_VAR; }
        }
        end_properties:{}
    }
    return nullptr;
}

__private const char* ParseScenes(const char* curr, Array<AScene>& scenes, 
                                  AStringAllocator& stringAllocator, FixedSizeGrowableAllocator<int>& intAllocator)
{
    curr = SkipUntill(curr, '[');
    curr++;
    AScene scene{};
    // read each node
    while (true)
    {
        // search for name
        while (*curr && *curr != '"')
        {
            if (*curr == '}')
            {
                scenes.Add(scene);
                MemsetZero(&scene, sizeof(AScene));
            }
            if (*curr++ == ']') return curr; // end all scenes
        }
        ASSERT(*curr != '\0' && "parsing scenes not possible, probably forgot to close brackets!");
        curr++; // skips the "
        
        if (StrCMP16(curr, "nodes"))
        {
            // find how many childs there are:
            while (!IsNumber(*curr)) curr++;
            const char* begin = curr;
            scene.numNodes = 1;
            while (true)
            {
                scene.numNodes += *curr == ',';
                if (*curr++ == ']') break;
            }
            curr = begin;
            scene.nodes = intAllocator.AllocateUninitialized(scene.numNodes);
            scene.numNodes = 0;

            while (*curr != ']')
            {
                if (IsNumber(*curr))
                {
                    scene.nodes[scene.numNodes] = ParsePositiveNumber(curr);
                    scene.numNodes++;
                }
                curr++;
            }
            curr++;// skip ]
        }
        else if (StrCMP16(curr, "name"))
        {
            curr = CopyStringInQuotes(scene.name, curr + 5, stringAllocator);
        }
    } 
}
inline char OGLWrapToWrap(int wrap)
{
    switch (wrap)
    {
        case 0x2901: return 0; // GL_REPEAT          10497
        case 0x812F: return 1; // GL_CLAMP_TO_EDGE   33071
        case 0x812D: return 2; // GL_CLAMP_TO_BORDER 33069
        case 0x8370: return 3; // GL_MIRRORED_REPEAT 33648
        default: ASSERT(0 && "wrong or undefined sampler type!"); return 0;
    }
}

__private const char* ParseSamplers(const char* curr, Array<ASampler>& samplers)
{
    curr = SkipUntill(curr, '[');
    curr++;
    
    ASampler sampler{};
    
    // read each node
    while (true)
    {
        // search for name
        while (*curr && *curr != '"')
        {
            if (*curr == '}')
            {
                samplers.Add(sampler);
                MemsetZero(&sampler, sizeof(ASampler));
            }
            if (*curr++ == ']') return curr; // end all nodes
        }
        ASSERT(*curr != '\0' && "parsing nodes not possible, probably forgot to close brackets!");
        curr++; // skips the "

        if      (StrCMP16(curr, "magFilter")) sampler.magFilter = (char)(ParsePositiveNumber(curr) - 0x2600); // GL_NEAREST 9728, GL_LINEAR 0x2601 9729
        else if (StrCMP16(curr, "minFilter")) sampler.minFilter = (char)(ParsePositiveNumber(curr) - 0x2600); // GL_NEAREST 9728, GL_LINEAR 0x2601 9729
        else if (StrCMP16(curr, "wrapS"))     sampler.wrapS = (char)OGLWrapToWrap(ParsePositiveNumber(curr));
        else if (StrCMP16(curr, "wrapT"))     sampler.wrapT = (char)OGLWrapToWrap(ParsePositiveNumber(curr));
        else { ASSERT(0 && "parse samplers failed!"); return (const char*)AError_UNKNOWN; }
    }
}

__private const char* ParseMaterialTexture(const char* curr, AMaterial::Texture& texture)
{
    curr = SkipUntill(curr, '{');
    curr++;
    while (true)
    {
        while (*curr && *curr != '"')
        {
            if (*curr++ == '}') return curr;
        }
        ASSERT(*curr && "parsing material failed, probably forgot to close brackets");
        curr++;

        if (StrCMP16(curr, "scale")) {
            curr = ParseFloat16(curr, texture.scale);
        }
        else if (StrCMP16(curr, "index")) {
            texture.index = (char)ParsePositiveNumber(curr);
        }
        else if (StrCMP16(curr, "texCoord")) {
            texture.texCoord = (char)ParsePositiveNumber(curr);
        }
        else if (StrCMP16(curr, "strength")) {
            curr = ParseFloat16(curr, texture.strength);
        }
        else if (StrCMP16(curr, "extensions")) {
            curr = SkipToNextNode(curr, '{', '}'); // currently extensions are not supported 
        }
        else {
            ASSERT(0 && "unknown material texture value");
            return (const char*)AError_UNKNOWN_MATERIAL_VAR;
        }
    }
    return nullptr;
}
__forceinline uint32_t PackColorRGBAU32(float* c) {
    return (uint32_t)(*c * 255.0f) | ((uint32_t)(c[1] * 255.0f) << 8) | ((uint32_t)(c[2] * 255.0f) << 16) | ((uint32_t)(c[3] * 255.0f) << 24);
}

__private const char* ParseMaterials(const char* curr, Array<AMaterial>& materials, AStringAllocator& stringAllocator)
{
    // mesh, name, children, 
    // matrix, translation, rotation, scale
    curr = SkipUntill(curr, '[');
    curr++; // skip '['
    AMaterial material{};
    // read each material
    while (true)
    {
        // search for name
        while (*curr && *curr != '"')
        {
            if (*curr == '}')
            {
                materials.Add(material);
                MemsetZero(&material, sizeof(AMaterial));
                material.baseColorTexture.index = -1;
            }
            if (*curr++ == ']') return curr; // end all nodes
        }
        ASSERT(*curr && "parsing material failed, probably forgot to close brackets");

        int texture = -1;
        curr++; // skips the "
        if (StrCMP16(curr, "name"))
        {
            curr = CopyStringInQuotes(material.name, curr + 5, stringAllocator);
        }
        else if (StrCMP16(curr, "doubleSided"))
        {
            curr += sizeof("doubleSided'"); // skip doubleSided"
            AX_NO_UNROLL while (!IsLower(*curr)) curr++;
            material.doubleSided = *curr == 't';
        }
        else if (StrCMP16(curr, "pbrMetallicRoug")) //pbrMetallicRoughhness
        {
            curr = SkipUntill(curr, '{');
    
            // parse until finish
            while (true)
            {
                // search for name
                while (*curr && *curr != '"')
                {
                    if (*curr++ == '}') { goto pbr_end; }
                }
                curr++; // skips the "
                
                if      (StrCMP16(curr, "baseColorTex"))  { curr = ParseMaterialTexture(curr, material.baseColorTexture); }
                else if (StrCMP16(curr, "metallicRough")) { curr = ParseMaterialTexture(curr, material.metallicRoughnessTexture); }
                else if (StrCMP16(curr, "baseColorFact"))
                {
                    float baseColorFactor[4] = { ParseFloat(curr), ParseFloat(curr), ParseFloat(curr), ParseFloat(curr)};
                    material.baseColorFactor = PackColorRGBAU32(baseColorFactor);
                    curr = SkipUntill(curr, ']');
                    curr++;
                }
                else if (StrCMP16(curr, "metallicFact"))
                {
                    curr = ParseFloat16(curr, material.metallicFactor);
                }
                else if (StrCMP16(curr, "roughnessFact"))
                {
                    curr = ParseFloat16(curr, material.roughnessFactor);
                }
                else
                {
                    ASSERT(0 && "unknown pbrMetallicRoughness value!");
                    return (char*)AError_UNKNOWN_PBR_VAR;
                }
            }
            pbr_end: {}
        }
        else if (StrCMP16(curr, "normalTexture"))    texture = 0;
        else if (StrCMP16(curr, "occlusionTextur"))  texture = 1;
        else if (StrCMP16(curr, "emissiveTexture"))  texture = 2;
        else if (StrCMP16(curr, "emissiveFactor")) 
        {
            curr = ParseFloat16(curr, material.emissiveFactor[0]); 
            curr = ParseFloat16(curr, material.emissiveFactor[1]);
            curr = ParseFloat16(curr, material.emissiveFactor[2]);
            curr = SkipUntill(curr, ']');
            curr++;
        }
        else if (StrCMP16(curr, "extensions"))
        {
            curr = SkipToNextNode(curr, '{', '}'); // currently extensions are not supported 
        }
        else if (StrCMP16(curr, "alphaMode"))
        {
            char text[16]={0};
            curr += sizeof("alphaMode'");
            curr = SkipUntill(curr, '"');
            curr = GetStringInQuotes(text, curr);
                 if (StrCMP16(text, "OPAQUE")) material.alphaMode = AMaterialAlphaMode_Opaque;
            else if (StrCMP16(text, "MASK"))   material.alphaMode = AMaterialAlphaMode_Mask;
            else if (StrCMP16(text, "BLEND"))  material.alphaMode = AMaterialAlphaMode_Blend;
        }
        else if (StrCMP16(curr, "alphaCutoff"))
        {
            material.alphaCutoff = ParseFloat(curr);
        }
        else {
            ASSERT(0 && "undefined material variable!");
            return (const char*)AError_UNKNOWN_MATERIAL_VAR;
        }

        if (texture != -1)
        {
            curr = ParseMaterialTexture(curr, material.textures[texture]);
            if ((uint64_t)curr == AError_UNKNOWN_MATERIAL_VAR) return curr;
        }
    }
    return curr;
}

static const char* ParseSkins(const char* curr, Array<ASkin>& skins, AStringAllocator& stringAllocator, AIntAllocator& intAllocator)
{
    curr = SkipAfter(curr, '[');
    
    ASkin skin{};
    skin.skeleton = -1;

    // read each node
    while (true)
    {
        // search for name
        while (*curr && *curr != '"')
        {
            if (*curr == '}')
            {
                skins.Add(skin);
                MemsetZero(&skin, sizeof(ASkin));
                skin.skeleton = -1;
            }
            if (*curr++ == ']') return curr; // end all nodes
        }
        ASSERT(*curr != '\0' && "parsing skins not possible, probably forgot to close brackets!");
        curr++; // skips the "

        if (StrCMP16(curr, "inverseBindMatrices"))
        {
            // we will parse later, because we are not sure we are parsed accessors at this point
            skin.inverseBindMatrices = (float*)(size_t)ParsePositiveNumber(curr);
        }
        else if (StrCMP16(curr, "skeleton"))            skin.skeleton = ParsePositiveNumber(curr);
        else if (StrCMP16(curr, "name"))                curr = CopyStringInQuotes(skin.name, curr + 5, stringAllocator);
        else if (StrCMP16(curr, "joints"))
        {
            IntPtrPair result = ParseIntArray(curr, intAllocator);
            skin.numJoints = result.numElements;
            skin.joints = result.ptr;
            curr++; // skip ]
        }
    }
    return curr;
}

static const char* ParseAnimations(const char* curr, Array<AAnimation>& animations, 
                                   AStringAllocator& stringAllocator, AIntAllocator& intAllocator)
{
    curr = SkipAfter(curr, '[');
    Array<AAnimChannel> channels;
    Array<AAnimSampler> samplers;

    AAnimation animation{};
    // read each node
    while (true)
    {
        // search for name
        while (*curr && *curr != '"')
        {
            if (*curr == '}')
            {
                animation.numSamplers = samplers.Size();
                animation.numChannels = channels.Size();
                animation.samplers = samplers.TakeOwnership();
                animation.channels = channels.TakeOwnership();
                animations.Add(animation);
                MemsetZero(&animation, sizeof(AAnimation));
            }
            if (*curr++ == ']') 
                return curr; // end all nodes
        }
        ASSERT(*curr != '\0' && "parsing animations not possible, probably forgot to close brackets!");
        curr++; // skips the "

        if (StrCMP16(curr, "name"))
        {
            curr = CopyStringInQuotes(animation.name, curr + sizeof("name'"), stringAllocator);
        }
        else if (StrCMP16(curr, "channels"))
        {
            curr += sizeof("channels'");
            AAnimChannel channel;
            bool parsingTarget = false;
            while (true)
            {
                while (*curr && *curr != '"')
                {
                    if (*curr == ']') { curr++; /* skip ] */ goto end_parsing; }
                    if (*curr == '}') 
                    {
                        if (parsingTarget) parsingTarget = false;
                        else 
                        {
                            channels.Add(channel);
                            MemsetZero(&channel, sizeof(AAnimChannel));
                        }
                    }
                    curr++;
                }
                ASSERT(*curr != '\0' && "parsing anim channels not possible, probably forgot to close brackets!");

                uint64_t hash;
                curr = HashStringInQuotes(&hash, curr);

                switch (hash)
                {
                    case AHashString8("sampler"): channel.sampler = ParsePositiveNumber(curr);     break;
                    case AHashString8("node"):    channel.targetNode = ParsePositiveNumber(curr);  break;
                    case AHashString8("target"):  curr += sizeof("target'"); parsingTarget = true; break; 
                    case AHashString8("path"):
                    {
                        curr = SkipAfter(curr, '"');
                        switch (*curr) {
                            case 't': channel.targetPath = AAnimTargetPath_Translation; curr += sizeof("translation'"); break;
                            case 'r': channel.targetPath = AAnimTargetPath_Rotation;    curr += sizeof("rotation'"); break;
                            case 's': channel.targetPath = AAnimTargetPath_Scale;       curr += sizeof("scale'"); break;
                            default: ASSERT(0 && "Unknown animation path value");
                        };
                        break;
                    }
                    default: ASSERT(0 && "Unknown animation channel value");
                };
            }
        }
        else if (StrCMP16(curr, "samplers"))
        {
            curr += sizeof("samplers'");
            AAnimSampler sampler;
            while (true)
            {
                while (*curr && *curr != '"')
                {
                    if (*curr == ']') { curr++; /* skip ] */ goto end_parsing; }
                    if (*curr == '}') 
                    {
                        samplers.Add(sampler);
                        MemsetZero(&sampler, sizeof(AAnimSampler));
                    }
                    curr++;
                }
                ASSERT(*curr != '\0' && "parsing anim channels not possible, probably forgot to close brackets!");

                uint64_t hash;
                curr = HashStringInQuotes(&hash, curr);

                switch (hash)
                {
                    case AHashString8("input"):         sampler.input  = (float*)(size_t)ParsePositiveNumber(curr); break;
                    case AHashString8("output"):        sampler.output = (float*)(size_t)ParsePositiveNumber(curr); break;
                    case AHashString8("interpol"): // you've been searching from interpol hands up!!
                    {
                        curr += sizeof("interpolation") - sizeof("interpol");
                        curr = SkipAfter(curr, '"');
                        switch (*curr)
                        {
                            case 'L': sampler.interpolation = 0; curr += sizeof("Linear'");      break; // Linear
                            case 'S': sampler.interpolation = 1; curr += sizeof("Step'");        break; // Step
                            case 'C': sampler.interpolation = 2; curr += sizeof("CubicSpline'"); break; // CubicSpline
                            default: ASSERT(0 && "Unknown animation path value"); break;
                        };
                        break;
                    }
                    default: ASSERT(0 && "Unknown animation sampler value"); break;
                };
            }
        }
        end_parsing:{}
    }
    return curr;
}

__public int ParseGLTF(const char* path, SceneBundle* result, float scale)
{
    ASSERT(result && path);
    uint64_t sourceSize = 0;
    char* source = ReadAllFile(path, nullptr);
    MemsetZero(result, sizeof(SceneBundle));

    if (source == nullptr) { result->error = AError_FILE_NOT_FOUND; ASSERT(0); return 0; }

#if defined(DEBUG) || defined(_DEBUG)
    // ascii utf8 support check
    // if (IsUTF8ASCII(source, sourceSize) != 1) { result->error = AError_NON_UTF8; return; }
#endif
    Array<GLTFBufferView> bufferViews ;
    Array<GLTFBuffer>     buffers     ;
    Array<GLTFAccessor>   accessors   ;

    AStringAllocator stringAllocator(2048);
    FixedSizeGrowableAllocator<int> intAllocator(512);

    Array<AMesh>  meshes; Array<ANode>        nodes; Array<AMaterial> materials; Array<ATexture>  textures;
    Array<AImage> images; Array<ASampler>  samplers; Array<ACamera>     cameras; Array<AScene>    scenes;
    Array<ASkin>  skins; Array<AAnimation> animations;

    const char* curr = source;
    while (*curr)
    {
        // search for descriptor for example, accessors, materials, images, samplers
        AX_NO_UNROLL while (*curr && *curr != '"') curr++;
        
        if (*curr == '\0') break;

        curr++; // skips the "
        if      (StrCMP16(curr, "accessors"))    curr = ParseAccessors(curr, accessors);
        else if (StrCMP16(curr, "scenes"))       curr = ParseScenes(curr, scenes, stringAllocator, intAllocator);
        else if (StrCMP16(curr, "scene"))        result->defaultSceneIndex = ParsePositiveNumber(curr);
        else if (StrCMP16(curr, "bufferViews"))  curr = ParseBufferViews(curr, bufferViews);
        else if (StrCMP16(curr, "buffers"))      curr = ParseBuffers(curr, path, buffers);     
        else if (StrCMP16(curr, "images"))       curr = ParseImages(curr, path, images, stringAllocator);       
        else if (StrCMP16(curr, "textures"))     curr = ParseTextures(curr, textures, stringAllocator);   
        else if (StrCMP16(curr, "meshes"))       curr = ParseMeshes(curr, meshes, stringAllocator);
        else if (StrCMP16(curr, "materials"))    curr = ParseMaterials(curr, materials, stringAllocator);
        else if (StrCMP16(curr, "nodes"))        curr = ParseNodes(curr, nodes, stringAllocator, intAllocator, scale);
        else if (StrCMP16(curr, "samplers"))     curr = ParseSamplers(curr, samplers);    
        else if (StrCMP16(curr, "cameras"))      curr = ParseCameras(curr, cameras, stringAllocator); 
        else if (StrCMP16(curr, "skins"))        curr = ParseSkins(curr, skins, stringAllocator, intAllocator); 
        else if (StrCMP16(curr, "animations"))   curr = ParseAnimations(curr, animations, stringAllocator, intAllocator); 
        else if (StrCMP16(curr, "asset"))        curr = SkipToNextNode(curr, '{', '}'); // it just has text data that doesn't have anything to do with meshes, (author etc..) if you want you can add this feature :)
        else if (StrCMP16(curr, "extensionsUsed") || StrCMP16(curr, "extensionsRequ")) curr = SkipToNextNode(curr, '[', ']');
        else { ASSERT(0); curr = (const char*)AError_UNKNOWN_DESCRIPTOR; }

        if (curr < (const char*)AError_MAX) // is failed?
        {
            result->error = (AErrorType)(uint64_t)curr;
            FreeAllText(source);
            return 0;
        }
    }

    for (int m = 0; m < meshes.Size(); ++m)
    {
        // get number of vertex, getting first attribute count because all of the others are same
        AMesh mesh = meshes[m];
        mesh.numPrimitives = SBCount(mesh.primitives);
        for (int p = 0; p < mesh.numPrimitives; p++)
        {
            APrimitive& primitive = mesh.primitives[p];
            // get position attrib's count because all attributes same
            int numVertex = accessors[(int)(size_t)primitive.vertexAttribs[0]].count; 
            primitive.numVertices = numVertex;
        
            // get number of index
            GLTFAccessor accessor = accessors[primitive.indiceIndex];
            primitive.numIndices = accessor.count;

            accessor = accessors[primitive.indiceIndex];
            GLTFBufferView view = bufferViews[accessor.bufferView];
            int64_t offset = (int64_t)accessor.byteOffset + view.byteOffset;
            // copy indices
            primitive.indices = ((char*)buffers[view.buffer].uri) + offset;
            primitive.indexType = accessor.componentType;
            
            // get joint data that we need for creating vertices
            const int jointIndex = TrailingZeroCount32(AAttribType_JOINTS);
            accessor = accessors[(int)(size_t)primitive.vertexAttribs[jointIndex]];
            primitive.jointType   = (short)accessor.componentType;
            primitive.jointCount  = (short)accessor.type;
            primitive.jointStride = (short)bufferViews[accessor.bufferView].byteStride;

            // get weight data that we need for creating vertices
            const int weightIndex = TrailingZeroCount32(AAttribType_WEIGHTS);
            accessor = accessors[(int)(size_t)primitive.vertexAttribs[weightIndex]];
            primitive.weightType   = (short)accessor.componentType;
            primitive.weightStride = (short)bufferViews[accessor.bufferView].byteStride;

            // position, normal, texcoord are different buffers, 
            // we are unifying all attributes to Vertex* buffer here
            // even though attrib definition in gltf is not ordered, this code will order it, because we traversing set bits
            // for example it converts from this: TexCoord, Normal, Position to Position, Normal, TexCoord
            unsigned attributes = primitive.attributes;
            for (int j = 0; attributes > 0 && j < AAttribType_Count; j += NextSetBit(&attributes))
            {
                accessor     = accessors[(int)(size_t)primitive.vertexAttribs[j]];
                view         = bufferViews[accessor.bufferView];
                offset       = int64_t(accessor.byteOffset) + view.byteOffset;
                
                primitive.vertexAttribs[j] = (char*)buffers[view.buffer].uri + offset;
            }
        }
    }

    for (int s = 0; s < skins.Size(); s++)
    {
        ASkin& skin = skins[s];
        size_t skinIndex = (size_t)skin.inverseBindMatrices;
        GLTFAccessor   accessor  = accessors[(int)skinIndex];
        GLTFBufferView view      = bufferViews[accessor.bufferView];
        int64_t        offset    = int64_t(accessor.byteOffset) + view.byteOffset;
        skin.inverseBindMatrices = (float*)((char*)buffers[view.buffer].uri + offset);
    }

    for (int a = 0; a < animations.Size(); a++)
    {
        AAnimation& animation = animations[a];
        animation.duration = 0.0f;

        for (int s = 0; s < animation.numSamplers; s++)
        {
            AAnimSampler& sampler = animation.samplers[s];
            size_t inputIndex = (size_t)sampler.input;
            GLTFAccessor   accessor  = accessors[(int)inputIndex];
            GLTFBufferView view      = bufferViews[accessor.bufferView];
            int64_t        offset    = int64_t(accessor.byteOffset) + view.byteOffset;
            
            sampler.input = (float*)((char*)buffers[view.buffer].uri + offset);
            sampler.count = accessor.count;
            
            size_t outputIndex = (size_t)sampler.output;
            accessor = accessors[(int)outputIndex];
            view     = bufferViews[accessor.bufferView];
            offset   = int64_t(accessor.byteOffset) + view.byteOffset;
            
            sampler.output = (float*)((char*)buffers[view.buffer].uri + offset);
            sampler.count = MIN(sampler.count, accessor.count);
            sampler.numComponent = accessor.type;
            
            animation.duration = MAX(animation.duration, sampler.input[sampler.count - 1]);
        }
    }

    // calculate num vertices and indices
    {
        int totalVertexCount = 0;
        int totalIndexCount = 0;
        // Calculate total vertices, and indices
        for (int m = 0; m < meshes.Size(); ++m)
        {
            AMesh mesh = meshes[m];
            for (int p = 0; p < mesh.numPrimitives; p++)
            {
                APrimitive& primitive = mesh.primitives[p];
                totalIndexCount  += primitive.numIndices;
                totalVertexCount += primitive.numVertices;
            }
        }

        result->totalIndices  = totalIndexCount;
        result->totalVertices = totalVertexCount;
    }

    result->stringAllocator = stringAllocator.TakeOwnership();
    result->intAllocator    = intAllocator.TakeOwnership();

    result->numMeshes     = meshes.Size();     result->meshes     = meshes.TakeOwnership();
    result->numNodes      = nodes.Size();      result->nodes      = nodes.TakeOwnership();
    result->numMaterials  = materials.Size();  result->materials  = materials.TakeOwnership();
    result->numTextures   = textures.Size();   result->textures   = textures.TakeOwnership(); 
    result->numImages     = images.Size();     result->images     = images.TakeOwnership();
    result->numSamplers   = samplers.Size();   result->samplers   = samplers.TakeOwnership();
    result->numCameras    = cameras.Size();    result->cameras    = cameras.TakeOwnership();
    result->numScenes     = scenes.Size();     result->scenes     = scenes.TakeOwnership();
    result->numBuffers    = buffers.Size();    result->buffers    = buffers.TakeOwnership();
    result->numAnimations = animations.Size(); result->animations = animations.TakeOwnership();
    result->numSkins      = skins.Size();      result->skins      = skins.TakeOwnership();
    result->scale = scale;
    result->error = AError_NONE;
    FreeAllText(source);
    return 1;
}

__public void FreeSceneBundleBuffers(SceneBundle* gltf)
{
    for (int i = 0; i < gltf->numBuffers; i++)
    {
        FreeAllText((char*)gltf->buffers[i].uri);
        gltf->buffers[i].uri = nullptr;
    }
    FreeAligned(gltf->buffers);
    gltf->numBuffers = 0;
    gltf->buffers = nullptr;
}

__public void FreeSceneBundle(SceneBundle* gltf)
{
    struct CharFragment { 
        CharFragment* next; char* ptr; int64_t   size;
    };

    struct IntFragment { 
        IntFragment* next; int* ptr; int64_t   size;
    };

    for (int i = 0; i < gltf->numBuffers; i++)
    {
        FreeAllText((char*)gltf->buffers[i].uri);
    }

    if (gltf->stringAllocator)
    {
        // free allocators
        CharFragment* base = (CharFragment*)gltf->stringAllocator;

        while (base)
        {
            FreeAligned(base->ptr);
            CharFragment* oldBase = base;
            base = base->next;
            FreeAligned(oldBase);
        }
    }
    
    if (gltf->intAllocator)
    {
        IntFragment* ibase = (IntFragment*)gltf->intAllocator;

        while (ibase )
        {
            FreeAligned(ibase->ptr);
            IntFragment* oldBase = ibase ;
            ibase  = ibase->next;
            FreeAligned(oldBase);
        }
    }

    // also controls if arrays null or not
    for (int i = 0; i < gltf->numMeshes; i++)
        SBFree(gltf->meshes[i].primitives);

    if (gltf->meshes)      FreeAligned(gltf->meshes);
    if (gltf->nodes)       FreeAligned(gltf->nodes);
    if (gltf->materials)   FreeAligned(gltf->materials);
    if (gltf->textures)    FreeAligned(gltf->textures);
    if (gltf->images)      FreeAligned(gltf->images);
    if (gltf->samplers)    FreeAligned(gltf->samplers);
    if (gltf->cameras)     FreeAligned(gltf->cameras);
    if (gltf->scenes)      FreeAligned(gltf->scenes);
    if (gltf->skins)       FreeAligned(gltf->skins);
    if (gltf->animations)
    {
        for (int i = 0; i < gltf->numAnimations; i++)
        {
            FreeAligned(gltf->animations[i].samplers);
            FreeAligned(gltf->animations[i].channels);
        }
        FreeAligned(gltf->animations);
    }
    if (gltf->allVertices) FreeAligned(gltf->allVertices);
    if (gltf->allIndices)  FreeAligned(gltf->allIndices);
    
    MemsetZero(gltf, sizeof(SceneBundle));
}

const char* ParsedSceneGetError(AErrorType error)
{
    const char* SceneParseErrorToStr[] = {"NONE", 
                                          "UNKNOWN",
                                          "UNKNOWN_ATTRIB",
                                          "UNKNOWN_MATERIAL_VAR",
                                          "UNKNOWN_PBR_VAR",
                                          "UNKNOWN_NODE_VAR",
                                          "UNKNOWN_TEXTURE_VAR",
                                          "UNKNOWN_ACCESSOR_VAR",
                                          "UNKNOWN_BUFFER_VIEW_VAR",
                                          "UNKNOWN_MESH_VAR",
                                          "UNKNOWN_CAMERA_VAR",
                                          "UNKNOWN_MESH_PRIMITIVE_VAR",
                                          "BUFFER_PARSE_FAIL",
                                          "BIN_NOT_EXIST",
                                          "FILE_NOT_FOUND",
                                          "UNKNOWN_DESCRIPTOR",
                                          "HASH_COLISSION",
                                          "NON_UTF8",
                                          "MAX" };
    return SceneParseErrorToStr[error];
}
#ifndef __cplusplus
} // extern C
#endif
