#define ZIP_TRUE       1
#define ZIP_FALSE      0
#ifndef EOPNOTSUPP
 #define EOPNOTSUPP    95
#endif
#ifndef EOVERFLOW
 #define EOVERFLOW     139
#endif

/* miniz.c v1.15 - public domain deflate/inflate, zlib-subset, ZIP reading/writing/appending, PNG writing
   See "unlicense" statement at the end of this file.
   Rich Geldreich <richgel99@gmail.com>, last updated Oct. 13, 2013
   Implements RFC 1950: http://www.ietf.org/rfc/rfc1950.txt and RFC 1951: http://www.ietf.org/rfc/rfc1951.txt

   Most API's defined in miniz.c are optional. For example, to disable the archive related functions just define
   MINIZ_NO_ARCHIVE_APIS, or to get rid of all stdio usage define MINIZ_NO_STDIO (see the list below for more macros).

 * Change History
     10/13/13 v1.15 r4 - Interim bugfix release while I work on the next major release with Zip64 support (almost there!):
       - Critical fix for the MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY bug (thanks kahmyong.moon@hp.com) which could cause locate files to not find files. This bug
        would only have occured in earlier versions if you explicitly used this flag, OR if you used mz_zip_extract_archive_file_to_heap() or mz_zip_add_mem_to_archive_file_in_place()
        (which used this flag). If you can't switch to v1.15 but want to fix this bug, just remove the uses of this flag from both helper funcs (and of course don't use the flag).
       - Bugfix in mz_zip_reader_extract_to_mem_no_alloc() from kymoon when pUser_read_buf is not NULL and compressed size is > uncompressed size
       - Fixing mz_zip_reader_extract_*() funcs so they don't try to extract compressed data from directory entries, to account for weird zipfiles which contain zero-size compressed data on dir entries.
         Hopefully this fix won't cause any issues on weird zip archives, because it assumes the low 16-bits of zip external attributes are DOS attributes (which I believe they always are in practice).
       - Fixing mz_zip_reader_is_file_a_directory() so it doesn't check the internal attributes, just the filename and external attributes
       - mz_zip_reader_init_file() - missing MZ_FCLOSE() call if the seek failed
       - Added cmake support for Linux builds which builds all the examples, tested with clang v3.3 and gcc v4.6.
       - Clang fix for tdefl_write_image_to_png_file_in_memory() from toffaletti
       - Merged MZ_FORCEINLINE fix from hdeanclark
       - Fix <time.h> include before config #ifdef, thanks emil.brink
       - Added tdefl_write_image_to_png_file_in_memory_ex(): supports Y flipping (super useful for OpenGL apps), and explicit control over the compression level (so you can
        set it to 1 for real-time compression).
       - Merged in some compiler fixes from paulharris's github repro.
       - Retested this build under Windows (VS 2010, including static analysis), tcc  0.9.26, gcc v4.6 and clang v3.3.
       - Added example6.c, which dumps an image of the mandelbrot set to a PNG file.
       - Modified example2 to help test the MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY flag more.
       - In r3: Bugfix to mz_zip_writer_add_file() found during merge: Fix possible src file fclose() leak if alignment bytes+local header file write faiiled
                 - In r4: Minor bugfix to mz_zip_writer_add_from_zip_reader(): Was pushing the wrong central dir header offset, appears harmless in this release, but it became a problem in the zip64 branch
     5/20/12 v1.14 - MinGW32/64 GCC 4.6.1 compiler fixes: added MZ_FORCEINLINE, #include <time.h> (thanks fermtect).
     5/19/12 v1.13 - From jason@cornsyrup.org and kelwert@mtu.edu - Fix mz_crc32() so it doesn't compute the wrong CRC-32's when mz_ulong is 64-bit.
       - Temporarily/locally slammed in "typedef unsigned long mz_ulong" and re-ran a randomized regression test on ~500k files.
       - Eliminated a bunch of warnings when compiling with GCC 32-bit/64.
       - Ran all examples, miniz.c, and tinfl.c through MSVC 2008's /analyze (static analysis) option and fixed all warnings (except for the silly
        "Use of the comma-operator in a tested expression.." analysis warning, which I purposely use to work around a MSVC compiler warning).
       - Created 32-bit and 64-bit Codeblocks projects/workspace. Built and tested Linux executables. The codeblocks workspace is compatible with Linux+Win32/x64.
       - Added miniz_tester solution/project, which is a useful little app derived from LZHAM's tester app that I use as part of the regression test.
       - Ran miniz.c and tinfl.c through another series of regression testing on ~500,000 files and archives.
       - Modified example5.c so it purposely disables a bunch of high-level functionality (MINIZ_NO_STDIO, etc.). (Thanks to corysama for the MINIZ_NO_STDIO bug report.)
       - Fix ftell() usage in examples so they exit with an error on files which are too large (a limitation of the examples, not miniz itself).
     4/12/12 v1.12 - More comments, added low-level example5.c, fixed a couple minor level_and_flags issues in the archive API's.
      level_and_flags can now be set to MZ_DEFAULT_COMPRESSION. Thanks to Bruce Dawson <bruced@valvesoftware.com> for the feedback/bug report.
     5/28/11 v1.11 - Added statement from unlicense.org
     5/27/11 v1.10 - Substantial compressor optimizations:
      - Level 1 is now ~4x faster than before. The L1 compressor's throughput now varies between 70-110MB/sec. on a
      - Core i7 (actual throughput varies depending on the type of data, and x64 vs. x86).
      - Improved baseline L2-L9 compression perf. Also, greatly improved compression perf. issues on some file types.
      - Refactored the compression code for better readability and maintainability.
      - Added level 10 compression level (L10 has slightly better ratio than level 9, but could have a potentially large
       drop in throughput on some files).
     5/15/11 v1.09 - Initial stable release.

 * Low-level Deflate/Inflate implementation notes:

     Compression: Use the "tdefl" API's. The compressor supports raw, static, and dynamic blocks, lazy or
     greedy parsing, match length filtering, RLE-only, and Huffman-only streams. It performs and compresses
     approximately as well as zlib.

     Decompression: Use the "tinfl" API's. The entire decompressor is implemented as a single function
     coroutine: see tinfl_decompress(). It supports decompression into a 32KB (or larger power of 2) wrapping buffer, or into a memory
     block large enough to hold the entire file.

     The low-level tdefl/tinfl API's do not make any use of dynamic memory allocation.

 * zlib-style API notes:

     miniz.c implements a fairly large subset of zlib. There's enough functionality present for it to be a drop-in
     zlib replacement in many apps:
        The z_stream struct, optional memory allocation callbacks
        deflateInit/deflateInit2/deflate/deflateReset/deflateEnd/deflateBound
        inflateInit/inflateInit2/inflate/inflateEnd
        compress, compress2, compressBound, uncompress
        CRC-32, Adler-32 - Using modern, minimal code size, CPU cache friendly routines.
        Supports raw deflate streams or standard zlib streams with adler-32 checking.

     Limitations:
      The callback API's are not implemented yet. No support for gzip headers or zlib static dictionaries.
      I've tried to closely emulate zlib's various flavors of stream flushing and return status codes, but
      there are no guarantees that miniz.c pulls this off perfectly.

 * PNG writing: See the tdefl_write_image_to_png_file_in_memory() function, originally written by
     Alex Evans. Supports 1-4 bytes/pixel images.

 * ZIP archive API notes:

     The ZIP archive API's where designed with simplicity and efficiency in mind, with just enough abstraction to
     get the job done with minimal fuss. There are simple API's to retrieve file information, read files from
     existing archives, create new archives, append new files to existing archives, or clone archive data from
     one archive to another. It supports archives located in memory or the heap, on disk (using stdio.h),
     or you can specify custom file read/write callbacks.

     - Archive reading: Just call this function to read a single file from a disk archive:

      void *mz_zip_extract_archive_file_to_heap(const char *pZip_filename, const char *pArchive_name,
        size_t *pSize, mz_uint zip_flags);

     For more complex cases, use the "mz_zip_reader" functions. Upon opening an archive, the entire central
     directory is located and read as-is into memory, and subsequent file access only occurs when reading individual files.

     - Archives file scanning: The simple way is to use this function to scan a loaded archive for a specific file:

     int mz_zip_reader_locate_file(mz_zip_archive *pZip, const char *pName, const char *pComment, mz_uint flags);

     The locate operation can optionally check file comments too, which (as one example) can be used to identify
     multiple versions of the same file in an archive. This function uses a simple linear search through the central
     directory, so it's not very fast.

     Alternately, you can iterate through all the files in an archive (using mz_zip_reader_get_num_files()) and
     retrieve detailed info on each file by calling mz_zip_reader_file_stat().

     - Archive creation: Use the "mz_zip_writer" functions. The ZIP writer immediately writes compressed file data
     to disk and builds an exact image of the central directory in memory. The central directory image is written
     all at once at the end of the archive file when the archive is finalized.

     The archive writer can optionally align each file's local header and file data to any power of 2 alignment,
     which can be useful when the archive will be read from optical media. Also, the writer supports placing
     arbitrary data blobs at the very beginning of ZIP archives. Archives written using either feature are still
     readable by any ZIP tool.

     - Archive appending: The simple way to add a single file to an archive is to call this function:

      mz_bool mz_zip_add_mem_to_archive_file_in_place(const char *pZip_filename, const char *pArchive_name,
        const void *pBuf, size_t buf_size, const void *pComment, mz_uint16 comment_size, mz_uint level_and_flags);

     The archive will be created if it doesn't already exist, otherwise it'll be appended to.
     Note the appending is done in-place and is not an atomic operation, so if something goes wrong
     during the operation it's possible the archive could be left without a central directory (although the local
     file headers and file data will be fine, so the archive will be recoverable).

     For more complex archive modification scenarios:
     1. The safest way is to use a mz_zip_reader to read the existing archive, cloning only those bits you want to
     preserve into a new archive using using the mz_zip_writer_add_from_zip_reader() function (which compiles the
     compressed file data as-is). When you're done, delete the old archive and rename the newly written archive, and
     you're done. This is safe but requires a bunch of temporary disk space or heap memory.

     2. Or, you can convert an mz_zip_reader in-place to an mz_zip_writer using mz_zip_writer_init_from_reader(),
     append new files as needed, then finalize the archive which will write an updated central directory to the
     original archive. (This is basically what mz_zip_add_mem_to_archive_file_in_place() does.) There's a
     possibility that the archive's central directory could be lost with this method if anything goes wrong, though.

     - ZIP archive support limitations:
     No zip64 or spanning support. Extraction functions can only handle unencrypted, stored or deflated files.
     Requires streams capable of seeking.

 * This is a header file library, like stb_image.c. To get only a header file, either cut and paste the
     below header, or create miniz.h, #define MINIZ_HEADER_FILE_ONLY, and then include miniz.c from it.

 * Important: For best perf. be sure to customize the below macros for your target platform:
 **#define MINIZ_USE_UNALIGNED_LOADS_AND_STORES 1
 **#define MINIZ_LITTLE_ENDIAN 1
 **#define MINIZ_HAS_64BIT_REGISTERS 1

 * On platforms using glibc, Be sure to "#define _LARGEFILE64_SOURCE 1" before including miniz.c to ensure miniz
     uses the 64-bit variants: fopen64(), stat64(), etc. Otherwise you won't be able to process large files
     (i.e. 32-bit stat() fails for me on files > 0x7FFFFFFF bytes).
 */

#ifndef MINIZ_HEADER_INCLUDED
 #define MINIZ_HEADER_INCLUDED

 #include <stdlib.h>

// Defines to completely disable specific portions of miniz.c:
// If all macros here are defined the only functionality remaining will be CRC-32, adler-32, tinfl, and tdefl.

// Define MINIZ_NO_STDIO to disable all usage and any functions which rely on stdio for file I/O.
//#define MINIZ_NO_STDIO

// If MINIZ_NO_TIME is specified then the ZIP archive functions will not be able to get the current time, or
// get/set file times, and the C run-time funcs that get/set times won't be called.
// The current downside is the times written to your archives will be from 1979.
//#define MINIZ_NO_TIME

// Define MINIZ_NO_ARCHIVE_APIS to disable all ZIP archive API's.
//#define MINIZ_NO_ARCHIVE_APIS

// Define MINIZ_NO_ARCHIVE_APIS to disable all writing related ZIP archive API's.
//#define MINIZ_NO_ARCHIVE_WRITING_APIS

// Define MINIZ_NO_ZLIB_APIS to remove all ZLIB-style compression/decompression API's.
//#define MINIZ_NO_ZLIB_APIS

// Define MINIZ_NO_ZLIB_COMPATIBLE_NAME to disable zlib names, to prevent conflicts against stock zlib.
//#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES

// Define MINIZ_NO_MALLOC to disable all calls to malloc, free, and realloc.
// Note if MINIZ_NO_MALLOC is defined then the user must always provide custom user alloc/free/realloc
// callbacks to the zlib and archive API's, and a few stand-alone helper API's which don't provide custom user
// functions (such as tdefl_compress_mem_to_heap() and tinfl_decompress_mem_to_heap()) won't work.
//#define MINIZ_NO_MALLOC

 #if defined(__TINYC__) && (defined(__linux) || defined(__linux__))
// TODO: Work around "error: include file 'sys\utime.h' when compiling with tcc on Linux
  #define MINIZ_NO_TIME
 #endif

 #if !defined(MINIZ_NO_TIME) && !defined(MINIZ_NO_ARCHIVE_APIS)
  #include <time.h>
 #endif

 #if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__i386) || defined(__i486__) || defined(__i486) || defined(i386) || defined(__ia64__) || defined(__x86_64__)
// MINIZ_X86_OR_X64_CPU is only used to help set the below macros.
  #define MINIZ_X86_OR_X64_CPU    1
 #endif

 #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || MINIZ_X86_OR_X64_CPU
// Set MINIZ_LITTLE_ENDIAN to 1 if the processor is little endian.
  #define MINIZ_LITTLE_ENDIAN    1
 #endif

 #if MINIZ_X86_OR_X64_CPU
// Set MINIZ_USE_UNALIGNED_LOADS_AND_STORES to 1 on CPU's that permit efficient integer loads and stores from unaligned addresses.
  #define MINIZ_USE_UNALIGNED_LOADS_AND_STORES    1
 #endif

 #if defined(_M_X64) || defined(_WIN64) || defined(__MINGW64__) || defined(_LP64) || defined(__LP64__) || defined(__ia64__) || defined(__x86_64__)
// Set MINIZ_HAS_64BIT_REGISTERS to 1 if operations on 64-bit integers are reasonably fast (and don't involve compiler generated calls to helper functions).
  #define MINIZ_HAS_64BIT_REGISTERS    1
 #endif

 #ifdef __cplusplus
extern "C" {
 #endif

// ------------------- zlib-style API Definitions.

// For more compatibility with zlib, miniz.c uses unsigned long for some parameters/struct members. Beware: mz_ulong can be either 32 or 64-bits!
typedef unsigned long   mz_ulong;

// mz_free() internally uses the MZ_FREE() macro (which by default calls free() unless you've modified the MZ_MALLOC macro) to release a block allocated from the heap.
void mz_free(void *p);

 #define MZ_ADLER32_INIT    (1)
// mz_adler32() returns the initial adler-32 value to use when called with ptr==NULL.
mz_ulong mz_adler32(mz_ulong adler, const unsigned char *ptr, size_t buf_len);

 #define MZ_CRC32_INIT    (0)
// mz_crc32() returns the initial CRC-32 value to use when called with ptr==NULL.
mz_ulong mz_crc32(mz_ulong crc, const unsigned char *ptr, size_t buf_len);

// Compression strategies.
enum {
    MZ_DEFAULT_STRATEGY = 0, MZ_FILTERED = 1, MZ_HUFFMAN_ONLY = 2, MZ_RLE = 3, MZ_FIXED = 4
};

// Method
 #define MZ_DEFLATED    8

 #ifndef MINIZ_NO_ZLIB_APIS

// Heap allocation callbacks.
// Note that mz_alloc_func parameter types purpsosely differ from zlib's: items/size is size_t, not unsigned long.
typedef void *(*mz_alloc_func)(void *opaque, size_t items, size_t size);
typedef void (*mz_free_func)(void *opaque, void *address);
typedef void *(*mz_realloc_func)(void *opaque, void *address, size_t items, size_t size);

  #define MZ_VERSION            "9.1.15"
  #define MZ_VERNUM             0x91F0
  #define MZ_VER_MAJOR          9
  #define MZ_VER_MINOR          1
  #define MZ_VER_REVISION       15
  #define MZ_VER_SUBREVISION    0

// Flush values. For typical usage you only need MZ_NO_FLUSH and MZ_FINISH. The other values are for advanced use (refer to the zlib docs).
enum {
    MZ_NO_FLUSH = 0, MZ_PARTIAL_FLUSH = 1, MZ_SYNC_FLUSH = 2, MZ_FULL_FLUSH = 3, MZ_FINISH = 4, MZ_BLOCK = 5
};

// Return status codes. MZ_PARAM_ERROR is non-standard.
enum {
    MZ_OK = 0, MZ_STREAM_END = 1, MZ_NEED_DICT = 2, MZ_ERRNO = -1, MZ_STREAM_ERROR = -2, MZ_DATA_ERROR = -3, MZ_MEM_ERROR = -4, MZ_BUF_ERROR = -5, MZ_VERSION_ERROR = -6, MZ_PARAM_ERROR = -10000
};

// Compression levels: 0-9 are the standard zlib-style levels, 10 is best possible compression (not zlib compatible, and may be very slow), MZ_DEFAULT_COMPRESSION=MZ_DEFAULT_LEVEL.
enum {
    MZ_NO_COMPRESSION = 0, MZ_BEST_SPEED = 1, MZ_BEST_COMPRESSION = 9, MZ_UBER_COMPRESSION = 10, MZ_DEFAULT_LEVEL = 6, MZ_DEFAULT_COMPRESSION = -1
};

// Window bits
  #define MZ_DEFAULT_WINDOW_BITS    15

struct mz_internal_state;

// Compression/decompression stream struct.
typedef struct mz_stream_s {
    const unsigned char      *next_in;  // pointer to next byte to read
    unsigned int             avail_in;  // number of bytes available at next_in
    mz_ulong                 total_in;  // total number of bytes consumed so far

    unsigned char            *next_out; // pointer to next byte to write
    unsigned int             avail_out; // number of bytes that can be written to next_out
    mz_ulong                 total_out; // total number of bytes produced so far

    char                     *msg;      // error msg (unused)
    struct mz_internal_state *state;    // internal state, allocated by zalloc/zfree

    mz_alloc_func            zalloc;    // optional heap allocation function (defaults to malloc)
    mz_free_func             zfree;     // optional heap free function (defaults to free)
    void                     *opaque;   // heap alloc function user pointer

    int                      data_type; // data_type (unused)
    mz_ulong                 adler;     // adler32 of the source or uncompressed data
    mz_ulong                 reserved;  // not used
} mz_stream;

typedef mz_stream * mz_streamp;

// Returns the version string of miniz.c.
const char *mz_version(void);

// mz_deflateInit() initializes a compressor with default options:
// Parameters:
//  pStream must point to an initialized mz_stream struct.
//  level must be between [MZ_NO_COMPRESSION, MZ_BEST_COMPRESSION].
//  level 1 enables a specially optimized compression function that's been optimized purely for performance, not ratio.
//  (This special func. is currently only enabled when MINIZ_USE_UNALIGNED_LOADS_AND_STORES and MINIZ_LITTLE_ENDIAN are defined.)
// Return values:
//  MZ_OK on success.
//  MZ_STREAM_ERROR if the stream is bogus.
//  MZ_PARAM_ERROR if the input parameters are bogus.
//  MZ_MEM_ERROR on out of memory.
int mz_deflateInit(mz_streamp pStream, int level);

// mz_deflateInit2() is like mz_deflate(), except with more control:
// Additional parameters:
//   method must be MZ_DEFLATED
//   window_bits must be MZ_DEFAULT_WINDOW_BITS (to wrap the deflate stream with zlib header/adler-32 footer) or -MZ_DEFAULT_WINDOW_BITS (raw deflate/no header or footer)
//   mem_level must be between [1, 9] (it's checked but ignored by miniz.c)
int mz_deflateInit2(mz_streamp pStream, int level, int method, int window_bits, int mem_level, int strategy);

// Quickly resets a compressor without having to reallocate anything. Same as calling mz_deflateEnd() followed by mz_deflateInit()/mz_deflateInit2().
int mz_deflateReset(mz_streamp pStream);

// mz_deflate() compresses the input to output, consuming as much of the input and producing as much output as possible.
// Parameters:
//   pStream is the stream to read from and write to. You must initialize/update the next_in, avail_in, next_out, and avail_out members.
//   flush may be MZ_NO_FLUSH, MZ_PARTIAL_FLUSH/MZ_SYNC_FLUSH, MZ_FULL_FLUSH, or MZ_FINISH.
// Return values:
//   MZ_OK on success (when flushing, or if more input is needed but not available, and/or there's more output to be written but the output buffer is full).
//   MZ_STREAM_END if all input has been consumed and all output bytes have been written. Don't call mz_deflate() on the stream anymore.
//   MZ_STREAM_ERROR if the stream is bogus.
//   MZ_PARAM_ERROR if one of the parameters is invalid.
//   MZ_BUF_ERROR if no forward progress is possible because the input and/or output buffers are empty. (Fill up the input buffer or free up some output space and try again.)
int mz_deflate(mz_streamp pStream, int flush);

// mz_deflateEnd() deinitializes a compressor:
// Return values:
//  MZ_OK on success.
//  MZ_STREAM_ERROR if the stream is bogus.
int mz_deflateEnd(mz_streamp pStream);

// mz_deflateBound() returns a (very) conservative upper bound on the amount of data that could be generated by deflate(), assuming flush is set to only MZ_NO_FLUSH or MZ_FINISH.
mz_ulong mz_deflateBound(mz_streamp pStream, mz_ulong source_len);

// Single-call compression functions mz_compress() and mz_compress2():
// Returns MZ_OK on success, or one of the error codes from mz_deflate() on failure.
int mz_compress(unsigned char *pDest, mz_ulong *pDest_len, const unsigned char *pSource, mz_ulong source_len);
int mz_compress2(unsigned char *pDest, mz_ulong *pDest_len, const unsigned char *pSource, mz_ulong source_len, int level);

// mz_compressBound() returns a (very) conservative upper bound on the amount of data that could be generated by calling mz_compress().
mz_ulong mz_compressBound(mz_ulong source_len);

// Initializes a decompressor.
int mz_inflateInit(mz_streamp pStream);

// mz_inflateInit2() is like mz_inflateInit() with an additional option that controls the window size and whether or not the stream has been wrapped with a zlib header/footer:
// window_bits must be MZ_DEFAULT_WINDOW_BITS (to parse zlib header/footer) or -MZ_DEFAULT_WINDOW_BITS (raw deflate).
int mz_inflateInit2(mz_streamp pStream, int window_bits);

// Decompresses the input stream to the output, consuming only as much of the input as needed, and writing as much to the output as possible.
// Parameters:
//   pStream is the stream to read from and write to. You must initialize/update the next_in, avail_in, next_out, and avail_out members.
//   flush may be MZ_NO_FLUSH, MZ_SYNC_FLUSH, or MZ_FINISH.
//   On the first call, if flush is MZ_FINISH it's assumed the input and output buffers are both sized large enough to decompress the entire stream in a single call (this is slightly faster).
//   MZ_FINISH implies that there are no more source bytes available beside what's already in the input buffer, and that the output buffer is large enough to hold the rest of the decompressed data.
// Return values:
//   MZ_OK on success. Either more input is needed but not available, and/or there's more output to be written but the output buffer is full.
//   MZ_STREAM_END if all needed input has been consumed and all output bytes have been written. For zlib streams, the adler-32 of the decompressed data has also been verified.
//   MZ_STREAM_ERROR if the stream is bogus.
//   MZ_DATA_ERROR if the deflate stream is invalid.
//   MZ_PARAM_ERROR if one of the parameters is invalid.
//   MZ_BUF_ERROR if no forward progress is possible because the input buffer is empty but the inflater needs more input to continue, or if the output buffer is not large enough. Call mz_inflate() again
//   with more input data, or with more room in the output buffer (except when using single call decompression, described above).
int mz_inflate(mz_streamp pStream, int flush);

// Deinitializes a decompressor.
int mz_inflateEnd(mz_streamp pStream);

// Single-call decompression.
// Returns MZ_OK on success, or one of the error codes from mz_inflate() on failure.
int mz_uncompress(unsigned char *pDest, mz_ulong *pDest_len, const unsigned char *pSource, mz_ulong source_len);

// Returns a string description of the specified error code, or NULL if the error code is invalid.
const char *mz_error(int err);

// Redefine zlib-compatible names to miniz equivalents, so miniz.c can be used as a drop-in replacement for the subset of zlib that miniz.c supports.
// Define MINIZ_NO_ZLIB_COMPATIBLE_NAMES to disable zlib-compatibility if you use zlib in the same project.
  #ifndef MINIZ_NO_ZLIB_COMPATIBLE_NAMES
typedef unsigned char   Byte;
typedef unsigned int    uInt;
typedef mz_ulong        uLong;
typedef Byte            Bytef;
typedef uInt            uIntf;
typedef char            charf;
typedef int             intf;
typedef void *          voidpf;
typedef uLong           uLongf;
typedef void *          voidp;
typedef void *const     voidpc;
   #define Z_NULL                   0
   #define Z_NO_FLUSH               MZ_NO_FLUSH
   #define Z_PARTIAL_FLUSH          MZ_PARTIAL_FLUSH
   #define Z_SYNC_FLUSH             MZ_SYNC_FLUSH
   #define Z_FULL_FLUSH             MZ_FULL_FLUSH
   #define Z_FINISH                 MZ_FINISH
   #define Z_BLOCK                  MZ_BLOCK
   #define Z_OK                     MZ_OK
   #define Z_STREAM_END             MZ_STREAM_END
   #define Z_NEED_DICT              MZ_NEED_DICT
   #define Z_ERRNO                  MZ_ERRNO
   #define Z_STREAM_ERROR           MZ_STREAM_ERROR
   #define Z_DATA_ERROR             MZ_DATA_ERROR
   #define Z_MEM_ERROR              MZ_MEM_ERROR
   #define Z_BUF_ERROR              MZ_BUF_ERROR
   #define Z_VERSION_ERROR          MZ_VERSION_ERROR
   #define Z_PARAM_ERROR            MZ_PARAM_ERROR
   #define Z_NO_COMPRESSION         MZ_NO_COMPRESSION
   #define Z_BEST_SPEED             MZ_BEST_SPEED
   #define Z_BEST_COMPRESSION       MZ_BEST_COMPRESSION
   #define Z_DEFAULT_COMPRESSION    MZ_DEFAULT_COMPRESSION
   #define Z_DEFAULT_STRATEGY       MZ_DEFAULT_STRATEGY
   #define Z_FILTERED               MZ_FILTERED
   #define Z_HUFFMAN_ONLY           MZ_HUFFMAN_ONLY
   #define Z_RLE                    MZ_RLE
   #define Z_FIXED                  MZ_FIXED
   #define Z_DEFLATED               MZ_DEFLATED
   #define Z_DEFAULT_WINDOW_BITS    MZ_DEFAULT_WINDOW_BITS
   #define alloc_func               mz_alloc_func
   #define free_func                mz_free_func
   #define internal_state           mz_internal_state
   #define z_stream                 mz_stream
   #define deflateInit              mz_deflateInit
   #define deflateInit2             mz_deflateInit2
   #define deflateReset             mz_deflateReset
   #define deflate                  mz_deflate
   #define deflateEnd               mz_deflateEnd
   #define deflateBound             mz_deflateBound
   #define compress                 mz_compress
   #define compress2                mz_compress2
   #define compressBound            mz_compressBound
   #define inflateInit              mz_inflateInit
   #define inflateInit2             mz_inflateInit2
   #define inflate                  mz_inflate
   #define inflateEnd               mz_inflateEnd
   #define uncompress               mz_uncompress
   #define crc32                    mz_crc32
   #define adler32                  mz_adler32
   #define MAX_WBITS                15
   #define MAX_MEM_LEVEL            9
   #define zError                   mz_error
   #define ZLIB_VERSION             MZ_VERSION
   #define ZLIB_VERNUM              MZ_VERNUM
   #define ZLIB_VER_MAJOR           MZ_VER_MAJOR
   #define ZLIB_VER_MINOR           MZ_VER_MINOR
   #define ZLIB_VER_REVISION        MZ_VER_REVISION
   #define ZLIB_VER_SUBREVISION     MZ_VER_SUBREVISION
   #define zlibVersion              mz_version
   #define zlib_version             mz_version()
  #endif // #ifndef MINIZ_NO_ZLIB_COMPATIBLE_NAMES
 #endif  // MINIZ_NO_ZLIB_APIS

// ------------------- Types and macros

typedef unsigned char        mz_uint8;
typedef signed short         mz_int16;
typedef unsigned short       mz_uint16;
typedef unsigned int         mz_uint32;
typedef unsigned int         mz_uint;
typedef long long            mz_int64;
typedef unsigned long long   mz_uint64;
typedef int                  mz_bool;

 #define MZ_FALSE    (0)
 #define MZ_TRUE     (1)

// An attempt to work around MSVC's spammy "warning C4127: conditional expression is constant" message.
 #ifdef _MSC_VER
  #define MZ_MACRO_END    while (0, 0)
 #else
  #define MZ_MACRO_END    while (0)
 #endif

// ------------------- ZIP archive reading/writing

 #ifndef MINIZ_NO_ARCHIVE_APIS

enum {
    MZ_ZIP_MAX_IO_BUF_SIZE               = 64 * 1024,
    MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE     = 260,
    MZ_ZIP_MAX_ARCHIVE_FILE_COMMENT_SIZE = 256
};

typedef struct {
    mz_uint32 m_file_index;
    mz_uint32 m_central_dir_ofs;
    mz_uint16 m_version_made_by;
    mz_uint16 m_version_needed;
    mz_uint16 m_bit_flag;
    mz_uint16 m_method;
  #ifndef MINIZ_NO_TIME
    time_t    m_time;
  #endif
    mz_uint32 m_crc32;
    mz_uint64 m_comp_size;
    mz_uint64 m_uncomp_size;
    mz_uint16 m_internal_attr;
    mz_uint32 m_external_attr;
    mz_uint64 m_local_header_ofs;
    mz_uint32 m_comment_size;
    char      m_filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
    char      m_comment[MZ_ZIP_MAX_ARCHIVE_FILE_COMMENT_SIZE];
} mz_zip_archive_file_stat;

typedef size_t (*mz_file_read_func)(void *pOpaque, mz_uint64 file_ofs, void *pBuf, size_t n);
typedef size_t (*mz_file_write_func)(void *pOpaque, mz_uint64 file_ofs, const void *pBuf, size_t n);

struct mz_zip_internal_state_tag;
typedef struct mz_zip_internal_state_tag   mz_zip_internal_state;

typedef enum {
    MZ_ZIP_MODE_INVALID                    = 0,
    MZ_ZIP_MODE_READING                    = 1,
    MZ_ZIP_MODE_WRITING                    = 2,
    MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED = 3
} mz_zip_mode;

typedef struct mz_zip_archive_tag {
    mz_uint64             m_archive_size;
    mz_uint64             m_central_directory_file_ofs;
    mz_uint               m_total_files;
    mz_zip_mode           m_zip_mode;

    mz_uint               m_file_offset_alignment;

    mz_alloc_func         m_pAlloc;
    mz_free_func          m_pFree;
    mz_realloc_func       m_pRealloc;
    void                  *m_pAlloc_opaque;

    mz_file_read_func     m_pRead;
    mz_file_write_func    m_pWrite;
    void                  *m_pIO_opaque;

    mz_zip_internal_state *m_pState;
} mz_zip_archive;

typedef enum {
    MZ_ZIP_FLAG_CASE_SENSITIVE                = 0x0100,
    MZ_ZIP_FLAG_IGNORE_PATH                   = 0x0200,
    MZ_ZIP_FLAG_COMPRESSED_DATA               = 0x0400,
    MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY = 0x0800
} mz_zip_flags;

// ZIP archive reading

// Inits a ZIP archive reader.
// These functions read and validate the archive's central directory.
mz_bool mz_zip_reader_init(mz_zip_archive *pZip, mz_uint64 size, mz_uint32 flags);
mz_bool mz_zip_reader_init_mem(mz_zip_archive *pZip, const void *pMem, size_t size, mz_uint32 flags);

  #ifndef MINIZ_NO_STDIO
mz_bool mz_zip_reader_init_file(mz_zip_archive *pZip, const char *pFilename, mz_uint32 flags);
  #endif

// Returns the total number of files in the archive.
mz_uint mz_zip_reader_get_num_files(mz_zip_archive *pZip);

// Returns detailed information about an archive file entry.
mz_bool mz_zip_reader_file_stat(mz_zip_archive *pZip, mz_uint file_index, mz_zip_archive_file_stat *pStat);

// Determines if an archive file entry is a directory entry.
mz_bool mz_zip_reader_is_file_a_directory(mz_zip_archive *pZip, mz_uint file_index);
mz_bool mz_zip_reader_is_file_encrypted(mz_zip_archive *pZip, mz_uint file_index);

// Retrieves the filename of an archive file entry.
// Returns the number of bytes written to pFilename, or if filename_buf_size is 0 this function returns the number of bytes needed to fully store the filename.
mz_uint mz_zip_reader_get_filename(mz_zip_archive *pZip, mz_uint file_index, char *pFilename, mz_uint filename_buf_size);

// Attempts to locates a file in the archive's central directory.
// Valid flags: MZ_ZIP_FLAG_CASE_SENSITIVE, MZ_ZIP_FLAG_IGNORE_PATH
// Returns -1 if the file cannot be found.
int mz_zip_reader_locate_file(mz_zip_archive *pZip, const char *pName, const char *pComment, mz_uint flags);

// Extracts a archive file to a memory buffer using no memory allocation.
mz_bool mz_zip_reader_extract_to_mem_no_alloc(mz_zip_archive *pZip, mz_uint file_index, void *pBuf, size_t buf_size, mz_uint flags, void *pUser_read_buf, size_t user_read_buf_size);
mz_bool mz_zip_reader_extract_file_to_mem_no_alloc(mz_zip_archive *pZip, const char *pFilename, void *pBuf, size_t buf_size, mz_uint flags, void *pUser_read_buf, size_t user_read_buf_size);

// Extracts a archive file to a memory buffer.
mz_bool mz_zip_reader_extract_to_mem(mz_zip_archive *pZip, mz_uint file_index, void *pBuf, size_t buf_size, mz_uint flags);
mz_bool mz_zip_reader_extract_file_to_mem(mz_zip_archive *pZip, const char *pFilename, void *pBuf, size_t buf_size, mz_uint flags);

// Extracts a archive file to a dynamically allocated heap buffer.
void *mz_zip_reader_extract_to_heap(mz_zip_archive *pZip, mz_uint file_index, size_t *pSize, mz_uint flags);
void *mz_zip_reader_extract_file_to_heap(mz_zip_archive *pZip, const char *pFilename, size_t *pSize, mz_uint flags);

// Extracts a archive file using a callback function to output the file's data.
mz_bool mz_zip_reader_extract_to_callback(mz_zip_archive *pZip, mz_uint file_index, mz_file_write_func pCallback, void *pOpaque, mz_uint flags);
mz_bool mz_zip_reader_extract_file_to_callback(mz_zip_archive *pZip, const char *pFilename, mz_file_write_func pCallback, void *pOpaque, mz_uint flags);

  #ifndef MINIZ_NO_STDIO
// Extracts a archive file to a disk file and sets its last accessed and modified times.
// This function only extracts files, not archive directory records.
mz_bool mz_zip_reader_extract_to_file(mz_zip_archive *pZip, mz_uint file_index, const char *pDst_filename, mz_uint flags);
mz_bool mz_zip_reader_extract_file_to_file(mz_zip_archive *pZip, const char *pArchive_filename, const char *pDst_filename, mz_uint flags);
  #endif

// Ends archive reading, freeing all allocations, and closing the input archive file if mz_zip_reader_init_file() was used.
mz_bool mz_zip_reader_end(mz_zip_archive *pZip);

// ZIP archive writing

  #ifndef MINIZ_NO_ARCHIVE_WRITING_APIS

// Inits a ZIP archive writer.
mz_bool mz_zip_writer_init(mz_zip_archive *pZip, mz_uint64 existing_size);
mz_bool mz_zip_writer_init_heap(mz_zip_archive *pZip, size_t size_to_reserve_at_beginning, size_t initial_allocation_size);

   #ifndef MINIZ_NO_STDIO
mz_bool mz_zip_writer_init_file(mz_zip_archive *pZip, const char *pFilename, mz_uint64 size_to_reserve_at_beginning);
   #endif

// Converts a ZIP archive reader object into a writer object, to allow efficient in-place file appends to occur on an existing archive.
// For archives opened using mz_zip_reader_init_file, pFilename must be the archive's filename so it can be reopened for writing. If the file can't be reopened, mz_zip_reader_end() will be called.
// For archives opened using mz_zip_reader_init_mem, the memory block must be growable using the realloc callback (which defaults to realloc unless you've overridden it).
// Finally, for archives opened using mz_zip_reader_init, the mz_zip_archive's user provided m_pWrite function cannot be NULL.
// Note: In-place archive modification is not recommended unless you know what you're doing, because if execution stops or something goes wrong before
// the archive is finalized the file's central directory will be hosed.
mz_bool mz_zip_writer_init_from_reader(mz_zip_archive *pZip, const char *pFilename);

// Adds the contents of a memory buffer to an archive. These functions record the current local time into the archive.
// To add a directory entry, call this method with an archive name ending in a forwardslash with empty buffer.
// level_and_flags - compression level (0-10, see MZ_BEST_SPEED, MZ_BEST_COMPRESSION, etc.) logically OR'd with zero or more mz_zip_flags, or just set to MZ_DEFAULT_COMPRESSION.
mz_bool mz_zip_writer_add_mem(mz_zip_archive *pZip, const char *pArchive_name, const void *pBuf, size_t buf_size, mz_uint level_and_flags);
mz_bool mz_zip_writer_add_mem_ex(mz_zip_archive *pZip, const char *pArchive_name, const void *pBuf, size_t buf_size, const void *pComment, mz_uint16 comment_size, mz_uint level_and_flags, mz_uint64 uncomp_size, mz_uint32 uncomp_crc32);

   #ifndef MINIZ_NO_STDIO
// Adds the contents of a disk file to an archive. This function also records the disk file's modified time into the archive.
// level_and_flags - compression level (0-10, see MZ_BEST_SPEED, MZ_BEST_COMPRESSION, etc.) logically OR'd with zero or more mz_zip_flags, or just set to MZ_DEFAULT_COMPRESSION.
mz_bool mz_zip_writer_add_file(mz_zip_archive *pZip, const char *pArchive_name, const char *pSrc_filename, const void *pComment, mz_uint16 comment_size, mz_uint level_and_flags);
   #endif

// Adds a file to an archive by fully cloning the data from another archive.
// This function fully clones the source file's compressed data (no recompression), along with its full filename, extra data, and comment fields.
mz_bool mz_zip_writer_add_from_zip_reader(mz_zip_archive *pZip, mz_zip_archive *pSource_zip, mz_uint file_index);

// Finalizes the archive by writing the central directory records followed by the end of central directory record.
// After an archive is finalized, the only valid call on the mz_zip_archive struct is mz_zip_writer_end().
// An archive must be manually finalized by calling this function for it to be valid.
mz_bool mz_zip_writer_finalize_archive(mz_zip_archive *pZip);
mz_bool mz_zip_writer_finalize_heap_archive(mz_zip_archive *pZip, void **pBuf, size_t *pSize);

// Ends archive writing, freeing all allocations, and closing the output file if mz_zip_writer_init_file() was used.
// Note for the archive to be valid, it must have been finalized before ending.
mz_bool mz_zip_writer_end(mz_zip_archive *pZip);

// Misc. high-level helper functions:

// mz_zip_add_mem_to_archive_file_in_place() efficiently (but not atomically) appends a memory blob to a ZIP archive.
// level_and_flags - compression level (0-10, see MZ_BEST_SPEED, MZ_BEST_COMPRESSION, etc.) logically OR'd with zero or more mz_zip_flags, or just set to MZ_DEFAULT_COMPRESSION.
mz_bool mz_zip_add_mem_to_archive_file_in_place(const char *pZip_filename, const char *pArchive_name, const void *pBuf, size_t buf_size, const void *pComment, mz_uint16 comment_size, mz_uint level_and_flags);

// Reads a single file from an archive into a heap block.
// Returns NULL on failure.
void *mz_zip_extract_archive_file_to_heap(const char *pZip_filename, const char *pArchive_name, size_t *pSize, mz_uint zip_flags);
  #endif // #ifndef MINIZ_NO_ARCHIVE_WRITING_APIS
 #endif  // #ifndef MINIZ_NO_ARCHIVE_APIS

// ------------------- Low-level Decompression API Definitions

// Decompression flags used by tinfl_decompress().
// TINFL_FLAG_PARSE_ZLIB_HEADER: If set, the input has a valid zlib header and ends with an adler32 checksum (it's a valid zlib stream). Otherwise, the input is a raw deflate stream.
// TINFL_FLAG_HAS_MORE_INPUT: If set, there are more input bytes available beyond the end of the supplied input buffer. If clear, the input buffer contains all remaining input.
// TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF: If set, the output buffer is large enough to hold the entire decompressed stream. If clear, the output buffer is at least the size of the dictionary (typically 32KB).
// TINFL_FLAG_COMPUTE_ADLER32: Force adler-32 checksum computation of the decompressed bytes.
enum {
    TINFL_FLAG_PARSE_ZLIB_HEADER             = 1,
    TINFL_FLAG_HAS_MORE_INPUT                = 2,
    TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF = 4,
    TINFL_FLAG_COMPUTE_ADLER32               = 8
};

// High level decompression functions:
// tinfl_decompress_mem_to_heap() decompresses a block in memory to a heap block allocated via malloc().
// On entry:
//  pSrc_buf, src_buf_len: Pointer and size of the Deflate or zlib source data to decompress.
// On return:
//  Function returns a pointer to the decompressed data, or NULL on failure.
//  *pOut_len will be set to the decompressed data's size, which could be larger than src_buf_len on uncompressible data.
//  The caller must call mz_free() on the returned block when it's no longer needed.
void *tinfl_decompress_mem_to_heap(const void *pSrc_buf, size_t src_buf_len, size_t *pOut_len, int flags);

// tinfl_decompress_mem_to_mem() decompresses a block in memory to another block in memory.
// Returns TINFL_DECOMPRESS_MEM_TO_MEM_FAILED on failure, or the number of bytes written on success.
 #define TINFL_DECOMPRESS_MEM_TO_MEM_FAILED    ((size_t)(-1))
size_t tinfl_decompress_mem_to_mem(void *pOut_buf, size_t out_buf_len, const void *pSrc_buf, size_t src_buf_len, int flags);

// tinfl_decompress_mem_to_callback() decompresses a block in memory to an internal 32KB buffer, and a user provided callback function will be called to flush the buffer.
// Returns 1 on success or 0 on failure.
typedef int (*tinfl_put_buf_func_ptr)(const void *pBuf, int len, void *pUser);
int tinfl_decompress_mem_to_callback(const void *pIn_buf, size_t *pIn_buf_size, tinfl_put_buf_func_ptr pPut_buf_func, void *pPut_buf_user, int flags);

struct tinfl_decompressor_tag;
typedef struct tinfl_decompressor_tag   tinfl_decompressor;

// Max size of LZ dictionary.
 #define TINFL_LZ_DICT_SIZE    32768

// Return status.
typedef enum {
    TINFL_STATUS_BAD_PARAM        = -3,
    TINFL_STATUS_ADLER32_MISMATCH = -2,
    TINFL_STATUS_FAILED           = -1,
    TINFL_STATUS_DONE             = 0,
    TINFL_STATUS_NEEDS_MORE_INPUT = 1,
    TINFL_STATUS_HAS_MORE_OUTPUT  = 2
} tinfl_status;

// Initializes the decompressor to its initial state.
 #define tinfl_init(r)           do { (r)->m_state = 0; } MZ_MACRO_END
 #define tinfl_get_adler32(r)    (r)->m_check_adler32

// Main low-level decompressor coroutine function. This is the only function actually needed for decompression. All the other functions are just high-level helpers for improved usability.
// This is a universal API, i.e. it can be used as a building block to build any desired higher level decompression API. In the limit case, it can be called once per every byte input or output.
tinfl_status tinfl_decompress(tinfl_decompressor *r, const mz_uint8 *pIn_buf_next, size_t *pIn_buf_size, mz_uint8 *pOut_buf_start, mz_uint8 *pOut_buf_next, size_t *pOut_buf_size, const mz_uint32 decomp_flags);

// Internal/private bits follow.
enum {
    TINFL_MAX_HUFF_TABLES  = 3, TINFL_MAX_HUFF_SYMBOLS_0 = 288, TINFL_MAX_HUFF_SYMBOLS_1 = 32, TINFL_MAX_HUFF_SYMBOLS_2 = 19,
    TINFL_FAST_LOOKUP_BITS = 10, TINFL_FAST_LOOKUP_SIZE = 1 << TINFL_FAST_LOOKUP_BITS
};

typedef struct {
    mz_uint8 m_code_size[TINFL_MAX_HUFF_SYMBOLS_0];
    mz_int16 m_look_up[TINFL_FAST_LOOKUP_SIZE], m_tree[TINFL_MAX_HUFF_SYMBOLS_0 * 2];
} tinfl_huff_table;

 #if MINIZ_HAS_64BIT_REGISTERS
  #define TINFL_USE_64BIT_BITBUF    1
 #endif

 #if TINFL_USE_64BIT_BITBUF
typedef mz_uint64   tinfl_bit_buf_t;
  #define TINFL_BITBUF_SIZE    (64)
 #else
typedef mz_uint32   tinfl_bit_buf_t;
  #define TINFL_BITBUF_SIZE    (32)
 #endif

struct tinfl_decompressor_tag {
    mz_uint32        m_state, m_num_bits, m_zhdr0, m_zhdr1, m_z_adler32, m_final, m_type, m_check_adler32, m_dist, m_counter, m_num_extra, m_table_sizes[TINFL_MAX_HUFF_TABLES];
    tinfl_bit_buf_t  m_bit_buf;
    size_t           m_dist_from_out_buf_start;
    tinfl_huff_table m_tables[TINFL_MAX_HUFF_TABLES];
    mz_uint8         m_raw_header[4], m_len_codes[TINFL_MAX_HUFF_SYMBOLS_0 + TINFL_MAX_HUFF_SYMBOLS_1 + 137];
};

// ------------------- Low-level Compression API Definitions

// Set TDEFL_LESS_MEMORY to 1 to use less memory (compression will be slightly slower, and raw/dynamic blocks will be output more frequently).
 #define TDEFL_LESS_MEMORY    0

// tdefl_init() compression flags logically OR'd together (low 12 bits contain the max. number of probes per dictionary search):
// TDEFL_DEFAULT_MAX_PROBES: The compressor defaults to 128 dictionary probes per dictionary search. 0=Huffman only, 1=Huffman+LZ (fastest/crap compression), 4095=Huffman+LZ (slowest/best compression).
enum {
    TDEFL_HUFFMAN_ONLY = 0, TDEFL_DEFAULT_MAX_PROBES = 128, TDEFL_MAX_PROBES_MASK = 0xFFF
};

// TDEFL_WRITE_ZLIB_HEADER: If set, the compressor outputs a zlib header before the deflate data, and the Adler-32 of the source data at the end. Otherwise, you'll get raw deflate data.
// TDEFL_COMPUTE_ADLER32: Always compute the adler-32 of the input data (even when not writing zlib headers).
// TDEFL_GREEDY_PARSING_FLAG: Set to use faster greedy parsing, instead of more efficient lazy parsing.
// TDEFL_NONDETERMINISTIC_PARSING_FLAG: Enable to decrease the compressor's initialization time to the minimum, but the output may vary from run to run given the same input (depending on the contents of memory).
// TDEFL_RLE_MATCHES: Only look for RLE matches (matches with a distance of 1)
// TDEFL_FILTER_MATCHES: Discards matches <= 5 chars if enabled.
// TDEFL_FORCE_ALL_STATIC_BLOCKS: Disable usage of optimized Huffman tables.
// TDEFL_FORCE_ALL_RAW_BLOCKS: Only use raw (uncompressed) deflate blocks.
// The low 12 bits are reserved to control the max # of hash probes per dictionary lookup (see TDEFL_MAX_PROBES_MASK).
enum {
    TDEFL_WRITE_ZLIB_HEADER             = 0x01000,
    TDEFL_COMPUTE_ADLER32               = 0x02000,
    TDEFL_GREEDY_PARSING_FLAG           = 0x04000,
    TDEFL_NONDETERMINISTIC_PARSING_FLAG = 0x08000,
    TDEFL_RLE_MATCHES                   = 0x10000,
    TDEFL_FILTER_MATCHES                = 0x20000,
    TDEFL_FORCE_ALL_STATIC_BLOCKS       = 0x40000,
    TDEFL_FORCE_ALL_RAW_BLOCKS          = 0x80000
};

// High level compression functions:
// tdefl_compress_mem_to_heap() compresses a block in memory to a heap block allocated via malloc().
// On entry:
//  pSrc_buf, src_buf_len: Pointer and size of source block to compress.
//  flags: The max match finder probes (default is 128) logically OR'd against the above flags. Higher probes are slower but improve compression.
// On return:
//  Function returns a pointer to the compressed data, or NULL on failure.
//  *pOut_len will be set to the compressed data's size, which could be larger than src_buf_len on uncompressible data.
//  The caller must free() the returned block when it's no longer needed.
void *tdefl_compress_mem_to_heap(const void *pSrc_buf, size_t src_buf_len, size_t *pOut_len, int flags);

// tdefl_compress_mem_to_mem() compresses a block in memory to another block in memory.
// Returns 0 on failure.
size_t tdefl_compress_mem_to_mem(void *pOut_buf, size_t out_buf_len, const void *pSrc_buf, size_t src_buf_len, int flags);

// Compresses an image to a compressed PNG file in memory.
// On entry:
//  pImage, w, h, and num_chans describe the image to compress. num_chans may be 1, 2, 3, or 4.
//  The image pitch in bytes per scanline will be w*num_chans. The leftmost pixel on the top scanline is stored first in memory.
//  level may range from [0,10], use MZ_NO_COMPRESSION, MZ_BEST_SPEED, MZ_BEST_COMPRESSION, etc. or a decent default is MZ_DEFAULT_LEVEL
//  If flip is ZIP_TRUE, the image will be flipped on the Y axis (useful for OpenGL apps).
// On return:
//  Function returns a pointer to the compressed data, or NULL on failure.
//  *pLen_out will be set to the size of the PNG image file.
//  The caller must mz_free() the returned heap block (which will typically be larger than *pLen_out) when it's no longer needed.
void *tdefl_write_image_to_png_file_in_memory_ex(const void *pImage, int w, int h, int num_chans, size_t *pLen_out, mz_uint level, mz_bool flip);
void *tdefl_write_image_to_png_file_in_memory(const void *pImage, int w, int h, int num_chans, size_t *pLen_out);

// Output stream interface. The compressor uses this interface to write compressed data. It'll typically be called TDEFL_OUT_BUF_SIZE at a time.
typedef mz_bool (*tdefl_put_buf_func_ptr)(const void *pBuf, int len, void *pUser);

// tdefl_compress_mem_to_output() compresses a block to an output stream. The above helpers use this function internally.
mz_bool tdefl_compress_mem_to_output(const void *pBuf, size_t buf_len, tdefl_put_buf_func_ptr pPut_buf_func, void *pPut_buf_user, int flags);

enum {
    TDEFL_MAX_HUFF_TABLES = 3, TDEFL_MAX_HUFF_SYMBOLS_0 = 288, TDEFL_MAX_HUFF_SYMBOLS_1 = 32, TDEFL_MAX_HUFF_SYMBOLS_2 = 19, TDEFL_LZ_DICT_SIZE = 32768, TDEFL_LZ_DICT_SIZE_MASK = TDEFL_LZ_DICT_SIZE - 1, TDEFL_MIN_MATCH_LEN = 3, TDEFL_MAX_MATCH_LEN = 258
};

// TDEFL_OUT_BUF_SIZE MUST be large enough to hold a single entire compressed output block (using static/fixed Huffman codes).
 #if TDEFL_LESS_MEMORY
enum {
    TDEFL_LZ_CODE_BUF_SIZE = 24 * 1024, TDEFL_OUT_BUF_SIZE = (TDEFL_LZ_CODE_BUF_SIZE * 13) / 10, TDEFL_MAX_HUFF_SYMBOLS = 288, TDEFL_LZ_HASH_BITS = 12, TDEFL_LEVEL1_HASH_SIZE_MASK = 4095, TDEFL_LZ_HASH_SHIFT = (TDEFL_LZ_HASH_BITS + 2) / 3, TDEFL_LZ_HASH_SIZE = 1 << TDEFL_LZ_HASH_BITS
};
 #else
enum {
    TDEFL_LZ_CODE_BUF_SIZE = 64 * 1024, TDEFL_OUT_BUF_SIZE = (TDEFL_LZ_CODE_BUF_SIZE * 13) / 10, TDEFL_MAX_HUFF_SYMBOLS = 288, TDEFL_LZ_HASH_BITS = 15, TDEFL_LEVEL1_HASH_SIZE_MASK = 4095, TDEFL_LZ_HASH_SHIFT = (TDEFL_LZ_HASH_BITS + 2) / 3, TDEFL_LZ_HASH_SIZE = 1 << TDEFL_LZ_HASH_BITS
};
 #endif

// The low-level tdefl functions below may be used directly if the above helper functions aren't flexible enough. The low-level functions don't make any heap allocations, unlike the above helper functions.
typedef enum {
    TDEFL_STATUS_BAD_PARAM      = -2,
    TDEFL_STATUS_PUT_BUF_FAILED = -1,
    TDEFL_STATUS_OKAY           = 0,
    TDEFL_STATUS_DONE           = 1,
} tdefl_status;

// Must map to MZ_NO_FLUSH, MZ_SYNC_FLUSH, etc. enums
typedef enum {
    TDEFL_NO_FLUSH   = 0,
    TDEFL_SYNC_FLUSH = 2,
    TDEFL_FULL_FLUSH = 3,
    TDEFL_FINISH     = 4
} tdefl_flush;

// tdefl's compression state structure.
typedef struct {
    tdefl_put_buf_func_ptr m_pPut_buf_func;
    void                   *m_pPut_buf_user;
    mz_uint                m_flags, m_max_probes[2];
    int                    m_greedy_parsing;
    mz_uint                m_adler32, m_lookahead_pos, m_lookahead_size, m_dict_size;
    mz_uint8               *m_pLZ_code_buf, *m_pLZ_flags, *m_pOutput_buf, *m_pOutput_buf_end;
    mz_uint                m_num_flags_left, m_total_lz_bytes, m_lz_code_buf_dict_pos, m_bits_in, m_bit_buffer;
    mz_uint                m_saved_match_dist, m_saved_match_len, m_saved_lit, m_output_flush_ofs, m_output_flush_remaining, m_finished, m_block_index, m_wants_to_finish;
    tdefl_status           m_prev_return_status;
    const void             *m_pIn_buf;
    void                   *m_pOut_buf;
    size_t                 *m_pIn_buf_size, *m_pOut_buf_size;
    tdefl_flush            m_flush;
    const mz_uint8         *m_pSrc;
    size_t                 m_src_buf_left, m_out_buf_ofs;
    mz_uint8               m_dict[TDEFL_LZ_DICT_SIZE + TDEFL_MAX_MATCH_LEN - 1];
    mz_uint16              m_huff_count[TDEFL_MAX_HUFF_TABLES][TDEFL_MAX_HUFF_SYMBOLS];
    mz_uint16              m_huff_codes[TDEFL_MAX_HUFF_TABLES][TDEFL_MAX_HUFF_SYMBOLS];
    mz_uint8               m_huff_code_sizes[TDEFL_MAX_HUFF_TABLES][TDEFL_MAX_HUFF_SYMBOLS];
    mz_uint8               m_lz_code_buf[TDEFL_LZ_CODE_BUF_SIZE];
    mz_uint16              m_next[TDEFL_LZ_DICT_SIZE];
    mz_uint16              m_hash[TDEFL_LZ_HASH_SIZE];
    mz_uint8               m_output_buf[TDEFL_OUT_BUF_SIZE];
} tdefl_compressor;

// Initializes the compressor.
// There is no corresponding deinit() function because the tdefl API's do not dynamically allocate memory.
// pBut_buf_func: If NULL, output data will be supplied to the specified callback. In this case, the user should call the tdefl_compress_buffer() API for compression.
// If pBut_buf_func is NULL the user should always call the tdefl_compress() API.
// flags: See the above enums (TDEFL_HUFFMAN_ONLY, TDEFL_WRITE_ZLIB_HEADER, etc.)
tdefl_status tdefl_init(tdefl_compressor *d, tdefl_put_buf_func_ptr pPut_buf_func, void *pPut_buf_user, int flags);

// Compresses a block of data, consuming as much of the specified input buffer as possible, and writing as much compressed data to the specified output buffer as possible.
tdefl_status tdefl_compress(tdefl_compressor *d, const void *pIn_buf, size_t *pIn_buf_size, void *pOut_buf, size_t *pOut_buf_size, tdefl_flush flush);

// tdefl_compress_buffer() is only usable when the tdefl_init() is called with a non-NULL tdefl_put_buf_func_ptr.
// tdefl_compress_buffer() always consumes the entire input buffer.
tdefl_status tdefl_compress_buffer(tdefl_compressor *d, const void *pIn_buf, size_t in_buf_size, tdefl_flush flush);

tdefl_status tdefl_get_prev_return_status(tdefl_compressor *d);
mz_uint32 tdefl_get_adler32(tdefl_compressor *d);

// Can't use tdefl_create_comp_flags_from_zip_params if MINIZ_NO_ZLIB_APIS isn't defined, because it uses some of its macros.
 #ifndef MINIZ_NO_ZLIB_APIS
// Create tdefl_compress() flags given zlib-style compression parameters.
// level may range from [0,10] (where 10 is absolute max compression, but may be much slower on some files)
// window_bits may be -15 (raw deflate) or 15 (zlib)
// strategy may be either MZ_DEFAULT_STRATEGY, MZ_FILTERED, MZ_HUFFMAN_ONLY, MZ_RLE, or MZ_FIXED
mz_uint tdefl_create_comp_flags_from_zip_params(int level, int window_bits, int strategy);
 #endif // #ifndef MINIZ_NO_ZLIB_APIS

 #ifdef __cplusplus
}
 #endif
#endif // MINIZ_HEADER_INCLUDED

// ------------------- End of Header: Implementation follows. (If you only want the header, define MINIZ_HEADER_FILE_ONLY.)

#ifndef MINIZ_HEADER_FILE_ONLY

typedef unsigned char   mz_validate_uint16[sizeof(mz_uint16) == 2 ? 1 : -1];
typedef unsigned char   mz_validate_uint32[sizeof(mz_uint32) == 4 ? 1 : -1];
typedef unsigned char   mz_validate_uint64[sizeof(mz_uint64) == 8 ? 1 : -1];

 #include <string.h>
 #include <assert.h>

 #define MZ_ASSERT(x)         assert(x)

 #ifdef MINIZ_NO_MALLOC
  #define MZ_MALLOC(x)        NULL
  #define MZ_FREE(x)          (void)x, ((void)0)
  #define MZ_REALLOC(p, x)    NULL
 #else
  #define MZ_MALLOC(x)        malloc(x)
  #define MZ_FREE(x)          free(x)
  #define MZ_REALLOC(p, x)    realloc(p, x)
 #endif

 #define MZ_MAX(a, b)         (((a) > (b)) ? (a) : (b))
 #define MZ_MIN(a, b)         (((a) < (b)) ? (a) : (b))
 #define MZ_CLEAR_OBJ(obj)    memset(&(obj), 0, sizeof(obj))

 #if MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN
  #define MZ_READ_LE16(p)     *((const mz_uint16 *)(p))
  #define MZ_READ_LE32(p)     *((const mz_uint32 *)(p))
 #else
  #define MZ_READ_LE16(p)     ((mz_uint32)(((const mz_uint8 *)(p))[0]) | ((mz_uint32)(((const mz_uint8 *)(p))[1]) << 8U))
  #define MZ_READ_LE32(p)     ((mz_uint32)(((const mz_uint8 *)(p))[0]) | ((mz_uint32)(((const mz_uint8 *)(p))[1]) << 8U) | ((mz_uint32)(((const mz_uint8 *)(p))[2]) << 16U) | ((mz_uint32)(((const mz_uint8 *)(p))[3]) << 24U))
 #endif

 #ifdef _MSC_VER
  #define MZ_FORCEINLINE    __forceinline
 #elif defined(__GNUC__)
  #define MZ_FORCEINLINE    inline __attribute__((__always_inline__))
 #else
  #define MZ_FORCEINLINE    inline
 #endif

 #ifdef __cplusplus
extern "C" {
 #endif

// ------------------- zlib-style API's

mz_ulong mz_adler32(mz_ulong adler, const unsigned char *ptr, size_t buf_len) {
    mz_uint32 i, s1 = (mz_uint32)(adler & 0xffff), s2 = (mz_uint32)(adler >> 16);
    size_t    block_len = buf_len % 5552;

    if (!ptr) return MZ_ADLER32_INIT;
    while (buf_len) {
        for (i = 0; i + 7 < block_len; i += 8, ptr += 8) {
            s1 += ptr[0], s2 += s1;
            s1 += ptr[1], s2 += s1;
            s1 += ptr[2], s2 += s1;
            s1 += ptr[3], s2 += s1;
            s1 += ptr[4], s2 += s1;
            s1 += ptr[5], s2 += s1;
            s1 += ptr[6], s2 += s1;
            s1 += ptr[7], s2 += s1;
        }
        for ( ; i < block_len; ++i) s1 += *ptr++, s2 += s1;
        s1       %= 65521U, s2 %= 65521U;
        buf_len  -= block_len;
        block_len = 5552;
    }
    return (s2 << 16) + s1;
}

// Karl Malbrain's compact CRC-32. See "A compact CCITT crc16 and crc32 C implementation that balances processor cache usage against speed": http://www.geocities.com/malbrain/
mz_ulong mz_crc32(mz_ulong crc, const mz_uint8 *ptr, size_t buf_len) {
    static const mz_uint32 s_crc32[16] = {
        0,          0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
    };
    mz_uint32 crcu32 = (mz_uint32)crc;

    if (!ptr) return MZ_CRC32_INIT;
    crcu32 = ~crcu32;
    while (buf_len--) {
        mz_uint8 b = *ptr++;
        crcu32 = (crcu32 >> 4) ^ s_crc32[(crcu32 & 0xF) ^ (b & 0xF)];
        crcu32 = (crcu32 >> 4) ^ s_crc32[(crcu32 & 0xF) ^ (b >> 4)];
    }
    return ~crcu32;
}

void mz_free(void *p) {
    MZ_FREE(p);
}

 #ifndef MINIZ_NO_ZLIB_APIS

static void *def_alloc_func(void *opaque, size_t items, size_t size) {
    (void)opaque, (void)items, (void)size;
    return MZ_MALLOC(items * size);
}

static void def_free_func(void *opaque, void *address) {
    (void)opaque, (void)address;
    MZ_FREE(address);
}

static void *def_realloc_func(void *opaque, void *address, size_t items, size_t size) {
    (void)opaque, (void)address, (void)items, (void)size;
    return MZ_REALLOC(address, items * size);
}

const char *mz_version(void) {
    return MZ_VERSION;
}

int mz_deflateInit(mz_streamp pStream, int level) {
    return mz_deflateInit2(pStream, level, MZ_DEFLATED, MZ_DEFAULT_WINDOW_BITS, 9, MZ_DEFAULT_STRATEGY);
}

int mz_deflateInit2(mz_streamp pStream, int level, int method, int window_bits, int mem_level, int strategy) {
    tdefl_compressor *pComp;
    mz_uint          comp_flags = TDEFL_COMPUTE_ADLER32 | tdefl_create_comp_flags_from_zip_params(level, window_bits, strategy);

    if (!pStream) return MZ_STREAM_ERROR;
    if ((method != MZ_DEFLATED) || ((mem_level < 1) || (mem_level > 9)) || ((window_bits != MZ_DEFAULT_WINDOW_BITS) && (-window_bits != MZ_DEFAULT_WINDOW_BITS))) return MZ_PARAM_ERROR;

    pStream->data_type = 0;
    pStream->adler     = MZ_ADLER32_INIT;
    pStream->msg       = NULL;
    pStream->reserved  = 0;
    pStream->total_in  = 0;
    pStream->total_out = 0;
    if (!pStream->zalloc) pStream->zalloc = def_alloc_func;
    if (!pStream->zfree) pStream->zfree = def_free_func;

    pComp = (tdefl_compressor *)pStream->zalloc(pStream->opaque, 1, sizeof(tdefl_compressor));
    if (!pComp)
        return MZ_MEM_ERROR;

    pStream->state = (struct mz_internal_state *)pComp;

    if (tdefl_init(pComp, NULL, NULL, comp_flags) != TDEFL_STATUS_OKAY) {
        mz_deflateEnd(pStream);
        return MZ_PARAM_ERROR;
    }

    return MZ_OK;
}

int mz_deflateReset(mz_streamp pStream) {
    if ((!pStream) || (!pStream->state) || (!pStream->zalloc) || (!pStream->zfree)) return MZ_STREAM_ERROR;
    pStream->total_in = pStream->total_out = 0;
    tdefl_init((tdefl_compressor *)pStream->state, NULL, NULL, ((tdefl_compressor *)pStream->state)->m_flags);
    return MZ_OK;
}

int mz_deflate(mz_streamp pStream, int flush) {
    size_t   in_bytes, out_bytes;
    mz_ulong orig_total_in, orig_total_out;
    int      mz_status = MZ_OK;

    if ((!pStream) || (!pStream->state) || (flush < 0) || (flush > MZ_FINISH) || (!pStream->next_out)) return MZ_STREAM_ERROR;
    if (!pStream->avail_out) return MZ_BUF_ERROR;

    if (flush == MZ_PARTIAL_FLUSH) flush = MZ_SYNC_FLUSH;

    if (((tdefl_compressor *)pStream->state)->m_prev_return_status == TDEFL_STATUS_DONE)
        return (flush == MZ_FINISH) ? MZ_STREAM_END : MZ_BUF_ERROR;

    orig_total_in  = pStream->total_in;
    orig_total_out = pStream->total_out;
    for ( ; ; ) {
        tdefl_status defl_status;
        in_bytes  = pStream->avail_in;
        out_bytes = pStream->avail_out;

        defl_status        = tdefl_compress((tdefl_compressor *)pStream->state, pStream->next_in, &in_bytes, pStream->next_out, &out_bytes, (tdefl_flush)flush);
        pStream->next_in  += (mz_uint)in_bytes;
        pStream->avail_in -= (mz_uint)in_bytes;
        pStream->total_in += (mz_uint)in_bytes;
        pStream->adler     = tdefl_get_adler32((tdefl_compressor *)pStream->state);

        pStream->next_out  += (mz_uint)out_bytes;
        pStream->avail_out -= (mz_uint)out_bytes;
        pStream->total_out += (mz_uint)out_bytes;

        if (defl_status < 0) {
            mz_status = MZ_STREAM_ERROR;
            break;
        } else if (defl_status == TDEFL_STATUS_DONE) {
            mz_status = MZ_STREAM_END;
            break;
        } else if (!pStream->avail_out)
            break;
        else if ((!pStream->avail_in) && (flush != MZ_FINISH)) {
            if ((flush) || (pStream->total_in != orig_total_in) || (pStream->total_out != orig_total_out))
                break;
            return MZ_BUF_ERROR; // Can't make forward progress without some input.
        }
    }
    return mz_status;
}

int mz_deflateEnd(mz_streamp pStream) {
    if (!pStream) return MZ_STREAM_ERROR;
    if (pStream->state) {
        pStream->zfree(pStream->opaque, pStream->state);
        pStream->state = NULL;
    }
    return MZ_OK;
}

mz_ulong mz_deflateBound(mz_streamp pStream, mz_ulong source_len) {
    (void)pStream;
    // This is really over conservative. (And lame, but it's actually pretty tricky to compute a ZIP_TRUE upper bound given the way tdefl's blocking works.)
    return MZ_MAX(128 + (source_len * 110) / 100, 128 + source_len + ((source_len / (31 * 1024)) + 1) * 5);
}

int mz_compress2(unsigned char *pDest, mz_ulong *pDest_len, const unsigned char *pSource, mz_ulong source_len, int level) {
    int       status;
    mz_stream stream;

    memset(&stream, 0, sizeof(stream));

    // In case mz_ulong is 64-bits (argh I hate longs).
    if ((source_len | *pDest_len) > 0xFFFFFFFFU) return MZ_PARAM_ERROR;

    stream.next_in   = pSource;
    stream.avail_in  = (mz_uint32)source_len;
    stream.next_out  = pDest;
    stream.avail_out = (mz_uint32) * pDest_len;

    status = mz_deflateInit(&stream, level);
    if (status != MZ_OK) return status;

    status = mz_deflate(&stream, MZ_FINISH);
    if (status != MZ_STREAM_END) {
        mz_deflateEnd(&stream);
        return (status == MZ_OK) ? MZ_BUF_ERROR : status;
    }

    *pDest_len = stream.total_out;
    return mz_deflateEnd(&stream);
}

int mz_compress(unsigned char *pDest, mz_ulong *pDest_len, const unsigned char *pSource, mz_ulong source_len) {
    return mz_compress2(pDest, pDest_len, pSource, source_len, MZ_DEFAULT_COMPRESSION);
}

mz_ulong mz_compressBound(mz_ulong source_len) {
    return mz_deflateBound(NULL, source_len);
}

typedef struct {
    tinfl_decompressor m_decomp;
    mz_uint            m_dict_ofs, m_dict_avail, m_first_call, m_has_flushed;
    int                m_window_bits;
    mz_uint8           m_dict[TINFL_LZ_DICT_SIZE];
    tinfl_status       m_last_status;
} inflate_state;

int mz_inflateInit2(mz_streamp pStream, int window_bits) {
    inflate_state *pDecomp;

    if (!pStream) return MZ_STREAM_ERROR;
    if ((window_bits != MZ_DEFAULT_WINDOW_BITS) && (-window_bits != MZ_DEFAULT_WINDOW_BITS)) return MZ_PARAM_ERROR;

    pStream->data_type = 0;
    pStream->adler     = 0;
    pStream->msg       = NULL;
    pStream->total_in  = 0;
    pStream->total_out = 0;
    pStream->reserved  = 0;
    if (!pStream->zalloc) pStream->zalloc = def_alloc_func;
    if (!pStream->zfree) pStream->zfree = def_free_func;

    pDecomp = (inflate_state *)pStream->zalloc(pStream->opaque, 1, sizeof(inflate_state));
    if (!pDecomp) return MZ_MEM_ERROR;

    pStream->state = (struct mz_internal_state *)pDecomp;

    tinfl_init(&pDecomp->m_decomp);
    pDecomp->m_dict_ofs    = 0;
    pDecomp->m_dict_avail  = 0;
    pDecomp->m_last_status = TINFL_STATUS_NEEDS_MORE_INPUT;
    pDecomp->m_first_call  = 1;
    pDecomp->m_has_flushed = 0;
    pDecomp->m_window_bits = window_bits;

    return MZ_OK;
}

int mz_inflateInit(mz_streamp pStream) {
    return mz_inflateInit2(pStream, MZ_DEFAULT_WINDOW_BITS);
}

int mz_inflate(mz_streamp pStream, int flush) {
    inflate_state *pState;
    mz_uint       n, first_call, decomp_flags = TINFL_FLAG_COMPUTE_ADLER32;
    size_t        in_bytes, out_bytes, orig_avail_in;
    tinfl_status  status;

    if ((!pStream) || (!pStream->state)) return MZ_STREAM_ERROR;
    if (flush == MZ_PARTIAL_FLUSH) flush = MZ_SYNC_FLUSH;
    if ((flush) && (flush != MZ_SYNC_FLUSH) && (flush != MZ_FINISH)) return MZ_STREAM_ERROR;

    pState = (inflate_state *)pStream->state;
    if (pState->m_window_bits > 0) decomp_flags |= TINFL_FLAG_PARSE_ZLIB_HEADER;
    orig_avail_in = pStream->avail_in;

    first_call           = pState->m_first_call;
    pState->m_first_call = 0;
    if (pState->m_last_status < 0) return MZ_DATA_ERROR;

    if (pState->m_has_flushed && (flush != MZ_FINISH)) return MZ_STREAM_ERROR;
    pState->m_has_flushed |= (flush == MZ_FINISH);

    if ((flush == MZ_FINISH) && (first_call)) {
        // MZ_FINISH on the first call implies that the input and output buffers are large enough to hold the entire compressed/decompressed file.
        decomp_flags         |= TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF;
        in_bytes              = pStream->avail_in;
        out_bytes             = pStream->avail_out;
        status                = tinfl_decompress(&pState->m_decomp, pStream->next_in, &in_bytes, pStream->next_out, pStream->next_out, &out_bytes, decomp_flags);
        pState->m_last_status = status;
        pStream->next_in     += (mz_uint)in_bytes;
        pStream->avail_in    -= (mz_uint)in_bytes;
        pStream->total_in    += (mz_uint)in_bytes;
        pStream->adler        = tinfl_get_adler32(&pState->m_decomp);
        pStream->next_out    += (mz_uint)out_bytes;
        pStream->avail_out   -= (mz_uint)out_bytes;
        pStream->total_out   += (mz_uint)out_bytes;

        if (status < 0)
            return MZ_DATA_ERROR;
        else if (status != TINFL_STATUS_DONE) {
            pState->m_last_status = TINFL_STATUS_FAILED;
            return MZ_BUF_ERROR;
        }
        return MZ_STREAM_END;
    }
    // flush != MZ_FINISH then we must assume there's more input.
    if (flush != MZ_FINISH) decomp_flags |= TINFL_FLAG_HAS_MORE_INPUT;

    if (pState->m_dict_avail) {
        n = MZ_MIN(pState->m_dict_avail, pStream->avail_out);
        memcpy(pStream->next_out, pState->m_dict + pState->m_dict_ofs, n);
        pStream->next_out    += n;
        pStream->avail_out   -= n;
        pStream->total_out   += n;
        pState->m_dict_avail -= n;
        pState->m_dict_ofs    = (pState->m_dict_ofs + n) & (TINFL_LZ_DICT_SIZE - 1);
        return ((pState->m_last_status == TINFL_STATUS_DONE) && (!pState->m_dict_avail)) ? MZ_STREAM_END : MZ_OK;
    }

    for ( ; ; ) {
        in_bytes  = pStream->avail_in;
        out_bytes = TINFL_LZ_DICT_SIZE - pState->m_dict_ofs;

        status = tinfl_decompress(&pState->m_decomp, pStream->next_in, &in_bytes, pState->m_dict, pState->m_dict + pState->m_dict_ofs, &out_bytes, decomp_flags);
        pState->m_last_status = status;

        pStream->next_in  += (mz_uint)in_bytes;
        pStream->avail_in -= (mz_uint)in_bytes;
        pStream->total_in += (mz_uint)in_bytes;
        pStream->adler     = tinfl_get_adler32(&pState->m_decomp);

        pState->m_dict_avail = (mz_uint)out_bytes;

        n = MZ_MIN(pState->m_dict_avail, pStream->avail_out);
        memcpy(pStream->next_out, pState->m_dict + pState->m_dict_ofs, n);
        pStream->next_out    += n;
        pStream->avail_out   -= n;
        pStream->total_out   += n;
        pState->m_dict_avail -= n;
        pState->m_dict_ofs    = (pState->m_dict_ofs + n) & (TINFL_LZ_DICT_SIZE - 1);

        if (status < 0)
            return MZ_DATA_ERROR;  // Stream is corrupted (there could be some uncompressed data left in the output dictionary - oh well).
        else if ((status == TINFL_STATUS_NEEDS_MORE_INPUT) && (!orig_avail_in))
            return MZ_BUF_ERROR;   // Signal caller that we can't make forward progress without supplying more input or by setting flush to MZ_FINISH.
        else if (flush == MZ_FINISH) {
            // The output buffer MUST be large to hold the remaining uncompressed data when flush==MZ_FINISH.
            if (status == TINFL_STATUS_DONE)
                return pState->m_dict_avail ? MZ_BUF_ERROR : MZ_STREAM_END;
            // status here must be TINFL_STATUS_HAS_MORE_OUTPUT, which means there's at least 1 more byte on the way. If there's no more room left in the output buffer then something is wrong.
            else if (!pStream->avail_out)
                return MZ_BUF_ERROR;
        } else if ((status == TINFL_STATUS_DONE) || (!pStream->avail_in) || (!pStream->avail_out) || (pState->m_dict_avail))
            break;
    }

    return ((status == TINFL_STATUS_DONE) && (!pState->m_dict_avail)) ? MZ_STREAM_END : MZ_OK;
}

int mz_inflateEnd(mz_streamp pStream) {
    if (!pStream)
        return MZ_STREAM_ERROR;
    if (pStream->state) {
        pStream->zfree(pStream->opaque, pStream->state);
        pStream->state = NULL;
    }
    return MZ_OK;
}

int mz_uncompress(unsigned char *pDest, mz_ulong *pDest_len, const unsigned char *pSource, mz_ulong source_len) {
    mz_stream stream;
    int       status;

    memset(&stream, 0, sizeof(stream));

    // In case mz_ulong is 64-bits (argh I hate longs).
    if ((source_len | *pDest_len) > 0xFFFFFFFFU) return MZ_PARAM_ERROR;

    stream.next_in   = pSource;
    stream.avail_in  = (mz_uint32)source_len;
    stream.next_out  = pDest;
    stream.avail_out = (mz_uint32) * pDest_len;

    status = mz_inflateInit(&stream);
    if (status != MZ_OK)
        return status;

    status = mz_inflate(&stream, MZ_FINISH);
    if (status != MZ_STREAM_END) {
        mz_inflateEnd(&stream);
        return ((status == MZ_BUF_ERROR) && (!stream.avail_in)) ? MZ_DATA_ERROR : status;
    }
    *pDest_len = stream.total_out;

    return mz_inflateEnd(&stream);
}

const char *mz_error(int err) {
    static struct {
        int        m_err;
        const char *m_pDesc;
    } s_error_descs[] =
    {
        { MZ_OK,         ""           }, { MZ_STREAM_END, "stream end"    }, { MZ_NEED_DICT, "need dictionary" }, { MZ_ERRNO,         "file error"    }, { MZ_STREAM_ERROR, "stream error"    },
        { MZ_DATA_ERROR, "data error" }, { MZ_MEM_ERROR,  "out of memory" }, { MZ_BUF_ERROR, "buf error"       }, { MZ_VERSION_ERROR, "version error" }, { MZ_PARAM_ERROR,  "parameter error" }
    };
    mz_uint i;

    for (i = 0; i < sizeof(s_error_descs) / sizeof(s_error_descs[0]); ++i) if (s_error_descs[i].m_err == err) return s_error_descs[i].m_pDesc;

    return NULL;
}
 #endif //MINIZ_NO_ZLIB_APIS

// ------------------- Low-level Decompression (completely independent from all compression API's)

 #define TINFL_MEMCPY(d, s, l)    memcpy(d, s, l)
 #define TINFL_MEMSET(p, c, l)    memset(p, c, l)

 #define TINFL_CR_BEGIN   \
    switch (r->m_state) { \
        case 0:
 #define TINFL_CR_RETURN(state_index, result)                         \
    do { status = result; r->m_state = state_index; goto common_exit; \
         case state_index:                                            \
             ; } MZ_MACRO_END
 #define TINFL_CR_RETURN_FOREVER(state_index, result)    do { for ( ; ; ) { TINFL_CR_RETURN(state_index, result); } } MZ_MACRO_END
 #define TINFL_CR_FINISH    }

// TODO: If the caller has indicated that there's no more input, and we attempt to read beyond the input buf, then something is wrong with the input because the inflator never
// reads ahead more than it needs to. Currently TINFL_GET_BYTE() pads the end of the stream with 0's in this scenario.
 #define TINFL_GET_BYTE(state_index, c)                                          \
    do {                                                                         \
        if (pIn_buf_cur >= pIn_buf_end) {                                        \
            for ( ; ; ) {                                                        \
                if (decomp_flags & TINFL_FLAG_HAS_MORE_INPUT) {                  \
                    TINFL_CR_RETURN(state_index, TINFL_STATUS_NEEDS_MORE_INPUT); \
                    if (pIn_buf_cur < pIn_buf_end) {                             \
                        c = *pIn_buf_cur++;                                      \
                        break;                                                   \
                    }                                                            \
                } else {                                                         \
                    c = 0;                                                       \
                    break;                                                       \
                }                                                                \
            }                                                                    \
        } else c = *pIn_buf_cur++; } MZ_MACRO_END

 #define TINFL_NEED_BITS(state_index, n)      do { mz_uint c; TINFL_GET_BYTE(state_index, c); bit_buf |= (((tinfl_bit_buf_t)c) << num_bits); num_bits += 8; } while (num_bits < (mz_uint)(n))
 #define TINFL_SKIP_BITS(state_index, n)      do { if (num_bits < (mz_uint)(n)) { TINFL_NEED_BITS(state_index, n); } bit_buf >>= (n); num_bits -= (n); } MZ_MACRO_END
 #define TINFL_GET_BITS(state_index, b, n)    do { if (num_bits < (mz_uint)(n)) { TINFL_NEED_BITS(state_index, n); } b = bit_buf & ((1 << (n)) - 1); bit_buf >>= (n); num_bits -= (n); } MZ_MACRO_END

// TINFL_HUFF_BITBUF_FILL() is only used rarely, when the number of bytes remaining in the input buffer falls below 2.
// It reads just enough bytes from the input stream that are needed to decode the next Huffman code (and absolutely no more). It works by trying to fully decode a
// Huffman code by using whatever bits are currently present in the bit buffer. If this fails, it reads another byte, and tries again until it succeeds or until the
// bit buffer contains >=15 bits (deflate's max. Huffman code size).
 #define TINFL_HUFF_BITBUF_FILL(state_index, pHuff)                                                     \
    do {                                                                                                \
        temp = (pHuff)->m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)];                              \
        if (temp >= 0) {                                                                                \
            code_len = temp >> 9;                                                                       \
            if ((code_len) && (num_bits >= code_len))                                                   \
                break;                                                                                  \
        } else if (num_bits > TINFL_FAST_LOOKUP_BITS) {                                                 \
            code_len = TINFL_FAST_LOOKUP_BITS;                                                          \
            do {                                                                                        \
                temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)];                          \
            } while ((temp < 0) && (num_bits >= (code_len + 1))); if (temp >= 0) break;                 \
        } TINFL_GET_BYTE(state_index, c); bit_buf |= (((tinfl_bit_buf_t)c) << num_bits); num_bits += 8; \
    } while (num_bits < 15);

// TINFL_HUFF_DECODE() decodes the next Huffman coded symbol. It's more complex than you would initially expect because the zlib API expects the decompressor to never read
// beyond the final byte of the deflate stream. (In other words, when this macro wants to read another byte from the input, it REALLY needs another byte in order to fully
// decode the next Huffman code.) Handling this properly is particularly important on raw deflate (non-zlib) streams, which aren't followed by a byte aligned adler-32.
// The slow path is only executed at the very end of the input buffer.
 #define TINFL_HUFF_DECODE(state_index, sym, pHuff)                                                                                                                   \
    do {                                                                                                                                                              \
        int temp; mz_uint code_len, c;                                                                                                                                \
        if (num_bits < 15) {                                                                                                                                          \
            if ((pIn_buf_end - pIn_buf_cur) < 2) {                                                                                                                    \
                TINFL_HUFF_BITBUF_FILL(state_index, pHuff);                                                                                                           \
            } else {                                                                                                                                                  \
                bit_buf |= (((tinfl_bit_buf_t)pIn_buf_cur[0]) << num_bits) | (((tinfl_bit_buf_t)pIn_buf_cur[1]) << (num_bits + 8)); pIn_buf_cur += 2; num_bits += 16; \
            }                                                                                                                                                         \
        }                                                                                                                                                             \
        if ((temp = (pHuff)->m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]) >= 0)                                                                                 \
            code_len = temp >> 9, temp &= 511;                                                                                                                        \
        else {                                                                                                                                                        \
            code_len = TINFL_FAST_LOOKUP_BITS; do { temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)]; } while (temp < 0);                                \
        } sym = temp; bit_buf >>= code_len; num_bits -= code_len; } MZ_MACRO_END

tinfl_status tinfl_decompress(tinfl_decompressor *r, const mz_uint8 *pIn_buf_next, size_t *pIn_buf_size, mz_uint8 *pOut_buf_start, mz_uint8 *pOut_buf_next, size_t *pOut_buf_size, const mz_uint32 decomp_flags) {
    static const int      s_length_base[31]     = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0 };
    static const int      s_length_extra[31]    = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0 };
    static const int      s_dist_base[32]       = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0, 0 };
    static const int      s_dist_extra[32]      = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };
    static const mz_uint8 s_length_dezigzag[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
    static const int      s_min_table_sizes[3]  = { 257, 1, 4 };

    tinfl_status    status = TINFL_STATUS_FAILED;
    mz_uint32       num_bits, dist, counter, num_extra;
    tinfl_bit_buf_t bit_buf;
    const mz_uint8  *pIn_buf_cur      = pIn_buf_next, *const pIn_buf_end = pIn_buf_next + *pIn_buf_size;
    mz_uint8        *pOut_buf_cur     = pOut_buf_next, *const pOut_buf_end = pOut_buf_next + *pOut_buf_size;
    size_t          out_buf_size_mask = (decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF) ? (size_t)-1 : ((pOut_buf_next - pOut_buf_start) + *pOut_buf_size) - 1, dist_from_out_buf_start;

    // Ensure the output buffer's size is a power of 2, unless the output buffer is large enough to hold the entire output file (in which case it doesn't matter).
    if (((out_buf_size_mask + 1) & out_buf_size_mask) || (pOut_buf_next < pOut_buf_start)) {
        *pIn_buf_size = *pOut_buf_size = 0;
        return TINFL_STATUS_BAD_PARAM;
    }

    num_bits  = r->m_num_bits;
    bit_buf   = r->m_bit_buf;
    dist      = r->m_dist;
    counter   = r->m_counter;
    num_extra = r->m_num_extra;
    dist_from_out_buf_start = r->m_dist_from_out_buf_start;
    TINFL_CR_BEGIN

        bit_buf = num_bits = dist = counter = num_extra = r->m_zhdr0 = r->m_zhdr1 = 0;
    r->m_z_adler32 = r->m_check_adler32 = 1;
    if (decomp_flags & TINFL_FLAG_PARSE_ZLIB_HEADER) {
        TINFL_GET_BYTE(1, r->m_zhdr0);
        TINFL_GET_BYTE(2, r->m_zhdr1);
        counter = (((r->m_zhdr0 * 256 + r->m_zhdr1) % 31 != 0) || (r->m_zhdr1 & 32) || ((r->m_zhdr0 & 15) != 8));
        if (!(decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF)) counter |= (((1U << (8U + (r->m_zhdr0 >> 4))) > 32768U) || ((out_buf_size_mask + 1) < (size_t)(1U << (8U + (r->m_zhdr0 >> 4)))));
        if (counter) {
            TINFL_CR_RETURN_FOREVER(36, TINFL_STATUS_FAILED);
        }
    }

    do {
        TINFL_GET_BITS(3, r->m_final, 3);
        r->m_type = r->m_final >> 1;
        if (r->m_type == 0) {
            TINFL_SKIP_BITS(5, num_bits & 7);
            for (counter = 0; counter < 4; ++counter) {
                if (num_bits) TINFL_GET_BITS(6, r->m_raw_header[counter], 8);
                else TINFL_GET_BYTE(7, r->m_raw_header[counter]);
            }
            if ((counter = (r->m_raw_header[0] | (r->m_raw_header[1] << 8))) != (mz_uint)(0xFFFF ^ (r->m_raw_header[2] | (r->m_raw_header[3] << 8)))) {
                TINFL_CR_RETURN_FOREVER(39, TINFL_STATUS_FAILED);
            }
            while ((counter) && (num_bits)) {
                TINFL_GET_BITS(51, dist, 8);
                while (pOut_buf_cur >= pOut_buf_end) {
                    TINFL_CR_RETURN(52, TINFL_STATUS_HAS_MORE_OUTPUT);
                }
                *pOut_buf_cur++ = (mz_uint8)dist;
                counter--;
            }
            while (counter) {
                size_t n;
                while (pOut_buf_cur >= pOut_buf_end) {
                    TINFL_CR_RETURN(9, TINFL_STATUS_HAS_MORE_OUTPUT);
                }
                while (pIn_buf_cur >= pIn_buf_end) {
                    if (decomp_flags & TINFL_FLAG_HAS_MORE_INPUT) {
                        TINFL_CR_RETURN(38, TINFL_STATUS_NEEDS_MORE_INPUT);
                    } else {
                        TINFL_CR_RETURN_FOREVER(40, TINFL_STATUS_FAILED);
                    }
                }
                n = MZ_MIN(MZ_MIN((size_t)(pOut_buf_end - pOut_buf_cur), (size_t)(pIn_buf_end - pIn_buf_cur)), counter);
                TINFL_MEMCPY(pOut_buf_cur, pIn_buf_cur, n);
                pIn_buf_cur  += n;
                pOut_buf_cur += n;
                counter      -= (mz_uint)n;
            }
        } else if (r->m_type == 3) {
            TINFL_CR_RETURN_FOREVER(10, TINFL_STATUS_FAILED);
        } else {
            if (r->m_type == 1) {
                mz_uint8 *p = r->m_tables[0].m_code_size;
                mz_uint  i;
                r->m_table_sizes[0] = 288;
                r->m_table_sizes[1] = 32;
                TINFL_MEMSET(r->m_tables[1].m_code_size, 5, 32);
                for (i = 0; i <= 143; ++i) *p++ = 8;
                for ( ; i <= 255; ++i) *p++ = 9;
                for ( ; i <= 279; ++i) *p++ = 7;
                for ( ; i <= 287; ++i) *p++ = 8;
            } else {
                for (counter = 0; counter < 3; counter++) {
                    TINFL_GET_BITS(11, r->m_table_sizes[counter], "\05\05\04"[counter]);
                    r->m_table_sizes[counter] += s_min_table_sizes[counter];
                }
                MZ_CLEAR_OBJ(r->m_tables[2].m_code_size);
                for (counter = 0; counter < r->m_table_sizes[2]; counter++) {
                    mz_uint s;
                    TINFL_GET_BITS(14, s, 3);
                    r->m_tables[2].m_code_size[s_length_dezigzag[counter]] = (mz_uint8)s;
                }
                r->m_table_sizes[2] = 19;
            }
            for ( ; (int)r->m_type >= 0; r->m_type--) {
                int tree_next, tree_cur;
                tinfl_huff_table *pTable;
                mz_uint          i, j, used_syms, total, sym_index, next_code[17], total_syms[16];
                pTable = &r->m_tables[r->m_type];
                MZ_CLEAR_OBJ(total_syms);
                MZ_CLEAR_OBJ(pTable->m_look_up);
                MZ_CLEAR_OBJ(pTable->m_tree);
                for (i = 0; i < r->m_table_sizes[r->m_type]; ++i) total_syms[pTable->m_code_size[i]]++;
                used_syms    = 0, total = 0;
                next_code[0] = next_code[1] = 0;
                for (i = 1; i <= 15; ++i) {
                    used_syms       += total_syms[i];
                    next_code[i + 1] = (total = ((total + total_syms[i]) << 1));
                }
                if ((65536 != total) && (used_syms > 1)) {
                    TINFL_CR_RETURN_FOREVER(35, TINFL_STATUS_FAILED);
                }
                for (tree_next = -1, sym_index = 0; sym_index < r->m_table_sizes[r->m_type]; ++sym_index) {
                    mz_uint rev_code = 0, l, cur_code, code_size = pTable->m_code_size[sym_index];
                    if (!code_size) continue;
                    cur_code = next_code[code_size]++;
                    for (l = code_size; l > 0; l--, cur_code >>= 1) rev_code = (rev_code << 1) | (cur_code & 1);
                    if (code_size <= TINFL_FAST_LOOKUP_BITS) {
                        mz_int16 k = (mz_int16)((code_size << 9) | sym_index);
                        while (rev_code < TINFL_FAST_LOOKUP_SIZE) {
                            pTable->m_look_up[rev_code] = k;
                            rev_code += (1 << code_size);
                        }
                        continue;
                    }
                    if (0 == (tree_cur = pTable->m_look_up[rev_code & (TINFL_FAST_LOOKUP_SIZE - 1)])) {
                        pTable->m_look_up[rev_code & (TINFL_FAST_LOOKUP_SIZE - 1)] = (mz_int16)tree_next;
                        tree_cur   = tree_next;
                        tree_next -= 2;
                    }
                    rev_code >>= (TINFL_FAST_LOOKUP_BITS - 1);
                    for (j = code_size; j > (TINFL_FAST_LOOKUP_BITS + 1); j--) {
                        tree_cur -= ((rev_code >>= 1) & 1);
                        if (!pTable->m_tree[-tree_cur - 1]) {
                            pTable->m_tree[-tree_cur - 1] = (mz_int16)tree_next;
                            tree_cur   = tree_next;
                            tree_next -= 2;
                        } else tree_cur = pTable->m_tree[-tree_cur - 1];
                    }
                    tree_cur -= ((rev_code >>= 1) & 1);
                    pTable->m_tree[-tree_cur - 1] = (mz_int16)sym_index;
                }
                if (r->m_type == 2) {
                    for (counter = 0; counter < (r->m_table_sizes[0] + r->m_table_sizes[1]); ) {
                        mz_uint s;
                        TINFL_HUFF_DECODE(16, dist, &r->m_tables[2]);
                        if (dist < 16) {
                            r->m_len_codes[counter++] = (mz_uint8)dist;
                            continue;
                        }
                        if ((dist == 16) && (!counter)) {
                            TINFL_CR_RETURN_FOREVER(17, TINFL_STATUS_FAILED);
                        }
                        num_extra = "\02\03\07"[dist - 16];
                        TINFL_GET_BITS(18, s, num_extra);
                        s += "\03\03\013"[dist - 16];
                        TINFL_MEMSET(r->m_len_codes + counter, (dist == 16) ? r->m_len_codes[counter - 1] : 0, s);
                        counter += s;
                    }
                    if ((r->m_table_sizes[0] + r->m_table_sizes[1]) != counter) {
                        TINFL_CR_RETURN_FOREVER(21, TINFL_STATUS_FAILED);
                    }
                    TINFL_MEMCPY(r->m_tables[0].m_code_size, r->m_len_codes, r->m_table_sizes[0]);
                    TINFL_MEMCPY(r->m_tables[1].m_code_size, r->m_len_codes + r->m_table_sizes[0], r->m_table_sizes[1]);
                }
            }
            for ( ; ; ) {
                mz_uint8 *pSrc;
                for ( ; ; ) {
                    if (((pIn_buf_end - pIn_buf_cur) < 4) || ((pOut_buf_end - pOut_buf_cur) < 2)) {
                        TINFL_HUFF_DECODE(23, counter, &r->m_tables[0]);
                        if (counter >= 256)
                            break;
                        while (pOut_buf_cur >= pOut_buf_end) {
                            TINFL_CR_RETURN(24, TINFL_STATUS_HAS_MORE_OUTPUT);
                        }
                        *pOut_buf_cur++ = (mz_uint8)counter;
                    } else {
                        int     sym2;
                        mz_uint code_len;
 #if TINFL_USE_64BIT_BITBUF
                        if (num_bits < 30) {
                            bit_buf     |= (((tinfl_bit_buf_t)MZ_READ_LE32(pIn_buf_cur)) << num_bits);
                            pIn_buf_cur += 4;
                            num_bits    += 32;
                        }
 #else
                        if (num_bits < 15) {
                            bit_buf     |= (((tinfl_bit_buf_t)MZ_READ_LE16(pIn_buf_cur)) << num_bits);
                            pIn_buf_cur += 2;
                            num_bits    += 16;
                        }
 #endif
                        if ((sym2 = r->m_tables[0].m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]) >= 0)
                            code_len = sym2 >> 9;
                        else {
                            code_len = TINFL_FAST_LOOKUP_BITS;
                            do {
                                sym2 = r->m_tables[0].m_tree[~sym2 + ((bit_buf >> code_len++) & 1)];
                            } while (sym2 < 0);
                        }
                        counter   = sym2;
                        bit_buf >>= code_len;
                        num_bits -= code_len;
                        if (counter & 256)
                            break;

 #if !TINFL_USE_64BIT_BITBUF
                        if (num_bits < 15) {
                            bit_buf     |= (((tinfl_bit_buf_t)MZ_READ_LE16(pIn_buf_cur)) << num_bits);
                            pIn_buf_cur += 2;
                            num_bits    += 16;
                        }
 #endif
                        if ((sym2 = r->m_tables[0].m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]) >= 0)
                            code_len = sym2 >> 9;
                        else {
                            code_len = TINFL_FAST_LOOKUP_BITS;
                            do {
                                sym2 = r->m_tables[0].m_tree[~sym2 + ((bit_buf >> code_len++) & 1)];
                            } while (sym2 < 0);
                        }
                        bit_buf >>= code_len;
                        num_bits -= code_len;

                        pOut_buf_cur[0] = (mz_uint8)counter;
                        if (sym2 & 256) {
                            pOut_buf_cur++;
                            counter = sym2;
                            break;
                        }
                        pOut_buf_cur[1] = (mz_uint8)sym2;
                        pOut_buf_cur   += 2;
                    }
                }
                if ((counter &= 511) == 256) break;

                num_extra = s_length_extra[counter - 257];
                counter   = s_length_base[counter - 257];
                if (num_extra) {
                    mz_uint extra_bits;
                    TINFL_GET_BITS(25, extra_bits, num_extra);
                    counter += extra_bits;
                }

                TINFL_HUFF_DECODE(26, dist, &r->m_tables[1]);
                num_extra = s_dist_extra[dist];
                dist      = s_dist_base[dist];
                if (num_extra) {
                    mz_uint extra_bits;
                    TINFL_GET_BITS(27, extra_bits, num_extra);
                    dist += extra_bits;
                }

                dist_from_out_buf_start = pOut_buf_cur - pOut_buf_start;
                if ((dist > dist_from_out_buf_start) && (decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF)) {
                    TINFL_CR_RETURN_FOREVER(37, TINFL_STATUS_FAILED);
                }

                pSrc = pOut_buf_start + ((dist_from_out_buf_start - dist) & out_buf_size_mask);

                if ((MZ_MAX(pOut_buf_cur, pSrc) + counter) > pOut_buf_end) {
                    while (counter--) {
                        while (pOut_buf_cur >= pOut_buf_end) {
                            TINFL_CR_RETURN(53, TINFL_STATUS_HAS_MORE_OUTPUT);
                        }
                        *pOut_buf_cur++ = pOut_buf_start[(dist_from_out_buf_start++ - dist) & out_buf_size_mask];
                    }
                    continue;
                }
 #if MINIZ_USE_UNALIGNED_LOADS_AND_STORES
                else if ((counter >= 9) && (counter <= dist)) {
                    const mz_uint8 *pSrc_end = pSrc + (counter & ~7);
                    do {
                        ((mz_uint32 *)pOut_buf_cur)[0] = ((const mz_uint32 *)pSrc)[0];
                        ((mz_uint32 *)pOut_buf_cur)[1] = ((const mz_uint32 *)pSrc)[1];
                        pOut_buf_cur += 8;
                    } while ((pSrc += 8) < pSrc_end);
                    if ((counter &= 7) < 3) {
                        if (counter) {
                            pOut_buf_cur[0] = pSrc[0];
                            if (counter > 1)
                                pOut_buf_cur[1] = pSrc[1];
                            pOut_buf_cur += counter;
                        }
                        continue;
                    }
                }
 #endif
                do {
                    pOut_buf_cur[0] = pSrc[0];
                    pOut_buf_cur[1] = pSrc[1];
                    pOut_buf_cur[2] = pSrc[2];
                    pOut_buf_cur   += 3;
                    pSrc           += 3;
                } while ((int)(counter -= 3) > 2);
                if ((int)counter > 0) {
                    pOut_buf_cur[0] = pSrc[0];
                    if ((int)counter > 1)
                        pOut_buf_cur[1] = pSrc[1];
                    pOut_buf_cur += counter;
                }
            }
        }
    } while (!(r->m_final & 1));
    if (decomp_flags & TINFL_FLAG_PARSE_ZLIB_HEADER) {
        TINFL_SKIP_BITS(32, num_bits & 7);
        for (counter = 0; counter < 4; ++counter) {
            mz_uint s;
            if (num_bits) TINFL_GET_BITS(41, s, 8);
            else TINFL_GET_BYTE(42, s);
            r->m_z_adler32 = (r->m_z_adler32 << 8) | s;
        }
    }
    TINFL_CR_RETURN_FOREVER(34, TINFL_STATUS_DONE);
    TINFL_CR_FINISH

common_exit:
    r->m_num_bits = num_bits;
    r->m_bit_buf   = bit_buf;
    r->m_dist      = dist;
    r->m_counter   = counter;
    r->m_num_extra = num_extra;
    r->m_dist_from_out_buf_start = dist_from_out_buf_start;
    *pIn_buf_size  = pIn_buf_cur - pIn_buf_next;
    *pOut_buf_size = pOut_buf_cur - pOut_buf_next;
    if ((decomp_flags & (TINFL_FLAG_PARSE_ZLIB_HEADER | TINFL_FLAG_COMPUTE_ADLER32)) && (status >= 0)) {
        const mz_uint8 *ptr    = pOut_buf_next;
        size_t         buf_len = *pOut_buf_size;
        mz_uint32      i, s1 = r->m_check_adler32 & 0xffff, s2 = r->m_check_adler32 >> 16;
        size_t         block_len = buf_len % 5552;
        while (buf_len) {
            for (i = 0; i + 7 < block_len; i += 8, ptr += 8) {
                s1 += ptr[0], s2 += s1;
                s1 += ptr[1], s2 += s1;
                s1 += ptr[2], s2 += s1;
                s1 += ptr[3], s2 += s1;
                s1 += ptr[4], s2 += s1;
                s1 += ptr[5], s2 += s1;
                s1 += ptr[6], s2 += s1;
                s1 += ptr[7], s2 += s1;
            }
            for ( ; i < block_len; ++i) s1 += *ptr++, s2 += s1;
            s1       %= 65521U, s2 %= 65521U;
            buf_len  -= block_len;
            block_len = 5552;
        }
        r->m_check_adler32 = (s2 << 16) + s1;
        if ((status == TINFL_STATUS_DONE) && (decomp_flags & TINFL_FLAG_PARSE_ZLIB_HEADER) && (r->m_check_adler32 != r->m_z_adler32)) status = TINFL_STATUS_ADLER32_MISMATCH;
    }
    return status;
}

// Higher level helper functions.
void *tinfl_decompress_mem_to_heap(const void *pSrc_buf, size_t src_buf_len, size_t *pOut_len, int flags) {
    tinfl_decompressor decomp;
    void   *pBuf       = NULL, *pNew_buf;
    size_t src_buf_ofs = 0, out_buf_capacity = 0;

    *pOut_len = 0;
    tinfl_init(&decomp);
    for ( ; ; ) {
        size_t       src_buf_size = src_buf_len - src_buf_ofs, dst_buf_size = out_buf_capacity - *pOut_len, new_out_buf_capacity;
        tinfl_status status       = tinfl_decompress(&decomp, (const mz_uint8 *)pSrc_buf + src_buf_ofs, &src_buf_size, (mz_uint8 *)pBuf, pBuf ? (mz_uint8 *)pBuf + *pOut_len : NULL, &dst_buf_size,
                                                     (flags & ~TINFL_FLAG_HAS_MORE_INPUT) | TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF);
        if ((status < 0) || (status == TINFL_STATUS_NEEDS_MORE_INPUT)) {
            MZ_FREE(pBuf);
            *pOut_len = 0;
            return NULL;
        }
        src_buf_ofs += src_buf_size;
        *pOut_len   += dst_buf_size;
        if (status == TINFL_STATUS_DONE) break;
        new_out_buf_capacity = out_buf_capacity * 2;
        if (new_out_buf_capacity < 128) new_out_buf_capacity = 128;
        pNew_buf = MZ_REALLOC(pBuf, new_out_buf_capacity);
        if (!pNew_buf) {
            MZ_FREE(pBuf);
            *pOut_len = 0;
            return NULL;
        }
        pBuf             = pNew_buf;
        out_buf_capacity = new_out_buf_capacity;
    }
    return pBuf;
}

size_t tinfl_decompress_mem_to_mem(void *pOut_buf, size_t out_buf_len, const void *pSrc_buf, size_t src_buf_len, int flags) {
    tinfl_decompressor decomp;
    tinfl_status       status;

    tinfl_init(&decomp);

    status = tinfl_decompress(&decomp, (const mz_uint8 *)pSrc_buf, &src_buf_len, (mz_uint8 *)pOut_buf, (mz_uint8 *)pOut_buf, &out_buf_len, (flags & ~TINFL_FLAG_HAS_MORE_INPUT) | TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF);
    return (status != TINFL_STATUS_DONE) ? TINFL_DECOMPRESS_MEM_TO_MEM_FAILED : out_buf_len;
}

int tinfl_decompress_mem_to_callback(const void *pIn_buf, size_t *pIn_buf_size, tinfl_put_buf_func_ptr pPut_buf_func, void *pPut_buf_user, int flags) {
    int result = 0;
    tinfl_decompressor decomp;
    mz_uint8           *pDict     = (mz_uint8 *)MZ_MALLOC(TINFL_LZ_DICT_SIZE);
    size_t             in_buf_ofs = 0, dict_ofs = 0;

    if (!pDict)
        return TINFL_STATUS_FAILED;
    tinfl_init(&decomp);
    for ( ; ; ) {
        size_t       in_buf_size = *pIn_buf_size - in_buf_ofs, dst_buf_size = TINFL_LZ_DICT_SIZE - dict_ofs;
        tinfl_status status      = tinfl_decompress(&decomp, (const mz_uint8 *)pIn_buf + in_buf_ofs, &in_buf_size, pDict, pDict + dict_ofs, &dst_buf_size,
                                                    (flags & ~(TINFL_FLAG_HAS_MORE_INPUT | TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF)));
        in_buf_ofs += in_buf_size;
        if ((dst_buf_size) && (!(*pPut_buf_func)(pDict + dict_ofs, (int)dst_buf_size, pPut_buf_user)))
            break;
        if (status != TINFL_STATUS_HAS_MORE_OUTPUT) {
            result = (status == TINFL_STATUS_DONE);
            break;
        }
        dict_ofs = (dict_ofs + dst_buf_size) & (TINFL_LZ_DICT_SIZE - 1);
    }
    MZ_FREE(pDict);
    *pIn_buf_size = in_buf_ofs;
    return result;
}

// ------------------- Low-level Compression (independent from all decompression API's)

// Purposely making these tables static for faster init and thread safety.
static const mz_uint16 s_tdefl_len_sym[256] = {
    257, 258, 259, 260, 261, 262, 263, 264, 265, 265, 266, 266, 267, 267, 268, 268, 269, 269, 269, 269, 270, 270, 270, 270, 271, 271, 271, 271, 272, 272, 272, 272,
    273, 273, 273, 273, 273, 273, 273, 273, 274, 274, 274, 274, 274, 274, 274, 274, 275, 275, 275, 275, 275, 275, 275, 275, 276, 276, 276, 276, 276, 276, 276, 276,
    277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
    279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
    281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281,
    282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
    283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283,
    284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 285
};

static const mz_uint8 s_tdefl_len_extra[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0
};

static const mz_uint8 s_tdefl_small_dist_sym[512] = {
    0,   1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17
};

static const mz_uint8 s_tdefl_small_dist_extra[512] = {
    0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7
};

static const mz_uint8 s_tdefl_large_dist_sym[128] = {
    0,   0, 18, 19, 20, 20, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
};

static const mz_uint8 s_tdefl_large_dist_extra[128] = {
    0,   0,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13
};

// Radix sorts tdefl_sym_freq[] array by 16-bit key m_key. Returns ptr to sorted values.
typedef struct {
    mz_uint16 m_key, m_sym_index;
} tdefl_sym_freq;
static tdefl_sym_freq *tdefl_radix_sort_syms(mz_uint num_syms, tdefl_sym_freq *pSyms0, tdefl_sym_freq *pSyms1) {
    mz_uint32      total_passes = 2, pass_shift, pass, i, hist[256 * 2];
    tdefl_sym_freq *pCur_syms   = pSyms0, *pNew_syms = pSyms1;

    MZ_CLEAR_OBJ(hist);

    for (i = 0; i < num_syms; i++) {
        mz_uint freq = pSyms0[i].m_key;
        hist[freq & 0xFF]++;
        hist[256 + ((freq >> 8) & 0xFF)]++;
    }
    while ((total_passes > 1) && (num_syms == hist[(total_passes - 1) * 256])) total_passes--;
    for (pass_shift = 0, pass = 0; pass < total_passes; pass++, pass_shift += 8) {
        const mz_uint32 *pHist = &hist[pass << 8];
        mz_uint         offsets[256], cur_ofs = 0;
        for (i = 0; i < 256; i++) {
            offsets[i] = cur_ofs;
            cur_ofs   += pHist[i];
        }
        for (i = 0; i < num_syms; i++) pNew_syms[offsets[(pCur_syms[i].m_key >> pass_shift) & 0xFF]++] = pCur_syms[i];
        {
            tdefl_sym_freq *t = pCur_syms;
            pCur_syms = pNew_syms;
            pNew_syms = t;
        }
    }
    return pCur_syms;
}

// tdefl_calculate_minimum_redundancy() originally written by: Alistair Moffat, alistair@cs.mu.oz.au, Jyrki Katajainen, jyrki@diku.dk, November 1996.
static void tdefl_calculate_minimum_redundancy(tdefl_sym_freq *A, int n) {
    int root, leaf, next, avbl, used, dpth;

    if (n == 0) return;
    else if (n == 1) {
        A[0].m_key = 1;
        return;
    }
    A[0].m_key += A[1].m_key;
    root        = 0;
    leaf        = 2;
    for (next = 1; next < n - 1; next++) {
        if ((leaf >= n) || (A[root].m_key < A[leaf].m_key)) {
            A[next].m_key   = A[root].m_key;
            A[root++].m_key = (mz_uint16)next;
        } else A[next].m_key = A[leaf++].m_key;
        if ((leaf >= n) || ((root < next) && (A[root].m_key < A[leaf].m_key))) {
            A[next].m_key   = (mz_uint16)(A[next].m_key + A[root].m_key);
            A[root++].m_key = (mz_uint16)next;
        } else A[next].m_key = (mz_uint16)(A[next].m_key + A[leaf++].m_key);
    }
    A[n - 2].m_key = 0;
    for (next = n - 3; next >= 0; next--) A[next].m_key = A[A[next].m_key].m_key + 1;
    avbl = 1;
    used = dpth = 0;
    root = n - 2;
    next = n - 1;
    while (avbl > 0) {
        while (root >= 0 && (int)A[root].m_key == dpth) {
            used++;
            root--;
        }
        while (avbl > used) {
            A[next--].m_key = (mz_uint16)(dpth);
            avbl--;
        }
        avbl = 2 * used;
        dpth++;
        used = 0;
    }
}

// Limits canonical Huffman code table's max code size.
enum {
    TDEFL_MAX_SUPPORTED_HUFF_CODESIZE = 32
};
static void tdefl_huffman_enforce_max_code_size(int *pNum_codes, int code_list_len, int max_code_size) {
    int       i;
    mz_uint32 total = 0;

    if (code_list_len <= 1) return;

    for (i = max_code_size + 1; i <= TDEFL_MAX_SUPPORTED_HUFF_CODESIZE; i++) pNum_codes[max_code_size] += pNum_codes[i];
    for (i = max_code_size; i > 0; i--) total += (((mz_uint32)pNum_codes[i]) << (max_code_size - i));
    while (total != (1UL << max_code_size)) {
        pNum_codes[max_code_size]--;
        for (i = max_code_size - 1; i > 0; i--) if (pNum_codes[i]) {
                pNum_codes[i]--;
                pNum_codes[i + 1] += 2;
                break;
            }
        total--;
    }
}

static void tdefl_optimize_huffman_table(tdefl_compressor *d, int table_num, int table_len, int code_size_limit, int static_table) {
    int     i, j, l, num_codes[1 + TDEFL_MAX_SUPPORTED_HUFF_CODESIZE];
    mz_uint next_code[TDEFL_MAX_SUPPORTED_HUFF_CODESIZE + 1];

    MZ_CLEAR_OBJ(num_codes);

    if (static_table) {
        for (i = 0; i < table_len; i++) num_codes[d->m_huff_code_sizes[table_num][i]]++;
    } else {
        tdefl_sym_freq  syms0[TDEFL_MAX_HUFF_SYMBOLS], syms1[TDEFL_MAX_HUFF_SYMBOLS], *pSyms;
        int             num_used_syms = 0;
        const mz_uint16 *pSym_count   = &d->m_huff_count[table_num][0];
        for (i = 0; i < table_len; i++) if (pSym_count[i]) {
                syms0[num_used_syms].m_key         = (mz_uint16)pSym_count[i];
                syms0[num_used_syms++].m_sym_index = (mz_uint16)i;
            }

        pSyms = tdefl_radix_sort_syms(num_used_syms, syms0, syms1);
        tdefl_calculate_minimum_redundancy(pSyms, num_used_syms);

        for (i = 0; i < num_used_syms; i++) num_codes[pSyms[i].m_key]++;

        tdefl_huffman_enforce_max_code_size(num_codes, num_used_syms, code_size_limit);

        MZ_CLEAR_OBJ(d->m_huff_code_sizes[table_num]);
        MZ_CLEAR_OBJ(d->m_huff_codes[table_num]);
        for (i = 1, j = num_used_syms; i <= code_size_limit; i++)
            for (l = num_codes[i]; l > 0; l--) d->m_huff_code_sizes[table_num][pSyms[--j].m_sym_index] = (mz_uint8)(i);
    }

    next_code[1] = 0;
    for (j = 0, i = 2; i <= code_size_limit; i++) next_code[i] = j = ((j + num_codes[i - 1]) << 1);

    for (i = 0; i < table_len; i++) {
        mz_uint rev_code = 0, code, code_size;
        if ((code_size = d->m_huff_code_sizes[table_num][i]) == 0) continue;
        code = next_code[code_size]++;
        for (l = code_size; l > 0; l--, code >>= 1) rev_code = (rev_code << 1) | (code & 1);
        d->m_huff_codes[table_num][i] = (mz_uint16)rev_code;
    }
}

 #define TDEFL_PUT_BITS(b, l)                                                     \
    do {                                                                          \
        mz_uint bits = b; mz_uint len = l; MZ_ASSERT(bits <= ((1U << len) - 1U)); \
        d->m_bit_buffer |= (bits << d->m_bits_in); d->m_bits_in += len;           \
        while (d->m_bits_in >= 8) {                                               \
            if (d->m_pOutput_buf < d->m_pOutput_buf_end)                          \
                *d->m_pOutput_buf++ = (mz_uint8)(d->m_bit_buffer);                \
            d->m_bit_buffer >>= 8;                                                \
            d->m_bits_in     -= 8;                                                \
        }                                                                         \
    } MZ_MACRO_END

 #define TDEFL_RLE_PREV_CODE_SIZE()                                                                                                                                                                             \
    { if (rle_repeat_count) {                                                                                                                                                                                   \
          if (rle_repeat_count < 3) {                                                                                                                                                                           \
              d->m_huff_count[2][prev_code_size] = (mz_uint16)(d->m_huff_count[2][prev_code_size] + rle_repeat_count);                                                                                          \
              while (rle_repeat_count--) packed_code_sizes[num_packed_code_sizes++] = prev_code_size;                                                                                                           \
          } else {                                                                                                                                                                                              \
              d->m_huff_count[2][16] = (mz_uint16)(d->m_huff_count[2][16] + 1); packed_code_sizes[num_packed_code_sizes++] = 16; packed_code_sizes[num_packed_code_sizes++] = (mz_uint8)(rle_repeat_count - 3); \
          } rle_repeat_count = 0; } }

 #define TDEFL_RLE_ZERO_CODE_SIZE()                                                                                                                                                                         \
    { if (rle_z_count) {                                                                                                                                                                                    \
          if (rle_z_count < 3) {                                                                                                                                                                            \
              d->m_huff_count[2][0] = (mz_uint16)(d->m_huff_count[2][0] + rle_z_count); while (rle_z_count--) packed_code_sizes[num_packed_code_sizes++] = 0;                                               \
          } else if (rle_z_count <= 10) {                                                                                                                                                                   \
              d->m_huff_count[2][17] = (mz_uint16)(d->m_huff_count[2][17] + 1); packed_code_sizes[num_packed_code_sizes++] = 17; packed_code_sizes[num_packed_code_sizes++] = (mz_uint8)(rle_z_count - 3);  \
          } else {                                                                                                                                                                                          \
              d->m_huff_count[2][18] = (mz_uint16)(d->m_huff_count[2][18] + 1); packed_code_sizes[num_packed_code_sizes++] = 18; packed_code_sizes[num_packed_code_sizes++] = (mz_uint8)(rle_z_count - 11); \
          } rle_z_count = 0; } }

static mz_uint8 s_tdefl_packed_code_size_syms_swizzle[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

static void tdefl_start_dynamic_block(tdefl_compressor *d) {
    int      num_lit_codes, num_dist_codes, num_bit_lengths;
    mz_uint  i, total_code_sizes_to_pack, num_packed_code_sizes, rle_z_count, rle_repeat_count, packed_code_sizes_index;
    mz_uint8 code_sizes_to_pack[TDEFL_MAX_HUFF_SYMBOLS_0 + TDEFL_MAX_HUFF_SYMBOLS_1], packed_code_sizes[TDEFL_MAX_HUFF_SYMBOLS_0 + TDEFL_MAX_HUFF_SYMBOLS_1], prev_code_size = 0xFF;

    d->m_huff_count[0][256] = 1;

    tdefl_optimize_huffman_table(d, 0, TDEFL_MAX_HUFF_SYMBOLS_0, 15, MZ_FALSE);
    tdefl_optimize_huffman_table(d, 1, TDEFL_MAX_HUFF_SYMBOLS_1, 15, MZ_FALSE);

    for (num_lit_codes = 286; num_lit_codes > 257; num_lit_codes--) if (d->m_huff_code_sizes[0][num_lit_codes - 1]) break;
    for (num_dist_codes = 30; num_dist_codes > 1; num_dist_codes--) if (d->m_huff_code_sizes[1][num_dist_codes - 1]) break;

    memcpy(code_sizes_to_pack, &d->m_huff_code_sizes[0][0], num_lit_codes);
    memcpy(code_sizes_to_pack + num_lit_codes, &d->m_huff_code_sizes[1][0], num_dist_codes);
    total_code_sizes_to_pack = num_lit_codes + num_dist_codes;
    num_packed_code_sizes    = 0;
    rle_z_count      = 0;
    rle_repeat_count = 0;

    memset(&d->m_huff_count[2][0], 0, sizeof(d->m_huff_count[2][0]) * TDEFL_MAX_HUFF_SYMBOLS_2);
    for (i = 0; i < total_code_sizes_to_pack; i++) {
        mz_uint8 code_size = code_sizes_to_pack[i];
        if (!code_size) {
            TDEFL_RLE_PREV_CODE_SIZE();
            if (++rle_z_count == 138) {
                TDEFL_RLE_ZERO_CODE_SIZE();
            }
        } else {
            TDEFL_RLE_ZERO_CODE_SIZE();
            if (code_size != prev_code_size) {
                TDEFL_RLE_PREV_CODE_SIZE();
                d->m_huff_count[2][code_size] = (mz_uint16)(d->m_huff_count[2][code_size] + 1);
                packed_code_sizes[num_packed_code_sizes++] = code_size;
            } else if (++rle_repeat_count == 6) {
                TDEFL_RLE_PREV_CODE_SIZE();
            }
        }
        prev_code_size = code_size;
    }
    if (rle_repeat_count) {
        TDEFL_RLE_PREV_CODE_SIZE();
    } else {
        TDEFL_RLE_ZERO_CODE_SIZE();
    }

    tdefl_optimize_huffman_table(d, 2, TDEFL_MAX_HUFF_SYMBOLS_2, 7, MZ_FALSE);

    TDEFL_PUT_BITS(2, 2);

    TDEFL_PUT_BITS(num_lit_codes - 257, 5);
    TDEFL_PUT_BITS(num_dist_codes - 1, 5);

    for (num_bit_lengths = 18; num_bit_lengths >= 0; num_bit_lengths--) if (d->m_huff_code_sizes[2][s_tdefl_packed_code_size_syms_swizzle[num_bit_lengths]]) break;
    num_bit_lengths = MZ_MAX(4, (num_bit_lengths + 1));
    TDEFL_PUT_BITS(num_bit_lengths - 4, 4);
    for (i = 0; (int)i < num_bit_lengths; i++) TDEFL_PUT_BITS(d->m_huff_code_sizes[2][s_tdefl_packed_code_size_syms_swizzle[i]], 3);

    for (packed_code_sizes_index = 0; packed_code_sizes_index < num_packed_code_sizes; ) {
        mz_uint code = packed_code_sizes[packed_code_sizes_index++];
        MZ_ASSERT(code < TDEFL_MAX_HUFF_SYMBOLS_2);
        TDEFL_PUT_BITS(d->m_huff_codes[2][code], d->m_huff_code_sizes[2][code]);
        if (code >= 16) TDEFL_PUT_BITS(packed_code_sizes[packed_code_sizes_index++], "\02\03\07"[code - 16]);
    }
}

static void tdefl_start_static_block(tdefl_compressor *d) {
    mz_uint  i;
    mz_uint8 *p = &d->m_huff_code_sizes[0][0];

    for (i = 0; i <= 143; ++i) *p++ = 8;
    for ( ; i <= 255; ++i) *p++ = 9;
    for ( ; i <= 279; ++i) *p++ = 7;
    for ( ; i <= 287; ++i) *p++ = 8;

    memset(d->m_huff_code_sizes[1], 5, 32);

    tdefl_optimize_huffman_table(d, 0, 288, 15, MZ_TRUE);
    tdefl_optimize_huffman_table(d, 1, 32, 15, MZ_TRUE);

    TDEFL_PUT_BITS(1, 2);
}

static const mz_uint mz_bitmasks[17] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF };

 #if MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN && MINIZ_HAS_64BIT_REGISTERS
static mz_bool tdefl_compress_lz_codes(tdefl_compressor *d) {
    mz_uint   flags;
    mz_uint8  *pLZ_codes;
    mz_uint8  *pOutput_buf      = d->m_pOutput_buf;
    mz_uint8  *pLZ_code_buf_end = d->m_pLZ_code_buf;
    mz_uint64 bit_buffer        = d->m_bit_buffer;
    mz_uint   bits_in           = d->m_bits_in;

  #define TDEFL_PUT_BITS_FAST(b, l)    { bit_buffer |= (((mz_uint64)(b)) << bits_in); bits_in += (l); }

    flags = 1;
    for (pLZ_codes = d->m_lz_code_buf; pLZ_codes < pLZ_code_buf_end; flags >>= 1) {
        if (flags == 1)
            flags = *pLZ_codes++ | 0x100;

        if (flags & 1) {
            mz_uint s0, s1, n0, n1, sym, num_extra_bits;
            mz_uint match_len = pLZ_codes[0], match_dist = *(const mz_uint16 *)(pLZ_codes + 1);
            pLZ_codes += 3;

            MZ_ASSERT(d->m_huff_code_sizes[0][s_tdefl_len_sym[match_len]]);
            TDEFL_PUT_BITS_FAST(d->m_huff_codes[0][s_tdefl_len_sym[match_len]], d->m_huff_code_sizes[0][s_tdefl_len_sym[match_len]]);
            TDEFL_PUT_BITS_FAST(match_len & mz_bitmasks[s_tdefl_len_extra[match_len]], s_tdefl_len_extra[match_len]);

            // This sequence coaxes MSVC into using cmov's vs. jmp's.
            s0             = s_tdefl_small_dist_sym[match_dist & 511];
            n0             = s_tdefl_small_dist_extra[match_dist & 511];
            s1             = s_tdefl_large_dist_sym[match_dist >> 8];
            n1             = s_tdefl_large_dist_extra[match_dist >> 8];
            sym            = (match_dist < 512) ? s0 : s1;
            num_extra_bits = (match_dist < 512) ? n0 : n1;

            MZ_ASSERT(d->m_huff_code_sizes[1][sym]);
            TDEFL_PUT_BITS_FAST(d->m_huff_codes[1][sym], d->m_huff_code_sizes[1][sym]);
            TDEFL_PUT_BITS_FAST(match_dist & mz_bitmasks[num_extra_bits], num_extra_bits);
        } else {
            mz_uint lit = *pLZ_codes++;
            MZ_ASSERT(d->m_huff_code_sizes[0][lit]);
            TDEFL_PUT_BITS_FAST(d->m_huff_codes[0][lit], d->m_huff_code_sizes[0][lit]);

            if (((flags & 2) == 0) && (pLZ_codes < pLZ_code_buf_end)) {
                flags >>= 1;
                lit     = *pLZ_codes++;
                MZ_ASSERT(d->m_huff_code_sizes[0][lit]);
                TDEFL_PUT_BITS_FAST(d->m_huff_codes[0][lit], d->m_huff_code_sizes[0][lit]);

                if (((flags & 2) == 0) && (pLZ_codes < pLZ_code_buf_end)) {
                    flags >>= 1;
                    lit     = *pLZ_codes++;
                    MZ_ASSERT(d->m_huff_code_sizes[0][lit]);
                    TDEFL_PUT_BITS_FAST(d->m_huff_codes[0][lit], d->m_huff_code_sizes[0][lit]);
                }
            }
        }

        if (pOutput_buf >= d->m_pOutput_buf_end)
            return MZ_FALSE;

        *(mz_uint64 *)pOutput_buf = bit_buffer;
        pOutput_buf += (bits_in >> 3);
        bit_buffer >>= (bits_in & ~7);
        bits_in     &= 7;
    }

  #undef TDEFL_PUT_BITS_FAST

    d->m_pOutput_buf = pOutput_buf;
    d->m_bits_in     = 0;
    d->m_bit_buffer  = 0;

    while (bits_in) {
        mz_uint32 n = MZ_MIN(bits_in, 16);
        TDEFL_PUT_BITS((mz_uint)bit_buffer & mz_bitmasks[n], n);
        bit_buffer >>= n;
        bits_in     -= n;
    }

    TDEFL_PUT_BITS(d->m_huff_codes[0][256], d->m_huff_code_sizes[0][256]);

    return d->m_pOutput_buf < d->m_pOutput_buf_end;
}

 #else
static mz_bool tdefl_compress_lz_codes(tdefl_compressor *d) {
    mz_uint  flags;
    mz_uint8 *pLZ_codes;

    flags = 1;
    for (pLZ_codes = d->m_lz_code_buf; pLZ_codes < d->m_pLZ_code_buf; flags >>= 1) {
        if (flags == 1)
            flags = *pLZ_codes++ | 0x100;
        if (flags & 1) {
            mz_uint sym, num_extra_bits;
            mz_uint match_len = pLZ_codes[0], match_dist = (pLZ_codes[1] | (pLZ_codes[2] << 8));
            pLZ_codes += 3;

            MZ_ASSERT(d->m_huff_code_sizes[0][s_tdefl_len_sym[match_len]]);
            TDEFL_PUT_BITS(d->m_huff_codes[0][s_tdefl_len_sym[match_len]], d->m_huff_code_sizes[0][s_tdefl_len_sym[match_len]]);
            TDEFL_PUT_BITS(match_len & mz_bitmasks[s_tdefl_len_extra[match_len]], s_tdefl_len_extra[match_len]);

            if (match_dist < 512) {
                sym            = s_tdefl_small_dist_sym[match_dist];
                num_extra_bits = s_tdefl_small_dist_extra[match_dist];
            } else {
                sym            = s_tdefl_large_dist_sym[match_dist >> 8];
                num_extra_bits = s_tdefl_large_dist_extra[match_dist >> 8];
            }
            MZ_ASSERT(d->m_huff_code_sizes[1][sym]);
            TDEFL_PUT_BITS(d->m_huff_codes[1][sym], d->m_huff_code_sizes[1][sym]);
            TDEFL_PUT_BITS(match_dist & mz_bitmasks[num_extra_bits], num_extra_bits);
        } else {
            mz_uint lit = *pLZ_codes++;
            MZ_ASSERT(d->m_huff_code_sizes[0][lit]);
            TDEFL_PUT_BITS(d->m_huff_codes[0][lit], d->m_huff_code_sizes[0][lit]);
        }
    }

    TDEFL_PUT_BITS(d->m_huff_codes[0][256], d->m_huff_code_sizes[0][256]);

    return d->m_pOutput_buf < d->m_pOutput_buf_end;
}
 #endif // MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN && MINIZ_HAS_64BIT_REGISTERS

static mz_bool tdefl_compress_block(tdefl_compressor *d, mz_bool static_block) {
    if (static_block)
        tdefl_start_static_block(d);
    else
        tdefl_start_dynamic_block(d);
    return tdefl_compress_lz_codes(d);
}

static int tdefl_flush_block(tdefl_compressor *d, int flush) {
    mz_uint  saved_bit_buf, saved_bits_in;
    mz_uint8 *pSaved_output_buf;
    mz_bool  comp_block_succeeded = MZ_FALSE;
    int      n, use_raw_block = ((d->m_flags & TDEFL_FORCE_ALL_RAW_BLOCKS) != 0) && (d->m_lookahead_pos - d->m_lz_code_buf_dict_pos) <= d->m_dict_size;
    mz_uint8 *pOutput_buf_start = ((d->m_pPut_buf_func == NULL) && ((*d->m_pOut_buf_size - d->m_out_buf_ofs) >= TDEFL_OUT_BUF_SIZE)) ? ((mz_uint8 *)d->m_pOut_buf + d->m_out_buf_ofs) : d->m_output_buf;

    d->m_pOutput_buf     = pOutput_buf_start;
    d->m_pOutput_buf_end = d->m_pOutput_buf + TDEFL_OUT_BUF_SIZE - 16;

    MZ_ASSERT(!d->m_output_flush_remaining);
    d->m_output_flush_ofs       = 0;
    d->m_output_flush_remaining = 0;

    *d->m_pLZ_flags    = (mz_uint8)(*d->m_pLZ_flags >> d->m_num_flags_left);
    d->m_pLZ_code_buf -= (d->m_num_flags_left == 8);

    if ((d->m_flags & TDEFL_WRITE_ZLIB_HEADER) && (!d->m_block_index)) {
        TDEFL_PUT_BITS(0x78, 8);
        TDEFL_PUT_BITS(0x01, 8);
    }

    TDEFL_PUT_BITS(flush == TDEFL_FINISH, 1);

    pSaved_output_buf = d->m_pOutput_buf;
    saved_bit_buf     = d->m_bit_buffer;
    saved_bits_in     = d->m_bits_in;

    if (!use_raw_block)
        comp_block_succeeded = tdefl_compress_block(d, (d->m_flags & TDEFL_FORCE_ALL_STATIC_BLOCKS) || (d->m_total_lz_bytes < 48));

    // If the block gets expanded, forget the current contents of the output buffer and send a raw block instead.
    if (((use_raw_block) || ((d->m_total_lz_bytes) && ((d->m_pOutput_buf - pSaved_output_buf + 1U) >= d->m_total_lz_bytes))) &&
        ((d->m_lookahead_pos - d->m_lz_code_buf_dict_pos) <= d->m_dict_size)) {
        mz_uint i;
        d->m_pOutput_buf = pSaved_output_buf;
        d->m_bit_buffer  = saved_bit_buf, d->m_bits_in = saved_bits_in;
        TDEFL_PUT_BITS(0, 2);
        if (d->m_bits_in) {
            TDEFL_PUT_BITS(0, 8 - d->m_bits_in);
        }
        for (i = 2; i; --i, d->m_total_lz_bytes ^= 0xFFFF) {
            TDEFL_PUT_BITS(d->m_total_lz_bytes & 0xFFFF, 16);
        }
        for (i = 0; i < d->m_total_lz_bytes; ++i) {
            TDEFL_PUT_BITS(d->m_dict[(d->m_lz_code_buf_dict_pos + i) & TDEFL_LZ_DICT_SIZE_MASK], 8);
        }
    }
    // Check for the extremely unlikely (if not impossible) case of the compressed block not fitting into the output buffer when using dynamic codes.
    else if (!comp_block_succeeded) {
        d->m_pOutput_buf = pSaved_output_buf;
        d->m_bit_buffer  = saved_bit_buf, d->m_bits_in = saved_bits_in;
        tdefl_compress_block(d, MZ_TRUE);
    }

    if (flush) {
        if (flush == TDEFL_FINISH) {
            if (d->m_bits_in) {
                TDEFL_PUT_BITS(0, 8 - d->m_bits_in);
            }
            if (d->m_flags & TDEFL_WRITE_ZLIB_HEADER) {
                mz_uint i, a = d->m_adler32;
                for (i = 0; i < 4; i++) {
                    TDEFL_PUT_BITS((a >> 24) & 0xFF, 8);
                    a <<= 8;
                }
            }
        } else {
            mz_uint i, z = 0;
            TDEFL_PUT_BITS(0, 3);
            if (d->m_bits_in) {
                TDEFL_PUT_BITS(0, 8 - d->m_bits_in);
            }
            for (i = 2; i; --i, z ^= 0xFFFF) {
                TDEFL_PUT_BITS(z & 0xFFFF, 16);
            }
        }
    }

    MZ_ASSERT(d->m_pOutput_buf < d->m_pOutput_buf_end);

    memset(&d->m_huff_count[0][0], 0, sizeof(d->m_huff_count[0][0]) * TDEFL_MAX_HUFF_SYMBOLS_0);
    memset(&d->m_huff_count[1][0], 0, sizeof(d->m_huff_count[1][0]) * TDEFL_MAX_HUFF_SYMBOLS_1);

    d->m_pLZ_code_buf          = d->m_lz_code_buf + 1;
    d->m_pLZ_flags             = d->m_lz_code_buf;
    d->m_num_flags_left        = 8;
    d->m_lz_code_buf_dict_pos += d->m_total_lz_bytes;
    d->m_total_lz_bytes        = 0;
    d->m_block_index++;

    if ((n = (int)(d->m_pOutput_buf - pOutput_buf_start)) != 0) {
        if (d->m_pPut_buf_func) {
            *d->m_pIn_buf_size = d->m_pSrc - (const mz_uint8 *)d->m_pIn_buf;
            if (!(*d->m_pPut_buf_func)(d->m_output_buf, n, d->m_pPut_buf_user))
                return d->m_prev_return_status = TDEFL_STATUS_PUT_BUF_FAILED;
        } else if (pOutput_buf_start == d->m_output_buf) {
            int bytes_to_copy = (int)MZ_MIN((size_t)n, (size_t)(*d->m_pOut_buf_size - d->m_out_buf_ofs));
            memcpy((mz_uint8 *)d->m_pOut_buf + d->m_out_buf_ofs, d->m_output_buf, bytes_to_copy);
            d->m_out_buf_ofs += bytes_to_copy;
            if ((n -= bytes_to_copy) != 0) {
                d->m_output_flush_ofs       = bytes_to_copy;
                d->m_output_flush_remaining = n;
            }
        } else {
            d->m_out_buf_ofs += n;
        }
    }

    return d->m_output_flush_remaining;
}

 #if MINIZ_USE_UNALIGNED_LOADS_AND_STORES
  #define TDEFL_READ_UNALIGNED_WORD(p)    *(const mz_uint16 *)(p)
static MZ_FORCEINLINE void tdefl_find_match(tdefl_compressor *d, mz_uint lookahead_pos, mz_uint max_dist, mz_uint max_match_len, mz_uint *pMatch_dist, mz_uint *pMatch_len) {
    mz_uint         dist, pos = lookahead_pos & TDEFL_LZ_DICT_SIZE_MASK, match_len = *pMatch_len, probe_pos = pos, next_probe_pos, probe_len;
    mz_uint         num_probes_left = d->m_max_probes[match_len >= 32];
    const mz_uint16 *s  = (const mz_uint16 *)(d->m_dict + pos), *p, *q;
    mz_uint16       c01 = TDEFL_READ_UNALIGNED_WORD(&d->m_dict[pos + match_len - 1]), s01 = TDEFL_READ_UNALIGNED_WORD(s);

    MZ_ASSERT(max_match_len <= TDEFL_MAX_MATCH_LEN);
    if (max_match_len <= match_len) return;
    for ( ; ; ) {
        for ( ; ; ) {
            if (--num_probes_left == 0) return;
  #define TDEFL_PROBE                                                                                   \
    next_probe_pos = d->m_next[probe_pos];                                                              \
    if ((!next_probe_pos) || ((dist = (mz_uint16)(lookahead_pos - next_probe_pos)) > max_dist)) return; \
    probe_pos = next_probe_pos & TDEFL_LZ_DICT_SIZE_MASK;                                               \
    if (TDEFL_READ_UNALIGNED_WORD(&d->m_dict[probe_pos + match_len - 1]) == c01) break;
            TDEFL_PROBE;
            TDEFL_PROBE;
            TDEFL_PROBE;
        }
        if (!dist) break;
        q = (const mz_uint16 *)(d->m_dict + probe_pos);
        if (TDEFL_READ_UNALIGNED_WORD(q) != s01) continue;
        p         = s;
        probe_len = 32;
        do { } while ((TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) && (TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) &&
                      (TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) && (TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) && (--probe_len > 0));
        if (!probe_len) {
            *pMatch_dist = dist;
            *pMatch_len  = MZ_MIN(max_match_len, TDEFL_MAX_MATCH_LEN);
            break;
        } else if ((probe_len = ((mz_uint)(p - s) * 2) + (mz_uint)(*(const mz_uint8 *)p == *(const mz_uint8 *)q)) > match_len) {
            *pMatch_dist = dist;
            if ((*pMatch_len = match_len = MZ_MIN(max_match_len, probe_len)) == max_match_len) break;
            c01 = TDEFL_READ_UNALIGNED_WORD(&d->m_dict[pos + match_len - 1]);
        }
    }
}

 #else
static MZ_FORCEINLINE void tdefl_find_match(tdefl_compressor *d, mz_uint lookahead_pos, mz_uint max_dist, mz_uint max_match_len, mz_uint *pMatch_dist, mz_uint *pMatch_len) {
    mz_uint        dist, pos = lookahead_pos & TDEFL_LZ_DICT_SIZE_MASK, match_len = *pMatch_len, probe_pos = pos, next_probe_pos, probe_len;
    mz_uint        num_probes_left = d->m_max_probes[match_len >= 32];
    const mz_uint8 *s = d->m_dict + pos, *p, *q;
    mz_uint8       c0 = d->m_dict[pos + match_len], c1 = d->m_dict[pos + match_len - 1];

    MZ_ASSERT(max_match_len <= TDEFL_MAX_MATCH_LEN);
    if (max_match_len <= match_len) return;
    for ( ; ; ) {
        for ( ; ; ) {
            if (--num_probes_left == 0) return;
  #define TDEFL_PROBE                                                                                   \
    next_probe_pos = d->m_next[probe_pos];                                                              \
    if ((!next_probe_pos) || ((dist = (mz_uint16)(lookahead_pos - next_probe_pos)) > max_dist)) return; \
    probe_pos = next_probe_pos & TDEFL_LZ_DICT_SIZE_MASK;                                               \
    if ((d->m_dict[probe_pos + match_len] == c0) && (d->m_dict[probe_pos + match_len - 1] == c1)) break;
            TDEFL_PROBE;
            TDEFL_PROBE;
            TDEFL_PROBE;
        }
        if (!dist) break;
        p = s;
        q = d->m_dict + probe_pos;
        for (probe_len = 0; probe_len < max_match_len; probe_len++) if (*p++ != *q++) break;
        if (probe_len > match_len) {
            *pMatch_dist = dist;
            if ((*pMatch_len = match_len = probe_len) == max_match_len) return;
            c0 = d->m_dict[pos + match_len];
            c1 = d->m_dict[pos + match_len - 1];
        }
    }
}
 #endif // #if MINIZ_USE_UNALIGNED_LOADS_AND_STORES

 #if MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN
static mz_bool tdefl_compress_fast(tdefl_compressor *d) {
    // Faster, minimally featured LZRW1-style match+parse loop with better register utilization. Intended for applications where raw throughput is valued more highly than ratio.
    mz_uint lookahead_pos  = d->m_lookahead_pos, lookahead_size = d->m_lookahead_size, dict_size = d->m_dict_size, total_lz_bytes = d->m_total_lz_bytes, num_flags_left = d->m_num_flags_left;
    mz_uint8 *pLZ_code_buf = d->m_pLZ_code_buf, *pLZ_flags = d->m_pLZ_flags;
    mz_uint cur_pos        = lookahead_pos & TDEFL_LZ_DICT_SIZE_MASK;

    while ((d->m_src_buf_left) || ((d->m_flush) && (lookahead_size))) {
        const mz_uint TDEFL_COMP_FAST_LOOKAHEAD_SIZE = 4096;
        mz_uint dst_pos = (lookahead_pos + lookahead_size) & TDEFL_LZ_DICT_SIZE_MASK;
        mz_uint num_bytes_to_process = (mz_uint)MZ_MIN(d->m_src_buf_left, TDEFL_COMP_FAST_LOOKAHEAD_SIZE - lookahead_size);
        d->m_src_buf_left -= num_bytes_to_process;
        lookahead_size    += num_bytes_to_process;

        while (num_bytes_to_process) {
            mz_uint32 n = MZ_MIN(TDEFL_LZ_DICT_SIZE - dst_pos, num_bytes_to_process);
            memcpy(d->m_dict + dst_pos, d->m_pSrc, n);
            if (dst_pos < (TDEFL_MAX_MATCH_LEN - 1))
                memcpy(d->m_dict + TDEFL_LZ_DICT_SIZE + dst_pos, d->m_pSrc, MZ_MIN(n, (TDEFL_MAX_MATCH_LEN - 1) - dst_pos));
            d->m_pSrc            += n;
            dst_pos               = (dst_pos + n) & TDEFL_LZ_DICT_SIZE_MASK;
            num_bytes_to_process -= n;
        }

        dict_size = MZ_MIN(TDEFL_LZ_DICT_SIZE - lookahead_size, dict_size);
        if ((!d->m_flush) && (lookahead_size < TDEFL_COMP_FAST_LOOKAHEAD_SIZE)) break;

        while (lookahead_size >= 4) {
            mz_uint cur_match_dist, cur_match_len = 1;
            mz_uint8 *pCur_dict   = d->m_dict + cur_pos;
            mz_uint first_trigram = (*(const mz_uint32 *)pCur_dict) & 0xFFFFFF;
            mz_uint hash          = (first_trigram ^ (first_trigram >> (24 - (TDEFL_LZ_HASH_BITS - 8)))) & TDEFL_LEVEL1_HASH_SIZE_MASK;
            mz_uint probe_pos     = d->m_hash[hash];
            d->m_hash[hash] = (mz_uint16)lookahead_pos;

            if (((cur_match_dist = (mz_uint16)(lookahead_pos - probe_pos)) <= dict_size) && ((*(const mz_uint32 *)(d->m_dict + (probe_pos &= TDEFL_LZ_DICT_SIZE_MASK)) & 0xFFFFFF) == first_trigram)) {
                const mz_uint16 *p  = (const mz_uint16 *)pCur_dict;
                const mz_uint16 *q  = (const mz_uint16 *)(d->m_dict + probe_pos);
                mz_uint32 probe_len = 32;
                do { } while ((TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) && (TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) &&
                              (TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) && (TDEFL_READ_UNALIGNED_WORD(++p) == TDEFL_READ_UNALIGNED_WORD(++q)) && (--probe_len > 0));
                cur_match_len = ((mz_uint)(p - (const mz_uint16 *)pCur_dict) * 2) + (mz_uint)(*(const mz_uint8 *)p == *(const mz_uint8 *)q);
                if (!probe_len)
                    cur_match_len = cur_match_dist ? TDEFL_MAX_MATCH_LEN : 0;

                if ((cur_match_len < TDEFL_MIN_MATCH_LEN) || ((cur_match_len == TDEFL_MIN_MATCH_LEN) && (cur_match_dist >= 8U * 1024U))) {
                    cur_match_len   = 1;
                    *pLZ_code_buf++ = (mz_uint8)first_trigram;
                    *pLZ_flags      = (mz_uint8)(*pLZ_flags >> 1);
                    d->m_huff_count[0][(mz_uint8)first_trigram]++;
                } else {
                    mz_uint32 s0, s1;
                    cur_match_len = MZ_MIN(cur_match_len, lookahead_size);

                    MZ_ASSERT((cur_match_len >= TDEFL_MIN_MATCH_LEN) && (cur_match_dist >= 1) && (cur_match_dist <= TDEFL_LZ_DICT_SIZE));

                    cur_match_dist--;

                    pLZ_code_buf[0] = (mz_uint8)(cur_match_len - TDEFL_MIN_MATCH_LEN);
                    *(mz_uint16 *)(&pLZ_code_buf[1]) = (mz_uint16)cur_match_dist;
                    pLZ_code_buf += 3;
                    *pLZ_flags    = (mz_uint8)((*pLZ_flags >> 1) | 0x80);

                    s0 = s_tdefl_small_dist_sym[cur_match_dist & 511];
                    s1 = s_tdefl_large_dist_sym[cur_match_dist >> 8];
                    d->m_huff_count[1][(cur_match_dist < 512) ? s0 : s1]++;

                    d->m_huff_count[0][s_tdefl_len_sym[cur_match_len - TDEFL_MIN_MATCH_LEN]]++;
                }
            } else {
                *pLZ_code_buf++ = (mz_uint8)first_trigram;
                *pLZ_flags      = (mz_uint8)(*pLZ_flags >> 1);
                d->m_huff_count[0][(mz_uint8)first_trigram]++;
            }

            if (--num_flags_left == 0) {
                num_flags_left = 8;
                pLZ_flags      = pLZ_code_buf++;
            }

            total_lz_bytes += cur_match_len;
            lookahead_pos  += cur_match_len;
            dict_size       = MZ_MIN(dict_size + cur_match_len, TDEFL_LZ_DICT_SIZE);
            cur_pos         = (cur_pos + cur_match_len) & TDEFL_LZ_DICT_SIZE_MASK;
            MZ_ASSERT(lookahead_size >= cur_match_len);
            lookahead_size -= cur_match_len;

            if (pLZ_code_buf > &d->m_lz_code_buf[TDEFL_LZ_CODE_BUF_SIZE - 8]) {
                int n;
                d->m_lookahead_pos  = lookahead_pos;
                d->m_lookahead_size = lookahead_size;
                d->m_dict_size      = dict_size;
                d->m_total_lz_bytes = total_lz_bytes;
                d->m_pLZ_code_buf   = pLZ_code_buf;
                d->m_pLZ_flags      = pLZ_flags;
                d->m_num_flags_left = num_flags_left;
                if ((n = tdefl_flush_block(d, 0)) != 0)
                    return (n < 0) ? MZ_FALSE : MZ_TRUE;
                total_lz_bytes = d->m_total_lz_bytes;
                pLZ_code_buf   = d->m_pLZ_code_buf;
                pLZ_flags      = d->m_pLZ_flags;
                num_flags_left = d->m_num_flags_left;
            }
        }

        while (lookahead_size) {
            mz_uint8 lit = d->m_dict[cur_pos];

            total_lz_bytes++;
            *pLZ_code_buf++ = lit;
            *pLZ_flags      = (mz_uint8)(*pLZ_flags >> 1);
            if (--num_flags_left == 0) {
                num_flags_left = 8;
                pLZ_flags      = pLZ_code_buf++;
            }

            d->m_huff_count[0][lit]++;

            lookahead_pos++;
            dict_size = MZ_MIN(dict_size + 1, TDEFL_LZ_DICT_SIZE);
            cur_pos   = (cur_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK;
            lookahead_size--;

            if (pLZ_code_buf > &d->m_lz_code_buf[TDEFL_LZ_CODE_BUF_SIZE - 8]) {
                int n;
                d->m_lookahead_pos  = lookahead_pos;
                d->m_lookahead_size = lookahead_size;
                d->m_dict_size      = dict_size;
                d->m_total_lz_bytes = total_lz_bytes;
                d->m_pLZ_code_buf   = pLZ_code_buf;
                d->m_pLZ_flags      = pLZ_flags;
                d->m_num_flags_left = num_flags_left;
                if ((n = tdefl_flush_block(d, 0)) != 0)
                    return (n < 0) ? MZ_FALSE : MZ_TRUE;
                total_lz_bytes = d->m_total_lz_bytes;
                pLZ_code_buf   = d->m_pLZ_code_buf;
                pLZ_flags      = d->m_pLZ_flags;
                num_flags_left = d->m_num_flags_left;
            }
        }
    }

    d->m_lookahead_pos  = lookahead_pos;
    d->m_lookahead_size = lookahead_size;
    d->m_dict_size      = dict_size;
    d->m_total_lz_bytes = total_lz_bytes;
    d->m_pLZ_code_buf   = pLZ_code_buf;
    d->m_pLZ_flags      = pLZ_flags;
    d->m_num_flags_left = num_flags_left;
    return MZ_TRUE;
}
 #endif // MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN

static MZ_FORCEINLINE void tdefl_record_literal(tdefl_compressor *d, mz_uint8 lit) {
    d->m_total_lz_bytes++;
    *d->m_pLZ_code_buf++ = lit;
    *d->m_pLZ_flags      = (mz_uint8)(*d->m_pLZ_flags >> 1);
    if (--d->m_num_flags_left == 0) {
        d->m_num_flags_left = 8;
        d->m_pLZ_flags      = d->m_pLZ_code_buf++;
    }
    d->m_huff_count[0][lit]++;
}

static MZ_FORCEINLINE void tdefl_record_match(tdefl_compressor *d, mz_uint match_len, mz_uint match_dist) {
    mz_uint32 s0, s1;

    MZ_ASSERT((match_len >= TDEFL_MIN_MATCH_LEN) && (match_dist >= 1) && (match_dist <= TDEFL_LZ_DICT_SIZE));

    d->m_total_lz_bytes += match_len;

    d->m_pLZ_code_buf[0] = (mz_uint8)(match_len - TDEFL_MIN_MATCH_LEN);

    match_dist          -= 1;
    d->m_pLZ_code_buf[1] = (mz_uint8)(match_dist & 0xFF);
    d->m_pLZ_code_buf[2] = (mz_uint8)(match_dist >> 8);
    d->m_pLZ_code_buf   += 3;

    *d->m_pLZ_flags = (mz_uint8)((*d->m_pLZ_flags >> 1) | 0x80);
    if (--d->m_num_flags_left == 0) {
        d->m_num_flags_left = 8;
        d->m_pLZ_flags      = d->m_pLZ_code_buf++;
    }

    s0 = s_tdefl_small_dist_sym[match_dist & 511];
    s1 = s_tdefl_large_dist_sym[(match_dist >> 8) & 127];
    d->m_huff_count[1][(match_dist < 512) ? s0 : s1]++;

    if (match_len >= TDEFL_MIN_MATCH_LEN) d->m_huff_count[0][s_tdefl_len_sym[match_len - TDEFL_MIN_MATCH_LEN]]++;
}

static mz_bool tdefl_compress_normal(tdefl_compressor *d) {
    const mz_uint8 *pSrc = d->m_pSrc;
    size_t src_buf_left  = d->m_src_buf_left;
    tdefl_flush flush    = d->m_flush;

    while ((src_buf_left) || ((flush) && (d->m_lookahead_size))) {
        mz_uint len_to_move, cur_match_dist, cur_match_len, cur_pos;
        // Update dictionary and hash chains. Keeps the lookahead size equal to TDEFL_MAX_MATCH_LEN.
        if ((d->m_lookahead_size + d->m_dict_size) >= (TDEFL_MIN_MATCH_LEN - 1)) {
            mz_uint dst_pos = (d->m_lookahead_pos + d->m_lookahead_size) & TDEFL_LZ_DICT_SIZE_MASK, ins_pos = d->m_lookahead_pos + d->m_lookahead_size - 2;
            mz_uint hash    = (d->m_dict[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] << TDEFL_LZ_HASH_SHIFT) ^ d->m_dict[(ins_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK];
            mz_uint num_bytes_to_process = (mz_uint)MZ_MIN(src_buf_left, TDEFL_MAX_MATCH_LEN - d->m_lookahead_size);
            const mz_uint8 *pSrc_end     = pSrc + num_bytes_to_process;
            src_buf_left        -= num_bytes_to_process;
            d->m_lookahead_size += num_bytes_to_process;
            while (pSrc != pSrc_end) {
                mz_uint8 c = *pSrc++;
                d->m_dict[dst_pos] = c;
                if (dst_pos < (TDEFL_MAX_MATCH_LEN - 1)) d->m_dict[TDEFL_LZ_DICT_SIZE + dst_pos] = c;
                hash = ((hash << TDEFL_LZ_HASH_SHIFT) ^ c) & (TDEFL_LZ_HASH_SIZE - 1);
                d->m_next[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] = d->m_hash[hash];
                d->m_hash[hash] = (mz_uint16)(ins_pos);
                dst_pos         = (dst_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK;
                ins_pos++;
            }
        } else {
            while ((src_buf_left) && (d->m_lookahead_size < TDEFL_MAX_MATCH_LEN)) {
                mz_uint8 c      = *pSrc++;
                mz_uint dst_pos = (d->m_lookahead_pos + d->m_lookahead_size) & TDEFL_LZ_DICT_SIZE_MASK;
                src_buf_left--;
                d->m_dict[dst_pos] = c;
                if (dst_pos < (TDEFL_MAX_MATCH_LEN - 1))
                    d->m_dict[TDEFL_LZ_DICT_SIZE + dst_pos] = c;
                if ((++d->m_lookahead_size + d->m_dict_size) >= TDEFL_MIN_MATCH_LEN) {
                    mz_uint ins_pos = d->m_lookahead_pos + (d->m_lookahead_size - 1) - 2;
                    mz_uint hash    = ((d->m_dict[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] << (TDEFL_LZ_HASH_SHIFT * 2)) ^ (d->m_dict[(ins_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK] << TDEFL_LZ_HASH_SHIFT) ^ c) & (TDEFL_LZ_HASH_SIZE - 1);
                    d->m_next[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] = d->m_hash[hash];
                    d->m_hash[hash] = (mz_uint16)(ins_pos);
                }
            }
        }
        d->m_dict_size = MZ_MIN(TDEFL_LZ_DICT_SIZE - d->m_lookahead_size, d->m_dict_size);
        if ((!flush) && (d->m_lookahead_size < TDEFL_MAX_MATCH_LEN))
            break;

        // Simple lazy/greedy parsing state machine.
        len_to_move    = 1;
        cur_match_dist = 0;
        cur_match_len  = d->m_saved_match_len ? d->m_saved_match_len : (TDEFL_MIN_MATCH_LEN - 1);
        cur_pos        = d->m_lookahead_pos & TDEFL_LZ_DICT_SIZE_MASK;
        if (d->m_flags & (TDEFL_RLE_MATCHES | TDEFL_FORCE_ALL_RAW_BLOCKS)) {
            if ((d->m_dict_size) && (!(d->m_flags & TDEFL_FORCE_ALL_RAW_BLOCKS))) {
                mz_uint8 c = d->m_dict[(cur_pos - 1) & TDEFL_LZ_DICT_SIZE_MASK];
                cur_match_len = 0;
                while (cur_match_len < d->m_lookahead_size) {
                    if (d->m_dict[cur_pos + cur_match_len] != c) break;
                    cur_match_len++;
                }
                if (cur_match_len < TDEFL_MIN_MATCH_LEN) cur_match_len = 0;
                else cur_match_dist = 1;
            }
        } else {
            tdefl_find_match(d, d->m_lookahead_pos, d->m_dict_size, d->m_lookahead_size, &cur_match_dist, &cur_match_len);
        }
        if (((cur_match_len == TDEFL_MIN_MATCH_LEN) && (cur_match_dist >= 8U * 1024U)) || (cur_pos == cur_match_dist) || ((d->m_flags & TDEFL_FILTER_MATCHES) && (cur_match_len <= 5))) {
            cur_match_dist = cur_match_len = 0;
        }
        if (d->m_saved_match_len) {
            if (cur_match_len > d->m_saved_match_len) {
                tdefl_record_literal(d, (mz_uint8)d->m_saved_lit);
                if (cur_match_len >= 128) {
                    tdefl_record_match(d, cur_match_len, cur_match_dist);
                    d->m_saved_match_len = 0;
                    len_to_move          = cur_match_len;
                } else {
                    d->m_saved_lit        = d->m_dict[cur_pos];
                    d->m_saved_match_dist = cur_match_dist;
                    d->m_saved_match_len  = cur_match_len;
                }
            } else {
                tdefl_record_match(d, d->m_saved_match_len, d->m_saved_match_dist);
                len_to_move          = d->m_saved_match_len - 1;
                d->m_saved_match_len = 0;
            }
        } else if (!cur_match_dist)
            tdefl_record_literal(d, d->m_dict[MZ_MIN(cur_pos, sizeof(d->m_dict) - 1)]);
        else if ((d->m_greedy_parsing) || (d->m_flags & TDEFL_RLE_MATCHES) || (cur_match_len >= 128)) {
            tdefl_record_match(d, cur_match_len, cur_match_dist);
            len_to_move = cur_match_len;
        } else {
            d->m_saved_lit        = d->m_dict[MZ_MIN(cur_pos, sizeof(d->m_dict) - 1)];
            d->m_saved_match_dist = cur_match_dist;
            d->m_saved_match_len  = cur_match_len;
        }
        // Move the lookahead forward by len_to_move bytes.
        d->m_lookahead_pos += len_to_move;
        MZ_ASSERT(d->m_lookahead_size >= len_to_move);
        d->m_lookahead_size -= len_to_move;
        d->m_dict_size       = MZ_MIN(d->m_dict_size + len_to_move, TDEFL_LZ_DICT_SIZE);
        // Check if it's time to flush the current LZ codes to the internal output buffer.
        if ((d->m_pLZ_code_buf > &d->m_lz_code_buf[TDEFL_LZ_CODE_BUF_SIZE - 8]) ||
            ((d->m_total_lz_bytes > 31 * 1024) && (((((mz_uint)(d->m_pLZ_code_buf - d->m_lz_code_buf) * 115) >> 7) >= d->m_total_lz_bytes) || (d->m_flags & TDEFL_FORCE_ALL_RAW_BLOCKS)))) {
            int n;
            d->m_pSrc         = pSrc;
            d->m_src_buf_left = src_buf_left;
            if ((n = tdefl_flush_block(d, 0)) != 0)
                return (n < 0) ? MZ_FALSE : MZ_TRUE;
        }
    }

    d->m_pSrc         = pSrc;
    d->m_src_buf_left = src_buf_left;
    return MZ_TRUE;
}

static tdefl_status tdefl_flush_output_buffer(tdefl_compressor *d) {
    if (d->m_pIn_buf_size) {
        *d->m_pIn_buf_size = d->m_pSrc - (const mz_uint8 *)d->m_pIn_buf;
    }

    if (d->m_pOut_buf_size) {
        size_t n = MZ_MIN(*d->m_pOut_buf_size - d->m_out_buf_ofs, d->m_output_flush_remaining);
        memcpy((mz_uint8 *)d->m_pOut_buf + d->m_out_buf_ofs, d->m_output_buf + d->m_output_flush_ofs, n);
        d->m_output_flush_ofs       += (mz_uint)n;
        d->m_output_flush_remaining -= (mz_uint)n;
        d->m_out_buf_ofs            += n;

        *d->m_pOut_buf_size = d->m_out_buf_ofs;
    }

    return (d->m_finished && !d->m_output_flush_remaining) ? TDEFL_STATUS_DONE : TDEFL_STATUS_OKAY;
}

tdefl_status tdefl_compress(tdefl_compressor *d, const void *pIn_buf, size_t *pIn_buf_size, void *pOut_buf, size_t *pOut_buf_size, tdefl_flush flush) {
    if (!d) {
        if (pIn_buf_size) *pIn_buf_size = 0;
        if (pOut_buf_size) *pOut_buf_size = 0;
        return TDEFL_STATUS_BAD_PARAM;
    }

    d->m_pIn_buf       = pIn_buf;
    d->m_pIn_buf_size  = pIn_buf_size;
    d->m_pOut_buf      = pOut_buf;
    d->m_pOut_buf_size = pOut_buf_size;
    d->m_pSrc          = (const mz_uint8 *)(pIn_buf);
    d->m_src_buf_left  = pIn_buf_size ? *pIn_buf_size : 0;
    d->m_out_buf_ofs   = 0;
    d->m_flush         = flush;

    if (((d->m_pPut_buf_func != NULL) == ((pOut_buf != NULL) || (pOut_buf_size != NULL))) || (d->m_prev_return_status != TDEFL_STATUS_OKAY) ||
        (d->m_wants_to_finish && (flush != TDEFL_FINISH)) || (pIn_buf_size && *pIn_buf_size && !pIn_buf) || (pOut_buf_size && *pOut_buf_size && !pOut_buf)) {
        if (pIn_buf_size) *pIn_buf_size = 0;
        if (pOut_buf_size) *pOut_buf_size = 0;
        return d->m_prev_return_status = TDEFL_STATUS_BAD_PARAM;
    }
    d->m_wants_to_finish |= (flush == TDEFL_FINISH);

    if ((d->m_output_flush_remaining) || (d->m_finished))
        return d->m_prev_return_status = tdefl_flush_output_buffer(d);

 #if MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN
    if (((d->m_flags & TDEFL_MAX_PROBES_MASK) == 1) &&
        ((d->m_flags & TDEFL_GREEDY_PARSING_FLAG) != 0) &&
        ((d->m_flags & (TDEFL_FILTER_MATCHES | TDEFL_FORCE_ALL_RAW_BLOCKS | TDEFL_RLE_MATCHES)) == 0)) {
        if (!tdefl_compress_fast(d))
            return d->m_prev_return_status;
    } else
 #endif // #if MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN
    {
        if (!tdefl_compress_normal(d))
            return d->m_prev_return_status;
    }

    if ((d->m_flags & (TDEFL_WRITE_ZLIB_HEADER | TDEFL_COMPUTE_ADLER32)) && (pIn_buf))
        d->m_adler32 = (mz_uint32)mz_adler32(d->m_adler32, (const mz_uint8 *)pIn_buf, d->m_pSrc - (const mz_uint8 *)pIn_buf);

    if ((flush) && (!d->m_lookahead_size) && (!d->m_src_buf_left) && (!d->m_output_flush_remaining)) {
        if (tdefl_flush_block(d, flush) < 0)
            return d->m_prev_return_status;
        d->m_finished = (flush == TDEFL_FINISH);
        if (flush == TDEFL_FULL_FLUSH) {
            MZ_CLEAR_OBJ(d->m_hash);
            MZ_CLEAR_OBJ(d->m_next);
            d->m_dict_size = 0;
        }
    }

    return d->m_prev_return_status = tdefl_flush_output_buffer(d);
}

tdefl_status tdefl_compress_buffer(tdefl_compressor *d, const void *pIn_buf, size_t in_buf_size, tdefl_flush flush) {
    MZ_ASSERT(d->m_pPut_buf_func);
    return tdefl_compress(d, pIn_buf, &in_buf_size, NULL, NULL, flush);
}

tdefl_status tdefl_init(tdefl_compressor *d, tdefl_put_buf_func_ptr pPut_buf_func, void *pPut_buf_user, int flags) {
    d->m_pPut_buf_func  = pPut_buf_func;
    d->m_pPut_buf_user  = pPut_buf_user;
    d->m_flags          = (mz_uint)(flags);
    d->m_max_probes[0]  = 1 + ((flags & 0xFFF) + 2) / 3;
    d->m_greedy_parsing = (flags & TDEFL_GREEDY_PARSING_FLAG) != 0;
    d->m_max_probes[1]  = 1 + (((flags & 0xFFF) >> 2) + 2) / 3;
    if (!(flags & TDEFL_NONDETERMINISTIC_PARSING_FLAG)) MZ_CLEAR_OBJ(d->m_hash);
    d->m_lookahead_pos      = d->m_lookahead_size = d->m_dict_size = d->m_total_lz_bytes = d->m_lz_code_buf_dict_pos = d->m_bits_in = 0;
    d->m_output_flush_ofs   = d->m_output_flush_remaining = d->m_finished = d->m_block_index = d->m_bit_buffer = d->m_wants_to_finish = 0;
    d->m_pLZ_code_buf       = d->m_lz_code_buf + 1;
    d->m_pLZ_flags          = d->m_lz_code_buf;
    d->m_num_flags_left     = 8;
    d->m_pOutput_buf        = d->m_output_buf;
    d->m_pOutput_buf_end    = d->m_output_buf;
    d->m_prev_return_status = TDEFL_STATUS_OKAY;
    d->m_saved_match_dist   = d->m_saved_match_len = d->m_saved_lit = 0;
    d->m_adler32            = 1;
    d->m_pIn_buf            = NULL;
    d->m_pOut_buf           = NULL;
    d->m_pIn_buf_size       = NULL;
    d->m_pOut_buf_size      = NULL;
    d->m_flush        = TDEFL_NO_FLUSH;
    d->m_pSrc         = NULL;
    d->m_src_buf_left = 0;
    d->m_out_buf_ofs  = 0;
    memset(&d->m_huff_count[0][0], 0, sizeof(d->m_huff_count[0][0]) * TDEFL_MAX_HUFF_SYMBOLS_0);
    memset(&d->m_huff_count[1][0], 0, sizeof(d->m_huff_count[1][0]) * TDEFL_MAX_HUFF_SYMBOLS_1);
    return TDEFL_STATUS_OKAY;
}

tdefl_status tdefl_get_prev_return_status(tdefl_compressor *d) {
    return d->m_prev_return_status;
}

mz_uint32 tdefl_get_adler32(tdefl_compressor *d) {
    return d->m_adler32;
}

mz_bool tdefl_compress_mem_to_output(const void *pBuf, size_t buf_len, tdefl_put_buf_func_ptr pPut_buf_func, void *pPut_buf_user, int flags) {
    tdefl_compressor *pComp;
    mz_bool succeeded;

    if (((buf_len) && (!pBuf)) || (!pPut_buf_func)) return MZ_FALSE;

    pComp = (tdefl_compressor *)MZ_MALLOC(sizeof(tdefl_compressor));
    if (!pComp) return MZ_FALSE;
    succeeded = (tdefl_init(pComp, pPut_buf_func, pPut_buf_user, flags) == TDEFL_STATUS_OKAY);
    succeeded = succeeded && (tdefl_compress_buffer(pComp, pBuf, buf_len, TDEFL_FINISH) == TDEFL_STATUS_DONE);
    MZ_FREE(pComp);
    return succeeded;
}

typedef struct {
    size_t m_size, m_capacity;
    mz_uint8 *m_pBuf;
    mz_bool m_expandable;
} tdefl_output_buffer;

static mz_bool tdefl_output_buffer_putter(const void *pBuf, int len, void *pUser) {
    tdefl_output_buffer *p = (tdefl_output_buffer *)pUser;
    size_t new_size        = p->m_size + len;

    if (new_size > p->m_capacity) {
        size_t new_capacity = p->m_capacity;
        mz_uint8 *pNew_buf;
        if (!p->m_expandable) return MZ_FALSE;
        do {
            new_capacity = MZ_MAX(128U, new_capacity << 1U);
        } while (new_size > new_capacity);
        pNew_buf = (mz_uint8 *)MZ_REALLOC(p->m_pBuf, new_capacity);
        if (!pNew_buf) return MZ_FALSE;
        p->m_pBuf     = pNew_buf;
        p->m_capacity = new_capacity;
    }
    memcpy((mz_uint8 *)p->m_pBuf + p->m_size, pBuf, len);
    p->m_size = new_size;
    return MZ_TRUE;
}

void *tdefl_compress_mem_to_heap(const void *pSrc_buf, size_t src_buf_len, size_t *pOut_len, int flags) {
    tdefl_output_buffer out_buf;

    MZ_CLEAR_OBJ(out_buf);

    if (!pOut_len) return MZ_FALSE;
    else *pOut_len = 0;
    out_buf.m_expandable = MZ_TRUE;
    if (!tdefl_compress_mem_to_output(pSrc_buf, src_buf_len, tdefl_output_buffer_putter, &out_buf, flags)) return NULL;
    *pOut_len = out_buf.m_size;
    return out_buf.m_pBuf;
}

size_t tdefl_compress_mem_to_mem(void *pOut_buf, size_t out_buf_len, const void *pSrc_buf, size_t src_buf_len, int flags) {
    tdefl_output_buffer out_buf;

    MZ_CLEAR_OBJ(out_buf);

    if (!pOut_buf) return 0;
    out_buf.m_pBuf     = (mz_uint8 *)pOut_buf;
    out_buf.m_capacity = out_buf_len;
    if (!tdefl_compress_mem_to_output(pSrc_buf, src_buf_len, tdefl_output_buffer_putter, &out_buf, flags)) return 0;
    return out_buf.m_size;
}

 #ifndef MINIZ_NO_ZLIB_APIS
static const mz_uint s_tdefl_num_probes[11] = { 0, 1, 6, 32, 16, 32, 128, 256, 512, 768, 1500 };

// level may actually range from [0,10] (10 is a "hidden" max level, where we want a bit more compression and it's fine if throughput to fall off a cliff on some files).
mz_uint tdefl_create_comp_flags_from_zip_params(int level, int window_bits, int strategy) {
    mz_uint comp_flags = s_tdefl_num_probes[(level >= 0) ? MZ_MIN(10, level) : MZ_DEFAULT_LEVEL] | ((level <= 3) ? TDEFL_GREEDY_PARSING_FLAG : 0);

    if (window_bits > 0) comp_flags |= TDEFL_WRITE_ZLIB_HEADER;

    if (!level) comp_flags |= TDEFL_FORCE_ALL_RAW_BLOCKS;
    else if (strategy == MZ_FILTERED) comp_flags |= TDEFL_FILTER_MATCHES;
    else if (strategy == MZ_HUFFMAN_ONLY) comp_flags &= ~TDEFL_MAX_PROBES_MASK;
    else if (strategy == MZ_FIXED) comp_flags |= TDEFL_FORCE_ALL_STATIC_BLOCKS;
    else if (strategy == MZ_RLE) comp_flags |= TDEFL_RLE_MATCHES;

    return comp_flags;
}
 #endif //MINIZ_NO_ZLIB_APIS

 #ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable:4204)// nonstandard extension used : non-constant aggregate initializer (also supported by GNU C and C99, so no big deal)
 #endif

// Simple PNG writer function by Alex Evans, 2011. Released into the public domain: https://gist.github.com/908299, more context at
// http://altdevblogaday.org/2011/04/06/a-smaller-jpg-encoder/.
// This is actually a modification of Alex's original code so PNG files generated by this function pass pngcheck.
void *tdefl_write_image_to_png_file_in_memory_ex(const void *pImage, int w, int h, int num_chans, size_t *pLen_out, mz_uint level, mz_bool flip) {
    // Using a local copy of this array here in case MINIZ_NO_ZLIB_APIS was defined.
    static const mz_uint s_tdefl_png_num_probes[11] = { 0, 1, 6, 32, 16, 32, 128, 256, 512, 768, 1500 };
    tdefl_compressor *pComp = (tdefl_compressor *)MZ_MALLOC(sizeof(tdefl_compressor));
    tdefl_output_buffer out_buf;
    int i, bpl = w * num_chans, y, z;
    mz_uint32 c;

    *pLen_out = 0;

    if (!pComp) return NULL;
    MZ_CLEAR_OBJ(out_buf);
    out_buf.m_expandable = MZ_TRUE;
    out_buf.m_capacity   = 57 + MZ_MAX(64, (1 + bpl) * h);
    if (NULL == (out_buf.m_pBuf = (mz_uint8 *)MZ_MALLOC(out_buf.m_capacity))) {
        MZ_FREE(pComp);
        return NULL;
    }
    // write dummy header
    for (z = 41; z; --z) tdefl_output_buffer_putter(&z, 1, &out_buf);
    // compress image data
    tdefl_init(pComp, tdefl_output_buffer_putter, &out_buf, s_tdefl_png_num_probes[MZ_MIN(10, level)] | TDEFL_WRITE_ZLIB_HEADER);
    for (y = 0; y < h; ++y) {
        tdefl_compress_buffer(pComp, &z, 1, TDEFL_NO_FLUSH);
        tdefl_compress_buffer(pComp, (mz_uint8 *)pImage + (flip ? (h - 1 - y) : y) * bpl, bpl, TDEFL_NO_FLUSH);
    }
    if (tdefl_compress_buffer(pComp, NULL, 0, TDEFL_FINISH) != TDEFL_STATUS_DONE) {
        MZ_FREE(pComp);
        MZ_FREE(out_buf.m_pBuf);
        return NULL;
    }
    // write real header
    *pLen_out = out_buf.m_size - 41;
    {
        static const mz_uint8 chans[] = { 0x00, 0x00, 0x04, 0x02, 0x06 };
        mz_uint8 pnghdr[41]           = {
            0x89,                                               0x50,                       0x4e,                  0x47, 0x0d, 0x0a,               0x1a,        0x0a, 0x00,             0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
            0,                                                     0, (mz_uint8)(w >> 8),         (mz_uint8)w,              0,    0, (mz_uint8)(h >> 8), (mz_uint8)h,    8, chans[num_chans],    0,    0,    0,    0,    0,    0,0,
            (mz_uint8)(*pLen_out >> 24), (mz_uint8)(*pLen_out >> 16), (mz_uint8)(*pLen_out >> 8), (mz_uint8) * pLen_out, 0x49, 0x44,               0x41, 0x54
        };
        c = (mz_uint32)mz_crc32(MZ_CRC32_INIT, pnghdr + 12, 17);
        for (i = 0; i < 4; ++i, c <<= 8) ((mz_uint8 *)(pnghdr + 29))[i] = (mz_uint8)(c >> 24);
        memcpy(out_buf.m_pBuf, pnghdr, 41);
    }
    // write footer (IDAT CRC-32, followed by IEND chunk)
    if (!tdefl_output_buffer_putter("\0\0\0\0\0\0\0\0\x49\x45\x4e\x44\xae\x42\x60\x82", 16, &out_buf)) {
        *pLen_out = 0;
        MZ_FREE(pComp);
        MZ_FREE(out_buf.m_pBuf);
        return NULL;
    }
    c = (mz_uint32)mz_crc32(MZ_CRC32_INIT, out_buf.m_pBuf + 41 - 4, *pLen_out + 4);
    for (i = 0; i < 4; ++i, c <<= 8) (out_buf.m_pBuf + out_buf.m_size - 16)[i] = (mz_uint8)(c >> 24);
    // compute final size of file, grab compressed data buffer and return
    *pLen_out += 57;
    MZ_FREE(pComp);
    return out_buf.m_pBuf;
}

void *tdefl_write_image_to_png_file_in_memory(const void *pImage, int w, int h, int num_chans, size_t *pLen_out) {
    // Level 6 corresponds to TDEFL_DEFAULT_MAX_PROBES or MZ_DEFAULT_LEVEL (but we can't depend on MZ_DEFAULT_LEVEL being available in case the zlib API's where #defined out)
    return tdefl_write_image_to_png_file_in_memory_ex(pImage, w, h, num_chans, pLen_out, 6, MZ_FALSE);
}

 #ifdef _MSC_VER
  #pragma warning (pop)
 #endif

// ------------------- .ZIP archive reading

 #ifndef MINIZ_NO_ARCHIVE_APIS

  #ifdef MINIZ_NO_STDIO
   #define MZ_FILE    void *
  #else
   #include <stdio.h>
   #include <sys/stat.h>

   #if defined(_MSC_VER) || defined(__MINGW64__)
static FILE *mz_fopen(const char *pFilename, const char *pMode) {
    FILE *pFile = NULL;

    fopen_s(&pFile, pFilename, pMode);
    return pFile;
}

static FILE *mz_freopen(const char *pPath, const char *pMode, FILE *pStream) {
    FILE *pFile = NULL;

    if (freopen_s(&pFile, pPath, pMode, pStream))
        return NULL;
    return pFile;
}

    #ifndef MINIZ_NO_TIME
     #include <sys/utime.h>
    #endif
    #define MZ_FILE                FILE
    #define MZ_FOPEN               mz_fopen
    #define MZ_FCLOSE              fclose
    #define MZ_FREAD               fread
    #define MZ_FWRITE              fwrite
    #define MZ_FTELL64             _ftelli64
    #define MZ_FSEEK64             _fseeki64
    #define MZ_FILE_STAT_STRUCT    _stat
    #define MZ_FILE_STAT           _stat
    #define MZ_FFLUSH              fflush
    #define MZ_FREOPEN             mz_freopen
    #define MZ_DELETE_FILE         remove
   #elif defined(__MINGW32__)
    #ifndef MINIZ_NO_TIME
     #include <sys/utime.h>
    #endif
    #define MZ_FILE                FILE
    #define MZ_FOPEN(f, m)    fopen(f, m)
    #define MZ_FCLOSE              fclose
    #define MZ_FREAD               fread
    #define MZ_FWRITE              fwrite
    #define MZ_FTELL64             ftello64
    #define MZ_FSEEK64             fseeko64
    #define MZ_FILE_STAT_STRUCT    _stat
    #define MZ_FILE_STAT           _stat
    #define MZ_FFLUSH              fflush
    #define MZ_FREOPEN(f, m, s)    freopen(f, m, s)
    #define MZ_DELETE_FILE         remove
   #elif defined(__TINYC__)
    #ifndef MINIZ_NO_TIME
     #include <sys/utime.h>
    #endif
    #define MZ_FILE                FILE
    #define MZ_FOPEN(f, m)    fopen(f, m)
    #define MZ_FCLOSE              fclose
    #define MZ_FREAD               fread
    #define MZ_FWRITE              fwrite
    #define MZ_FTELL64             ftell
    #define MZ_FSEEK64             fseek
    #define MZ_FILE_STAT_STRUCT    stat
    #define MZ_FILE_STAT           stat
    #define MZ_FFLUSH              fflush
    #define MZ_FREOPEN(f, m, s)    freopen(f, m, s)
    #define MZ_DELETE_FILE         remove
   #elif defined(__GNUC__) && _LARGEFILE64_SOURCE
    #ifndef MINIZ_NO_TIME
     #include <utime.h>
    #endif
    #define MZ_FILE                FILE
    #define MZ_FOPEN(f, m)    fopen64(f, m)
    #define MZ_FCLOSE              fclose
    #define MZ_FREAD               fread
    #define MZ_FWRITE              fwrite
    #define MZ_FTELL64             ftello64
    #define MZ_FSEEK64             fseeko64
    #define MZ_FILE_STAT_STRUCT    stat64
    #define MZ_FILE_STAT           stat64
    #define MZ_FFLUSH              fflush
    #define MZ_FREOPEN(p, m, s)    freopen64(p, m, s)
    #define MZ_DELETE_FILE         remove
   #else
    #ifndef MINIZ_NO_TIME
     #include <utime.h>
    #endif
    #define MZ_FILE                FILE
    #define MZ_FOPEN(f, m)    fopen(f, m)
    #define MZ_FCLOSE              fclose
    #define MZ_FREAD               fread
    #define MZ_FWRITE              fwrite
    #define MZ_FTELL64             ftello
    #define MZ_FSEEK64             fseeko
    #define MZ_FILE_STAT_STRUCT    stat
    #define MZ_FILE_STAT           stat
    #define MZ_FFLUSH              fflush
    #define MZ_FREOPEN(f, m, s)    freopen(f, m, s)
    #define MZ_DELETE_FILE         remove
   #endif // #ifdef _MSC_VER
  #endif  // #ifdef MINIZ_NO_STDIO

  #define MZ_TOLOWER(c)    ((((c) >= 'A') && ((c) <= 'Z')) ? ((c) - 'A' + 'a') : (c))

// Various ZIP archive enums. To completely avoid cross platform compiler alignment and platform endian issues, miniz.c doesn't use structs for any of this stuff.
enum {
    // ZIP archive identifiers and record sizes
    MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIG = 0x06054b50, MZ_ZIP_CENTRAL_DIR_HEADER_SIG = 0x02014b50, MZ_ZIP_LOCAL_DIR_HEADER_SIG = 0x04034b50,
    MZ_ZIP_LOCAL_DIR_HEADER_SIZE         = 30, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE = 46, MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE = 22,
    // Central directory header record offsets
    MZ_ZIP_CDH_SIG_OFS                   = 0, MZ_ZIP_CDH_VERSION_MADE_BY_OFS = 4, MZ_ZIP_CDH_VERSION_NEEDED_OFS = 6, MZ_ZIP_CDH_BIT_FLAG_OFS = 8,
    MZ_ZIP_CDH_METHOD_OFS                = 10, MZ_ZIP_CDH_FILE_TIME_OFS = 12, MZ_ZIP_CDH_FILE_DATE_OFS = 14, MZ_ZIP_CDH_CRC32_OFS = 16,
    MZ_ZIP_CDH_COMPRESSED_SIZE_OFS       = 20, MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS = 24, MZ_ZIP_CDH_FILENAME_LEN_OFS = 28, MZ_ZIP_CDH_EXTRA_LEN_OFS = 30,
    MZ_ZIP_CDH_COMMENT_LEN_OFS           = 32, MZ_ZIP_CDH_DISK_START_OFS = 34, MZ_ZIP_CDH_INTERNAL_ATTR_OFS = 36, MZ_ZIP_CDH_EXTERNAL_ATTR_OFS = 38, MZ_ZIP_CDH_LOCAL_HEADER_OFS = 42,
    // Local directory header offsets
    MZ_ZIP_LDH_SIG_OFS                   = 0, MZ_ZIP_LDH_VERSION_NEEDED_OFS = 4, MZ_ZIP_LDH_BIT_FLAG_OFS = 6, MZ_ZIP_LDH_METHOD_OFS = 8, MZ_ZIP_LDH_FILE_TIME_OFS = 10,
    MZ_ZIP_LDH_FILE_DATE_OFS             = 12, MZ_ZIP_LDH_CRC32_OFS = 14, MZ_ZIP_LDH_COMPRESSED_SIZE_OFS = 18, MZ_ZIP_LDH_DECOMPRESSED_SIZE_OFS = 22,
    MZ_ZIP_LDH_FILENAME_LEN_OFS          = 26, MZ_ZIP_LDH_EXTRA_LEN_OFS = 28,
    // End of central directory offsets
    MZ_ZIP_ECDH_SIG_OFS                  = 0, MZ_ZIP_ECDH_NUM_THIS_DISK_OFS = 4, MZ_ZIP_ECDH_NUM_DISK_CDIR_OFS = 6, MZ_ZIP_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS = 8,
    MZ_ZIP_ECDH_CDIR_TOTAL_ENTRIES_OFS   = 10, MZ_ZIP_ECDH_CDIR_SIZE_OFS = 12, MZ_ZIP_ECDH_CDIR_OFS_OFS = 16, MZ_ZIP_ECDH_COMMENT_SIZE_OFS = 20,
};

typedef struct {
    void *m_p;
    size_t m_size, m_capacity;
    mz_uint m_element_size;
} mz_zip_array;

struct mz_zip_internal_state_tag {
    mz_zip_array m_central_dir;
    mz_zip_array m_central_dir_offsets;
    mz_zip_array m_sorted_central_dir_offsets;
    MZ_FILE *m_pFile;
    void *m_pMem;
    size_t m_mem_size;
    size_t m_mem_capacity;
};

  #define MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(array_ptr, element_size)    (array_ptr)->m_element_size = element_size
  #define MZ_ZIP_ARRAY_ELEMENT(array_ptr, element_type, index)      ((element_type *)((array_ptr)->m_p))[index]

static MZ_FORCEINLINE void mz_zip_array_clear(mz_zip_archive *pZip, mz_zip_array *pArray) {
    pZip->m_pFree(pZip->m_pAlloc_opaque, pArray->m_p);
    memset(pArray, 0, sizeof(mz_zip_array));
}

static mz_bool mz_zip_array_ensure_capacity(mz_zip_archive *pZip, mz_zip_array *pArray, size_t min_new_capacity, mz_uint growing) {
    void *pNew_p;
    size_t new_capacity = min_new_capacity;

    MZ_ASSERT(pArray->m_element_size);
    if (pArray->m_capacity >= min_new_capacity) return MZ_TRUE;

    if (growing) {
        new_capacity = MZ_MAX(1, pArray->m_capacity);
        while (new_capacity < min_new_capacity) new_capacity *= 2;
    }
    if (NULL == (pNew_p = pZip->m_pRealloc(pZip->m_pAlloc_opaque, pArray->m_p, pArray->m_element_size, new_capacity))) return MZ_FALSE;
    pArray->m_p        = pNew_p;
    pArray->m_capacity = new_capacity;
    return MZ_TRUE;
}

static MZ_FORCEINLINE mz_bool mz_zip_array_reserve(mz_zip_archive *pZip, mz_zip_array *pArray, size_t new_capacity, mz_uint growing) {
    if (new_capacity > pArray->m_capacity) {
        if (!mz_zip_array_ensure_capacity(pZip, pArray, new_capacity, growing)) return MZ_FALSE;
    }
    return MZ_TRUE;
}

static MZ_FORCEINLINE mz_bool mz_zip_array_resize(mz_zip_archive *pZip, mz_zip_array *pArray, size_t new_size, mz_uint growing) {
    if (new_size > pArray->m_capacity) {
        if (!mz_zip_array_ensure_capacity(pZip, pArray, new_size, growing)) return MZ_FALSE;
    }
    pArray->m_size = new_size;
    return MZ_TRUE;
}

static MZ_FORCEINLINE mz_bool mz_zip_array_ensure_room(mz_zip_archive *pZip, mz_zip_array *pArray, size_t n) {
    return mz_zip_array_reserve(pZip, pArray, pArray->m_size + n, MZ_TRUE);
}

static MZ_FORCEINLINE mz_bool mz_zip_array_push_back(mz_zip_archive *pZip, mz_zip_array *pArray, const void *pElements, size_t n) {
    size_t orig_size = pArray->m_size;

    if (!mz_zip_array_resize(pZip, pArray, orig_size + n, MZ_TRUE)) return MZ_FALSE;

    memcpy((mz_uint8 *)pArray->m_p + orig_size * pArray->m_element_size, pElements, n * pArray->m_element_size);
    return MZ_TRUE;
}

  #ifndef MINIZ_NO_TIME
static time_t mz_zip_dos_to_time_t(int dos_time, int dos_date) {
    struct tm tm;

    memset(&tm, 0, sizeof(tm));
    tm.tm_isdst = -1;
    tm.tm_year  = ((dos_date >> 9) & 127) + 1980 - 1900;
    tm.tm_mon   = ((dos_date >> 5) & 15) - 1;
    tm.tm_mday  = dos_date & 31;
    tm.tm_hour  = (dos_time >> 11) & 31;
    tm.tm_min   = (dos_time >> 5) & 63;
    tm.tm_sec   = (dos_time << 1) & 62;
    return mktime(&tm);
}

static void mz_zip_time_to_dos_time(time_t time, mz_uint16 *pDOS_time, mz_uint16 *pDOS_date) {
   #ifdef _MSC_VER
    struct tm tm_struct;
    struct tm *tm = &tm_struct;
    errno_t err   = localtime_s(tm, &time);
    if (err) {
        *pDOS_date = 0;
        *pDOS_time = 0;
        return;
    }
   #else
    struct tm *tm = localtime(&time);
   #endif
    *pDOS_time = (mz_uint16)(((tm->tm_hour) << 11) + ((tm->tm_min) << 5) + ((tm->tm_sec) >> 1));
    *pDOS_date = (mz_uint16)(((tm->tm_year + 1900 - 1980) << 9) + ((tm->tm_mon + 1) << 5) + tm->tm_mday);
}
  #endif

  #ifndef MINIZ_NO_STDIO
static mz_bool mz_zip_get_file_modified_time(const char *pFilename, mz_uint16 *pDOS_time, mz_uint16 *pDOS_date) {
   #ifdef MINIZ_NO_TIME
    (void)pFilename;
    *pDOS_date = *pDOS_time = 0;
   #else
    struct MZ_FILE_STAT_STRUCT file_stat;
    // On Linux with x86 glibc, this call will fail on large files (>= 0x80000000 bytes) unless you compiled with _LARGEFILE64_SOURCE. Argh.
    if (MZ_FILE_STAT(pFilename, &file_stat) != 0)
        return MZ_FALSE;
    mz_zip_time_to_dos_time(file_stat.st_mtime, pDOS_time, pDOS_date);
   #endif // #ifdef MINIZ_NO_TIME
    return MZ_TRUE;
}

   #ifndef MINIZ_NO_TIME
static mz_bool mz_zip_set_file_times(const char *pFilename, time_t access_time, time_t modified_time) {
    struct utimbuf t;

    t.actime  = access_time;
    t.modtime = modified_time;

    return !utime(pFilename, &t);
}
   #endif // #ifndef MINIZ_NO_TIME
  #endif  // #ifndef MINIZ_NO_STDIO

static mz_bool mz_zip_reader_init_internal(mz_zip_archive *pZip, mz_uint32 flags) {
    (void)flags;
    if ((!pZip) || (pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_INVALID))
        return MZ_FALSE;

    if (!pZip->m_pAlloc) pZip->m_pAlloc = def_alloc_func;
    if (!pZip->m_pFree) pZip->m_pFree = def_free_func;
    if (!pZip->m_pRealloc) pZip->m_pRealloc = def_realloc_func;

    pZip->m_zip_mode     = MZ_ZIP_MODE_READING;
    pZip->m_archive_size = 0;
    pZip->m_central_directory_file_ofs = 0;
    pZip->m_total_files = 0;

    if (NULL == (pZip->m_pState = (mz_zip_internal_state *)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(mz_zip_internal_state))))
        return MZ_FALSE;
    memset(pZip->m_pState, 0, sizeof(mz_zip_internal_state));
    MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_central_dir, sizeof(mz_uint8));
    MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_central_dir_offsets, sizeof(mz_uint32));
    MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_sorted_central_dir_offsets, sizeof(mz_uint32));
    return MZ_TRUE;
}

static MZ_FORCEINLINE mz_bool mz_zip_reader_filename_less(const mz_zip_array *pCentral_dir_array, const mz_zip_array *pCentral_dir_offsets, mz_uint l_index, mz_uint r_index) {
    const mz_uint8 *pL = &MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_array, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_offsets, mz_uint32, l_index)), *pE;
    const mz_uint8 *pR = &MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_array, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_offsets, mz_uint32, r_index));
    mz_uint l_len      = MZ_READ_LE16(pL + MZ_ZIP_CDH_FILENAME_LEN_OFS), r_len = MZ_READ_LE16(pR + MZ_ZIP_CDH_FILENAME_LEN_OFS);
    mz_uint8 l         = 0, r = 0;

    pL += MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;
    pR += MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;
    pE  = pL + MZ_MIN(l_len, r_len);
    while (pL < pE) {
        if ((l = MZ_TOLOWER(*pL)) != (r = MZ_TOLOWER(*pR)))
            break;
        pL++;
        pR++;
    }
    return (pL == pE) ? (l_len < r_len) : (l < r);
}

  #define MZ_SWAP_UINT32(a, b)    do { mz_uint32 t = a; a = b; b = t; } MZ_MACRO_END

// Heap sort of lowercased filenames, used to help accelerate plain central directory searches by mz_zip_reader_locate_file(). (Could also use qsort(), but it could allocate memory.)
static void mz_zip_reader_sort_central_dir_offsets_by_filename(mz_zip_archive *pZip) {
    mz_zip_internal_state *pState            = pZip->m_pState;
    const mz_zip_array *pCentral_dir_offsets = &pState->m_central_dir_offsets;
    const mz_zip_array *pCentral_dir         = &pState->m_central_dir;
    mz_uint32 *pIndices = &MZ_ZIP_ARRAY_ELEMENT(&pState->m_sorted_central_dir_offsets, mz_uint32, 0);
    const int size      = pZip->m_total_files;
    int start           = (size - 2) >> 1, end;

    while (start >= 0) {
        int child, root = start;
        for ( ; ; ) {
            if ((child = (root << 1) + 1) >= size)
                break;
            child += (((child + 1) < size) && (mz_zip_reader_filename_less(pCentral_dir, pCentral_dir_offsets, pIndices[child], pIndices[child + 1])));
            if (!mz_zip_reader_filename_less(pCentral_dir, pCentral_dir_offsets, pIndices[root], pIndices[child]))
                break;
            MZ_SWAP_UINT32(pIndices[root], pIndices[child]);
            root = child;
        }
        start--;
    }

    end = size - 1;
    while (end > 0) {
        int child, root = 0;
        MZ_SWAP_UINT32(pIndices[end], pIndices[0]);
        for ( ; ; ) {
            if ((child = (root << 1) + 1) >= end)
                break;
            child += (((child + 1) < end) && mz_zip_reader_filename_less(pCentral_dir, pCentral_dir_offsets, pIndices[child], pIndices[child + 1]));
            if (!mz_zip_reader_filename_less(pCentral_dir, pCentral_dir_offsets, pIndices[root], pIndices[child]))
                break;
            MZ_SWAP_UINT32(pIndices[root], pIndices[child]);
            root = child;
        }
        end--;
    }
}

static mz_bool mz_zip_reader_read_central_dir(mz_zip_archive *pZip, mz_uint32 flags) {
    mz_uint cdir_size, num_this_disk, cdir_disk_index;
    mz_uint64 cdir_ofs;
    mz_int64 cur_file_ofs;
    const mz_uint8 *p;
    mz_uint32 buf_u32[4096 / sizeof(mz_uint32)];
    mz_uint8 *pBuf           = (mz_uint8 *)buf_u32;
    mz_bool sort_central_dir = ((flags & MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY) == 0);

    // Basic sanity checks - reject files which are too small, and check the first 4 bytes of the file to make sure a local header is there.
    if (pZip->m_archive_size < MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)
        return MZ_FALSE;
    // Find the end of central directory record by scanning the file from the end towards the beginning.
    cur_file_ofs = MZ_MAX((mz_int64)pZip->m_archive_size - (mz_int64)sizeof(buf_u32), 0);
    for ( ; ; ) {
        int i, n = (int)MZ_MIN(sizeof(buf_u32), pZip->m_archive_size - cur_file_ofs);
        if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pBuf, n) != (mz_uint)n)
            return MZ_FALSE;
        for (i = n - 4; i >= 0; --i)
            if (MZ_READ_LE32(pBuf + i) == MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIG)
                break;
        if (i >= 0) {
            cur_file_ofs += i;
            break;
        }
        if ((!cur_file_ofs) || ((pZip->m_archive_size - cur_file_ofs) >= (0xFFFF + MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)))
            return MZ_FALSE;
        cur_file_ofs = MZ_MAX(cur_file_ofs - (sizeof(buf_u32) - 3), 0);
    }
    // Read and verify the end of central directory record.
    if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pBuf, MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE) != MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)
        return MZ_FALSE;
    if ((MZ_READ_LE32(pBuf + MZ_ZIP_ECDH_SIG_OFS) != MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIG) ||
        ((pZip->m_total_files = MZ_READ_LE16(pBuf + MZ_ZIP_ECDH_CDIR_TOTAL_ENTRIES_OFS)) != MZ_READ_LE16(pBuf + MZ_ZIP_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS)))
        return MZ_FALSE;

    num_this_disk   = MZ_READ_LE16(pBuf + MZ_ZIP_ECDH_NUM_THIS_DISK_OFS);
    cdir_disk_index = MZ_READ_LE16(pBuf + MZ_ZIP_ECDH_NUM_DISK_CDIR_OFS);
    if (((num_this_disk | cdir_disk_index) != 0) && ((num_this_disk != 1) || (cdir_disk_index != 1)))
        return MZ_FALSE;

    if ((cdir_size = MZ_READ_LE32(pBuf + MZ_ZIP_ECDH_CDIR_SIZE_OFS)) < pZip->m_total_files * MZ_ZIP_CENTRAL_DIR_HEADER_SIZE)
        return MZ_FALSE;

    cdir_ofs = MZ_READ_LE32(pBuf + MZ_ZIP_ECDH_CDIR_OFS_OFS);
    if ((cdir_ofs + (mz_uint64)cdir_size) > pZip->m_archive_size)
        return MZ_FALSE;

    pZip->m_central_directory_file_ofs = cdir_ofs;

    if (pZip->m_total_files) {
        mz_uint i, n;

        // Read the entire central directory into a heap block, and allocate another heap block to hold the unsorted central dir file record offsets, and another to hold the sorted indices.
        if ((!mz_zip_array_resize(pZip, &pZip->m_pState->m_central_dir, cdir_size, MZ_FALSE)) ||
            (!mz_zip_array_resize(pZip, &pZip->m_pState->m_central_dir_offsets, pZip->m_total_files, MZ_FALSE)))
            return MZ_FALSE;

        if (sort_central_dir) {
            if (!mz_zip_array_resize(pZip, &pZip->m_pState->m_sorted_central_dir_offsets, pZip->m_total_files, MZ_FALSE))
                return MZ_FALSE;
        }

        if (pZip->m_pRead(pZip->m_pIO_opaque, cdir_ofs, pZip->m_pState->m_central_dir.m_p, cdir_size) != cdir_size)
            return MZ_FALSE;

        // Now create an index into the central directory file records, do some basic sanity checking on each record, and check for zip64 entries (which are not yet supported).
        p = (const mz_uint8 *)pZip->m_pState->m_central_dir.m_p;
        for (n = cdir_size, i = 0; i < pZip->m_total_files; ++i) {
            mz_uint total_header_size, comp_size, decomp_size, disk_index;
            if ((n < MZ_ZIP_CENTRAL_DIR_HEADER_SIZE) || (MZ_READ_LE32(p) != MZ_ZIP_CENTRAL_DIR_HEADER_SIG))
                return MZ_FALSE;
            MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, i) = (mz_uint32)(p - (const mz_uint8 *)pZip->m_pState->m_central_dir.m_p);
            if (sort_central_dir)
                MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_sorted_central_dir_offsets, mz_uint32, i) = i;
            comp_size   = MZ_READ_LE32(p + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS);
            decomp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS);
            if (((!MZ_READ_LE32(p + MZ_ZIP_CDH_METHOD_OFS)) && (decomp_size != comp_size)) || (decomp_size && !comp_size) || (decomp_size == 0xFFFFFFFF) || (comp_size == 0xFFFFFFFF))
                return MZ_FALSE;
            disk_index = MZ_READ_LE16(p + MZ_ZIP_CDH_DISK_START_OFS);
            if ((disk_index != num_this_disk) && (disk_index != 1))
                return MZ_FALSE;
            if (((mz_uint64)MZ_READ_LE32(p + MZ_ZIP_CDH_LOCAL_HEADER_OFS) + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + comp_size) > pZip->m_archive_size)
                return MZ_FALSE;
            if ((total_header_size = MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS) + MZ_READ_LE16(p + MZ_ZIP_CDH_EXTRA_LEN_OFS) + MZ_READ_LE16(p + MZ_ZIP_CDH_COMMENT_LEN_OFS)) > n)
                return MZ_FALSE;
            n -= total_header_size;
            p += total_header_size;
        }
    }

    if (sort_central_dir)
        mz_zip_reader_sort_central_dir_offsets_by_filename(pZip);

    return MZ_TRUE;
}

mz_bool mz_zip_reader_init(mz_zip_archive *pZip, mz_uint64 size, mz_uint32 flags) {
    if ((!pZip) || (!pZip->m_pRead))
        return MZ_FALSE;
    if (!mz_zip_reader_init_internal(pZip, flags))
        return MZ_FALSE;
    pZip->m_archive_size = size;
    if (!mz_zip_reader_read_central_dir(pZip, flags)) {
        mz_zip_reader_end(pZip);
        return MZ_FALSE;
    }
    return MZ_TRUE;
}

static size_t mz_zip_mem_read_func(void *pOpaque, mz_uint64 file_ofs, void *pBuf, size_t n) {
    mz_zip_archive *pZip = (mz_zip_archive *)pOpaque;
    size_t s             = (file_ofs >= pZip->m_archive_size) ? 0 : (size_t)MZ_MIN(pZip->m_archive_size - file_ofs, n);

    memcpy(pBuf, (const mz_uint8 *)pZip->m_pState->m_pMem + file_ofs, s);
    return s;
}

mz_bool mz_zip_reader_init_mem(mz_zip_archive *pZip, const void *pMem, size_t size, mz_uint32 flags) {
    if (!mz_zip_reader_init_internal(pZip, flags))
        return MZ_FALSE;
    pZip->m_archive_size = size;
    pZip->m_pRead        = mz_zip_mem_read_func;
    pZip->m_pIO_opaque   = pZip;
  #ifdef __cplusplus
    pZip->m_pState->m_pMem = const_cast < void * > (pMem);
  #else
    pZip->m_pState->m_pMem = (void *)pMem;
  #endif
    pZip->m_pState->m_mem_size = size;
    if (!mz_zip_reader_read_central_dir(pZip, flags)) {
        mz_zip_reader_end(pZip);
        return MZ_FALSE;
    }
    return MZ_TRUE;
}

  #ifndef MINIZ_NO_STDIO
static size_t mz_zip_file_read_func(void *pOpaque, mz_uint64 file_ofs, void *pBuf, size_t n) {
    mz_zip_archive *pZip = (mz_zip_archive *)pOpaque;
    mz_int64 cur_ofs     = MZ_FTELL64(pZip->m_pState->m_pFile);

    if (((mz_int64)file_ofs < 0) || (((cur_ofs != (mz_int64)file_ofs)) && (MZ_FSEEK64(pZip->m_pState->m_pFile, (mz_int64)file_ofs, SEEK_SET))))
        return 0;
    return MZ_FREAD(pBuf, 1, n, pZip->m_pState->m_pFile);
}

mz_bool mz_zip_reader_init_file(mz_zip_archive *pZip, const char *pFilename, mz_uint32 flags) {
    mz_uint64 file_size;
    MZ_FILE *pFile = MZ_FOPEN(pFilename, "rb");

    if (!pFile)
        return MZ_FALSE;
    if (MZ_FSEEK64(pFile, 0, SEEK_END)) {
        MZ_FCLOSE(pFile);
        return MZ_FALSE;
    }
    file_size = MZ_FTELL64(pFile);
    if (!mz_zip_reader_init_internal(pZip, flags)) {
        MZ_FCLOSE(pFile);
        return MZ_FALSE;
    }
    pZip->m_pRead           = mz_zip_file_read_func;
    pZip->m_pIO_opaque      = pZip;
    pZip->m_pState->m_pFile = pFile;
    pZip->m_archive_size    = file_size;
    if (!mz_zip_reader_read_central_dir(pZip, flags)) {
        mz_zip_reader_end(pZip);
        return MZ_FALSE;
    }
    return MZ_TRUE;
}
  #endif // #ifndef MINIZ_NO_STDIO

mz_uint mz_zip_reader_get_num_files(mz_zip_archive *pZip) {
    return pZip ? pZip->m_total_files : 0;
}

static MZ_FORCEINLINE const mz_uint8 *mz_zip_reader_get_cdh(mz_zip_archive *pZip, mz_uint file_index) {
    if ((!pZip) || (!pZip->m_pState) || (file_index >= pZip->m_total_files) || (pZip->m_zip_mode != MZ_ZIP_MODE_READING))
        return NULL;
    return &MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, file_index));
}

mz_bool mz_zip_reader_is_file_encrypted(mz_zip_archive *pZip, mz_uint file_index) {
    mz_uint m_bit_flag;
    const mz_uint8 *p = mz_zip_reader_get_cdh(pZip, file_index);

    if (!p)
        return MZ_FALSE;
    m_bit_flag = MZ_READ_LE16(p + MZ_ZIP_CDH_BIT_FLAG_OFS);
    return m_bit_flag & 1;
}

mz_bool mz_zip_reader_is_file_a_directory(mz_zip_archive *pZip, mz_uint file_index) {
    mz_uint filename_len, external_attr;
    const mz_uint8 *p = mz_zip_reader_get_cdh(pZip, file_index);

    if (!p)
        return MZ_FALSE;

    // First see if the filename ends with a '/' character.
    filename_len = MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS);
    if (filename_len) {
        if (*(p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + filename_len - 1) == '/')
            return MZ_TRUE;
    }

    // Bugfix: This code was also checking if the internal attribute was non-zero, which wasn't correct.
    // Most/all zip writers (hopefully) set DOS file/directory attributes in the low 16-bits, so check for the DOS directory flag and ignore the source OS ID in the created by field.
    // FIXME: Remove this check? Is it necessary - we already check the filename.
    external_attr = MZ_READ_LE32(p + MZ_ZIP_CDH_EXTERNAL_ATTR_OFS);
    if ((external_attr & 0x10) != 0)
        return MZ_TRUE;

    return MZ_FALSE;
}

mz_bool mz_zip_reader_file_stat(mz_zip_archive *pZip, mz_uint file_index, mz_zip_archive_file_stat *pStat) {
    mz_uint n;
    const mz_uint8 *p = mz_zip_reader_get_cdh(pZip, file_index);

    if ((!p) || (!pStat))
        return MZ_FALSE;

    // Unpack the central directory record.
    pStat->m_file_index      = file_index;
    pStat->m_central_dir_ofs = MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, file_index);
    pStat->m_version_made_by = MZ_READ_LE16(p + MZ_ZIP_CDH_VERSION_MADE_BY_OFS);
    pStat->m_version_needed  = MZ_READ_LE16(p + MZ_ZIP_CDH_VERSION_NEEDED_OFS);
    pStat->m_bit_flag        = MZ_READ_LE16(p + MZ_ZIP_CDH_BIT_FLAG_OFS);
    pStat->m_method          = MZ_READ_LE16(p + MZ_ZIP_CDH_METHOD_OFS);
  #ifndef MINIZ_NO_TIME
    pStat->m_time = mz_zip_dos_to_time_t(MZ_READ_LE16(p + MZ_ZIP_CDH_FILE_TIME_OFS), MZ_READ_LE16(p + MZ_ZIP_CDH_FILE_DATE_OFS));
  #endif
    pStat->m_crc32            = MZ_READ_LE32(p + MZ_ZIP_CDH_CRC32_OFS);
    pStat->m_comp_size        = MZ_READ_LE32(p + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS);
    pStat->m_uncomp_size      = MZ_READ_LE32(p + MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS);
    pStat->m_internal_attr    = MZ_READ_LE16(p + MZ_ZIP_CDH_INTERNAL_ATTR_OFS);
    pStat->m_external_attr    = MZ_READ_LE32(p + MZ_ZIP_CDH_EXTERNAL_ATTR_OFS);
    pStat->m_local_header_ofs = MZ_READ_LE32(p + MZ_ZIP_CDH_LOCAL_HEADER_OFS);

    // Copy as much of the filename and comment as possible.
    n = MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS);
    n = MZ_MIN(n, MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE - 1);
    memcpy(pStat->m_filename, p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE, n);
    pStat->m_filename[n] = '\0';

    n = MZ_READ_LE16(p + MZ_ZIP_CDH_COMMENT_LEN_OFS);
    n = MZ_MIN(n, MZ_ZIP_MAX_ARCHIVE_FILE_COMMENT_SIZE - 1);
    pStat->m_comment_size = n;
    memcpy(pStat->m_comment, p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS) + MZ_READ_LE16(p + MZ_ZIP_CDH_EXTRA_LEN_OFS), n);
    pStat->m_comment[n] = '\0';

    return MZ_TRUE;
}

mz_uint mz_zip_reader_get_filename(mz_zip_archive *pZip, mz_uint file_index, char *pFilename, mz_uint filename_buf_size) {
    mz_uint n;
    const mz_uint8 *p = mz_zip_reader_get_cdh(pZip, file_index);

    if (!p) {
        if (filename_buf_size) pFilename[0] = '\0';
        return 0;
    }
    n = MZ_READ_LE16(p + MZ_ZIP_CDH_FILENAME_LEN_OFS);
    if (filename_buf_size) {
        n = MZ_MIN(n, filename_buf_size - 1);
        memcpy(pFilename, p + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE, n);
        pFilename[n] = '\0';
    }
    return n + 1;
}

static MZ_FORCEINLINE mz_bool mz_zip_reader_string_equal(const char *pA, const char *pB, mz_uint len, mz_uint flags) {
    mz_uint i;

    if (flags & MZ_ZIP_FLAG_CASE_SENSITIVE)
        return 0 == memcmp(pA, pB, len);
    for (i = 0; i < len; ++i)
        if (MZ_TOLOWER(pA[i]) != MZ_TOLOWER(pB[i]))
            return MZ_FALSE;
    return MZ_TRUE;
}

static MZ_FORCEINLINE int mz_zip_reader_filename_compare(const mz_zip_array *pCentral_dir_array, const mz_zip_array *pCentral_dir_offsets, mz_uint l_index, const char *pR, mz_uint r_len) {
    const mz_uint8 *pL = &MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_array, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(pCentral_dir_offsets, mz_uint32, l_index)), *pE;
    mz_uint l_len      = MZ_READ_LE16(pL + MZ_ZIP_CDH_FILENAME_LEN_OFS);
    mz_uint8 l         = 0, r = 0;

    pL += MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;
    pE  = pL + MZ_MIN(l_len, r_len);
    while (pL < pE) {
        if ((l = MZ_TOLOWER(*pL)) != (r = MZ_TOLOWER(*pR)))
            break;
        pL++;
        pR++;
    }
    return (pL == pE) ? (int)(l_len - r_len) : (l - r);
}

static int mz_zip_reader_locate_file_binary_search(mz_zip_archive *pZip, const char *pFilename) {
    mz_zip_internal_state *pState            = pZip->m_pState;
    const mz_zip_array *pCentral_dir_offsets = &pState->m_central_dir_offsets;
    const mz_zip_array *pCentral_dir         = &pState->m_central_dir;
    mz_uint32 *pIndices        = &MZ_ZIP_ARRAY_ELEMENT(&pState->m_sorted_central_dir_offsets, mz_uint32, 0);
    const int size             = pZip->m_total_files;
    const mz_uint filename_len = (mz_uint)strlen(pFilename);
    int l = 0, h = size - 1;

    while (l <= h) {
        int m = (l + h) >> 1, file_index = pIndices[m], comp = mz_zip_reader_filename_compare(pCentral_dir, pCentral_dir_offsets, file_index, pFilename, filename_len);
        if (!comp)
            return file_index;
        else if (comp < 0)
            l = m + 1;
        else
            h = m - 1;
    }
    return -1;
}

int mz_zip_reader_locate_file(mz_zip_archive *pZip, const char *pName, const char *pComment, mz_uint flags) {
    mz_uint file_index;
    size_t name_len, comment_len;

    if ((!pZip) || (!pZip->m_pState) || (!pName) || (pZip->m_zip_mode != MZ_ZIP_MODE_READING))
        return -1;
    if (((flags & (MZ_ZIP_FLAG_IGNORE_PATH | MZ_ZIP_FLAG_CASE_SENSITIVE)) == 0) && (!pComment) && (pZip->m_pState->m_sorted_central_dir_offsets.m_size))
        return mz_zip_reader_locate_file_binary_search(pZip, pName);
    name_len = strlen(pName);
    if (name_len > 0xFFFF) return -1;
    comment_len = pComment ? strlen(pComment) : 0;
    if (comment_len > 0xFFFF) return -1;
    for (file_index = 0; file_index < pZip->m_total_files; file_index++) {
        const mz_uint8 *pHeader = &MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir, mz_uint8, MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, file_index));
        mz_uint filename_len    = MZ_READ_LE16(pHeader + MZ_ZIP_CDH_FILENAME_LEN_OFS);
        const char *pFilename   = (const char *)pHeader + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;
        if (filename_len < name_len)
            continue;
        if (comment_len) {
            mz_uint file_extra_len    = MZ_READ_LE16(pHeader + MZ_ZIP_CDH_EXTRA_LEN_OFS), file_comment_len = MZ_READ_LE16(pHeader + MZ_ZIP_CDH_COMMENT_LEN_OFS);
            const char *pFile_comment = pFilename + filename_len + file_extra_len;
            if ((file_comment_len != comment_len) || (!mz_zip_reader_string_equal(pComment, pFile_comment, file_comment_len, flags)))
                continue;
        }
        if ((flags & MZ_ZIP_FLAG_IGNORE_PATH) && (filename_len)) {
            int ofs = filename_len - 1;
            do {
                if ((pFilename[ofs] == '/') || (pFilename[ofs] == '\\') || (pFilename[ofs] == ':'))
                    break;
            } while (--ofs >= 0);
            ofs++;
            pFilename    += ofs;
            filename_len -= ofs;
        }
        if ((filename_len == name_len) && (mz_zip_reader_string_equal(pName, pFilename, filename_len, flags)))
            return file_index;
    }
    return -1;
}

mz_bool mz_zip_reader_extract_to_mem_no_alloc(mz_zip_archive *pZip, mz_uint file_index, void *pBuf, size_t buf_size, mz_uint flags, void *pUser_read_buf, size_t user_read_buf_size) {
    int status = TINFL_STATUS_DONE;
    mz_uint64 needed_size, cur_file_ofs, comp_remaining, out_buf_ofs = 0, read_buf_size, read_buf_ofs = 0, read_buf_avail;
    mz_zip_archive_file_stat file_stat;
    void *pRead_buf;
    mz_uint32 local_header_u32[(MZ_ZIP_LOCAL_DIR_HEADER_SIZE + sizeof(mz_uint32) - 1) / sizeof(mz_uint32)];
    mz_uint8 *pLocal_header = (mz_uint8 *)local_header_u32;
    tinfl_decompressor inflator;

    if ((buf_size) && (!pBuf))
        return MZ_FALSE;

    if (!mz_zip_reader_file_stat(pZip, file_index, &file_stat))
        return MZ_FALSE;

    // Empty file, or a directory (but not always a directory - I've seen odd zips with directories that have compressed data which inflates to 0 bytes)
    if (!file_stat.m_comp_size)
        return MZ_TRUE;

    // Entry is a subdirectory (I've seen old zips with dir entries which have compressed deflate data which inflates to 0 bytes, but these entries claim to uncompress to 512 bytes in the headers).
    // I'm torn how to handle this case - should it fail instead?
    if (mz_zip_reader_is_file_a_directory(pZip, file_index))
        return MZ_TRUE;

    // Encryption and patch files are not supported.
    if (file_stat.m_bit_flag & (1 | 32))
        return MZ_FALSE;

    // This function only supports stored and deflate.
    if ((!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA)) && (file_stat.m_method != 0) && (file_stat.m_method != MZ_DEFLATED))
        return MZ_FALSE;

    // Ensure supplied output buffer is large enough.
    needed_size = (flags & MZ_ZIP_FLAG_COMPRESSED_DATA) ? file_stat.m_comp_size : file_stat.m_uncomp_size;
    if (buf_size < needed_size)
        return MZ_FALSE;

    // Read and parse the local directory entry.
    cur_file_ofs = file_stat.m_local_header_ofs;
    if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
        return MZ_FALSE;
    if (MZ_READ_LE32(pLocal_header) != MZ_ZIP_LOCAL_DIR_HEADER_SIG)
        return MZ_FALSE;

    cur_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_FILENAME_LEN_OFS) + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_EXTRA_LEN_OFS);
    if ((cur_file_ofs + file_stat.m_comp_size) > pZip->m_archive_size)
        return MZ_FALSE;

    if ((flags & MZ_ZIP_FLAG_COMPRESSED_DATA) || (!file_stat.m_method)) {
        // The file is stored or the caller has requested the compressed data.
        if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pBuf, (size_t)needed_size) != needed_size)
            return MZ_FALSE;
        return ((flags & MZ_ZIP_FLAG_COMPRESSED_DATA) != 0) || (mz_crc32(MZ_CRC32_INIT, (const mz_uint8 *)pBuf, (size_t)file_stat.m_uncomp_size) == file_stat.m_crc32);
    }

    // Decompress the file either directly from memory or from a file input buffer.
    tinfl_init(&inflator);

    if (pZip->m_pState->m_pMem) {
        // Read directly from the archive in memory.
        pRead_buf      = (mz_uint8 *)pZip->m_pState->m_pMem + cur_file_ofs;
        read_buf_size  = read_buf_avail = file_stat.m_comp_size;
        comp_remaining = 0;
    } else if (pUser_read_buf) {
        // Use a user provided read buffer.
        if (!user_read_buf_size)
            return MZ_FALSE;
        pRead_buf      = (mz_uint8 *)pUser_read_buf;
        read_buf_size  = user_read_buf_size;
        read_buf_avail = 0;
        comp_remaining = file_stat.m_comp_size;
    } else {
        // Temporarily allocate a read buffer.
        read_buf_size = MZ_MIN(file_stat.m_comp_size, MZ_ZIP_MAX_IO_BUF_SIZE);
  #ifdef _MSC_VER
        if (((0, (sizeof(size_t) == sizeof(mz_uint32)))) && (read_buf_size > 0x7FFFFFFF))
  #else
        if (((sizeof(size_t) == sizeof(mz_uint32))) && (read_buf_size > 0x7FFFFFFF))
  #endif
            return MZ_FALSE;
        if (NULL == (pRead_buf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, (size_t)read_buf_size)))
            return MZ_FALSE;
        read_buf_avail = 0;
        comp_remaining = file_stat.m_comp_size;
    }

    do {
        size_t in_buf_size, out_buf_size = (size_t)(file_stat.m_uncomp_size - out_buf_ofs);
        if ((!read_buf_avail) && (!pZip->m_pState->m_pMem)) {
            read_buf_avail = MZ_MIN(read_buf_size, comp_remaining);
            if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pRead_buf, (size_t)read_buf_avail) != read_buf_avail) {
                status = TINFL_STATUS_FAILED;
                break;
            }
            cur_file_ofs   += read_buf_avail;
            comp_remaining -= read_buf_avail;
            read_buf_ofs    = 0;
        }
        in_buf_size     = (size_t)read_buf_avail;
        status          = tinfl_decompress(&inflator, (mz_uint8 *)pRead_buf + read_buf_ofs, &in_buf_size, (mz_uint8 *)pBuf, (mz_uint8 *)pBuf + out_buf_ofs, &out_buf_size, TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF | (comp_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0));
        read_buf_avail -= in_buf_size;
        read_buf_ofs   += in_buf_size;
        out_buf_ofs    += out_buf_size;
    } while (status == TINFL_STATUS_NEEDS_MORE_INPUT);

    if (status == TINFL_STATUS_DONE) {
        // Make sure the entire file was decompressed, and check its CRC.
        if ((out_buf_ofs != file_stat.m_uncomp_size) || (mz_crc32(MZ_CRC32_INIT, (const mz_uint8 *)pBuf, (size_t)file_stat.m_uncomp_size) != file_stat.m_crc32))
            status = TINFL_STATUS_FAILED;
    }

    if ((!pZip->m_pState->m_pMem) && (!pUser_read_buf))
        pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);

    return status == TINFL_STATUS_DONE;
}

mz_bool mz_zip_reader_extract_file_to_mem_no_alloc(mz_zip_archive *pZip, const char *pFilename, void *pBuf, size_t buf_size, mz_uint flags, void *pUser_read_buf, size_t user_read_buf_size) {
    int file_index = mz_zip_reader_locate_file(pZip, pFilename, NULL, flags);

    if (file_index < 0)
        return MZ_FALSE;
    return mz_zip_reader_extract_to_mem_no_alloc(pZip, file_index, pBuf, buf_size, flags, pUser_read_buf, user_read_buf_size);
}

mz_bool mz_zip_reader_extract_to_mem(mz_zip_archive *pZip, mz_uint file_index, void *pBuf, size_t buf_size, mz_uint flags) {
    return mz_zip_reader_extract_to_mem_no_alloc(pZip, file_index, pBuf, buf_size, flags, NULL, 0);
}

mz_bool mz_zip_reader_extract_file_to_mem(mz_zip_archive *pZip, const char *pFilename, void *pBuf, size_t buf_size, mz_uint flags) {
    return mz_zip_reader_extract_file_to_mem_no_alloc(pZip, pFilename, pBuf, buf_size, flags, NULL, 0);
}

void *mz_zip_reader_extract_to_heap(mz_zip_archive *pZip, mz_uint file_index, size_t *pSize, mz_uint flags) {
    mz_uint64 comp_size, uncomp_size, alloc_size;
    const mz_uint8 *p = mz_zip_reader_get_cdh(pZip, file_index);
    void *pBuf;

    if (pSize)
        *pSize = 0;
    if (!p)
        return NULL;

    comp_size   = MZ_READ_LE32(p + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS);
    uncomp_size = MZ_READ_LE32(p + MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS);

    alloc_size = (flags & MZ_ZIP_FLAG_COMPRESSED_DATA) ? comp_size : uncomp_size;
  #ifdef _MSC_VER
    if (((0, (sizeof(size_t) == sizeof(mz_uint32)))) && (alloc_size > 0x7FFFFFFF))
  #else
    if (((sizeof(size_t) == sizeof(mz_uint32))) && (alloc_size > 0x7FFFFFFF))
  #endif
        return NULL;
    if (NULL == (pBuf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, (size_t)alloc_size)))
        return NULL;

    if (!mz_zip_reader_extract_to_mem(pZip, file_index, pBuf, (size_t)alloc_size, flags)) {
        pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
        return NULL;
    }

    if (pSize) *pSize = (size_t)alloc_size;
    return pBuf;
}

void *mz_zip_reader_extract_file_to_heap(mz_zip_archive *pZip, const char *pFilename, size_t *pSize, mz_uint flags) {
    int file_index = mz_zip_reader_locate_file(pZip, pFilename, NULL, flags);

    if (file_index < 0) {
        if (pSize) *pSize = 0;
        return MZ_FALSE;
    }
    return mz_zip_reader_extract_to_heap(pZip, file_index, pSize, flags);
}

mz_bool mz_zip_reader_extract_to_callback(mz_zip_archive *pZip, mz_uint file_index, mz_file_write_func pCallback, void *pOpaque, mz_uint flags) {
    int status         = TINFL_STATUS_DONE;
    mz_uint file_crc32 = MZ_CRC32_INIT;
    mz_uint64 read_buf_size, read_buf_ofs = 0, read_buf_avail, comp_remaining, out_buf_ofs = 0, cur_file_ofs;
    mz_zip_archive_file_stat file_stat;
    void *pRead_buf  = NULL;
    void *pWrite_buf = NULL;
    mz_uint32 local_header_u32[(MZ_ZIP_LOCAL_DIR_HEADER_SIZE + sizeof(mz_uint32) - 1) / sizeof(mz_uint32)];
    mz_uint8 *pLocal_header = (mz_uint8 *)local_header_u32;

    if (!mz_zip_reader_file_stat(pZip, file_index, &file_stat))
        return MZ_FALSE;

    // Empty file, or a directory (but not always a directory - I've seen odd zips with directories that have compressed data which inflates to 0 bytes)
    if (!file_stat.m_comp_size)
        return MZ_TRUE;

    // Entry is a subdirectory (I've seen old zips with dir entries which have compressed deflate data which inflates to 0 bytes, but these entries claim to uncompress to 512 bytes in the headers).
    // I'm torn how to handle this case - should it fail instead?
    if (mz_zip_reader_is_file_a_directory(pZip, file_index))
        return MZ_TRUE;

    // Encryption and patch files are not supported.
    if (file_stat.m_bit_flag & (1 | 32))
        return MZ_FALSE;

    // This function only supports stored and deflate.
    if ((!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA)) && (file_stat.m_method != 0) && (file_stat.m_method != MZ_DEFLATED))
        return MZ_FALSE;

    // Read and parse the local directory entry.
    cur_file_ofs = file_stat.m_local_header_ofs;
    if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
        return MZ_FALSE;
    if (MZ_READ_LE32(pLocal_header) != MZ_ZIP_LOCAL_DIR_HEADER_SIG)
        return MZ_FALSE;

    cur_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_FILENAME_LEN_OFS) + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_EXTRA_LEN_OFS);
    if ((cur_file_ofs + file_stat.m_comp_size) > pZip->m_archive_size)
        return MZ_FALSE;

    // Decompress the file either directly from memory or from a file input buffer.
    if (pZip->m_pState->m_pMem) {
        pRead_buf      = (mz_uint8 *)pZip->m_pState->m_pMem + cur_file_ofs;
        read_buf_size  = read_buf_avail = file_stat.m_comp_size;
        comp_remaining = 0;
    } else {
        read_buf_size = MZ_MIN(file_stat.m_comp_size, MZ_ZIP_MAX_IO_BUF_SIZE);
        if (NULL == (pRead_buf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, (size_t)read_buf_size)))
            return MZ_FALSE;
        read_buf_avail = 0;
        comp_remaining = file_stat.m_comp_size;
    }

    if ((flags & MZ_ZIP_FLAG_COMPRESSED_DATA) || (!file_stat.m_method)) {
        // The file is stored or the caller has requested the compressed data.
        if (pZip->m_pState->m_pMem) {
  #ifdef _MSC_VER
            if (((0, (sizeof(size_t) == sizeof(mz_uint32)))) && (file_stat.m_comp_size > 0xFFFFFFFF))
  #else
            if (((sizeof(size_t) == sizeof(mz_uint32))) && (file_stat.m_comp_size > 0xFFFFFFFF))
  #endif
                return MZ_FALSE;
            if (pCallback(pOpaque, out_buf_ofs, pRead_buf, (size_t)file_stat.m_comp_size) != file_stat.m_comp_size)
                status = TINFL_STATUS_FAILED;
            else if (!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA))
                file_crc32 = (mz_uint32)mz_crc32(file_crc32, (const mz_uint8 *)pRead_buf, (size_t)file_stat.m_comp_size);
            cur_file_ofs  += file_stat.m_comp_size;
            out_buf_ofs   += file_stat.m_comp_size;
            comp_remaining = 0;
        } else {
            while (comp_remaining) {
                read_buf_avail = MZ_MIN(read_buf_size, comp_remaining);
                if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pRead_buf, (size_t)read_buf_avail) != read_buf_avail) {
                    status = TINFL_STATUS_FAILED;
                    break;
                }

                if (!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA))
                    file_crc32 = (mz_uint32)mz_crc32(file_crc32, (const mz_uint8 *)pRead_buf, (size_t)read_buf_avail);

                if (pCallback(pOpaque, out_buf_ofs, pRead_buf, (size_t)read_buf_avail) != read_buf_avail) {
                    status = TINFL_STATUS_FAILED;
                    break;
                }
                cur_file_ofs   += read_buf_avail;
                out_buf_ofs    += read_buf_avail;
                comp_remaining -= read_buf_avail;
            }
        }
    } else {
        tinfl_decompressor inflator;
        tinfl_init(&inflator);

        if (NULL == (pWrite_buf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, TINFL_LZ_DICT_SIZE)))
            status = TINFL_STATUS_FAILED;
        else {
            do {
                mz_uint8 *pWrite_buf_cur = (mz_uint8 *)pWrite_buf + (out_buf_ofs & (TINFL_LZ_DICT_SIZE - 1));
                size_t in_buf_size, out_buf_size = TINFL_LZ_DICT_SIZE - (out_buf_ofs & (TINFL_LZ_DICT_SIZE - 1));
                if ((!read_buf_avail) && (!pZip->m_pState->m_pMem)) {
                    read_buf_avail = MZ_MIN(read_buf_size, comp_remaining);
                    if (pZip->m_pRead(pZip->m_pIO_opaque, cur_file_ofs, pRead_buf, (size_t)read_buf_avail) != read_buf_avail) {
                        status = TINFL_STATUS_FAILED;
                        break;
                    }
                    cur_file_ofs   += read_buf_avail;
                    comp_remaining -= read_buf_avail;
                    read_buf_ofs    = 0;
                }

                in_buf_size     = (size_t)read_buf_avail;
                status          = tinfl_decompress(&inflator, (const mz_uint8 *)pRead_buf + read_buf_ofs, &in_buf_size, (mz_uint8 *)pWrite_buf, pWrite_buf_cur, &out_buf_size, comp_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0);
                read_buf_avail -= in_buf_size;
                read_buf_ofs   += in_buf_size;

                if (out_buf_size) {
                    if (pCallback(pOpaque, out_buf_ofs, pWrite_buf_cur, out_buf_size) != out_buf_size) {
                        status = TINFL_STATUS_FAILED;
                        break;
                    }
                    file_crc32 = (mz_uint32)mz_crc32(file_crc32, pWrite_buf_cur, out_buf_size);
                    if ((out_buf_ofs += out_buf_size) > file_stat.m_uncomp_size) {
                        status = TINFL_STATUS_FAILED;
                        break;
                    }
                }
            } while ((status == TINFL_STATUS_NEEDS_MORE_INPUT) || (status == TINFL_STATUS_HAS_MORE_OUTPUT));
        }
    }

    if ((status == TINFL_STATUS_DONE) && (!(flags & MZ_ZIP_FLAG_COMPRESSED_DATA))) {
        // Make sure the entire file was decompressed, and check its CRC.
        if ((out_buf_ofs != file_stat.m_uncomp_size) || (file_crc32 != file_stat.m_crc32))
            status = TINFL_STATUS_FAILED;
    }

    if (!pZip->m_pState->m_pMem)
        pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
    if (pWrite_buf)
        pZip->m_pFree(pZip->m_pAlloc_opaque, pWrite_buf);

    return status == TINFL_STATUS_DONE;
}

mz_bool mz_zip_reader_extract_file_to_callback(mz_zip_archive *pZip, const char *pFilename, mz_file_write_func pCallback, void *pOpaque, mz_uint flags) {
    int file_index = mz_zip_reader_locate_file(pZip, pFilename, NULL, flags);

    if (file_index < 0)
        return MZ_FALSE;
    return mz_zip_reader_extract_to_callback(pZip, file_index, pCallback, pOpaque, flags);
}

  #ifndef MINIZ_NO_STDIO
static size_t mz_zip_file_write_callback(void *pOpaque, mz_uint64 ofs, const void *pBuf, size_t n) {
    (void)ofs;
    return MZ_FWRITE(pBuf, 1, n, (MZ_FILE *)pOpaque);
}

mz_bool mz_zip_reader_extract_to_file(mz_zip_archive *pZip, mz_uint file_index, const char *pDst_filename, mz_uint flags) {
    mz_bool status;
    mz_zip_archive_file_stat file_stat;
    MZ_FILE *pFile;

    if (!mz_zip_reader_file_stat(pZip, file_index, &file_stat))
        return MZ_FALSE;
    pFile = MZ_FOPEN(pDst_filename, "wb");
    if (!pFile)
        return MZ_FALSE;
    status = mz_zip_reader_extract_to_callback(pZip, file_index, mz_zip_file_write_callback, pFile, flags);
    if (MZ_FCLOSE(pFile) == EOF)
        return MZ_FALSE;
   #ifndef MINIZ_NO_TIME
    if (status)
        mz_zip_set_file_times(pDst_filename, file_stat.m_time, file_stat.m_time);
   #endif
    return status;
}
  #endif // #ifndef MINIZ_NO_STDIO

mz_bool mz_zip_reader_end(mz_zip_archive *pZip) {
    if ((!pZip) || (!pZip->m_pState) || (!pZip->m_pAlloc) || (!pZip->m_pFree) || (pZip->m_zip_mode != MZ_ZIP_MODE_READING))
        return MZ_FALSE;

    if (pZip->m_pState) {
        mz_zip_internal_state *pState = pZip->m_pState;
        pZip->m_pState = NULL;
        mz_zip_array_clear(pZip, &pState->m_central_dir);
        mz_zip_array_clear(pZip, &pState->m_central_dir_offsets);
        mz_zip_array_clear(pZip, &pState->m_sorted_central_dir_offsets);

  #ifndef MINIZ_NO_STDIO
        if (pState->m_pFile) {
            MZ_FCLOSE(pState->m_pFile);
            pState->m_pFile = NULL;
        }
  #endif // #ifndef MINIZ_NO_STDIO

        pZip->m_pFree(pZip->m_pAlloc_opaque, pState);
    }
    pZip->m_zip_mode = MZ_ZIP_MODE_INVALID;

    return MZ_TRUE;
}

  #ifndef MINIZ_NO_STDIO
mz_bool mz_zip_reader_extract_file_to_file(mz_zip_archive *pZip, const char *pArchive_filename, const char *pDst_filename, mz_uint flags) {
    int file_index = mz_zip_reader_locate_file(pZip, pArchive_filename, NULL, flags);

    if (file_index < 0)
        return MZ_FALSE;
    return mz_zip_reader_extract_to_file(pZip, file_index, pDst_filename, flags);
}
  #endif

// ------------------- .ZIP archive writing

  #ifndef MINIZ_NO_ARCHIVE_WRITING_APIS

static void mz_write_le16(mz_uint8 *p, mz_uint16 v) {
    p[0] = (mz_uint8)v;
    p[1] = (mz_uint8)(v >> 8);
}

static void mz_write_le32(mz_uint8 *p, mz_uint32 v) {
    p[0] = (mz_uint8)v;
    p[1] = (mz_uint8)(v >> 8);
    p[2] = (mz_uint8)(v >> 16);
    p[3] = (mz_uint8)(v >> 24);
}

   #define MZ_WRITE_LE16(p, v)    mz_write_le16((mz_uint8 *)(p), (mz_uint16)(v))
   #define MZ_WRITE_LE32(p, v)    mz_write_le32((mz_uint8 *)(p), (mz_uint32)(v))

mz_bool mz_zip_writer_init(mz_zip_archive *pZip, mz_uint64 existing_size) {
    if ((!pZip) || (pZip->m_pState) || (!pZip->m_pWrite) || (pZip->m_zip_mode != MZ_ZIP_MODE_INVALID))
        return MZ_FALSE;

    if (pZip->m_file_offset_alignment) {
        // Ensure user specified file offset alignment is a power of 2.
        if (pZip->m_file_offset_alignment & (pZip->m_file_offset_alignment - 1))
            return MZ_FALSE;
    }

    if (!pZip->m_pAlloc) pZip->m_pAlloc = def_alloc_func;
    if (!pZip->m_pFree) pZip->m_pFree = def_free_func;
    if (!pZip->m_pRealloc) pZip->m_pRealloc = def_realloc_func;

    pZip->m_zip_mode     = MZ_ZIP_MODE_WRITING;
    pZip->m_archive_size = existing_size;
    pZip->m_central_directory_file_ofs = 0;
    pZip->m_total_files = 0;

    if (NULL == (pZip->m_pState = (mz_zip_internal_state *)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(mz_zip_internal_state))))
        return MZ_FALSE;
    memset(pZip->m_pState, 0, sizeof(mz_zip_internal_state));
    MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_central_dir, sizeof(mz_uint8));
    MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_central_dir_offsets, sizeof(mz_uint32));
    MZ_ZIP_ARRAY_SET_ELEMENT_SIZE(&pZip->m_pState->m_sorted_central_dir_offsets, sizeof(mz_uint32));
    return MZ_TRUE;
}

static size_t mz_zip_heap_write_func(void *pOpaque, mz_uint64 file_ofs, const void *pBuf, size_t n) {
    mz_zip_archive *pZip          = (mz_zip_archive *)pOpaque;
    mz_zip_internal_state *pState = pZip->m_pState;
    mz_uint64 new_size            = MZ_MAX(file_ofs + n, pState->m_mem_size);

   #ifdef _MSC_VER
    if ((!n) || ((0, (sizeof(size_t) == sizeof(mz_uint32))) && (new_size > 0x7FFFFFFF)))
   #else
    if ((!n) || ((sizeof(size_t) == sizeof(mz_uint32)) && (new_size > 0x7FFFFFFF)))
   #endif
        return 0;
    if (new_size > pState->m_mem_capacity) {
        void *pNew_block;
        size_t new_capacity = MZ_MAX(64, pState->m_mem_capacity);
        while (new_capacity < new_size) new_capacity *= 2;
        if (NULL == (pNew_block = pZip->m_pRealloc(pZip->m_pAlloc_opaque, pState->m_pMem, 1, new_capacity)))
            return 0;
        pState->m_pMem         = pNew_block;
        pState->m_mem_capacity = new_capacity;
    }
    memcpy((mz_uint8 *)pState->m_pMem + file_ofs, pBuf, n);
    pState->m_mem_size = (size_t)new_size;
    return n;
}

mz_bool mz_zip_writer_init_heap(mz_zip_archive *pZip, size_t size_to_reserve_at_beginning, size_t initial_allocation_size) {
    pZip->m_pWrite     = mz_zip_heap_write_func;
    pZip->m_pIO_opaque = pZip;
    if (!mz_zip_writer_init(pZip, size_to_reserve_at_beginning))
        return MZ_FALSE;
    if (0 != (initial_allocation_size = MZ_MAX(initial_allocation_size, size_to_reserve_at_beginning))) {
        if (NULL == (pZip->m_pState->m_pMem = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, initial_allocation_size))) {
            mz_zip_writer_end(pZip);
            return MZ_FALSE;
        }
        pZip->m_pState->m_mem_capacity = initial_allocation_size;
    }
    return MZ_TRUE;
}

   #ifndef MINIZ_NO_STDIO
static size_t mz_zip_file_write_func(void *pOpaque, mz_uint64 file_ofs, const void *pBuf, size_t n) {
    mz_zip_archive *pZip = (mz_zip_archive *)pOpaque;
    mz_int64 cur_ofs     = MZ_FTELL64(pZip->m_pState->m_pFile);

    if (((mz_int64)file_ofs < 0) || (((cur_ofs != (mz_int64)file_ofs)) && (MZ_FSEEK64(pZip->m_pState->m_pFile, (mz_int64)file_ofs, SEEK_SET))))
        return 0;
    return MZ_FWRITE(pBuf, 1, n, pZip->m_pState->m_pFile);
}

mz_bool mz_zip_writer_init_file(mz_zip_archive *pZip, const char *pFilename, mz_uint64 size_to_reserve_at_beginning) {
    MZ_FILE *pFile;

    pZip->m_pWrite     = mz_zip_file_write_func;
    pZip->m_pIO_opaque = pZip;
    if (!mz_zip_writer_init(pZip, size_to_reserve_at_beginning))
        return MZ_FALSE;
    if (NULL == (pFile = MZ_FOPEN(pFilename, "wb"))) {
        mz_zip_writer_end(pZip);
        return MZ_FALSE;
    }
    pZip->m_pState->m_pFile = pFile;
    if (size_to_reserve_at_beginning) {
        mz_uint64 cur_ofs = 0;
        char buf[4096];
        MZ_CLEAR_OBJ(buf);
        do {
            size_t n = (size_t)MZ_MIN(sizeof(buf), size_to_reserve_at_beginning);
            if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_ofs, buf, n) != n) {
                mz_zip_writer_end(pZip);
                return MZ_FALSE;
            }
            cur_ofs += n;
            size_to_reserve_at_beginning -= n;
        } while (size_to_reserve_at_beginning);
    }
    return MZ_TRUE;
}
   #endif // #ifndef MINIZ_NO_STDIO

mz_bool mz_zip_writer_init_from_reader(mz_zip_archive *pZip, const char *pFilename) {
    mz_zip_internal_state *pState;

    if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_READING))
        return MZ_FALSE;
    // No sense in trying to write to an archive that's already at the support max size
    if ((pZip->m_total_files == 0xFFFF) || ((pZip->m_archive_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + MZ_ZIP_LOCAL_DIR_HEADER_SIZE) > 0xFFFFFFFF))
        return MZ_FALSE;

    pState = pZip->m_pState;

    if (pState->m_pFile) {
   #ifdef MINIZ_NO_STDIO
        pFilename;
        return MZ_FALSE;
   #else
        // Archive is being read from stdio - try to reopen as writable.
        if (pZip->m_pIO_opaque != pZip)
            return MZ_FALSE;
        if (!pFilename)
            return MZ_FALSE;
        pZip->m_pWrite = mz_zip_file_write_func;
        if (NULL == (pState->m_pFile = MZ_FREOPEN(pFilename, "r+b", pState->m_pFile))) {
            // The mz_zip_archive is now in a bogus state because pState->m_pFile is NULL, so just close it.
            mz_zip_reader_end(pZip);
            return MZ_FALSE;
        }
   #endif // #ifdef MINIZ_NO_STDIO
    } else if (pState->m_pMem) {
        // Archive lives in a memory block. Assume it's from the heap that we can resize using the realloc callback.
        if (pZip->m_pIO_opaque != pZip)
            return MZ_FALSE;
        pState->m_mem_capacity = pState->m_mem_size;
        pZip->m_pWrite         = mz_zip_heap_write_func;
    }
    // Archive is being read via a user provided read function - make sure the user has specified a write function too.
    else if (!pZip->m_pWrite)
        return MZ_FALSE;

    // Start writing new files at the archive's current central directory location.
    pZip->m_archive_size = pZip->m_central_directory_file_ofs;
    pZip->m_zip_mode     = MZ_ZIP_MODE_WRITING;
    pZip->m_central_directory_file_ofs = 0;

    return MZ_TRUE;
}

mz_bool mz_zip_writer_add_mem(mz_zip_archive *pZip, const char *pArchive_name, const void *pBuf, size_t buf_size, mz_uint level_and_flags) {
    return mz_zip_writer_add_mem_ex(pZip, pArchive_name, pBuf, buf_size, NULL, 0, level_and_flags, 0, 0);
}

typedef struct {
    mz_zip_archive *m_pZip;
    mz_uint64 m_cur_archive_file_ofs;
    mz_uint64 m_comp_size;
} mz_zip_writer_add_state;

static mz_bool mz_zip_writer_add_put_buf_callback(const void *pBuf, int len, void *pUser) {
    mz_zip_writer_add_state *pState = (mz_zip_writer_add_state *)pUser;

    if ((int)pState->m_pZip->m_pWrite(pState->m_pZip->m_pIO_opaque, pState->m_cur_archive_file_ofs, pBuf, len) != len)
        return MZ_FALSE;
    pState->m_cur_archive_file_ofs += len;
    pState->m_comp_size            += len;
    return MZ_TRUE;
}

static mz_bool mz_zip_writer_create_local_dir_header(mz_zip_archive *pZip, mz_uint8 *pDst, mz_uint16 filename_size, mz_uint16 extra_size, mz_uint64 uncomp_size, mz_uint64 comp_size, mz_uint32 uncomp_crc32, mz_uint16 method, mz_uint16 bit_flags, mz_uint16 dos_time, mz_uint16 dos_date) {
    (void)pZip;
    memset(pDst, 0, MZ_ZIP_LOCAL_DIR_HEADER_SIZE);
    MZ_WRITE_LE32(pDst + MZ_ZIP_LDH_SIG_OFS, MZ_ZIP_LOCAL_DIR_HEADER_SIG);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_VERSION_NEEDED_OFS, method ? 20 : 0);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_BIT_FLAG_OFS, bit_flags);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_METHOD_OFS, method);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_FILE_TIME_OFS, dos_time);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_FILE_DATE_OFS, dos_date);
    MZ_WRITE_LE32(pDst + MZ_ZIP_LDH_CRC32_OFS, uncomp_crc32);
    MZ_WRITE_LE32(pDst + MZ_ZIP_LDH_COMPRESSED_SIZE_OFS, comp_size);
    MZ_WRITE_LE32(pDst + MZ_ZIP_LDH_DECOMPRESSED_SIZE_OFS, uncomp_size);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_FILENAME_LEN_OFS, filename_size);
    MZ_WRITE_LE16(pDst + MZ_ZIP_LDH_EXTRA_LEN_OFS, extra_size);
    return MZ_TRUE;
}

static mz_bool mz_zip_writer_create_central_dir_header(mz_zip_archive *pZip, mz_uint8 *pDst, mz_uint16 filename_size, mz_uint16 extra_size, mz_uint16 comment_size, mz_uint64 uncomp_size, mz_uint64 comp_size, mz_uint32 uncomp_crc32, mz_uint16 method, mz_uint16 bit_flags, mz_uint16 dos_time, mz_uint16 dos_date, mz_uint64 local_header_ofs, mz_uint32 ext_attributes) {
    (void)pZip;
    memset(pDst, 0, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE);
    MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_SIG_OFS, MZ_ZIP_CENTRAL_DIR_HEADER_SIG);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_VERSION_NEEDED_OFS, method ? 20 : 0);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_BIT_FLAG_OFS, bit_flags);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_METHOD_OFS, method);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_FILE_TIME_OFS, dos_time);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_FILE_DATE_OFS, dos_date);
    MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_CRC32_OFS, uncomp_crc32);
    MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS, comp_size);
    MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS, uncomp_size);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_FILENAME_LEN_OFS, filename_size);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_EXTRA_LEN_OFS, extra_size);
    MZ_WRITE_LE16(pDst + MZ_ZIP_CDH_COMMENT_LEN_OFS, comment_size);
    MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_EXTERNAL_ATTR_OFS, ext_attributes);
    MZ_WRITE_LE32(pDst + MZ_ZIP_CDH_LOCAL_HEADER_OFS, local_header_ofs);
    return MZ_TRUE;
}

static mz_bool mz_zip_writer_add_to_central_dir(mz_zip_archive *pZip, const char *pFilename, mz_uint16 filename_size, const void *pExtra, mz_uint16 extra_size, const void *pComment, mz_uint16 comment_size, mz_uint64 uncomp_size, mz_uint64 comp_size, mz_uint32 uncomp_crc32, mz_uint16 method, mz_uint16 bit_flags, mz_uint16 dos_time, mz_uint16 dos_date, mz_uint64 local_header_ofs, mz_uint32 ext_attributes) {
    mz_zip_internal_state *pState = pZip->m_pState;
    mz_uint32 central_dir_ofs     = (mz_uint32)pState->m_central_dir.m_size;
    size_t orig_central_dir_size  = pState->m_central_dir.m_size;
    mz_uint8 central_dir_header[MZ_ZIP_CENTRAL_DIR_HEADER_SIZE];

    // No zip64 support yet
    if ((local_header_ofs > 0xFFFFFFFF) || (((mz_uint64)pState->m_central_dir.m_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + filename_size + extra_size + comment_size) > 0xFFFFFFFF))
        return MZ_FALSE;

    if (!mz_zip_writer_create_central_dir_header(pZip, central_dir_header, filename_size, extra_size, comment_size, uncomp_size, comp_size, uncomp_crc32, method, bit_flags, dos_time, dos_date, local_header_ofs, ext_attributes))
        return MZ_FALSE;

    if ((!mz_zip_array_push_back(pZip, &pState->m_central_dir, central_dir_header, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE)) ||
        (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pFilename, filename_size)) ||
        (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pExtra, extra_size)) ||
        (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pComment, comment_size)) ||
        (!mz_zip_array_push_back(pZip, &pState->m_central_dir_offsets, &central_dir_ofs, 1))) {
        // Try to push the central directory array back into its original state.
        mz_zip_array_resize(pZip, &pState->m_central_dir, orig_central_dir_size, MZ_FALSE);
        return MZ_FALSE;
    }

    return MZ_TRUE;
}

static mz_bool mz_zip_writer_validate_archive_name(const char *pArchive_name) {
    // Basic ZIP archive filename validity checks: Valid filenames cannot start with a forward slash, cannot contain a drive letter, and cannot use DOS-style backward slashes.
    if (*pArchive_name == '/')
        return MZ_FALSE;
    while (*pArchive_name) {
        if ((*pArchive_name == '\\') || (*pArchive_name == ':'))
            return MZ_FALSE;
        pArchive_name++;
    }
    return MZ_TRUE;
}

static mz_uint mz_zip_writer_compute_padding_needed_for_file_alignment(mz_zip_archive *pZip) {
    mz_uint32 n;

    if (!pZip->m_file_offset_alignment)
        return 0;
    n = (mz_uint32)(pZip->m_archive_size & (pZip->m_file_offset_alignment - 1));
    return (pZip->m_file_offset_alignment - n) & (pZip->m_file_offset_alignment - 1);
}

static mz_bool mz_zip_writer_write_zeros(mz_zip_archive *pZip, mz_uint64 cur_file_ofs, mz_uint32 n) {
    char buf[4096];

    memset(buf, 0, MZ_MIN(sizeof(buf), n));
    while (n) {
        mz_uint32 s = MZ_MIN(sizeof(buf), n);
        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_file_ofs, buf, s) != s)
            return MZ_FALSE;
        cur_file_ofs += s;
        n            -= s;
    }
    return MZ_TRUE;
}

mz_bool mz_zip_writer_add_mem_ex(mz_zip_archive *pZip, const char *pArchive_name, const void *pBuf, size_t buf_size, const void *pComment, mz_uint16 comment_size, mz_uint level_and_flags, mz_uint64 uncomp_size, mz_uint32 uncomp_crc32) {
    mz_uint16 method = 0, dos_time = 0, dos_date = 0;
    mz_uint level, ext_attributes = 0, num_alignment_padding_bytes;
    mz_uint64 local_dir_header_ofs = pZip->m_archive_size, cur_archive_file_ofs = pZip->m_archive_size, comp_size = 0;
    size_t archive_name_size;
    mz_uint8 local_dir_header[MZ_ZIP_LOCAL_DIR_HEADER_SIZE];
    tdefl_compressor *pComp = NULL;
    mz_bool store_data_uncompressed;
    mz_zip_internal_state *pState;

    if ((int)level_and_flags < 0)
        level_and_flags = MZ_DEFAULT_LEVEL;
    level = level_and_flags & 0xF;
    store_data_uncompressed = ((!level) || (level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA));

    if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING) || ((buf_size) && (!pBuf)) || (!pArchive_name) || ((comment_size) && (!pComment)) || (pZip->m_total_files == 0xFFFF) || (level > MZ_UBER_COMPRESSION))
        return MZ_FALSE;

    pState = pZip->m_pState;

    if ((!(level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA)) && (uncomp_size))
        return MZ_FALSE;
    // No zip64 support yet
    if ((buf_size > 0xFFFFFFFF) || (uncomp_size > 0xFFFFFFFF))
        return MZ_FALSE;
    if (!mz_zip_writer_validate_archive_name(pArchive_name))
        return MZ_FALSE;

   #ifndef MINIZ_NO_TIME
    {
        time_t cur_time;
        time(&cur_time);
        mz_zip_time_to_dos_time(cur_time, &dos_time, &dos_date);
    }
   #endif // #ifndef MINIZ_NO_TIME

    archive_name_size = strlen(pArchive_name);
    if (archive_name_size > 0xFFFF)
        return MZ_FALSE;

    num_alignment_padding_bytes = mz_zip_writer_compute_padding_needed_for_file_alignment(pZip);

    // no zip64 support yet
    if ((pZip->m_total_files == 0xFFFF) || ((pZip->m_archive_size + num_alignment_padding_bytes + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + comment_size + archive_name_size) > 0xFFFFFFFF))
        return MZ_FALSE;

    if ((archive_name_size) && (pArchive_name[archive_name_size - 1] == '/')) {
        // Set DOS Subdirectory attribute bit.
        ext_attributes |= 0x10;
        // Subdirectories cannot contain data.
        if ((buf_size) || (uncomp_size))
            return MZ_FALSE;
    }

    // Try to do any allocations before writing to the archive, so if an allocation fails the file remains unmodified. (A good idea if we're doing an in-place modification.)
    if ((!mz_zip_array_ensure_room(pZip, &pState->m_central_dir, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + archive_name_size + comment_size)) || (!mz_zip_array_ensure_room(pZip, &pState->m_central_dir_offsets, 1)))
        return MZ_FALSE;

    if ((!store_data_uncompressed) && (buf_size)) {
        if (NULL == (pComp = (tdefl_compressor *)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(tdefl_compressor))))
            return MZ_FALSE;
    }

    if (!mz_zip_writer_write_zeros(pZip, cur_archive_file_ofs, num_alignment_padding_bytes + sizeof(local_dir_header))) {
        pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
        return MZ_FALSE;
    }
    local_dir_header_ofs += num_alignment_padding_bytes;
    if (pZip->m_file_offset_alignment) {
        MZ_ASSERT((local_dir_header_ofs & (pZip->m_file_offset_alignment - 1)) == 0);
    }
    cur_archive_file_ofs += num_alignment_padding_bytes + sizeof(local_dir_header);

    MZ_CLEAR_OBJ(local_dir_header);
    if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pArchive_name, archive_name_size) != archive_name_size) {
        pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
        return MZ_FALSE;
    }
    cur_archive_file_ofs += archive_name_size;

    if (!(level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA)) {
        uncomp_crc32 = (mz_uint32)mz_crc32(MZ_CRC32_INIT, (const mz_uint8 *)pBuf, buf_size);
        uncomp_size  = buf_size;
        if (uncomp_size <= 3) {
            level = 0;
            store_data_uncompressed = MZ_TRUE;
        }
    }

    if (store_data_uncompressed) {
        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pBuf, buf_size) != buf_size) {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
            return MZ_FALSE;
        }

        cur_archive_file_ofs += buf_size;
        comp_size             = buf_size;

        if (level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA)
            method = MZ_DEFLATED;
    } else if (buf_size) {
        mz_zip_writer_add_state state;

        state.m_pZip = pZip;
        state.m_cur_archive_file_ofs = cur_archive_file_ofs;
        state.m_comp_size            = 0;

        if ((tdefl_init(pComp, mz_zip_writer_add_put_buf_callback, &state, tdefl_create_comp_flags_from_zip_params(level, -15, MZ_DEFAULT_STRATEGY)) != TDEFL_STATUS_OKAY) ||
            (tdefl_compress_buffer(pComp, pBuf, buf_size, TDEFL_FINISH) != TDEFL_STATUS_DONE)) {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
            return MZ_FALSE;
        }

        comp_size            = state.m_comp_size;
        cur_archive_file_ofs = state.m_cur_archive_file_ofs;

        method = MZ_DEFLATED;
    }

    pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
    pComp = NULL;

    // no zip64 support yet
    if ((comp_size > 0xFFFFFFFF) || (cur_archive_file_ofs > 0xFFFFFFFF))
        return MZ_FALSE;

    if (!mz_zip_writer_create_local_dir_header(pZip, local_dir_header, (mz_uint16)archive_name_size, 0, uncomp_size, comp_size, uncomp_crc32, method, 0, dos_time, dos_date))
        return MZ_FALSE;

    if (pZip->m_pWrite(pZip->m_pIO_opaque, local_dir_header_ofs, local_dir_header, sizeof(local_dir_header)) != sizeof(local_dir_header))
        return MZ_FALSE;

    if (!mz_zip_writer_add_to_central_dir(pZip, pArchive_name, (mz_uint16)archive_name_size, NULL, 0, pComment, comment_size, uncomp_size, comp_size, uncomp_crc32, method, 0, dos_time, dos_date, local_dir_header_ofs, ext_attributes))
        return MZ_FALSE;

    pZip->m_total_files++;
    pZip->m_archive_size = cur_archive_file_ofs;

    return MZ_TRUE;
}

   #ifndef MINIZ_NO_STDIO
mz_bool mz_zip_writer_add_file(mz_zip_archive *pZip, const char *pArchive_name, const char *pSrc_filename, const void *pComment, mz_uint16 comment_size, mz_uint level_and_flags) {
    mz_uint uncomp_crc32           = MZ_CRC32_INIT, level, num_alignment_padding_bytes;
    mz_uint16 method               = 0, dos_time = 0, dos_date = 0, ext_attributes = 0;
    mz_uint64 local_dir_header_ofs = pZip->m_archive_size, cur_archive_file_ofs = pZip->m_archive_size, uncomp_size = 0, comp_size = 0;
    size_t archive_name_size;
    mz_uint8 local_dir_header[MZ_ZIP_LOCAL_DIR_HEADER_SIZE];
    MZ_FILE *pSrc_file = NULL;

    if ((int)level_and_flags < 0)
        level_and_flags = MZ_DEFAULT_LEVEL;
    level = level_and_flags & 0xF;

    if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING) || (!pArchive_name) || ((comment_size) && (!pComment)) || (level > MZ_UBER_COMPRESSION))
        return MZ_FALSE;
    if (level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA)
        return MZ_FALSE;
    if (!mz_zip_writer_validate_archive_name(pArchive_name))
        return MZ_FALSE;

    archive_name_size = strlen(pArchive_name);
    if (archive_name_size > 0xFFFF)
        return MZ_FALSE;

    num_alignment_padding_bytes = mz_zip_writer_compute_padding_needed_for_file_alignment(pZip);

    // no zip64 support yet
    if ((pZip->m_total_files == 0xFFFF) || ((pZip->m_archive_size + num_alignment_padding_bytes + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + comment_size + archive_name_size) > 0xFFFFFFFF))
        return MZ_FALSE;

    if (!mz_zip_get_file_modified_time(pSrc_filename, &dos_time, &dos_date))
        return MZ_FALSE;

    pSrc_file = MZ_FOPEN(pSrc_filename, "rb");
    if (!pSrc_file)
        return MZ_FALSE;
    MZ_FSEEK64(pSrc_file, 0, SEEK_END);
    uncomp_size = MZ_FTELL64(pSrc_file);
    MZ_FSEEK64(pSrc_file, 0, SEEK_SET);

    if (uncomp_size > 0xFFFFFFFF) {
        // No zip64 support yet
        MZ_FCLOSE(pSrc_file);
        return MZ_FALSE;
    }
    if (uncomp_size <= 3)
        level = 0;

    if (!mz_zip_writer_write_zeros(pZip, cur_archive_file_ofs, num_alignment_padding_bytes + sizeof(local_dir_header))) {
        MZ_FCLOSE(pSrc_file);
        return MZ_FALSE;
    }
    local_dir_header_ofs += num_alignment_padding_bytes;
    if (pZip->m_file_offset_alignment) {
        MZ_ASSERT((local_dir_header_ofs & (pZip->m_file_offset_alignment - 1)) == 0);
    }
    cur_archive_file_ofs += num_alignment_padding_bytes + sizeof(local_dir_header);

    MZ_CLEAR_OBJ(local_dir_header);
    if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pArchive_name, archive_name_size) != archive_name_size) {
        MZ_FCLOSE(pSrc_file);
        return MZ_FALSE;
    }
    cur_archive_file_ofs += archive_name_size;

    if (uncomp_size) {
        mz_uint64 uncomp_remaining = uncomp_size;
        void *pRead_buf            = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, MZ_ZIP_MAX_IO_BUF_SIZE);
        if (!pRead_buf) {
            MZ_FCLOSE(pSrc_file);
            return MZ_FALSE;
        }

        if (!level) {
            while (uncomp_remaining) {
                mz_uint n = (mz_uint)MZ_MIN(MZ_ZIP_MAX_IO_BUF_SIZE, uncomp_remaining);
                if ((MZ_FREAD(pRead_buf, 1, n, pSrc_file) != n) || (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pRead_buf, n) != n)) {
                    pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                    MZ_FCLOSE(pSrc_file);
                    return MZ_FALSE;
                }
                uncomp_crc32          = (mz_uint32)mz_crc32(uncomp_crc32, (const mz_uint8 *)pRead_buf, n);
                uncomp_remaining     -= n;
                cur_archive_file_ofs += n;
            }
            comp_size = uncomp_size;
        } else {
            mz_bool result = MZ_FALSE;
            mz_zip_writer_add_state state;
            tdefl_compressor *pComp = (tdefl_compressor *)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(tdefl_compressor));
            if (!pComp) {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                MZ_FCLOSE(pSrc_file);
                return MZ_FALSE;
            }

            state.m_pZip = pZip;
            state.m_cur_archive_file_ofs = cur_archive_file_ofs;
            state.m_comp_size            = 0;

            if (tdefl_init(pComp, mz_zip_writer_add_put_buf_callback, &state, tdefl_create_comp_flags_from_zip_params(level, -15, MZ_DEFAULT_STRATEGY)) != TDEFL_STATUS_OKAY) {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
                pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                MZ_FCLOSE(pSrc_file);
                return MZ_FALSE;
            }

            for ( ; ; ) {
                size_t in_buf_size = (mz_uint32)MZ_MIN(uncomp_remaining, MZ_ZIP_MAX_IO_BUF_SIZE);
                tdefl_status status;

                if (MZ_FREAD(pRead_buf, 1, in_buf_size, pSrc_file) != in_buf_size)
                    break;

                uncomp_crc32      = (mz_uint32)mz_crc32(uncomp_crc32, (const mz_uint8 *)pRead_buf, in_buf_size);
                uncomp_remaining -= in_buf_size;

                status = tdefl_compress_buffer(pComp, pRead_buf, in_buf_size, uncomp_remaining ? TDEFL_NO_FLUSH : TDEFL_FINISH);
                if (status == TDEFL_STATUS_DONE) {
                    result = MZ_TRUE;
                    break;
                } else if (status != TDEFL_STATUS_OKAY)
                    break;
            }

            pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);

            if (!result) {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                MZ_FCLOSE(pSrc_file);
                return MZ_FALSE;
            }

            comp_size            = state.m_comp_size;
            cur_archive_file_ofs = state.m_cur_archive_file_ofs;

            method = MZ_DEFLATED;
        }

        pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
    }

    MZ_FCLOSE(pSrc_file);
    pSrc_file = NULL;

    // no zip64 support yet
    if ((comp_size > 0xFFFFFFFF) || (cur_archive_file_ofs > 0xFFFFFFFF))
        return MZ_FALSE;

    if (!mz_zip_writer_create_local_dir_header(pZip, local_dir_header, (mz_uint16)archive_name_size, 0, uncomp_size, comp_size, uncomp_crc32, method, 0, dos_time, dos_date))
        return MZ_FALSE;

    if (pZip->m_pWrite(pZip->m_pIO_opaque, local_dir_header_ofs, local_dir_header, sizeof(local_dir_header)) != sizeof(local_dir_header))
        return MZ_FALSE;

    if (!mz_zip_writer_add_to_central_dir(pZip, pArchive_name, (mz_uint16)archive_name_size, NULL, 0, pComment, comment_size, uncomp_size, comp_size, uncomp_crc32, method, 0, dos_time, dos_date, local_dir_header_ofs, ext_attributes))
        return MZ_FALSE;

    pZip->m_total_files++;
    pZip->m_archive_size = cur_archive_file_ofs;

    return MZ_TRUE;
}
   #endif // #ifndef MINIZ_NO_STDIO

mz_bool mz_zip_writer_add_from_zip_reader(mz_zip_archive *pZip, mz_zip_archive *pSource_zip, mz_uint file_index) {
    mz_uint n, bit_flags, num_alignment_padding_bytes;
    mz_uint64 comp_bytes_remaining, local_dir_header_ofs;
    mz_uint64 cur_src_file_ofs, cur_dst_file_ofs;
    mz_uint32 local_header_u32[(MZ_ZIP_LOCAL_DIR_HEADER_SIZE + sizeof(mz_uint32) - 1) / sizeof(mz_uint32)];
    mz_uint8 *pLocal_header = (mz_uint8 *)local_header_u32;
    mz_uint8 central_header[MZ_ZIP_CENTRAL_DIR_HEADER_SIZE];
    size_t orig_central_dir_size;
    mz_zip_internal_state *pState;
    void *pBuf;
    const mz_uint8 *pSrc_central_header;

    if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING))
        return MZ_FALSE;
    if (NULL == (pSrc_central_header = mz_zip_reader_get_cdh(pSource_zip, file_index)))
        return MZ_FALSE;
    pState = pZip->m_pState;

    num_alignment_padding_bytes = mz_zip_writer_compute_padding_needed_for_file_alignment(pZip);

    // no zip64 support yet
    if ((pZip->m_total_files == 0xFFFF) || ((pZip->m_archive_size + num_alignment_padding_bytes + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE) > 0xFFFFFFFF))
        return MZ_FALSE;

    cur_src_file_ofs = MZ_READ_LE32(pSrc_central_header + MZ_ZIP_CDH_LOCAL_HEADER_OFS);
    cur_dst_file_ofs = pZip->m_archive_size;

    if (pSource_zip->m_pRead(pSource_zip->m_pIO_opaque, cur_src_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
        return MZ_FALSE;
    if (MZ_READ_LE32(pLocal_header) != MZ_ZIP_LOCAL_DIR_HEADER_SIG)
        return MZ_FALSE;
    cur_src_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE;

    if (!mz_zip_writer_write_zeros(pZip, cur_dst_file_ofs, num_alignment_padding_bytes))
        return MZ_FALSE;
    cur_dst_file_ofs    += num_alignment_padding_bytes;
    local_dir_header_ofs = cur_dst_file_ofs;
    if (pZip->m_file_offset_alignment) {
        MZ_ASSERT((local_dir_header_ofs & (pZip->m_file_offset_alignment - 1)) == 0);
    }

    if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_dst_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
        return MZ_FALSE;
    cur_dst_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE;

    n = MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_FILENAME_LEN_OFS) + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_EXTRA_LEN_OFS);
    comp_bytes_remaining = n + MZ_READ_LE32(pSrc_central_header + MZ_ZIP_CDH_COMPRESSED_SIZE_OFS);

    if (NULL == (pBuf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, (size_t)MZ_MAX(sizeof(mz_uint32) * 4, MZ_MIN(MZ_ZIP_MAX_IO_BUF_SIZE, comp_bytes_remaining)))))
        return MZ_FALSE;

    while (comp_bytes_remaining) {
        n = (mz_uint)MZ_MIN(MZ_ZIP_MAX_IO_BUF_SIZE, comp_bytes_remaining);
        if (pSource_zip->m_pRead(pSource_zip->m_pIO_opaque, cur_src_file_ofs, pBuf, n) != n) {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
            return MZ_FALSE;
        }
        cur_src_file_ofs += n;

        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_dst_file_ofs, pBuf, n) != n) {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
            return MZ_FALSE;
        }
        cur_dst_file_ofs += n;

        comp_bytes_remaining -= n;
    }

    bit_flags = MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_BIT_FLAG_OFS);
    if (bit_flags & 8) {
        // Copy data descriptor
        if (pSource_zip->m_pRead(pSource_zip->m_pIO_opaque, cur_src_file_ofs, pBuf, sizeof(mz_uint32) * 4) != sizeof(mz_uint32) * 4) {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
            return MZ_FALSE;
        }

        n = sizeof(mz_uint32) * ((MZ_READ_LE32(pBuf) == 0x08074b50) ? 4 : 3);
        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_dst_file_ofs, pBuf, n) != n) {
            pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);
            return MZ_FALSE;
        }

        cur_src_file_ofs += n;
        cur_dst_file_ofs += n;
    }
    pZip->m_pFree(pZip->m_pAlloc_opaque, pBuf);

    // no zip64 support yet
    if (cur_dst_file_ofs > 0xFFFFFFFF)
        return MZ_FALSE;

    orig_central_dir_size = pState->m_central_dir.m_size;

    memcpy(central_header, pSrc_central_header, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE);
    MZ_WRITE_LE32(central_header + MZ_ZIP_CDH_LOCAL_HEADER_OFS, local_dir_header_ofs);
    if (!mz_zip_array_push_back(pZip, &pState->m_central_dir, central_header, MZ_ZIP_CENTRAL_DIR_HEADER_SIZE))
        return MZ_FALSE;

    n = MZ_READ_LE16(pSrc_central_header + MZ_ZIP_CDH_FILENAME_LEN_OFS) + MZ_READ_LE16(pSrc_central_header + MZ_ZIP_CDH_EXTRA_LEN_OFS) + MZ_READ_LE16(pSrc_central_header + MZ_ZIP_CDH_COMMENT_LEN_OFS);
    if (!mz_zip_array_push_back(pZip, &pState->m_central_dir, pSrc_central_header + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE, n)) {
        mz_zip_array_resize(pZip, &pState->m_central_dir, orig_central_dir_size, MZ_FALSE);
        return MZ_FALSE;
    }

    if (pState->m_central_dir.m_size > 0xFFFFFFFF)
        return MZ_FALSE;
    n = (mz_uint32)orig_central_dir_size;
    if (!mz_zip_array_push_back(pZip, &pState->m_central_dir_offsets, &n, 1)) {
        mz_zip_array_resize(pZip, &pState->m_central_dir, orig_central_dir_size, MZ_FALSE);
        return MZ_FALSE;
    }

    pZip->m_total_files++;
    pZip->m_archive_size = cur_dst_file_ofs;

    return MZ_TRUE;
}

mz_bool mz_zip_writer_finalize_archive(mz_zip_archive *pZip) {
    mz_zip_internal_state *pState;
    mz_uint64 central_dir_ofs, central_dir_size;
    mz_uint8 hdr[MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE];

    if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING))
        return MZ_FALSE;

    pState = pZip->m_pState;

    // no zip64 support yet
    if ((pZip->m_total_files > 0xFFFF) || ((pZip->m_archive_size + pState->m_central_dir.m_size + MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE) > 0xFFFFFFFF))
        return MZ_FALSE;

    central_dir_ofs  = 0;
    central_dir_size = 0;
    if (pZip->m_total_files) {
        // Write central directory
        central_dir_ofs  = pZip->m_archive_size;
        central_dir_size = pState->m_central_dir.m_size;
        pZip->m_central_directory_file_ofs = central_dir_ofs;
        if (pZip->m_pWrite(pZip->m_pIO_opaque, central_dir_ofs, pState->m_central_dir.m_p, (size_t)central_dir_size) != central_dir_size)
            return MZ_FALSE;
        pZip->m_archive_size += central_dir_size;
    }

    // Write end of central directory record
    MZ_CLEAR_OBJ(hdr);
    MZ_WRITE_LE32(hdr + MZ_ZIP_ECDH_SIG_OFS, MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIG);
    MZ_WRITE_LE16(hdr + MZ_ZIP_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS, pZip->m_total_files);
    MZ_WRITE_LE16(hdr + MZ_ZIP_ECDH_CDIR_TOTAL_ENTRIES_OFS, pZip->m_total_files);
    MZ_WRITE_LE32(hdr + MZ_ZIP_ECDH_CDIR_SIZE_OFS, central_dir_size);
    MZ_WRITE_LE32(hdr + MZ_ZIP_ECDH_CDIR_OFS_OFS, central_dir_ofs);

    if (pZip->m_pWrite(pZip->m_pIO_opaque, pZip->m_archive_size, hdr, sizeof(hdr)) != sizeof(hdr))
        return MZ_FALSE;
   #ifndef MINIZ_NO_STDIO
    if ((pState->m_pFile) && (MZ_FFLUSH(pState->m_pFile) == EOF))
        return MZ_FALSE;
   #endif // #ifndef MINIZ_NO_STDIO

    pZip->m_archive_size += sizeof(hdr);

    pZip->m_zip_mode = MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED;
    return MZ_TRUE;
}

mz_bool mz_zip_writer_finalize_heap_archive(mz_zip_archive *pZip, void **pBuf, size_t *pSize) {
    if ((!pZip) || (!pZip->m_pState) || (!pBuf) || (!pSize))
        return MZ_FALSE;
    if (pZip->m_pWrite != mz_zip_heap_write_func)
        return MZ_FALSE;
    if (!mz_zip_writer_finalize_archive(pZip))
        return MZ_FALSE;

    *pBuf  = pZip->m_pState->m_pMem;
    *pSize = pZip->m_pState->m_mem_size;
    pZip->m_pState->m_pMem     = NULL;
    pZip->m_pState->m_mem_size = pZip->m_pState->m_mem_capacity = 0;
    return MZ_TRUE;
}

mz_bool mz_zip_writer_end(mz_zip_archive *pZip) {
    mz_zip_internal_state *pState;
    mz_bool status = MZ_TRUE;

    if ((!pZip) || (!pZip->m_pState) || (!pZip->m_pAlloc) || (!pZip->m_pFree) || ((pZip->m_zip_mode != MZ_ZIP_MODE_WRITING) && (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED)))
        return MZ_FALSE;

    pState         = pZip->m_pState;
    pZip->m_pState = NULL;
    mz_zip_array_clear(pZip, &pState->m_central_dir);
    mz_zip_array_clear(pZip, &pState->m_central_dir_offsets);
    mz_zip_array_clear(pZip, &pState->m_sorted_central_dir_offsets);

   #ifndef MINIZ_NO_STDIO
    if (pState->m_pFile) {
        MZ_FCLOSE(pState->m_pFile);
        pState->m_pFile = NULL;
    }
   #endif // #ifndef MINIZ_NO_STDIO

    if ((pZip->m_pWrite == mz_zip_heap_write_func) && (pState->m_pMem)) {
        pZip->m_pFree(pZip->m_pAlloc_opaque, pState->m_pMem);
        pState->m_pMem = NULL;
    }

    pZip->m_pFree(pZip->m_pAlloc_opaque, pState);
    pZip->m_zip_mode = MZ_ZIP_MODE_INVALID;
    return status;
}

   #ifndef MINIZ_NO_STDIO
mz_bool mz_zip_add_mem_to_archive_file_in_place(const char *pZip_filename, const char *pArchive_name, const void *pBuf, size_t buf_size, const void *pComment, mz_uint16 comment_size, mz_uint level_and_flags) {
    mz_bool status, created_new_archive = MZ_FALSE;
    mz_zip_archive zip_archive;
    struct MZ_FILE_STAT_STRUCT file_stat;

    MZ_CLEAR_OBJ(zip_archive);
    if ((int)level_and_flags < 0)
        level_and_flags = MZ_DEFAULT_LEVEL;
    if ((!pZip_filename) || (!pArchive_name) || ((buf_size) && (!pBuf)) || ((comment_size) && (!pComment)) || ((level_and_flags & 0xF) > MZ_UBER_COMPRESSION))
        return MZ_FALSE;
    if (!mz_zip_writer_validate_archive_name(pArchive_name))
        return MZ_FALSE;
    if (MZ_FILE_STAT(pZip_filename, &file_stat) != 0) {
        // Create a new archive.
        if (!mz_zip_writer_init_file(&zip_archive, pZip_filename, 0))
            return MZ_FALSE;
        created_new_archive = MZ_TRUE;
    } else {
        // Append to an existing archive.
        if (!mz_zip_reader_init_file(&zip_archive, pZip_filename, level_and_flags | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
            return MZ_FALSE;
        if (!mz_zip_writer_init_from_reader(&zip_archive, pZip_filename)) {
            mz_zip_reader_end(&zip_archive);
            return MZ_FALSE;
        }
    }
    status = mz_zip_writer_add_mem_ex(&zip_archive, pArchive_name, pBuf, buf_size, pComment, comment_size, level_and_flags, 0, 0);
    // Always finalize, even if adding failed for some reason, so we have a valid central directory. (This may not always succeed, but we can try.)
    if (!mz_zip_writer_finalize_archive(&zip_archive))
        status = MZ_FALSE;
    if (!mz_zip_writer_end(&zip_archive))
        status = MZ_FALSE;
    if ((!status) && (created_new_archive)) {
        // It's a new archive and something went wrong, so just delete it.
        int ignoredStatus = MZ_DELETE_FILE(pZip_filename);
        (void)ignoredStatus;
    }
    return status;
}

void *mz_zip_extract_archive_file_to_heap(const char *pZip_filename, const char *pArchive_name, size_t *pSize, mz_uint flags) {
    int file_index;
    mz_zip_archive zip_archive;
    void *p = NULL;

    if (pSize)
        *pSize = 0;

    if ((!pZip_filename) || (!pArchive_name))
        return NULL;

    MZ_CLEAR_OBJ(zip_archive);
    if (!mz_zip_reader_init_file(&zip_archive, pZip_filename, flags | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
        return NULL;

    if ((file_index = mz_zip_reader_locate_file(&zip_archive, pArchive_name, NULL, flags)) >= 0)
        p = mz_zip_reader_extract_to_heap(&zip_archive, file_index, pSize, flags);

    mz_zip_reader_end(&zip_archive);
    return p;
}
   #endif // #ifndef MINIZ_NO_STDIO
  #endif  // #ifndef MINIZ_NO_ARCHIVE_WRITING_APIS
 #endif   // #ifndef MINIZ_NO_ARCHIVE_APIS

 #ifdef __cplusplus
}
 #endif
#endif // MINIZ_HEADER_FILE_ONLY

/*
   This is free and unencumbered software released into the public domain.

   Anyone is free to copy, modify, publish, use, compile, sell, or
   distribute this software, either in source code form or as a compiled
   binary, for any purpose, commercial or non-commercial, and by any
   means.

   In jurisdictions that recognize copyright laws, the author or authors
   of this software dedicate any and all copyright interest in the
   software to the public domain. We make this dedication for the benefit
   of the public at large and to the detriment of our heirs and
   successors. We intend this dedication to be an overt act of
   relinquishment in perpetuity of all present and future rights to this
   software under copyright law.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.

   For more information, please refer to <http://unlicense.org/>
 */

#ifndef _HAD_ZIPCONF_H
 #define _HAD_ZIPCONF_H

/*
   zipconf.h -- platform specific include file

   This file was generated automatically by ./make_zipconf.sh
   based on ../config.h.
 */

 #define LIBZIP_VERSION          "0.10.1"
 #define LIBZIP_VERSION_MAJOR    0
 #define LIBZIP_VERSION_MINOR    10
 #define LIBZIP_VERSION_MICRO    0

 #include <inttypes.h>

typedef int8_t     zip_int8_t;
 #define ZIP_INT8_MIN      INT8_MIN
 #define ZIP_INT8_MAX      INT8_MAX

typedef uint8_t    zip_uint8_t;
 #define ZIP_UINT8_MAX     UINT8_MAX

typedef int16_t    zip_int16_t;
 #define ZIP_INT16_MIN     INT16_MIN
 #define ZIP_INT16_MAX     INT16_MAX

typedef uint16_t   zip_uint16_t;
 #define ZIP_UINT16_MAX    UINT16_MAX

typedef int32_t    zip_int32_t;
 #define ZIP_INT32_MIN     INT32_MIN
 #define ZIP_INT32_MAX     INT32_MAX

typedef uint32_t   zip_uint32_t;
 #define ZIP_UINT32_MAX    UINT32_MAX

typedef int64_t    zip_int64_t;
 #define ZIP_INT64_MIN     INT64_MIN
 #define ZIP_INT64_MAX     INT64_MAX

typedef uint64_t   zip_uint64_t;
 #define ZIP_UINT64_MAX    UINT64_MAX
#endif /* zipconf.h */

#ifndef _HAD_ZIP_H
 #define _HAD_ZIP_H

/*
   zip.h -- exported declarations.
   Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
   3. The names of the authors may not be used to endorse or promote
   products derived from this software without specific prior
   written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


 #define ZIP_EXTERN

 #ifdef __cplusplus
extern "C" {
  #if 0
} /* fix autoindent */
  #endif
 #endif

 #include <sys/types.h>
 #include <stdio.h>
 #include <time.h>
 #ifndef _WIN32
  #include <sys/types.h>
  #include <unistd.h>
 #endif

/* flags for zip_open */

 #define ZIP_CREATE       1
 #define ZIP_EXCL         2
 #define ZIP_CHECKCONS    4
 #define ZIP_TRUNCATE     8
 #define ZIP_RDONLY       16


/* flags for zip_name_locate, zip_fopen, zip_stat, ... */

 #define ZIP_FL_NOCASE        1u    /* ignore case on name lookup */
 #define ZIP_FL_NODIR         2u    /* ignore directory component */
 #define ZIP_FL_COMPRESSED    4u    /* read compressed data */
 #define ZIP_FL_UNCHANGED     8u    /* use original data, ignoring changes */
 #define ZIP_FL_RECOMPRESS    16u   /* force recompression of data */
 #define ZIP_FL_ENCRYPTED     32u   /* read encrypted data (implies ZIP_FL_COMPRESSED) */
 #define ZIP_FL_ENC_GUESS     0u    /* guess string encoding (is default) */
 #define ZIP_FL_ENC_RAW       64u   /* get unmodified string */
 #define ZIP_FL_ENC_STRICT    128u  /* follow specification strictly */
 #define ZIP_FL_LOCAL         256u  /* in local header */
 #define ZIP_FL_CENTRAL       512u  /* in central directory */
/*                           1024u    reserved for internal use */
 #define ZIP_FL_ENC_UTF_8     2048u /* string is UTF-8 encoded */
 #define ZIP_FL_ENC_CP437     4096u /* string is CP437 encoded */
 #define ZIP_FL_OVERWRITE     8192u /* zip_file_add: if file with name exists, overwrite (replace) it */

/* archive global flags flags */

 #define ZIP_AFL_RDONLY    2u      /* read only -- cannot be cleared */


/* create a new extra field */

 #define ZIP_EXTRA_FIELD_ALL    ZIP_UINT16_MAX
 #define ZIP_EXTRA_FIELD_NEW    ZIP_UINT16_MAX


/* libzip error codes */

 #define ZIP_ER_OK             0  /* N No error */
 #define ZIP_ER_MULTIDISK      1  /* N Multi-disk zip archives not supported */
 #define ZIP_ER_RENAME         2  /* S Renaming temporary file failed */
 #define ZIP_ER_CLOSE          3  /* S Closing zip archive failed */
 #define ZIP_ER_SEEK           4  /* S Seek error */
 #define ZIP_ER_READ           5  /* S Read error */
 #define ZIP_ER_WRITE          6  /* S Write error */
 #define ZIP_ER_CRC            7  /* N CRC error */
 #define ZIP_ER_ZIPCLOSED      8  /* N Containing zip archive was closed */
 #define ZIP_ER_NOENT          9  /* N No such file */
 #define ZIP_ER_EXISTS         10 /* N File already exists */
 #define ZIP_ER_OPEN           11 /* S Can't open file */
 #define ZIP_ER_TMPOPEN        12 /* S Failure to create temporary file */
 #define ZIP_ER_ZLIB           13 /* Z Zlib error */
 #define ZIP_ER_MEMORY         14 /* N Malloc failure */
 #define ZIP_ER_CHANGED        15 /* N Entry has been changed */
 #define ZIP_ER_COMPNOTSUPP    16 /* N Compression method not supported */
 #define ZIP_ER_EOF            17 /* N Premature end of file */
 #define ZIP_ER_INVAL          18 /* N Invalid argument */
 #define ZIP_ER_NOZIP          19 /* N Not a zip archive */
 #define ZIP_ER_INTERNAL       20 /* N Internal error */
 #define ZIP_ER_INCONS         21 /* N Zip archive inconsistent */
 #define ZIP_ER_REMOVE         22 /* S Can't remove file */
 #define ZIP_ER_DELETED        23 /* N Entry has been deleted */
 #define ZIP_ER_ENCRNOTSUPP    24 /* N Encryption method not supported */
 #define ZIP_ER_RDONLY         25 /* N Read-only archive */
 #define ZIP_ER_NOPASSWD       26 /* N No password provided */
 #define ZIP_ER_WRONGPASSWD    27 /* N Wrong password provided */
 #define ZIP_ER_OPNOTSUPP      28 /* N Operation not supported */
 #define ZIP_ER_INUSE          29 /* N Resource still in use */
 #define ZIP_ER_TELL           30 /* S Tell error */

/* type of system error value */

 #define ZIP_ET_NONE    0        /* sys_err unused */
 #define ZIP_ET_SYS     1        /* sys_err is errno */
 #define ZIP_ET_ZLIB    2        /* sys_err is zlib error code */

/* compression methods */

 #define ZIP_CM_DEFAULT           -1 /* better of deflate or store */
 #define ZIP_CM_STORE             0  /* stored (uncompressed) */
 #define ZIP_CM_SHRINK            1  /* shrunk */
 #define ZIP_CM_REDUCE_1          2  /* reduced with factor 1 */
 #define ZIP_CM_REDUCE_2          3  /* reduced with factor 2 */
 #define ZIP_CM_REDUCE_3          4  /* reduced with factor 3 */
 #define ZIP_CM_REDUCE_4          5  /* reduced with factor 4 */
 #define ZIP_CM_IMPLODE           6  /* imploded */
/* 7 - Reserved for Tokenizing compression algorithm */
 #define ZIP_CM_DEFLATE           8  /* deflated */
 #define ZIP_CM_DEFLATE64         9  /* deflate64 */
 #define ZIP_CM_PKWARE_IMPLODE    10 /* PKWARE imploding */
/* 11 - Reserved by PKWARE */
 #define ZIP_CM_BZIP2             12 /* compressed using BZIP2 algorithm */
/* 13 - Reserved by PKWARE */
 #define ZIP_CM_LZMA              14 /* LZMA (EFS) */
/* 15-17 - Reserved by PKWARE */
 #define ZIP_CM_TERSE             18 /* compressed using IBM TERSE (new) */
 #define ZIP_CM_LZ77              19 /* IBM LZ77 z Architecture (PFS) */
 #define ZIP_CM_WAVPACK           97 /* WavPack compressed data */
 #define ZIP_CM_PPMD              98 /* PPMd version I, Rev 1 */

/* encryption methods */

 #define ZIP_EM_NONE                0      /* not encrypted */
 #define ZIP_EM_TRAD_PKWARE         1      /* traditional PKWARE encryption */
 #if 0                                     /* Strong Encryption Header not parsed yet */
  #define ZIP_EM_DES                0x6601 /* strong encryption: DES */
  #define ZIP_EM_RC2_OLD            0x6602 /* strong encryption: RC2, version < 5.2 */
  #define ZIP_EM_3DES_168           0x6603
  #define ZIP_EM_3DES_112           0x6609
  #define ZIP_EM_AES_128            0x660e
  #define ZIP_EM_AES_192            0x660f
  #define ZIP_EM_AES_256            0x6610
  #define ZIP_EM_RC2                0x6702 /* strong encryption: RC2, version >= 5.2 */
  #define ZIP_EM_RC4                0x6801
 #endif
 #define ZIP_EM_UNKNOWN             0xffff /* unknown algorithm */

 #define ZIP_OPSYS_DOS              0x00u
 #define ZIP_OPSYS_AMIGA            0x01u
 #define ZIP_OPSYS_OPENVMS          0x02u
 #define ZIP_OPSYS_UNIX             0x03u
 #define ZIP_OPSYS_VM_CMS           0x04u
 #define ZIP_OPSYS_ATARI_ST         0x05u
 #define ZIP_OPSYS_OS_2             0x06u
 #define ZIP_OPSYS_MACINTOSH        0x07u
 #define ZIP_OPSYS_Z_SYSTEM         0x08u
 #define ZIP_OPSYS_CPM              0x09u
 #define ZIP_OPSYS_WINDOWS_NTFS     0x0au
 #define ZIP_OPSYS_MVS              0x0bu
 #define ZIP_OPSYS_VSE              0x0cu
 #define ZIP_OPSYS_ACORN_RISC       0x0du
 #define ZIP_OPSYS_VFAT             0x0eu
 #define ZIP_OPSYS_ALTERNATE_MVS    0x0fu
 #define ZIP_OPSYS_BEOS             0x10u
 #define ZIP_OPSYS_TANDEM           0x11u
 #define ZIP_OPSYS_OS_400           0x12u
 #define ZIP_OPSYS_OS_X             0x13u

 #define ZIP_OPSYS_DEFAULT          ZIP_OPSYS_UNIX


enum zip_source_cmd {
    ZIP_SOURCE_OPEN,            /* prepare for reading */
    ZIP_SOURCE_READ,            /* read data */
    ZIP_SOURCE_CLOSE,           /* reading is done */
    ZIP_SOURCE_STAT,            /* get meta information */
    ZIP_SOURCE_ERROR,           /* get error information */
    ZIP_SOURCE_FREE,            /* cleanup and free resources */
    ZIP_SOURCE_SEEK,            /* set position for reading */
    ZIP_SOURCE_TELL,            /* get read position */
    ZIP_SOURCE_BEGIN_WRITE,     /* prepare for writing */
    ZIP_SOURCE_COMMIT_WRITE,    /* writing is done */
    ZIP_SOURCE_ROLLBACK_WRITE,  /* discard written changes */
    ZIP_SOURCE_WRITE,           /* write data */
    ZIP_SOURCE_SEEK_WRITE,      /* set position for writing */
    ZIP_SOURCE_TELL_WRITE,      /* get write position */
    ZIP_SOURCE_SUPPORTS,        /* check whether source supports command */
    ZIP_SOURCE_REMOVE           /* remove file */
};
typedef enum zip_source_cmd   zip_source_cmd_t;

 #define ZIP_SOURCE_MAKE_COMMAND_BITMASK(cmd)    (1 << (cmd))

 #define ZIP_SOURCE_SUPPORTS_READABLE                    \
    (ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_OPEN)    \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_READ)  \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_CLOSE) \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_STAT)  \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_ERROR) \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_FREE))

 #define ZIP_SOURCE_SUPPORTS_SEEKABLE                   \
    (ZIP_SOURCE_SUPPORTS_READABLE                       \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_SEEK) \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_TELL) \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_SUPPORTS))

 #define ZIP_SOURCE_SUPPORTS_WRITABLE                             \
    (ZIP_SOURCE_SUPPORTS_SEEKABLE                                 \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_BEGIN_WRITE)    \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_COMMIT_WRITE)   \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_ROLLBACK_WRITE) \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_WRITE)          \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_SEEK_WRITE)     \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_TELL_WRITE)     \
     | ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_REMOVE))

/* for use by sources */
struct zip_source_args_seek {
    zip_int64_t offset;
    int whence;
};

typedef struct zip_source_args_seek   zip_source_args_seek_t;
 #define ZIP_SOURCE_GET_ARGS(type, data, len, error)    ((len) < sizeof(type) ? zip_error_set((error), ZIP_ER_INVAL, 0), (type *)NULL : (type *)(data))


/* error information */
/* use zip_error_*() to access */
struct zip_error {
    int zip_err;        /* libzip error code (ZIP_ER_*) */
    int sys_err;        /* copy of errno (E*) or zlib error code */
    char *str;          /* string representation or NULL */
};

 #define ZIP_STAT_NAME                 0x0001u
 #define ZIP_STAT_INDEX                0x0002u
 #define ZIP_STAT_SIZE                 0x0004u
 #define ZIP_STAT_COMP_SIZE            0x0008u
 #define ZIP_STAT_MTIME                0x0010u
 #define ZIP_STAT_CRC                  0x0020u
 #define ZIP_STAT_COMP_METHOD          0x0040u
 #define ZIP_STAT_ENCRYPTION_METHOD    0x0080u
 #define ZIP_STAT_FLAGS                0x0100u

struct zip_stat {
    zip_uint64_t valid;                 /* which fields have valid values */
    const char *name;                   /* name of the file */
    zip_uint64_t index;                 /* index within archive */
    zip_uint64_t size;                  /* size of file (uncompressed) */
    zip_uint64_t comp_size;             /* size of file (compressed) */
    time_t mtime;                       /* modification time */
    zip_uint32_t crc;                   /* crc of file data */
    zip_uint16_t comp_method;           /* compression method used */
    zip_uint16_t encryption_method;     /* encryption method used */
    zip_uint32_t flags;                 /* reserved for future use */
};

struct zip;
struct zip_file;
struct zip_source;

typedef struct zip          zip_t;
typedef struct zip_error    zip_error_t;
typedef struct zip_file     zip_file_t;
typedef struct zip_source   zip_source_t;
typedef struct zip_stat     zip_stat_t;

typedef zip_uint32_t        zip_flags_t;

typedef zip_int64_t (*zip_source_callback)(void *, void *, zip_uint64_t, zip_source_cmd_t);


 #ifndef ZIP_DISABLE_DEPRECATED
ZIP_EXTERN zip_int64_t zip_add(zip_t *, const char *, zip_source_t *);          /* use zip_file_add */
ZIP_EXTERN zip_int64_t zip_add_dir(zip_t *, const char *);                      /* use zip_dir_add */
ZIP_EXTERN const char *zip_get_file_comment(zip_t *, zip_uint64_t, int *, int); /* use zip_file_get_comment */
ZIP_EXTERN int zip_get_num_files(zip_t *);                                      /* use zip_get_num_entries instead */
ZIP_EXTERN int zip_rename(zip_t *, zip_uint64_t, const char *);                 /* use zip_file_rename */
ZIP_EXTERN int zip_replace(zip_t *, zip_uint64_t, zip_source_t *);              /* use zip_file_replace */
ZIP_EXTERN int zip_set_file_comment(zip_t *, zip_uint64_t, const char *, int);  /* use zip_file_set_comment */
ZIP_EXTERN int zip_error_get_sys_type(int);                                     /* use zip_error_system_type */
ZIP_EXTERN void zip_error_get(zip_t *, int *, int *);                           /* use zip_get_error, zip_error_code_zip / zip_error_code_system */
ZIP_EXTERN int zip_error_to_str(char *, zip_uint64_t, int, int);
ZIP_EXTERN void zip_file_error_get(zip_file_t *, int *, int *);                 /* use zip_file_get_error, zip_error_code_zip / zip_error_code_system */
 #endif

ZIP_EXTERN int zip_archive_set_tempdir(zip_t *, const char *);
ZIP_EXTERN int zip_close(zip_t *);
ZIP_EXTERN int zip_delete(zip_t *, zip_uint64_t);
ZIP_EXTERN zip_int64_t zip_dir_add(zip_t *, const char *, zip_flags_t);
ZIP_EXTERN void zip_discard(zip_t *);

ZIP_EXTERN zip_error_t *zip_get_error(zip_t *);
ZIP_EXTERN void zip_error_clear(zip_t *);
ZIP_EXTERN int zip_error_code_zip(const zip_error_t *);
ZIP_EXTERN int zip_error_code_system(const zip_error_t *);
ZIP_EXTERN void zip_error_fini(zip_error_t *);
ZIP_EXTERN void zip_error_init(zip_error_t *);
ZIP_EXTERN void zip_error_init_with_code(zip_error_t *, int);
ZIP_EXTERN void zip_error_set(zip_error_t *, int, int);
ZIP_EXTERN const char *zip_error_strerror(zip_error_t *);
ZIP_EXTERN int zip_error_system_type(const zip_error_t *);
ZIP_EXTERN zip_int64_t zip_error_to_data(const zip_error_t *, void *, zip_uint64_t);

ZIP_EXTERN int zip_fclose(zip_file_t *);
ZIP_EXTERN zip_t *zip_fdopen(int, int, int *);
ZIP_EXTERN zip_int64_t zip_file_add(zip_t *, const char *, zip_source_t *, zip_flags_t);
ZIP_EXTERN void zip_file_error_clear(zip_file_t *);
ZIP_EXTERN int zip_file_extra_field_delete(zip_t *, zip_uint64_t, zip_uint16_t, zip_flags_t);
ZIP_EXTERN int zip_file_extra_field_delete_by_id(zip_t *, zip_uint64_t, zip_uint16_t, zip_uint16_t, zip_flags_t);
ZIP_EXTERN int zip_file_extra_field_set(zip_t *, zip_uint64_t, zip_uint16_t, zip_uint16_t, const zip_uint8_t *, zip_uint16_t, zip_flags_t);
ZIP_EXTERN zip_int16_t zip_file_extra_fields_count(zip_t *, zip_uint64_t, zip_flags_t);
ZIP_EXTERN zip_int16_t zip_file_extra_fields_count_by_id(zip_t *, zip_uint64_t, zip_uint16_t, zip_flags_t);
ZIP_EXTERN const zip_uint8_t *zip_file_extra_field_get(zip_t *, zip_uint64_t, zip_uint16_t, zip_uint16_t *, zip_uint16_t *, zip_flags_t);
ZIP_EXTERN const zip_uint8_t *zip_file_extra_field_get_by_id(zip_t *, zip_uint64_t, zip_uint16_t, zip_uint16_t, zip_uint16_t *, zip_flags_t);
ZIP_EXTERN const char *zip_file_get_comment(zip_t *, zip_uint64_t, zip_uint32_t *, zip_flags_t);
ZIP_EXTERN zip_error_t *zip_file_get_error(zip_file_t *);
ZIP_EXTERN int zip_file_get_external_attributes(zip_t *, zip_uint64_t, zip_flags_t, zip_uint8_t *, zip_uint32_t *);
ZIP_EXTERN int zip_file_rename(zip_t *, zip_uint64_t, const char *, zip_flags_t);
ZIP_EXTERN int zip_file_replace(zip_t *, zip_uint64_t, zip_source_t *, zip_flags_t);
ZIP_EXTERN int zip_file_set_comment(zip_t *, zip_uint64_t, const char *, zip_uint16_t, zip_flags_t);
ZIP_EXTERN int zip_file_set_external_attributes(zip_t *, zip_uint64_t, zip_flags_t, zip_uint8_t, zip_uint32_t);
ZIP_EXTERN int zip_file_set_mtime(zip_t *, zip_uint64_t, time_t, zip_flags_t);
ZIP_EXTERN const char *zip_file_strerror(zip_file_t *);
ZIP_EXTERN zip_file_t *zip_fopen(zip_t *, const char *, zip_flags_t);
ZIP_EXTERN zip_file_t *zip_fopen_encrypted(zip_t *, const char *, zip_flags_t, const char *);
ZIP_EXTERN zip_file_t *zip_fopen_index(zip_t *, zip_uint64_t, zip_flags_t);
ZIP_EXTERN zip_file_t *zip_fopen_index_encrypted(zip_t *, zip_uint64_t, zip_flags_t, const char *);
ZIP_EXTERN zip_int64_t zip_fread(zip_file_t *, void *, zip_uint64_t);
ZIP_EXTERN const char *zip_get_archive_comment(zip_t *, int *, zip_flags_t);
ZIP_EXTERN int zip_get_archive_flag(zip_t *, zip_flags_t, zip_flags_t);
ZIP_EXTERN const char *zip_get_name(zip_t *, zip_uint64_t, zip_flags_t);
ZIP_EXTERN zip_int64_t zip_get_num_entries(zip_t *, zip_flags_t);
ZIP_EXTERN zip_int64_t zip_name_locate(zip_t *, const char *, zip_flags_t);
ZIP_EXTERN zip_t *zip_open(const char *, int, int *);
ZIP_EXTERN zip_t *zip_open_from_source(zip_source_t *, int, zip_error_t *);
ZIP_EXTERN int zip_set_archive_comment(zip_t *, const char *, zip_uint16_t);
ZIP_EXTERN int zip_set_archive_flag(zip_t *, zip_flags_t, int);
ZIP_EXTERN int zip_set_default_password(zip_t *, const char *);
ZIP_EXTERN int zip_set_file_compression(zip_t *, zip_uint64_t, zip_int32_t, zip_uint32_t);
ZIP_EXTERN int zip_source_begin_write(zip_source_t *);
ZIP_EXTERN zip_source_t *zip_source_buffer(zip_t *, const void *, zip_uint64_t, int);
ZIP_EXTERN zip_source_t *zip_source_buffer_create(const void *, zip_uint64_t, int, zip_error_t *);
ZIP_EXTERN int zip_source_close(zip_source_t *);
ZIP_EXTERN int zip_source_commit_write(zip_source_t *);
ZIP_EXTERN zip_error_t *zip_source_error(zip_source_t *src);
ZIP_EXTERN zip_source_t *zip_source_file(zip_t *, const char *, zip_uint64_t, zip_int64_t);
ZIP_EXTERN zip_source_t *zip_source_file_create(const char *, zip_uint64_t, zip_int64_t, zip_error_t *);
ZIP_EXTERN zip_source_t *zip_source_filep(zip_t *, FILE *, zip_uint64_t, zip_int64_t);
ZIP_EXTERN zip_source_t *zip_source_filep_create(FILE *, zip_uint64_t, zip_int64_t, zip_error_t *);
ZIP_EXTERN void zip_source_free(zip_source_t *);
ZIP_EXTERN zip_source_t *zip_source_function(zip_t *, zip_source_callback, void *);
ZIP_EXTERN zip_source_t *zip_source_function_create(zip_source_callback, void *, zip_error_t *);
ZIP_EXTERN int zip_source_is_deleted(zip_source_t *);
ZIP_EXTERN void zip_source_keep(zip_source_t *);
ZIP_EXTERN zip_int64_t zip_source_make_command_bitmap(zip_source_cmd_t, ...);
ZIP_EXTERN int zip_source_open(zip_source_t *);
ZIP_EXTERN zip_int64_t zip_source_read(zip_source_t *, void *, zip_uint64_t);
ZIP_EXTERN void zip_source_rollback_write(zip_source_t *);
ZIP_EXTERN int zip_source_seek(zip_source_t *, zip_int64_t, int);
ZIP_EXTERN zip_int64_t zip_source_seek_compute_offset(zip_uint64_t, zip_uint64_t, void *, zip_uint64_t, zip_error_t *);
ZIP_EXTERN int zip_source_seek_write(zip_source_t *, zip_int64_t, int);
ZIP_EXTERN int zip_source_stat(zip_source_t *, zip_stat_t *);
ZIP_EXTERN zip_int64_t zip_source_tell(zip_source_t *);
ZIP_EXTERN zip_int64_t zip_source_tell_write(zip_source_t *);

 #ifdef _WIN32
ZIP_EXTERN zip_source_t *zip_source_win32a(zip_t *, const char *, zip_uint64_t, zip_int64_t);
ZIP_EXTERN zip_source_t *zip_source_win32a_create(const char *, zip_uint64_t, zip_int64_t, zip_error_t *);
ZIP_EXTERN zip_source_t *zip_source_win32handle(zip_t *, void *, zip_uint64_t, zip_int64_t);
ZIP_EXTERN zip_source_t *zip_source_win32handle_create(void *, zip_uint64_t, zip_int64_t, zip_error_t *);
ZIP_EXTERN zip_source_t *zip_source_win32w(zip_t *, const wchar_t *, zip_uint64_t, zip_int64_t);
ZIP_EXTERN zip_source_t *zip_source_win32w_create(const wchar_t *, zip_uint64_t, zip_int64_t, zip_error_t *);
 #endif
ZIP_EXTERN zip_int64_t zip_source_write(zip_source_t *, const void *, zip_uint64_t);
ZIP_EXTERN zip_source_t *zip_source_zip(zip_t *, zip_t *, zip_uint64_t, zip_flags_t, zip_uint64_t, zip_int64_t);
ZIP_EXTERN int zip_stat(zip_t *, const char *, zip_flags_t, zip_stat_t *);
ZIP_EXTERN int zip_stat_index(zip_t *, zip_uint64_t, zip_flags_t, zip_stat_t *);
ZIP_EXTERN void zip_stat_init(zip_stat_t *);
ZIP_EXTERN const char *zip_strerror(zip_t *);
ZIP_EXTERN int zip_unchange(zip_t *, zip_uint64_t);
ZIP_EXTERN int zip_unchange_all(zip_t *);
ZIP_EXTERN int zip_unchange_archive(zip_t *);

 #ifdef __cplusplus
}
 #endif
#endif /* _HAD_ZIP_H */



/* Adapted from NetBSB libc by Dieter Baron */

/*	NetBSD: gettemp.c,v 1.13 2003/12/05 00:57:36 uebayasi Exp   */

/*
 * Copyright (c) 1987, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#ifdef _WIN32
 #include <io.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#ifndef O_BINARY
 #define O_BINARY    0
#endif


int
_zip_mkstemp(char *path) {
#ifdef _WIN32
    int ret;
    ret = _creat(_mktemp(path), _S_IREAD | _S_IWRITE);
    if (ret == -1) {
        return 0;
    } else {
        return ret;
    }
#else
    int fd;
    char *start, *trv;
    struct stat sbuf;
    pid_t pid;

    /* To guarantee multiple calls generate unique names even if
       the file is not created. 676 different possibilities with 7
       or more X's, 26 with 6 or less. */
    static char xtra[2] = { 'a', 'a' };//"aa";
    int xcnt            = 0;

    pid = getpid();

    /* Move to end of path and count trailing X's. */
    for (trv = path; *trv; ++trv)
        if (*trv == 'X')
            xcnt++;
        else
            xcnt = 0;

    /* Use at least one from xtra.  Use 2 if more than 6 X's. */
    if (*(trv - 1) == 'X')
        *--trv = xtra[0];
    if ((xcnt > 6) && (*(trv - 1) == 'X'))
        *--trv = xtra[1];

    /* Set remaining X's to pid digits with 0's to the left. */
    while (*--trv == 'X') {
        *trv = (pid % 10) + '0';
        pid /= 10;
    }

    /* update xtra for next call. */
    if (xtra[0] != 'z')
        xtra[0]++;
    else {
        xtra[0] = 'a';
        if (xtra[1] != 'z')
            xtra[1]++;
        else
            xtra[1] = 'a';
    }

    /*
     * check the target directory; if you have six X's and it
     * doesn't exist this runs for a *very* long time.
     */
    for (start = trv + 1; ; --trv) {
        if (trv <= path)
            break;
        if (*trv == '/') {
            *trv = '\0';
            if (stat(path, &sbuf))
                return 0;
            if (!S_ISDIR(sbuf.st_mode)) {
                errno = ENOTDIR;
                return 0;
            }
            *trv = '/';
            break;
        }
    }

    for ( ; ; ) {
        if ((fd = open(path, O_CREAT | O_EXCL | O_RDWR | O_BINARY, 0600)) >= 0)
            return fd;
        if (errno != EEXIST)
            return 0;

        /* tricky little algorithm for backward compatibility */
        for (trv = start; ; ) {
            if (!*trv)
                return 0;
            if (*trv == 'z')
                *trv++ = 'a';
            else {
                if (isdigit((unsigned char)*trv))
                    *trv = 'a';
                else
                    ++*trv;
                break;
            }
        }
    }
    /*NOTREACHED*/
#endif
}

#ifndef _HAD_ZIPINT_H
 #define _HAD_ZIPINT_H

/*
   zipint.h -- internal declarations.
   Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
   3. The names of the authors may not be used to endorse or promote
   products derived from this software without specific prior
   written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

 #ifdef HAVE_CONFIG_H
  #include "config.h"
 #endif

/* to have *_MAX definitions for all types when compiling with g++ */
 #define __STDC_LIMIT_MACROS
 #include <stdint.h>

//#include <zlib.h>

 #ifdef _WIN32
/* for dup(), close(), etc. */
  #include <io.h>
 #endif

 #ifndef _ZIP_COMPILING_DEPRECATED
  #define ZIP_DISABLE_DEPRECATED
 #endif


 #define ZIP_TRUE     1
 #define ZIP_FALSE    0

 #ifdef _WIN32
  #if defined(HAVE__CLOSE)
   #define close    _close
  #endif
  #if defined(HAVE__DUP)
   #define dup      _dup
  #endif
/* crashes reported when using fdopen instead of _fdopen on Windows/Visual Studio 10/Win64 */
  #if defined(HAVE__FDOPEN)
   #define fdopen    _fdopen
  #endif
  #if defined(HAVE__FILENO)
   #define fileno    _fileno
  #endif
/* Windows' open() doesn't understand Unix permissions */
  #if defined(HAVE__OPEN)
   #define open(a, b, c)    _open((a), (b))
  #endif
  #if defined(HAVE__SNPRINTF)
   #define snprintf    _snprintf
  #endif
  #if defined(HAVE__STRDUP)
   #if !defined(HAVE_STRDUP) || defined(_WIN32)
    #undef strdup
    #define strdup    _strdup
   #endif
  #endif
 #endif

 #ifndef HAVE_FSEEKO
  #define fseeko(s, o, w)    (fseek((s), (long int)(o), (w)))
 #endif

 #ifndef HAVE_FTELLO
  #define ftello(s)    ((long)ftell((s)))
 #endif

 #ifndef HAVE_MKSTEMP
int _zip_mkstemp(char *);

  #define mkstemp    _zip_mkstemp
 #endif

 #if !defined(HAVE_STRCASECMP)
  #if defined(HAVE__STRICMP)
   #define strcasecmp    _stricmp
  #endif
 #endif

 #if SIZEOF_OFF_T == 8
  #define ZIP_OFF_MAX     ZIP_INT64_MAX
  #define ZIP_OFF_MIN     ZIP_INT64_MIN
 #elif SIZEOF_OFF_T == 4
  #define ZIP_OFF_MAX     ZIP_INT32_MAX
  #define ZIP_OFF_MIN     ZIP_INT32_MIN
 #elif SIZEOF_OFF_T == 2
  #define ZIP_OFF_MAX     ZIP_INT16_MAX
  #define ZIP_OFF_MIN     ZIP_INT16_MIN
 #else
  #define ZIP_OFF_MAX     ZIP_INT32_MAX
  #define ZIP_OFF_MIN     ZIP_INT32_MIN
  #ifdef _WIN64
   #define ZIP_OFF_MAX    ZIP_INT64_MAX
   #define ZIP_OFF_MIN    ZIP_INT64_MIN
  #endif

  #ifdef __x86_64
   #define ZIP_OFF_MAX    ZIP_INT64_MAX
   #define ZIP_OFF_MIN    ZIP_INT64_MIN
  #endif

  #ifdef _M_IA64
   #define ZIP_OFF_MAX    ZIP_INT64_MAX
   #define ZIP_OFF_MIN    ZIP_INT64_MIN
  #endif

  #ifdef _LP64
   #define ZIP_OFF_MAX    ZIP_INT64_MAX
   #define ZIP_OFF_MIN    ZIP_INT64_MIN
  #endif

  #ifdef __LP64__
   #define ZIP_OFF_MAX    ZIP_INT64_MAX
   #define ZIP_OFF_MIN    ZIP_INT64_MIN
  #endif
 #endif

 #if defined(HAVE_FTELLO) && defined(HAVE_FSEEKO)
  #define ZIP_FSEEK_MAX    ZIP_OFF_MAX
  #define ZIP_FSEEK_MIN    ZIP_OFF_MIN
 #else
  #define ZIP_FSEEK_MAX    LONG_MAX
  #define ZIP_FSEEK_MIN    LONG_MIN
 #endif

 #ifndef SIZE_MAX
  #if SIZEOF_SIZE_T == 8
   #define SIZE_MAX    ZIP_INT64_MAX
  #elif SIZEOF_SIZE_T == 4
   #define SIZE_MAX    ZIP_INT32_MAX
  #elif SIZEOF_SIZE_T == 2
   #define SIZE_MAX    ZIP_INT16_MAX
  #else
   #error unsupported size of size_t
  #endif
 #endif

 #define CENTRAL_MAGIC              "PK\1\2"
 #define LOCAL_MAGIC                "PK\3\4"
 #define EOCD_MAGIC                 "PK\5\6"
 #define DATADES_MAGIC              "PK\7\8"
 #define EOCD64LOC_MAGIC            "PK\6\7"
 #define EOCD64_MAGIC               "PK\6\6"
 #define CDENTRYSIZE                46u
 #define LENTRYSIZE                 30
 #define MAXCOMLEN                  65536
 #define MAXEXTLEN                  65536
 #define EOCDLEN                    22
 #define EOCD64LOCLEN               20
 #define EOCD64LEN                  56
 #define CDBUFSIZE                  (MAXCOMLEN + EOCDLEN + EOCD64LOCLEN)
 #define BUFSIZE                    8192
 #define EFZIP64SIZE                28

 #define ZIP_CM_REPLACED_DEFAULT    (-2)

 #define ZIP_CM_IS_DEFAULT(x)    ((x) == ZIP_CM_DEFAULT || (x) == ZIP_CM_REPLACED_DEFAULT)

 #define ZIP_EF_UTF_8_COMMENT    0x6375
 #define ZIP_EF_UTF_8_NAME       0x7075
 #define ZIP_EF_ZIP64            0x0001

 #define ZIP_EF_IS_INTERNAL(id)    ((id) == ZIP_EF_UTF_8_COMMENT || (id) == ZIP_EF_UTF_8_NAME || (id) == ZIP_EF_ZIP64)

/* according to unzip-6.0's zipinfo.c, this corresponds to a regular file with rw permissions for everyone */
 #define ZIP_EXT_ATTRIB_DEFAULT        (0100666u << 16)
/* according to unzip-6.0's zipinfo.c, this corresponds to a directory with rwx permissions for everyone */
 #define ZIP_EXT_ATTRIB_DEFAULT_DIR    (0040777u << 16)


 #define ZIP_MAX(a, b)    ((a) > (b) ? (a) : (b))
 #define ZIP_MIN(a, b)    ((a) < (b) ? (a) : (b))

/* This section contains API that won't materialize like this.  It's
   placed in the internal section, pending cleanup. */

/* flags for compression and encryption sources */

 #define ZIP_CODEC_DECODE    0    /* decompress/decrypt (encode flag not set) */
 #define ZIP_CODEC_ENCODE    1    /* compress/encrypt */


typedef zip_source_t *(*zip_compression_implementation)(zip_t *, zip_source_t *, zip_int32_t, int);
typedef zip_source_t *(*zip_encryption_implementation)(zip_t *, zip_source_t *, zip_uint16_t, int, const char *);

zip_compression_implementation _zip_get_compression_implementation(zip_int32_t);
zip_encryption_implementation _zip_get_encryption_implementation(zip_uint16_t);



/* This API is not final yet, but we need it internally, so it's private for now. */

const zip_uint8_t *zip_get_extra_field_by_id(zip_t *, int, int, zip_uint16_t, int, zip_uint16_t *);

/* This section contains API that is of limited use until support for
   user-supplied compression/encryption implementation is finished.
   Thus we will keep it private for now. */

typedef zip_int64_t (*zip_source_layered_callback)(zip_source_t *, void *, void *, zip_uint64_t, enum zip_source_cmd);
zip_source_t *zip_source_crc(zip_t *, zip_source_t *, int);
zip_source_t *zip_source_deflate(zip_t *, zip_source_t *, zip_int32_t, int);
zip_source_t *zip_source_layered(zip_t *, zip_source_t *, zip_source_layered_callback, void *);
zip_source_t *zip_source_layered_create(zip_source_t *src, zip_source_layered_callback cb, void *ud, zip_error_t *error);
zip_source_t *zip_source_pkware(zip_t *, zip_source_t *, zip_uint16_t, int, const char *);
int zip_source_remove(zip_source_t *);
zip_int64_t zip_source_supports(zip_source_t *src);
zip_source_t *zip_source_window(zip_t *, zip_source_t *, zip_uint64_t, zip_uint64_t);


/* error source for layered sources */

enum zip_les {
    ZIP_LES_NONE, ZIP_LES_UPPER, ZIP_LES_LOWER, ZIP_LES_INVAL
};

/* directory entry: general purpose bit flags */

 #define ZIP_GPBF_ENCRYPTED            0x0001   /* is encrypted */
 #define ZIP_GPBF_DATA_DESCRIPTOR      0x0008   /* crc/size after file data */
 #define ZIP_GPBF_STRONG_ENCRYPTION    0x0040   /* uses strong encryption */
 #define ZIP_GPBF_ENCODING_UTF_8       0x0800   /* file name encoding is UTF-8 */


/* extra fields */
 #define ZIP_EF_LOCAL           ZIP_FL_LOCAL                    /* include in local header */
 #define ZIP_EF_CENTRAL         ZIP_FL_CENTRAL                  /* include in central directory */
 #define ZIP_EF_BOTH            (ZIP_EF_LOCAL | ZIP_EF_CENTRAL) /* include in both */

 #define ZIP_FL_FORCE_ZIP64     1024                            /* force zip64 extra field (_zip_dirent_write) */

 #define ZIP_FL_ENCODING_ALL    (ZIP_FL_ENC_GUESS | ZIP_FL_ENC_CP437 | ZIP_FL_ENC_UTF_8)


/* encoding type */
enum zip_encoding_type {
    ZIP_ENCODING_UNKNOWN,       /* not yet analyzed */
    ZIP_ENCODING_ASCII,         /* plain ASCII */
    ZIP_ENCODING_UTF8_KNOWN,    /* is UTF-8 */
    ZIP_ENCODING_UTF8_GUESSED,  /* possibly UTF-8 */
    ZIP_ENCODING_CP437,         /* Code Page 437 */
    ZIP_ENCODING_ERROR          /* should be UTF-8 but isn't */
};

typedef enum zip_encoding_type   zip_encoding_type_t;

typedef struct zip_cdir          zip_cdir_t;
typedef struct zip_dirent        zip_dirent_t;
typedef struct zip_entry         zip_entry_t;
typedef struct zip_extra_field   zip_extra_field_t;
typedef struct zip_string        zip_string_t;
typedef struct zip_buffer        zip_buffer_t;


/* zip archive, part of API */

struct zip {
    zip_source_t *src;               /* data source for archive */
    unsigned int open_flags;         /* flags passed to zip_open */
    zip_error_t error;               /* error information */

    unsigned int flags;              /* archive global flags */
    unsigned int ch_flags;           /* changed archive global flags */

    char *default_password;          /* password used when no other supplied */

    zip_string_t *comment_orig;      /* archive comment */
    zip_string_t *comment_changes;   /* changed archive comment */
    char comment_changed;            /* whether archive comment was changed */

    zip_uint64_t nentry;             /* number of entries */
    zip_uint64_t nentry_alloc;       /* number of entries allocated */
    zip_entry_t *entry;              /* entries */

    unsigned int nopen_source;       /* number of open sources using archive */
    unsigned int nopen_source_alloc; /* number of sources allocated */
    zip_source_t **open_source;      /* open sources using archive */

    char *tempdir;                   /* custom temp dir (needed e.g. for OS X sandboxing) */
};

/* file in zip archive, part of API */

struct zip_file {
    zip_t *za;          /* zip archive containing this file */
    zip_error_t error;  /* error information */
    char eof;
    zip_source_t *src;  /* data source */
};

/* zip archive directory entry (central or local) */

 #define ZIP_DIRENT_COMP_METHOD    0x0001u
 #define ZIP_DIRENT_FILENAME       0x0002u
 #define ZIP_DIRENT_COMMENT        0x0004u
 #define ZIP_DIRENT_EXTRA_FIELD    0x0008u
 #define ZIP_DIRENT_ATTRIBUTES     0x0010u
 #define ZIP_DIRENT_LAST_MOD       0x0020u
 #define ZIP_DIRENT_ALL            0xffffu

struct zip_dirent {
    zip_uint32_t changed;
    char local_extra_fields_read;       /*      whether we already read in local header extra fields */
    char cloned;                        /*      whether this instance is cloned, and thus shares non-changed strings */

    zip_uint16_t version_madeby;        /* (c)  version of creator */
    zip_uint16_t version_needed;        /* (cl) version needed to extract */
    zip_uint16_t bitflags;              /* (cl) general purpose bit flag */
    zip_int32_t comp_method;            /* (cl) compression method used (uint16 and ZIP_CM_DEFAULT (-1)) */
    time_t last_mod;                    /* (cl) time of last modification */
    zip_uint32_t crc;                   /* (cl) CRC-32 of uncompressed data */
    zip_uint64_t comp_size;             /* (cl) size of compressed data */
    zip_uint64_t uncomp_size;           /* (cl) size of uncompressed data */
    zip_string_t *filename;             /* (cl) file name (NUL-terminated) */
    zip_extra_field_t *extra_fields;    /* (cl) extra fields, parsed */
    zip_string_t *comment;              /* (c)  file comment */
    zip_uint32_t disk_number;           /* (c)  disk number start */
    zip_uint16_t int_attrib;            /* (c)  internal file attributes */
    zip_uint32_t ext_attrib;            /* (c)  external file attributes */
    zip_uint64_t offset;                /* (c)  offset of local header */
};

/* zip archive central directory */

struct zip_cdir {
    zip_entry_t *entry;                 /* directory entries */
    zip_uint64_t nentry;                /* number of entries */
    zip_uint64_t nentry_alloc;          /* number of entries allocated */

    zip_uint64_t size;                  /* size of central directory */
    zip_uint64_t offset;                /* offset of central directory in file */
    zip_string_t *comment;              /* zip archive comment */
};

struct zip_extra_field {
    zip_extra_field_t *next;
    zip_flags_t flags;                  /* in local/central header */
    zip_uint16_t id;                    /* header id */
    zip_uint16_t size;                  /* data size */
    zip_uint8_t *data;
};

enum zip_source_write_state {
    ZIP_SOURCE_WRITE_CLOSED,    /* write is not in progress */
    ZIP_SOURCE_WRITE_OPEN,      /* write is in progress */
    ZIP_SOURCE_WRITE_FAILED,    /* commit failed, only rollback allowed */
    ZIP_SOURCE_WRITE_REMOVED    /* file was removed */
};
typedef enum zip_source_write_state   zip_source_write_state_t;

struct zip_source {
    zip_source_t *src;
    union {
        zip_source_callback f;
        zip_source_layered_callback l;
    } cb;
    void *ud;
    zip_error_t error;
    zip_int64_t supports;                 /* supported commands */
    unsigned int open_count;              /* number of times source was opened (directly or as lower layer) */
    zip_source_write_state_t write_state; /* whether source is open for writing */
    char source_closed;                   /* set if source archive is closed */
    zip_t *source_archive;                /* zip archive we're reading from, NULL if not from archive */
    unsigned int refcount;
};

 #define ZIP_SOURCE_IS_OPEN_READING(src)    ((src)->open_count > 0)
 #define ZIP_SOURCE_IS_OPEN_WRITING(src)    ((src)->write_state == ZIP_SOURCE_WRITE_OPEN)
 #define ZIP_SOURCE_IS_LAYERED(src)         ((src)->src != NULL)

/* entry in zip archive directory */

struct zip_entry {
    zip_dirent_t *orig;
    zip_dirent_t *changes;
    zip_source_t *source;
    char deleted;
};


/* file or archive comment, or filename */

struct zip_string {
    zip_uint8_t *raw;                   /* raw string */
    zip_uint16_t length;                /* length of raw string */
    enum zip_encoding_type encoding;    /* autorecognized encoding */
    zip_uint8_t *converted;             /* autoconverted string */
    zip_uint32_t converted_length;      /* length of converted */
};


/* bounds checked access to memory buffer */

struct zip_buffer {
    char ok;
    char free_data;

    zip_uint8_t *data;
    zip_uint64_t size;
    zip_uint64_t offset;
};

/* which files to write in which order */

struct zip_filelist {
    zip_uint64_t idx;
    // TODO    const char *name;
};

typedef struct zip_filelist   zip_filelist_t;


extern const char *const _zip_err_str[];
extern const int _zip_nerr_str;
extern const int _zip_err_type[];


 #define ZIP_ENTRY_CHANGED(e, f)      ((e)->changes && ((e)->changes->changed & (f)))

 #define ZIP_ENTRY_DATA_CHANGED(x)    ((x)->source != NULL)

 #define ZIP_IS_RDONLY(za)            ((za)->ch_flags & ZIP_AFL_RDONLY)


zip_int64_t _zip_add_entry(zip_t *);

zip_uint8_t *_zip_buffer_data(zip_buffer_t *buffer);
char _zip_buffer_eof(zip_buffer_t *buffer);
void _zip_buffer_free(zip_buffer_t *buffer);
zip_uint8_t *_zip_buffer_get(zip_buffer_t *buffer, zip_uint64_t length);
zip_uint16_t _zip_buffer_get_16(zip_buffer_t *buffer);
zip_uint32_t _zip_buffer_get_32(zip_buffer_t *buffer);
zip_uint64_t _zip_buffer_get_64(zip_buffer_t *buffer);
zip_uint8_t _zip_buffer_get_8(zip_buffer_t *buffer);
zip_uint64_t _zip_buffer_left(zip_buffer_t *buffer);
zip_buffer_t *_zip_buffer_new(zip_uint8_t *data, zip_uint64_t size);
zip_buffer_t *_zip_buffer_new_from_source(zip_source_t *src, zip_uint64_t size, zip_uint8_t *buf, zip_error_t *error);
zip_uint64_t _zip_buffer_offset(zip_buffer_t *buffer);
char _zip_buffer_ok(zip_buffer_t *buffer);
int _zip_buffer_put(zip_buffer_t *buffer, const void *src, size_t length);
int _zip_buffer_put_16(zip_buffer_t *buffer, zip_uint16_t i);
int _zip_buffer_put_32(zip_buffer_t *buffer, zip_uint32_t i);
int _zip_buffer_put_64(zip_buffer_t *buffer, zip_uint64_t i);
int _zip_buffer_put_8(zip_buffer_t *buffer, zip_uint8_t i);
int _zip_buffer_set_offset(zip_buffer_t *buffer, zip_uint64_t offset);
zip_uint64_t _zip_buffer_size(zip_buffer_t *buffer);

int _zip_cdir_compute_crc(zip_t *, uLong *);
void _zip_cdir_free(zip_cdir_t *);
zip_cdir_t *_zip_cdir_new(zip_uint64_t, zip_error_t *);
zip_int64_t _zip_cdir_write(zip_t *za, const zip_filelist_t *filelist, zip_uint64_t survivors);
void _zip_deregister_source(zip_t *za, zip_source_t *src);

zip_dirent_t *_zip_dirent_clone(const zip_dirent_t *);
void _zip_dirent_free(zip_dirent_t *);
void _zip_dirent_finalize(zip_dirent_t *);
void _zip_dirent_init(zip_dirent_t *);
char _zip_dirent_needs_zip64(const zip_dirent_t *, zip_flags_t);
zip_dirent_t *_zip_dirent_new(void);
zip_int64_t _zip_dirent_read(zip_dirent_t *zde, zip_source_t *src, zip_buffer_t *buffer, char local, zip_error_t *error);
zip_int32_t _zip_dirent_size(zip_source_t *src, zip_uint16_t, zip_error_t *);
int _zip_dirent_write(zip_t *za, zip_dirent_t *dirent, zip_flags_t flags);

zip_extra_field_t *_zip_ef_clone(const zip_extra_field_t *, zip_error_t *);
zip_extra_field_t *_zip_ef_delete_by_id(zip_extra_field_t *, zip_uint16_t, zip_uint16_t, zip_flags_t);
void _zip_ef_free(zip_extra_field_t *);
const zip_uint8_t *_zip_ef_get_by_id(const zip_extra_field_t *, zip_uint16_t *, zip_uint16_t, zip_uint16_t, zip_flags_t, zip_error_t *);
zip_extra_field_t *_zip_ef_merge(zip_extra_field_t *, zip_extra_field_t *);
zip_extra_field_t *_zip_ef_new(zip_uint16_t, zip_uint16_t, const zip_uint8_t *, zip_flags_t);
zip_extra_field_t *_zip_ef_parse(const zip_uint8_t *, zip_uint16_t, zip_flags_t, zip_error_t *);
zip_extra_field_t *_zip_ef_remove_internal(zip_extra_field_t *);
zip_uint16_t _zip_ef_size(const zip_extra_field_t *, zip_flags_t);
int _zip_ef_write(zip_t *za, const zip_extra_field_t *ef, zip_flags_t flags);

void _zip_entry_finalize(zip_entry_t *);
void _zip_entry_init(zip_entry_t *);

void _zip_error_clear(zip_error_t *);
void _zip_error_get(const zip_error_t *, int *, int *);

void _zip_error_copy(zip_error_t *dst, const zip_error_t *src);
void _zip_error_set_from_source(zip_error_t *, zip_source_t *);

const zip_uint8_t *_zip_extract_extra_field_by_id(zip_error_t *, zip_uint16_t, int, const zip_uint8_t *, zip_uint16_t, zip_uint16_t *);

int _zip_file_extra_field_prepare_for_change(zip_t *, zip_uint64_t);
int _zip_file_fillbuf(void *, size_t, zip_file_t *);
zip_uint64_t _zip_file_get_offset(const zip_t *, zip_uint64_t, zip_error_t *);

int _zip_filerange_crc(zip_source_t *src, zip_uint64_t offset, zip_uint64_t length, uLong *crcp, zip_error_t *error);

zip_dirent_t *_zip_get_dirent(zip_t *, zip_uint64_t, zip_flags_t, zip_error_t *);

enum zip_encoding_type _zip_guess_encoding(zip_string_t *, enum zip_encoding_type);
zip_uint8_t *_zip_cp437_to_utf8(const zip_uint8_t *const, zip_uint32_t, zip_uint32_t *, zip_error_t *);

zip_t *_zip_open(zip_source_t *, unsigned int, zip_error_t *);

int _zip_read(zip_source_t *src, zip_uint8_t *data, zip_uint64_t length, zip_error_t *error);
int _zip_read_at_offset(zip_source_t *src, zip_uint64_t offset, unsigned char *b, size_t length, zip_error_t *error);
zip_uint8_t *_zip_read_data(zip_buffer_t *buffer, zip_source_t *src, size_t length, char nulp, zip_error_t *error);
int _zip_read_local_ef(zip_t *, zip_uint64_t);
zip_string_t *_zip_read_string(zip_buffer_t *buffer, zip_source_t *src, zip_uint16_t lenght, char nulp, zip_error_t *error);
int _zip_register_source(zip_t *za, zip_source_t *src);

void _zip_set_open_error(int *zep, const zip_error_t *err, int ze);

zip_int64_t _zip_source_call(zip_source_t *src, void *data, zip_uint64_t length, zip_source_cmd_t command);
zip_source_t *_zip_source_file_or_p(const char *, FILE *, zip_uint64_t, zip_int64_t, const zip_stat_t *, zip_error_t *error);
void _zip_source_invalidate(zip_source_t *src);
zip_source_t *_zip_source_new(zip_error_t *error);
int _zip_source_set_source_archive(zip_source_t *, zip_t *);
zip_source_t *_zip_source_window_new(zip_source_t *src, zip_uint64_t start, zip_uint64_t length, zip_stat_t *st, zip_error_t *error);
zip_source_t *_zip_source_zip_new(zip_t *, zip_t *, zip_uint64_t, zip_flags_t, zip_uint64_t, zip_uint64_t, const char *);

int _zip_stat_merge(zip_stat_t *dst, const zip_stat_t *src, zip_error_t *error);
int _zip_string_equal(const zip_string_t *, const zip_string_t *);
void _zip_string_free(zip_string_t *);
zip_uint32_t _zip_string_crc32(const zip_string_t *);
const zip_uint8_t *_zip_string_get(zip_string_t *, zip_uint32_t *, zip_flags_t, zip_error_t *);
zip_uint16_t _zip_string_length(const zip_string_t *);
zip_string_t *_zip_string_new(const zip_uint8_t *, zip_uint16_t, zip_flags_t, zip_error_t *);
int _zip_string_write(zip_t *za, const zip_string_t *string);

int _zip_changed(const zip_t *, zip_uint64_t *);
const char *_zip_get_name(zip_t *, zip_uint64_t, zip_flags_t, zip_error_t *);
int _zip_local_header_read(zip_t *, int);
void *_zip_memdup(const void *, size_t, zip_error_t *);
zip_int64_t _zip_name_locate(zip_t *, const char *, zip_flags_t, zip_error_t *);
zip_t *_zip_new(zip_error_t *);

zip_int64_t _zip_file_replace(zip_t *, zip_uint64_t, const char *, zip_source_t *, zip_flags_t);
int _zip_set_name(zip_t *, zip_uint64_t, const char *, zip_flags_t);
void _zip_u2d_time(time_t, zip_uint16_t *, zip_uint16_t *);
int _zip_unchange(zip_t *, zip_uint64_t, int);
void _zip_unchange_data(zip_entry_t *);
int _zip_write(zip_t *za, const void *data, zip_uint64_t length);
#endif /* zipint.h */

/*
   zip_add.c -- add file via callback function
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#define _ZIP_COMPILING_DEPRECATED


/*
   NOTE: Return type is signed so we can return -1 on error.
        The index can not be larger than ZIP_INT64_MAX since the size
        of the central directory cannot be larger than
        ZIP_UINT64_MAX, and each entry is larger than 2 bytes.
 */

ZIP_EXTERN zip_int64_t
zip_add(zip_t *za, const char *name, zip_source_t *source) {
    return zip_file_add(za, name, source, 0);
}

/*
   zip_add_dir.c -- add directory
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#define _ZIP_COMPILING_DEPRECATED


/* NOTE: Signed due to -1 on error.  See zip_add.c for more details. */

ZIP_EXTERN zip_int64_t
zip_add_dir(zip_t *za, const char *name) {
    return zip_dir_add(za, name, 0);
}

/*
   zip_add_entry.c -- create and init struct zip_entry
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>



/* NOTE: Signed due to -1 on error.  See zip_add.c for more details. */

zip_int64_t
_zip_add_entry(zip_t *za) {
    zip_uint64_t idx;

    if (za->nentry + 1 >= za->nentry_alloc) {
        zip_entry_t *rentries;
        zip_uint64_t nalloc       = za->nentry_alloc + 16;
        zip_uint64_t realloc_size = sizeof(struct zip_entry) * (size_t)nalloc;

        if (sizeof(struct zip_entry) * (size_t)za->nentry_alloc > realloc_size) {
            zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
            return -1;
        }
        rentries = (zip_entry_t *)realloc(za->entry, sizeof(struct zip_entry) * (size_t)nalloc);
        if (!rentries) {
            zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
            return -1;
        }
        za->entry        = rentries;
        za->nentry_alloc = nalloc;
    }

    idx = za->nentry++;

    _zip_entry_init(za->entry + idx);

    return (zip_int64_t)idx;
}

/*
   zip_buffer.c -- bounds checked access to memory buffer
   Copyright (C) 2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
   3. The names of the authors may not be used to endorse or promote
   products derived from this software without specific prior
   written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <string.h>



zip_uint8_t *
_zip_buffer_data(zip_buffer_t *buffer) {
    return buffer->data;
}

void
_zip_buffer_free(zip_buffer_t *buffer) {
    if (buffer == NULL) {
        return;
    }

    if (buffer->free_data) {
        free(buffer->data);
    }

    free(buffer);
}

char
_zip_buffer_eof(zip_buffer_t *buffer) {
    return buffer->ok && buffer->offset == buffer->size;
}

zip_uint8_t *
_zip_buffer_get(zip_buffer_t *buffer, zip_uint64_t length) {
    zip_uint8_t *data;

    if (!buffer->ok || (buffer->offset + length < length) || (buffer->offset + length > buffer->size)) {
        buffer->ok = ZIP_FALSE;
        return NULL;
    }

    data            = buffer->data + buffer->offset;
    buffer->offset += length;
    return data;
}

zip_uint16_t
_zip_buffer_get_16(zip_buffer_t *buffer) {
    zip_uint8_t *data = _zip_buffer_get(buffer, 2);

    if (data == NULL) {
        return 0;
    }

    return (zip_uint16_t)(data[0] + (data[1] << 8));
}

zip_uint32_t
_zip_buffer_get_32(zip_buffer_t *buffer) {
    zip_uint8_t *data = _zip_buffer_get(buffer, 4);

    if (data == NULL) {
        return 0;
    }

    return ((((((zip_uint32_t)data[3] << 8) + data[2]) << 8) + data[1]) << 8) + data[0];
}

zip_uint64_t
_zip_buffer_get_64(zip_buffer_t *buffer) {
    zip_uint8_t *data = _zip_buffer_get(buffer, 8);

    if (data == NULL) {
        return 0;
    }

    return ((zip_uint64_t)data[7] << 56) + ((zip_uint64_t)data[6] << 48) + ((zip_uint64_t)data[5] << 40) + ((zip_uint64_t)data[4] << 32) + ((zip_uint64_t)data[3] << 24) + ((zip_uint64_t)data[2] << 16) + ((zip_uint64_t)data[1] << 8) + (zip_uint64_t)data[0];
}

zip_uint8_t
_zip_buffer_get_8(zip_buffer_t *buffer) {
    zip_uint8_t *data = _zip_buffer_get(buffer, 1);

    if (data == NULL) {
        return 0;
    }

    return data[0];
}

zip_uint64_t
_zip_buffer_left(zip_buffer_t *buffer) {
    return buffer->ok ? buffer->size - buffer->offset : 0;
}

zip_buffer_t *
_zip_buffer_new(zip_uint8_t *data, zip_uint64_t size) {
    char free_data = (data == NULL);
    zip_buffer_t *buffer;

    if (data == NULL) {
        if ((data = (zip_uint8_t *)malloc(size)) == NULL) {
            return NULL;
        }
    }

    if ((buffer = (zip_buffer_t *)malloc(sizeof(*buffer))) == NULL) {
        if (free_data) {
            free(data);
        }
        return NULL;
    }

    buffer->ok        = ZIP_TRUE;
    buffer->data      = data;
    buffer->size      = size;
    buffer->offset    = 0;
    buffer->free_data = free_data;

    return buffer;
}

zip_buffer_t *
_zip_buffer_new_from_source(zip_source_t *src, zip_uint64_t size, zip_uint8_t *buf, zip_error_t *error) {
    zip_buffer_t *buffer;

    if ((buffer = _zip_buffer_new(buf, size)) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    if (_zip_read(src, buffer->data, size, error) < 0) {
        _zip_buffer_free(buffer);
        return NULL;
    }

    return buffer;
}

zip_uint64_t
_zip_buffer_offset(zip_buffer_t *buffer) {
    return buffer->ok ? buffer->offset : 0;
}

char
_zip_buffer_ok(zip_buffer_t *buffer) {
    return buffer->ok;
}

int
_zip_buffer_put(zip_buffer_t *buffer, const void *src, size_t length) {
    zip_uint8_t *dst = _zip_buffer_get(buffer, length);

    if (dst == NULL) {
        return -1;
    }

    memcpy(dst, src, length);
    return 0;
}

int
_zip_buffer_put_16(zip_buffer_t *buffer, zip_uint16_t i) {
    zip_uint8_t *data = _zip_buffer_get(buffer, 2);

    if (data == NULL) {
        return -1;
    }

    data[0] = (zip_uint8_t)(i & 0xff);
    data[1] = (zip_uint8_t)((i >> 8) & 0xff);

    return 0;
}

int
_zip_buffer_put_32(zip_buffer_t *buffer, zip_uint32_t i) {
    zip_uint8_t *data = _zip_buffer_get(buffer, 4);

    if (data == NULL) {
        return -1;
    }

    data[0] = (zip_uint8_t)(i & 0xff);
    data[1] = (zip_uint8_t)((i >> 8) & 0xff);
    data[2] = (zip_uint8_t)((i >> 16) & 0xff);
    data[3] = (zip_uint8_t)((i >> 24) & 0xff);

    return 0;
}

int
_zip_buffer_put_64(zip_buffer_t *buffer, zip_uint64_t i) {
    zip_uint8_t *data = _zip_buffer_get(buffer, 8);

    if (data == NULL) {
        return -1;
    }

    data[0] = (zip_uint8_t)(i & 0xff);
    data[1] = (zip_uint8_t)((i >> 8) & 0xff);
    data[2] = (zip_uint8_t)((i >> 16) & 0xff);
    data[3] = (zip_uint8_t)((i >> 24) & 0xff);
    data[4] = (zip_uint8_t)((i >> 32) & 0xff);
    data[5] = (zip_uint8_t)((i >> 40) & 0xff);
    data[6] = (zip_uint8_t)((i >> 48) & 0xff);
    data[7] = (zip_uint8_t)((i >> 56) & 0xff);

    return 0;
}

int
_zip_buffer_put_8(zip_buffer_t *buffer, zip_uint8_t i) {
    zip_uint8_t *data = _zip_buffer_get(buffer, 1);

    if (data == NULL) {
        return -1;
    }

    data[0] = i;

    return 0;
}

int
_zip_buffer_set_offset(zip_buffer_t *buffer, zip_uint64_t offset) {
    if (offset > buffer->size) {
        buffer->ok = ZIP_FALSE;
        return -1;
    }

    buffer->ok     = ZIP_TRUE;
    buffer->offset = offset;

    return 0;
}

zip_uint64_t
_zip_buffer_size(zip_buffer_t *buffer) {
    return buffer->size;
}

/*
   zip_close.c -- close zip archive and update changes
   Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
 #include <strings.h>
#endif
#include <errno.h>
#ifdef HAVE_UNISTD_H
 #include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
 #include <io.h>
 #include <fcntl.h>
#endif


/* max deflate size increase: size + ceil(size/16k)*5+6 */
#define MAX_DEFLATE_SIZE_32    4293656963u

static int add_data(zip_t *, zip_source_t *, zip_dirent_t *);
static int copy_data(zip_t *, zip_uint64_t);
static int copy_source(zip_t *, zip_source_t *);
static int write_cdir(zip_t *, const zip_filelist_t *, zip_uint64_t);


ZIP_EXTERN int
zip_close(zip_t *za) {
    zip_uint64_t i, j, survivors;
    zip_int64_t off;
    int error;
    zip_filelist_t *filelist;
    int changed;

    if (za == NULL)
        return -1;

    changed = _zip_changed(za, &survivors);

    /* don't create zip files with no entries */
    if (survivors == 0) {
        if ((za->open_flags & ZIP_TRUNCATE) || changed) {
            if (zip_source_remove(za->src) < 0) {
                _zip_error_set_from_source(&za->error, za->src);
                return -1;
            }
        }
        zip_discard(za);
        return 0;
    }

    if (!changed) {
        zip_discard(za);
        return 0;
    }

    if (survivors > za->nentry) {
        zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
        return -1;
    }

    if ((filelist = (zip_filelist_t *)malloc(sizeof(filelist[0]) * (size_t)survivors)) == NULL)
        return -1;

    /* create list of files with index into original archive  */
    for (i = j = 0; i < za->nentry; i++) {
        if (za->entry[i].deleted)
            continue;

        if (j >= survivors) {
            free(filelist);
            zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
            return -1;
        }

        filelist[j].idx = i;
        j++;
    }
    if (j < survivors) {
        free(filelist);
        zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
        return -1;
    }

    if (zip_source_begin_write(za->src) < 0) {
        _zip_error_set_from_source(&za->error, za->src);
        free(filelist);
        return -1;
    }

    error = 0;
    for (j = 0; j < survivors; j++) {
        int new_data;
        zip_entry_t *entry;
        zip_dirent_t *de;

        i     = filelist[j].idx;
        entry = za->entry + i;

        new_data = (ZIP_ENTRY_DATA_CHANGED(entry) || ZIP_ENTRY_CHANGED(entry, ZIP_DIRENT_COMP_METHOD));

        /* create new local directory entry */
        if (entry->changes == NULL) {
            if ((entry->changes = _zip_dirent_clone(entry->orig)) == NULL) {
                zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
                error = 1;
                break;
            }
        }
        de = entry->changes;

        if (_zip_read_local_ef(za, i) < 0) {
            error = 1;
            break;
        }

        if ((off = zip_source_tell_write(za->src)) < 0) {
            error = 1;
            break;
        }
        de->offset = (zip_uint64_t)off;

        if (new_data) {
            zip_source_t *zs;

            zs = NULL;
            if (!ZIP_ENTRY_DATA_CHANGED(entry)) {
                if ((zs = _zip_source_zip_new(za, za, i, ZIP_FL_UNCHANGED, 0, 0, NULL)) == NULL) {
                    error = 1;
                    break;
                }
            }

            /* add_data writes dirent */
            if (add_data(za, zs ? zs : entry->source, de) < 0) {
                error = 1;
                if (zs)
                    zip_source_free(zs);
                break;
            }
            if (zs)
                zip_source_free(zs);
        } else {
            zip_uint64_t offset;

            /* when copying data, all sizes are known -> no data descriptor needed */
            de->bitflags &= (zip_uint16_t) ~ZIP_GPBF_DATA_DESCRIPTOR;
            if (_zip_dirent_write(za, de, ZIP_FL_LOCAL) < 0) {
                error = 1;
                break;
            }
            if ((offset = _zip_file_get_offset(za, i, &za->error)) == 0) {
                error = 1;
                break;
            }
            if (zip_source_seek(za->src, (zip_int64_t)offset, SEEK_SET) < 0) {
                _zip_error_set_from_source(&za->error, za->src);
                error = 1;
                break;
            }
            if (copy_data(za, de->comp_size) < 0) {
                error = 1;
                break;
            }
        }
    }

    if (!error) {
        if (write_cdir(za, filelist, survivors) < 0)
            error = 1;
    }

    free(filelist);

    if (!error) {
        if (zip_source_commit_write(za->src) != 0) {
            _zip_error_set_from_source(&za->error, za->src);
            error = 1;
        }
    }

    if (error) {
        zip_source_rollback_write(za->src);
        return -1;
    }

    zip_discard(za);

    return 0;
}

static int
add_data(zip_t *za, zip_source_t *src, zip_dirent_t *de) {
    zip_int64_t offstart, offdata, offend;
    struct zip_stat st;
    zip_source_t *s2;
    int ret;
    int is_zip64;
    zip_flags_t flags;

    if (zip_source_stat(src, &st) < 0) {
        _zip_error_set_from_source(&za->error, src);
        return -1;
    }

    if ((st.valid & ZIP_STAT_COMP_METHOD) == 0) {
        st.valid      |= ZIP_STAT_COMP_METHOD;
        st.comp_method = ZIP_CM_STORE;
    }

    if (ZIP_CM_IS_DEFAULT(de->comp_method) && (st.comp_method != ZIP_CM_STORE))
        de->comp_method = st.comp_method;
    else if ((de->comp_method == ZIP_CM_STORE) && (st.valid & ZIP_STAT_SIZE)) {
        st.valid    |= ZIP_STAT_COMP_SIZE;
        st.comp_size = st.size;
    } else {
        /* we'll recompress */
        st.valid &= ~ZIP_STAT_COMP_SIZE;
    }


    flags = ZIP_EF_LOCAL;

    if ((st.valid & ZIP_STAT_SIZE) == 0)
        flags |= ZIP_FL_FORCE_ZIP64;
    else {
        de->uncomp_size = st.size;

        if ((st.valid & ZIP_STAT_COMP_SIZE) == 0) {
            if (((((de->comp_method == ZIP_CM_DEFLATE) || ZIP_CM_IS_DEFAULT(de->comp_method)) && (st.size > MAX_DEFLATE_SIZE_32)) ||
                 ((de->comp_method != ZIP_CM_STORE) && (de->comp_method != ZIP_CM_DEFLATE) && !ZIP_CM_IS_DEFAULT(de->comp_method))))
                flags |= ZIP_FL_FORCE_ZIP64;
        } else
            de->comp_size = st.comp_size;
    }

    if ((offstart = zip_source_tell_write(za->src)) < 0) {
        return -1;
    }

    /* as long as we don't support non-seekable output, clear data descriptor bit */
    de->bitflags &= (zip_uint16_t) ~ZIP_GPBF_DATA_DESCRIPTOR;
    if ((is_zip64 = _zip_dirent_write(za, de, flags)) < 0)
        return -1;


    if ((st.comp_method == ZIP_CM_STORE) || (ZIP_CM_IS_DEFAULT(de->comp_method) && (st.comp_method != de->comp_method))) {
        zip_source_t *s_store, *s_crc;
        zip_compression_implementation comp_impl;

        if (st.comp_method != ZIP_CM_STORE) {
            if ((comp_impl = _zip_get_compression_implementation(st.comp_method)) == NULL) {
                zip_error_set(&za->error, ZIP_ER_COMPNOTSUPP, 0);
                return -1;
            }
            if ((s_store = comp_impl(za, src, st.comp_method, ZIP_CODEC_DECODE)) == NULL) {
                /* error set by comp_impl */
                return -1;
            }
        } else {
            /* to have the same reference count to src as in the case where it's not stored */
            zip_source_keep(src);
            s_store = src;
        }

        s_crc = zip_source_crc(za, s_store, 0);
        zip_source_free(s_store);
        if (s_crc == NULL) {
            return -1;
        }

        if ((de->comp_method != ZIP_CM_STORE) && (((st.valid & ZIP_STAT_SIZE) == 0) || (st.size != 0))) {
            if ((comp_impl = _zip_get_compression_implementation(de->comp_method)) == NULL) {
                zip_error_set(&za->error, ZIP_ER_COMPNOTSUPP, 0);
                zip_source_free(s_crc);
                return -1;
            }
            s2 = comp_impl(za, s_crc, de->comp_method, ZIP_CODEC_ENCODE);
            zip_source_free(s_crc);
            if (s2 == NULL) {
                return -1;
            }
        } else {
            s2 = s_crc;
        }
    } else {
        zip_source_keep(src);
        s2 = src;
    }

    if ((offdata = zip_source_tell_write(za->src)) < 0) {
        return -1;
    }

    ret = copy_source(za, s2);

    if (zip_source_stat(s2, &st) < 0)
        ret = -1;

    zip_source_free(s2);

    if (ret < 0)
        return -1;

    if ((offend = zip_source_tell_write(za->src)) < 0) {
        return -1;
    }

    if (zip_source_seek_write(za->src, offstart, SEEK_SET) < 0) {
        _zip_error_set_from_source(&za->error, za->src);
        return -1;
    }

    if ((st.valid & (ZIP_STAT_COMP_METHOD | ZIP_STAT_CRC | ZIP_STAT_SIZE)) != (ZIP_STAT_COMP_METHOD | ZIP_STAT_CRC | ZIP_STAT_SIZE)) {
        zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
        return -1;
    }

    if ((de->changed & ZIP_DIRENT_LAST_MOD) == 0) {
        if (st.valid & ZIP_STAT_MTIME)
            de->last_mod = st.mtime;
        else
            time(&de->last_mod);
    }
    de->comp_method = st.comp_method;
    de->crc         = st.crc;
    de->uncomp_size = st.size;
    de->comp_size   = (zip_uint64_t)(offend - offdata);

    if ((ret = _zip_dirent_write(za, de, flags)) < 0)
        return -1;

    if (is_zip64 != ret) {
        /* Zip64 mismatch between preliminary file header written before data and final file header written afterwards */
        zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
        return -1;
    }


    if (zip_source_seek_write(za->src, offend, SEEK_SET) < 0) {
        _zip_error_set_from_source(&za->error, za->src);
        return -1;
    }

    return 0;
}

static int
copy_data(zip_t *za, zip_uint64_t len) {
    zip_uint8_t buf[BUFSIZE];
    size_t n;

    while (len > 0) {
        n = len > sizeof(buf) ? sizeof(buf) : len;
        if (_zip_read(za->src, buf, n, &za->error) < 0) {
            return -1;
        }

        if (_zip_write(za, buf, n) < 0) {
            return -1;
        }

        len -= n;
    }

    return 0;
}

static int
copy_source(zip_t *za, zip_source_t *src) {
    zip_uint8_t buf[BUFSIZE];
    zip_int64_t n;
    int ret;

    if (zip_source_open(src) < 0) {
        _zip_error_set_from_source(&za->error, src);
        return -1;
    }

    ret = 0;
    while ((n = zip_source_read(src, buf, sizeof(buf))) > 0) {
        if (_zip_write(za, buf, (zip_uint64_t)n) < 0) {
            ret = -1;
            break;
        }
    }

    if (n < 0) {
        _zip_error_set_from_source(&za->error, src);
        ret = -1;
    }

    zip_source_close(src);

    return ret;
}

static int
write_cdir(zip_t *za, const zip_filelist_t *filelist, zip_uint64_t survivors) {
    zip_int64_t cd_start, end, size;

    if ((cd_start = zip_source_tell_write(za->src)) < 0) {
        return -1;
    }

    if ((size = _zip_cdir_write(za, filelist, survivors)) < 0) {
        return -1;
    }

    if ((end = zip_source_tell_write(za->src)) < 0) {
        return -1;
    }

    return 0;
}

int
_zip_changed(const zip_t *za, zip_uint64_t *survivorsp) {
    int changed;
    zip_uint64_t i, survivors;

    changed   = 0;
    survivors = 0;

    if (za->comment_changed || (za->ch_flags != za->flags))
        changed = 1;

    for (i = 0; i < za->nentry; i++) {
        if (za->entry[i].deleted || za->entry[i].source || (za->entry[i].changes && (za->entry[i].changes->changed != 0)))
            changed = 1;
        if (!za->entry[i].deleted)
            survivors++;
    }

    if (survivorsp)
        *survivorsp = survivors;

    return changed;
}

/*
   zip_delete.c -- delete file from zip archive
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_delete(zip_t *za, zip_uint64_t idx) {
    if (idx >= za->nentry) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    /* allow duplicate file names, because the file will
     * be removed directly afterwards */
    if (_zip_unchange(za, idx, 1) != 0)
        return -1;

    za->entry[idx].deleted = 1;

    return 0;
}

/*
   zip_dir_add.c -- add directory
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>
#include <string.h>



/* NOTE: Signed due to -1 on error.  See zip_add.c for more details. */

ZIP_EXTERN zip_int64_t
zip_dir_add(zip_t *za, const char *name, zip_flags_t flags) {
    size_t len;
    zip_int64_t idx;
    char *s;
    zip_source_t *source;

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    if (name == NULL) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    s   = NULL;
    len = strlen(name);

    if (name[len - 1] != '/') {
        if ((s = (char *)malloc(len + 2)) == NULL) {
            zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
            return -1;
        }
        strcpy(s, name);
        s[len]     = '/';
        s[len + 1] = '\0';
    }

    if ((source = zip_source_buffer(za, NULL, 0, 0)) == NULL) {
        free(s);
        return -1;
    }

    idx = _zip_file_replace(za, ZIP_UINT64_MAX, s ? s : name, source, flags);

    free(s);

    if (idx < 0)
        zip_source_free(source);
    else {
        if (zip_file_set_external_attributes(za, (zip_uint64_t)idx, 0, ZIP_OPSYS_DEFAULT, ZIP_EXT_ATTRIB_DEFAULT_DIR) < 0) {
            zip_delete(za, (zip_uint64_t)idx);
            return -1;
        }
    }

    return idx;
}

/*
   zip_dirent.c -- read directory entry (local or central), clean dirent
   Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>



static time_t _zip_d2u_time(zip_uint16_t, zip_uint16_t);
static zip_string_t *_zip_dirent_process_ef_utf_8(const zip_dirent_t *de, zip_uint16_t id, zip_string_t *str);
static zip_extra_field_t *_zip_ef_utf8(zip_uint16_t, zip_string_t *, zip_error_t *);


void
_zip_cdir_free(zip_cdir_t *cd) {
    zip_uint64_t i;

    if (!cd)
        return;

    for (i = 0; i < cd->nentry; i++)
        _zip_entry_finalize(cd->entry + i);
    free(cd->entry);
    _zip_string_free(cd->comment);
    free(cd);
}

zip_cdir_t *
_zip_cdir_new(zip_uint64_t nentry, zip_error_t *error) {
    zip_cdir_t *cd;
    zip_uint64_t i;

    if ((cd = (zip_cdir_t *)malloc(sizeof(*cd))) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    if (nentry == 0)
        cd->entry = NULL;
    else if ((nentry > SIZE_MAX / sizeof(*(cd->entry))) || ((cd->entry = (zip_entry_t *)malloc(sizeof(*(cd->entry)) * (size_t)nentry)) == NULL)) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        free(cd);
        return NULL;
    }

    for (i = 0; i < nentry; i++)
        _zip_entry_init(cd->entry + i);

    cd->nentry  = cd->nentry_alloc = nentry;
    cd->size    = cd->offset = 0;
    cd->comment = NULL;

    return cd;
}

zip_int64_t
_zip_cdir_write(zip_t *za, const zip_filelist_t *filelist, zip_uint64_t survivors) {
    zip_uint64_t offset, size;
    zip_string_t *comment;
    zip_uint8_t buf[EOCDLEN + EOCD64LEN + EOCD64LOCLEN];
    zip_buffer_t *buffer;
    zip_int64_t off;
    zip_uint64_t i;
    char is_zip64;
    int ret;

    if ((off = zip_source_tell_write(za->src)) < 0) {
        _zip_error_set_from_source(&za->error, za->src);
        return -1;
    }
    offset = (zip_uint64_t)off;

    is_zip64 = 0;

    for (i = 0; i < survivors; i++) {
        zip_entry_t *entry = za->entry + filelist[i].idx;

        if ((ret = _zip_dirent_write(za, entry->changes ? entry->changes : entry->orig, ZIP_FL_CENTRAL)) < 0)
            return -1;
        if (ret)
            is_zip64 = ZIP_TRUE;
    }

    if ((off = zip_source_tell_write(za->src)) < 0) {
        _zip_error_set_from_source(&za->error, za->src);
        return -1;
    }
    size = (zip_uint64_t)off - offset;

    if ((offset > ZIP_UINT32_MAX) || (survivors > ZIP_UINT16_MAX))
        is_zip64 = ZIP_TRUE;


    if ((buffer = _zip_buffer_new(buf, sizeof(buf))) == NULL) {
        zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
        return -1;
    }

    if (is_zip64) {
        _zip_buffer_put(buffer, EOCD64_MAGIC, 4);
        _zip_buffer_put_64(buffer, EOCD64LEN - 12);
        _zip_buffer_put_16(buffer, 45);
        _zip_buffer_put_16(buffer, 45);
        _zip_buffer_put_32(buffer, 0);
        _zip_buffer_put_32(buffer, 0);
        _zip_buffer_put_64(buffer, survivors);
        _zip_buffer_put_64(buffer, survivors);
        _zip_buffer_put_64(buffer, size);
        _zip_buffer_put_64(buffer, offset);
        _zip_buffer_put(buffer, EOCD64LOC_MAGIC, 4);
        _zip_buffer_put_32(buffer, 0);
        _zip_buffer_put_64(buffer, offset + size);
        _zip_buffer_put_32(buffer, 1);
    }

    _zip_buffer_put(buffer, EOCD_MAGIC, 4);
    _zip_buffer_put_32(buffer, 0);
    _zip_buffer_put_16(buffer, (zip_uint16_t)(survivors >= ZIP_UINT16_MAX ? ZIP_UINT16_MAX : survivors));
    _zip_buffer_put_16(buffer, (zip_uint16_t)(survivors >= ZIP_UINT16_MAX ? ZIP_UINT16_MAX : survivors));
    _zip_buffer_put_32(buffer, size >= ZIP_UINT32_MAX ? ZIP_UINT32_MAX : (zip_uint32_t)size);
    _zip_buffer_put_32(buffer, offset >= ZIP_UINT32_MAX ? ZIP_UINT32_MAX : (zip_uint32_t)offset);

    comment = za->comment_changed ? za->comment_changes : za->comment_orig;

    _zip_buffer_put_16(buffer, (zip_uint16_t)(comment ? comment->length : 0));

    if (!_zip_buffer_ok(buffer)) {
        zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
        _zip_buffer_free(buffer);
        return -1;
    }

    if (_zip_write(za, _zip_buffer_data(buffer), _zip_buffer_offset(buffer)) < 0) {
        _zip_buffer_free(buffer);
        return -1;
    }

    _zip_buffer_free(buffer);

    if (comment) {
        if (_zip_write(za, comment->raw, comment->length) < 0) {
            return -1;
        }
    }

    return (zip_int64_t)size;
}

zip_dirent_t *
_zip_dirent_clone(const zip_dirent_t *sde) {
    zip_dirent_t *tde;

    if ((tde = (zip_dirent_t *)malloc(sizeof(*tde))) == NULL)
        return NULL;

    if (sde)
        memcpy(tde, sde, sizeof(*sde));
    else
        _zip_dirent_init(tde);

    tde->changed = 0;
    tde->cloned  = 1;

    return tde;
}

void
_zip_dirent_finalize(zip_dirent_t *zde) {
    if (!zde->cloned || zde->changed & ZIP_DIRENT_FILENAME) {
        _zip_string_free(zde->filename);
        zde->filename = NULL;
    }
    if (!zde->cloned || zde->changed & ZIP_DIRENT_EXTRA_FIELD) {
        _zip_ef_free(zde->extra_fields);
        zde->extra_fields = NULL;
    }
    if (!zde->cloned || zde->changed & ZIP_DIRENT_COMMENT) {
        _zip_string_free(zde->comment);
        zde->comment = NULL;
    }
}

void
_zip_dirent_free(zip_dirent_t *zde) {
    if (zde == NULL)
        return;

    _zip_dirent_finalize(zde);
    free(zde);
}

void
_zip_dirent_init(zip_dirent_t *de) {
    de->changed = 0;
    de->local_extra_fields_read = 0;
    de->cloned = 0;

    de->version_madeby = 20 | (ZIP_OPSYS_DEFAULT << 8);
    de->version_needed = 20; /* 2.0 */
    de->bitflags       = 0;
    de->comp_method    = ZIP_CM_DEFAULT;
    de->last_mod       = 0;
    de->crc            = 0;
    de->comp_size      = 0;
    de->uncomp_size    = 0;
    de->filename       = NULL;
    de->extra_fields   = NULL;
    de->comment        = NULL;
    de->disk_number    = 0;
    de->int_attrib     = 0;
    de->ext_attrib     = ZIP_EXT_ATTRIB_DEFAULT;
    de->offset         = 0;
}

char
_zip_dirent_needs_zip64(const zip_dirent_t *de, zip_flags_t flags) {
    if ((de->uncomp_size >= ZIP_UINT32_MAX) || (de->comp_size >= ZIP_UINT32_MAX) ||
        ((flags & ZIP_FL_CENTRAL) && (de->offset >= ZIP_UINT32_MAX)))
        return ZIP_TRUE;

    return ZIP_FALSE;
}

zip_dirent_t *
_zip_dirent_new(void) {
    zip_dirent_t *de;

    if ((de = (zip_dirent_t *)malloc(sizeof(*de))) == NULL)
        return NULL;

    _zip_dirent_init(de);
    return de;
}

/* _zip_dirent_read(zde, fp, bufp, left, localp, error):
   Fills the zip directory entry zde.

   If buffer is non-NULL, data is taken from there; otherwise data is read from fp as needed.

   If local is ZIP_TRUE, it reads a local header instead of a central directory entry.

   Returns size of dirent read if successful. On error, error is filled in and -1 is returned.
 */

zip_int64_t
_zip_dirent_read(zip_dirent_t *zde, zip_source_t *src, zip_buffer_t *buffer, char local, zip_error_t *error) {
    zip_uint8_t buf[CDENTRYSIZE];
    zip_uint16_t dostime, dosdate;
    zip_uint32_t size, variable_size;
    zip_uint16_t filename_len, comment_len, ef_len;

    char from_buffer = (buffer != NULL);

    size = local ? LENTRYSIZE : CDENTRYSIZE;

    if (buffer) {
        if (_zip_buffer_left(buffer) < size) {
            zip_error_set(error, ZIP_ER_NOZIP, 0);
            return -1;
        }
    } else {
        if ((buffer = _zip_buffer_new_from_source(src, size, buf, error)) == NULL) {
            return -1;
        }
    }

    if (memcmp(_zip_buffer_get(buffer, 4), (local ? LOCAL_MAGIC : CENTRAL_MAGIC), 4) != 0) {
        zip_error_set(error, ZIP_ER_NOZIP, 0);
        if (!from_buffer) {
            _zip_buffer_free(buffer);
        }
        return -1;
    }

    /* convert buffercontents to zip_dirent */

    _zip_dirent_init(zde);
    if (!local)
        zde->version_madeby = _zip_buffer_get_16(buffer);
    else
        zde->version_madeby = 0;
    zde->version_needed = _zip_buffer_get_16(buffer);
    zde->bitflags       = _zip_buffer_get_16(buffer);
    zde->comp_method    = _zip_buffer_get_16(buffer);

    /* convert to time_t */
    dostime       = _zip_buffer_get_16(buffer);
    dosdate       = _zip_buffer_get_16(buffer);
    zde->last_mod = _zip_d2u_time(dostime, dosdate);

    zde->crc         = _zip_buffer_get_32(buffer);
    zde->comp_size   = _zip_buffer_get_32(buffer);
    zde->uncomp_size = _zip_buffer_get_32(buffer);

    filename_len = _zip_buffer_get_16(buffer);
    ef_len       = _zip_buffer_get_16(buffer);

    if (local) {
        comment_len      = 0;
        zde->disk_number = 0;
        zde->int_attrib  = 0;
        zde->ext_attrib  = 0;
        zde->offset      = 0;
    } else {
        comment_len      = _zip_buffer_get_16(buffer);
        zde->disk_number = _zip_buffer_get_16(buffer);
        zde->int_attrib  = _zip_buffer_get_16(buffer);
        zde->ext_attrib  = _zip_buffer_get_32(buffer);
        zde->offset      = _zip_buffer_get_32(buffer);
    }

    if (!_zip_buffer_ok(buffer)) {
        zip_error_set(error, ZIP_ER_INTERNAL, 0);
        if (!from_buffer) {
            _zip_buffer_free(buffer);
        }
        return -1;
    }

    zde->filename     = NULL;
    zde->extra_fields = NULL;
    zde->comment      = NULL;

    variable_size = (zip_uint32_t)filename_len + (zip_uint32_t)ef_len + (zip_uint32_t)comment_len;

    if (from_buffer) {
        if (_zip_buffer_left(buffer) < variable_size) {
            zip_error_set(error, ZIP_ER_INCONS, 0);
            return -1;
        }
    } else {
        _zip_buffer_free(buffer);

        if ((buffer = _zip_buffer_new_from_source(src, variable_size, NULL, error)) == NULL) {
            return -1;
        }
    }

    if (filename_len) {
        zde->filename = _zip_read_string(buffer, src, filename_len, 1, error);
        if (!zde->filename) {
            if (zip_error_code_zip(error) == ZIP_ER_EOF) {
                zip_error_set(error, ZIP_ER_INCONS, 0);
            }
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
            return -1;
        }

        if (zde->bitflags & ZIP_GPBF_ENCODING_UTF_8) {
            if (_zip_guess_encoding(zde->filename, ZIP_ENCODING_UTF8_KNOWN) == ZIP_ENCODING_ERROR) {
                zip_error_set(error, ZIP_ER_INCONS, 0);
                if (!from_buffer) {
                    _zip_buffer_free(buffer);
                }
                return -1;
            }
        }
    }

    if (ef_len) {
        zip_uint8_t *ef = _zip_read_data(buffer, src, ef_len, 0, error);

        if (ef == NULL) {
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
            return -1;
        }
        if ((zde->extra_fields = _zip_ef_parse(ef, ef_len, local ? ZIP_EF_LOCAL : ZIP_EF_CENTRAL, error)) == NULL) {
            free(ef);
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
            return -1;
        }
        free(ef);
        if (local)
            zde->local_extra_fields_read = 1;
    }

    if (comment_len) {
        zde->comment = _zip_read_string(buffer, src, comment_len, 0, error);
        if (!zde->comment) {
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
            return -1;
        }
        if (zde->bitflags & ZIP_GPBF_ENCODING_UTF_8) {
            if (_zip_guess_encoding(zde->comment, ZIP_ENCODING_UTF8_KNOWN) == ZIP_ENCODING_ERROR) {
                zip_error_set(error, ZIP_ER_INCONS, 0);
                if (!from_buffer) {
                    _zip_buffer_free(buffer);
                }
                return -1;
            }
        }
    }

    zde->filename = _zip_dirent_process_ef_utf_8(zde, ZIP_EF_UTF_8_NAME, zde->filename);
    zde->comment  = _zip_dirent_process_ef_utf_8(zde, ZIP_EF_UTF_8_COMMENT, zde->comment);

    /* Zip64 */

    if ((zde->uncomp_size == ZIP_UINT32_MAX) || (zde->comp_size == ZIP_UINT32_MAX) || (zde->offset == ZIP_UINT32_MAX)) {
        zip_uint16_t got_len;
        zip_buffer_t *ef_buffer;
        const zip_uint8_t *ef = _zip_ef_get_by_id(zde->extra_fields, &got_len, ZIP_EF_ZIP64, 0, local ? ZIP_EF_LOCAL : ZIP_EF_CENTRAL, error);
        /* TODO: if got_len == 0 && !ZIP64_EOCD: no error, 0xffffffff is valid value */
        if (ef == NULL) {
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
            return -1;
        }

        if ((ef_buffer = _zip_buffer_new((zip_uint8_t *)ef, got_len)) == NULL) {
            zip_error_set(error, ZIP_ER_MEMORY, 0);
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
            return -1;
        }

        if (zde->uncomp_size == ZIP_UINT32_MAX)
            zde->uncomp_size = _zip_buffer_get_64(ef_buffer);
        else if (local)
            ef += 8;
        if (zde->comp_size == ZIP_UINT32_MAX)
            zde->comp_size = _zip_buffer_get_64(ef_buffer);
        if (!local) {
            if (zde->offset == ZIP_UINT32_MAX)
                zde->offset = _zip_buffer_get_64(ef_buffer);
            if (zde->disk_number == ZIP_UINT16_MAX)
                zde->disk_number = _zip_buffer_get_32(buffer);
        }

        if (!_zip_buffer_eof(ef_buffer)) {
            zip_error_set(error, ZIP_ER_INCONS, 0);
            _zip_buffer_free(ef_buffer);
            if (!from_buffer) {
                _zip_buffer_free(buffer);
            }
            return -1;
        }
        _zip_buffer_free(ef_buffer);
    }

    if (!_zip_buffer_ok(buffer)) {
        zip_error_set(error, ZIP_ER_INTERNAL, 0);
        if (!from_buffer) {
            _zip_buffer_free(buffer);
        }
        return -1;
    }
    if (!from_buffer) {
        _zip_buffer_free(buffer);
    }

    /* zip_source_seek / zip_source_tell don't support values > ZIP_INT64_MAX */
    if (zde->offset > ZIP_INT64_MAX) {
        zip_error_set(error, ZIP_ER_SEEK, EFBIG);
        return -1;
    }

    zde->extra_fields = _zip_ef_remove_internal(zde->extra_fields);

    return (zip_int64_t)(size + variable_size);
}

static zip_string_t *
_zip_dirent_process_ef_utf_8(const zip_dirent_t *de, zip_uint16_t id, zip_string_t *str) {
    zip_uint16_t ef_len;
    zip_uint32_t ef_crc;
    zip_buffer_t *buffer;

    const zip_uint8_t *ef = _zip_ef_get_by_id(de->extra_fields, &ef_len, id, 0, ZIP_EF_BOTH, NULL);

    if ((ef == NULL) || (ef_len < 5) || (ef[0] != 1)) {
        return str;
    }

    if ((buffer = _zip_buffer_new((zip_uint8_t *)ef, ef_len)) == NULL) {
        return str;
    }

    _zip_buffer_get_8(buffer);
    ef_crc = _zip_buffer_get_32(buffer);

    if (_zip_string_crc32(str) == ef_crc) {
        zip_uint16_t len     = (zip_uint16_t)_zip_buffer_left(buffer);
        zip_string_t *ef_str = _zip_string_new(_zip_buffer_get(buffer, len), len, ZIP_FL_ENC_UTF_8, NULL);

        if (ef_str != NULL) {
            _zip_string_free(str);
            str = ef_str;
        }
    }

    _zip_buffer_free(buffer);

    return str;
}

zip_int32_t
_zip_dirent_size(zip_source_t *src, zip_uint16_t flags, zip_error_t *error) {
    zip_int32_t size;
    char local = (flags & ZIP_EF_LOCAL) != 0;
    int i;
    zip_uint8_t b[6];
    zip_buffer_t *buffer;

    size = local ? LENTRYSIZE : CDENTRYSIZE;

    if (zip_source_seek(src, local ? 26 : 28, SEEK_CUR) < 0) {
        _zip_error_set_from_source(error, src);
        return -1;
    }

    if ((buffer = _zip_buffer_new_from_source(src, local ? 4 : 6, b, error)) == NULL) {
        return -1;
    }

    for (i = 0; i < (local ? 2 : 3); i++) {
        size += _zip_buffer_get_16(buffer);
    }

    if (!_zip_buffer_eof(buffer)) {
        zip_error_set(error, ZIP_ER_INTERNAL, 0);
        _zip_buffer_free(buffer);
        return -1;
    }

    _zip_buffer_free(buffer);
    return size;
}

/* _zip_dirent_write
   Writes zip directory entry.

   If flags & ZIP_EF_LOCAL, it writes a local header instead of a central
   directory entry.  If flags & ZIP_EF_FORCE_ZIP64, a ZIP64 extra field is written, even if not needed.

   Returns 0 if successful, 1 if successful and wrote ZIP64 extra field. On error, error is filled in and -1 is
   returned.
 */

int
_zip_dirent_write(zip_t *za, zip_dirent_t *de, zip_flags_t flags) {
    zip_uint16_t dostime, dosdate;
    zip_encoding_type_t com_enc, name_enc;
    zip_extra_field_t *ef;
    char is_zip64;
    char is_really_zip64;
    zip_uint8_t buf[CDENTRYSIZE];
    zip_buffer_t *buffer;

    ef = NULL;

    is_zip64 = ZIP_FALSE;

    name_enc = _zip_guess_encoding(de->filename, ZIP_ENCODING_UNKNOWN);
    com_enc  = _zip_guess_encoding(de->comment, ZIP_ENCODING_UNKNOWN);

    if (((name_enc == ZIP_ENCODING_UTF8_KNOWN) && (com_enc == ZIP_ENCODING_ASCII)) ||
        ((name_enc == ZIP_ENCODING_ASCII) && (com_enc == ZIP_ENCODING_UTF8_KNOWN)) ||
        ((name_enc == ZIP_ENCODING_UTF8_KNOWN) && (com_enc == ZIP_ENCODING_UTF8_KNOWN)))
        de->bitflags |= ZIP_GPBF_ENCODING_UTF_8;
    else {
        de->bitflags &= (zip_uint16_t) ~ZIP_GPBF_ENCODING_UTF_8;
        if (name_enc == ZIP_ENCODING_UTF8_KNOWN) {
            ef = _zip_ef_utf8(ZIP_EF_UTF_8_NAME, de->filename, &za->error);
            if (ef == NULL)
                return -1;
        }
        if (((flags & ZIP_FL_LOCAL) == 0) && (com_enc == ZIP_ENCODING_UTF8_KNOWN)) {
            zip_extra_field_t *ef2 = _zip_ef_utf8(ZIP_EF_UTF_8_COMMENT, de->comment, &za->error);
            if (ef2 == NULL) {
                _zip_ef_free(ef);
                return -1;
            }
            ef2->next = ef;
            ef        = ef2;
        }
    }

    is_really_zip64 = _zip_dirent_needs_zip64(de, flags);
    is_zip64        = (flags & (ZIP_FL_LOCAL | ZIP_FL_FORCE_ZIP64)) == (ZIP_FL_LOCAL | ZIP_FL_FORCE_ZIP64) || is_really_zip64;

    if (is_zip64) {
        zip_uint8_t ef_zip64[EFZIP64SIZE];
        zip_buffer_t *ef_buffer = _zip_buffer_new(ef_zip64, sizeof(ef_zip64));
        if (ef_buffer == NULL) {
            zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
            _zip_ef_free(ef);
            return -1;
        }

        if (flags & ZIP_FL_LOCAL) {
            if ((flags & ZIP_FL_FORCE_ZIP64) || (de->comp_size > ZIP_UINT32_MAX) || (de->uncomp_size > ZIP_UINT32_MAX)) {
                _zip_buffer_put_64(ef_buffer, de->uncomp_size);
                _zip_buffer_put_64(ef_buffer, de->comp_size);
            }
        } else {
            if ((flags & ZIP_FL_FORCE_ZIP64) || (de->comp_size > ZIP_UINT32_MAX) || (de->uncomp_size > ZIP_UINT32_MAX) || (de->offset > ZIP_UINT32_MAX)) {
                if (de->uncomp_size >= ZIP_UINT32_MAX) {
                    _zip_buffer_put_64(ef_buffer, de->uncomp_size);
                }
                if (de->comp_size >= ZIP_UINT32_MAX) {
                    _zip_buffer_put_64(ef_buffer, de->comp_size);
                }
                if (de->offset >= ZIP_UINT32_MAX) {
                    _zip_buffer_put_64(ef_buffer, de->offset);
                }
            }
        }

        if (!_zip_buffer_ok(ef_buffer)) {
            zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
            _zip_buffer_free(ef_buffer);
            _zip_ef_free(ef);
            return -1;
        }

        zip_extra_field_t *ef64 = _zip_ef_new(ZIP_EF_ZIP64, (zip_uint16_t)(_zip_buffer_offset(ef_buffer)), ef_zip64, ZIP_EF_BOTH);
        _zip_buffer_free(ef_buffer);
        ef64->next = ef;
        ef         = ef64;
    }

    if ((buffer = _zip_buffer_new(buf, sizeof(buf))) == NULL) {
        zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
        _zip_ef_free(ef);
        return -1;
    }

    _zip_buffer_put(buffer, (flags & ZIP_FL_LOCAL) ? LOCAL_MAGIC : CENTRAL_MAGIC, 4);

    if ((flags & ZIP_FL_LOCAL) == 0) {
        _zip_buffer_put_16(buffer, (zip_uint16_t)(is_really_zip64 ? 45 : de->version_madeby));
    }
    _zip_buffer_put_16(buffer, (zip_uint16_t)(is_really_zip64 ? 45 : de->version_needed));
    _zip_buffer_put_16(buffer, de->bitflags & 0xfff9); /* clear compression method specific flags */
    _zip_buffer_put_16(buffer, (zip_uint16_t)de->comp_method);

    _zip_u2d_time(de->last_mod, &dostime, &dosdate);
    _zip_buffer_put_16(buffer, dostime);
    _zip_buffer_put_16(buffer, dosdate);

    _zip_buffer_put_32(buffer, de->crc);

    if (((flags & ZIP_FL_LOCAL) == ZIP_FL_LOCAL) && ((de->comp_size >= ZIP_UINT32_MAX) || (de->uncomp_size >= ZIP_UINT32_MAX))) {
        /* In local headers, if a ZIP64 EF is written, it MUST contain
         * both compressed and uncompressed sizes (even if one of the
         * two is smaller than 0xFFFFFFFF); on the other hand, those
         * may only appear when the corresponding standard entry is
         * 0xFFFFFFFF.  (appnote.txt 4.5.3) */
        _zip_buffer_put_32(buffer, ZIP_UINT32_MAX);
        _zip_buffer_put_32(buffer, ZIP_UINT32_MAX);
    } else {
        if (de->comp_size < ZIP_UINT32_MAX) {
            _zip_buffer_put_32(buffer, (zip_uint32_t)de->comp_size);
        } else {
            _zip_buffer_put_32(buffer, ZIP_UINT32_MAX);
        }
        if (de->uncomp_size < ZIP_UINT32_MAX) {
            _zip_buffer_put_32(buffer, (zip_uint32_t)de->uncomp_size);
        } else {
            _zip_buffer_put_32(buffer, ZIP_UINT32_MAX);
        }
    }

    _zip_buffer_put_16(buffer, _zip_string_length(de->filename));
    /* TODO: check for overflow */
    zip_uint32_t ef_total_size = (zip_uint32_t)_zip_ef_size(de->extra_fields, flags) + (zip_uint32_t)_zip_ef_size(ef, ZIP_EF_BOTH);
    _zip_buffer_put_16(buffer, (zip_uint16_t)ef_total_size);

    if ((flags & ZIP_FL_LOCAL) == 0) {
        _zip_buffer_put_16(buffer, _zip_string_length(de->comment));
        _zip_buffer_put_16(buffer, (zip_uint16_t)de->disk_number);
        _zip_buffer_put_16(buffer, de->int_attrib);
        _zip_buffer_put_32(buffer, de->ext_attrib);
        if (de->offset < ZIP_UINT32_MAX)
            _zip_buffer_put_32(buffer, (zip_uint32_t)de->offset);
        else
            _zip_buffer_put_32(buffer, ZIP_UINT32_MAX);
    }

    if (!_zip_buffer_ok(buffer)) {
        zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
        _zip_buffer_free(buffer);
        _zip_ef_free(ef);
        return -1;
    }

    if (_zip_write(za, buf, _zip_buffer_offset(buffer)) < 0) {
        _zip_buffer_free(buffer);
        _zip_ef_free(ef);
        return -1;
    }

    _zip_buffer_free(buffer);

    if (de->filename) {
        if (_zip_string_write(za, de->filename) < 0) {
            _zip_ef_free(ef);
            return -1;
        }
    }

    if (ef) {
        if (_zip_ef_write(za, ef, ZIP_EF_BOTH) < 0) {
            _zip_ef_free(ef);
            return -1;
        }
    }
    _zip_ef_free(ef);
    if (de->extra_fields) {
        if (_zip_ef_write(za, de->extra_fields, flags) < 0) {
            return -1;
        }
    }

    if ((flags & ZIP_FL_LOCAL) == 0) {
        if (de->comment) {
            if (_zip_string_write(za, de->comment) < 0) {
                return -1;
            }
        }
    }


    return is_zip64;
}

static time_t
_zip_d2u_time(zip_uint16_t dtime, zip_uint16_t ddate) {
    struct tm tm;

    memset(&tm, 0, sizeof(tm));

    /* let mktime decide if DST is in effect */
    tm.tm_isdst = -1;

    tm.tm_year = ((ddate >> 9) & 127) + 1980 - 1900;
    tm.tm_mon  = ((ddate >> 5) & 15) - 1;
    tm.tm_mday = ddate & 31;

    tm.tm_hour = (dtime >> 11) & 31;
    tm.tm_min  = (dtime >> 5) & 63;
    tm.tm_sec  = (dtime << 1) & 62;

    return mktime(&tm);
}

static zip_extra_field_t *
_zip_ef_utf8(zip_uint16_t id, zip_string_t *str, zip_error_t *error) {
    const zip_uint8_t *raw;
    zip_uint32_t len;
    zip_buffer_t *buffer;
    zip_extra_field_t *ef;

    raw = _zip_string_get(str, &len, ZIP_FL_ENC_RAW, NULL);

    if (len + 5 > ZIP_UINT16_MAX) {
        zip_error_set(error, ZIP_ER_INVAL, 0); /* TODO: better error code? */
        return NULL;
    }

    if ((buffer = _zip_buffer_new(NULL, len + 5)) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    _zip_buffer_put_8(buffer, 1);
    _zip_buffer_put_32(buffer, _zip_string_crc32(str));
    _zip_buffer_put(buffer, raw, len);

    if (!_zip_buffer_ok(buffer)) {
        zip_error_set(error, ZIP_ER_INTERNAL, 0);
        _zip_buffer_free(buffer);
        return NULL;
    }

    ef = _zip_ef_new(id, (zip_uint16_t)(_zip_buffer_offset(buffer)), _zip_buffer_data(buffer), ZIP_EF_BOTH);
    _zip_buffer_free(buffer);

    return ef;
}

zip_dirent_t *
_zip_get_dirent(zip_t *za, zip_uint64_t idx, zip_flags_t flags, zip_error_t *error) {
    if (error == NULL)
        error = &za->error;

    if (idx >= za->nentry) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    if ((flags & ZIP_FL_UNCHANGED) || (za->entry[idx].changes == NULL)) {
        if (za->entry[idx].orig == NULL) {
            zip_error_set(error, ZIP_ER_INVAL, 0);
            return NULL;
        }
        if (za->entry[idx].deleted && ((flags & ZIP_FL_UNCHANGED) == 0)) {
            zip_error_set(error, ZIP_ER_DELETED, 0);
            return NULL;
        }
        return za->entry[idx].orig;
    } else
        return za->entry[idx].changes;
}

void
_zip_u2d_time(time_t intime, zip_uint16_t *dtime, zip_uint16_t *ddate) {
    struct tm *tm;

    tm     = localtime(&intime);
    *ddate = (zip_uint16_t)(((tm->tm_year + 1900 - 1980) << 9) + ((tm->tm_mon + 1) << 5) + tm->tm_mday);
    *dtime = (zip_uint16_t)(((tm->tm_hour) << 11) + ((tm->tm_min) << 5) + ((tm->tm_sec) >> 1));

    return;
}

/*
   zip_discard.c -- discard and free struct zip
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>



/* zip_discard:
   frees the space allocated to a zipfile struct, and closes the
   corresponding file. */

void
zip_discard(zip_t *za) {
    zip_uint64_t i;

    if (za == NULL)
        return;

    if (za->src) {
        zip_source_close(za->src);
        zip_source_free(za->src);
    }

    free(za->default_password);
    _zip_string_free(za->comment_orig);
    _zip_string_free(za->comment_changes);

    if (za->entry) {
        for (i = 0; i < za->nentry; i++)
            _zip_entry_finalize(za->entry + i);
        free(za->entry);
    }

    for (i = 0; i < za->nopen_source; i++) {
        _zip_source_invalidate(za->open_source[i]);
    }
    free(za->open_source);

    zip_error_fini(&za->error);

    free(za);

    return;
}

/*
   zip_entry.c -- struct zip_entry helper functions
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



void
_zip_entry_finalize(zip_entry_t *e) {
    _zip_unchange_data(e);
    _zip_dirent_free(e->orig);
    _zip_dirent_free(e->changes);
}

void
_zip_entry_init(zip_entry_t *e) {
    e->orig    = NULL;
    e->changes = NULL;
    e->source  = NULL;
    e->deleted = 0;
}

/*
   This file was generated automatically by ./make_zip_err_str.sh
   from ./zip.h; make changes there.
 */



const char *const _zip_err_str[] = {
    "No error",
    "Multi-disk zip archives not supported",
    "Renaming temporary file failed",
    "Closing zip archive failed",
    "Seek error",
    "Read error",
    "Write error",
    "CRC error",
    "Containing zip archive was closed",
    "No such file",
    "File already exists",
    "Can't open file",
    "Failure to create temporary file",
    "Zlib error",
    "Malloc failure",
    "Entry has been changed",
    "Compression method not supported",
    "Premature end of file",
    "Invalid argument",
    "Not a zip archive",
    "Internal error",
    "Zip archive inconsistent",
    "Can't remove file",
    "Entry has been deleted",
    "Encryption method not supported",
    "Read-only archive",
    "No password provided",
    "Wrong password provided",
    "Operation not supported",
    "Resource still in use",
    "Tell error",
};

const int _zip_nerr_str = sizeof(_zip_err_str) / sizeof(_zip_err_str[0]);

#define N    ZIP_ET_NONE
#define S    ZIP_ET_SYS
#define Z    ZIP_ET_ZLIB

const int _zip_err_type[] = {
    N,
    N,
    S,
    S,
    S,
    S,
    S,
    N,
    N,
    N,
    N,
    S,
    S,
    Z,
    N,
    N,
    N,
    N,
    N,
    N,
    N,
    N,
    S,
    N,
    N,
    N,
    N,
    N,
    N,
    N,
    S,
};
#undef N
#undef S
#undef Z

/*
   zip_error.c -- zip_error_t helper functions
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <errno.h>
#include <stdlib.h>



ZIP_EXTERN int
zip_error_code_system(const zip_error_t *error) {
    return error->sys_err;
}

ZIP_EXTERN int
zip_error_code_zip(const zip_error_t *error) {
    return error->zip_err;
}

ZIP_EXTERN void
zip_error_fini(zip_error_t *err) {
    free(err->str);
    err->str = NULL;
}

ZIP_EXTERN void
zip_error_init(zip_error_t *err) {
    err->zip_err = ZIP_ER_OK;
    err->sys_err = 0;
    err->str     = NULL;
}

ZIP_EXTERN void
zip_error_init_with_code(zip_error_t *error, int ze) {
    zip_error_init(error);
    error->zip_err = ze;
    switch (zip_error_system_type(error)) {
        case ZIP_ET_SYS:
            error->sys_err = errno;
            break;

        default:
            error->sys_err = 0;
            break;
    }
}

ZIP_EXTERN int
zip_error_system_type(const zip_error_t *error) {
    if ((error->zip_err < 0) || (error->zip_err >= _zip_nerr_str))
        return ZIP_ET_NONE;

    return _zip_err_type[error->zip_err];
}

void
_zip_error_clear(zip_error_t *err) {
    if (err == NULL)
        return;

    err->zip_err = ZIP_ER_OK;
    err->sys_err = 0;
}

void
_zip_error_copy(zip_error_t *dst, const zip_error_t *src) {
    dst->zip_err = src->zip_err;
    dst->sys_err = src->sys_err;
}

void
_zip_error_get(const zip_error_t *err, int *zep, int *sep) {
    if (zep)
        *zep = err->zip_err;
    if (sep) {
        if (zip_error_system_type(err) != ZIP_ET_NONE)
            *sep = err->sys_err;
        else
            *sep = 0;
    }
}

void
zip_error_set(zip_error_t *err, int ze, int se) {
    if (err) {
        err->zip_err = ze;
        err->sys_err = se;
    }
}

void
_zip_error_set_from_source(zip_error_t *err, zip_source_t *src) {
    _zip_error_copy(err, zip_source_error(src));
}

zip_int64_t
zip_error_to_data(const zip_error_t *error, void *data, zip_uint64_t length) {
    int *e = (int *)data;

    if (length < sizeof(int) * 2) {
        return -1;
    }

    e[0] = zip_error_code_zip(error);
    e[1] = zip_error_code_system(error);
    return sizeof(int) * 2;
}

/*
   zip_error_clear.c -- clear zip error
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN void
zip_error_clear(zip_t *za) {
    if (za == NULL)
        return;

    _zip_error_clear(&za->error);
}

/*
   zip_error_get.c -- get zip error
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#define _ZIP_COMPILING_DEPRECATED



ZIP_EXTERN void
zip_error_get(zip_t *za, int *zep, int *sep) {
    _zip_error_get(&za->error, zep, sep);
}

ZIP_EXTERN zip_error_t *
zip_get_error(zip_t *za) {
    return &za->error;
}

ZIP_EXTERN zip_error_t *
zip_file_get_error(zip_file_t *f) {
    return &f->error;
}

/*
   zip_error_get_sys_type.c -- return type of system error code
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _ZIP_COMPILING_DEPRECATED



ZIP_EXTERN int
zip_error_get_sys_type(int ze) {
    if ((ze < 0) || (ze >= _zip_nerr_str))
        return 0;

    return _zip_err_type[ze];
}

/*
   zip_error_sterror.c -- get string representation of struct zip_error
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



ZIP_EXTERN const char *
zip_error_strerror(zip_error_t *err) {
    const char *zs, *ss;
    char buf[128], *s;

    zip_error_fini(err);

    if ((err->zip_err < 0) || (err->zip_err >= _zip_nerr_str)) {
        sprintf(buf, "Unknown error %d", err->zip_err);
        zs = NULL;
        ss = buf;
    } else {
        zs = _zip_err_str[err->zip_err];

        switch (_zip_err_type[err->zip_err]) {
            case ZIP_ET_SYS:
                ss = strerror(err->sys_err);
                break;

            case ZIP_ET_ZLIB:
                ss = zError(err->sys_err);
                break;

            default:
                ss = NULL;
        }
    }

    if (ss == NULL)
        return zs;
    else {
        if ((s = (char *)malloc(strlen(ss)
                                + (zs ? strlen(zs) + 2 : 0) + 1)) == NULL)
            return _zip_err_str[ZIP_ER_MEMORY];

        sprintf(s, "%s%s%s",
                (zs ? zs : ""),
                (zs ? ": " : ""),
                ss);
        err->str = s;

        return s;
    }
}

/*
   zip_error_to_str.c -- get string representation of zip error code
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _ZIP_COMPILING_DEPRECATED



ZIP_EXTERN int
zip_error_to_str(char *buf, zip_uint64_t len, int ze, int se) {
    const char *zs, *ss;

    if ((ze < 0) || (ze >= _zip_nerr_str))
        return snprintf(buf, len, "Unknown error %d", ze);

    zs = _zip_err_str[ze];

    switch (_zip_err_type[ze]) {
        case ZIP_ET_SYS:
            ss = strerror(se);
            break;

        case ZIP_ET_ZLIB:
            ss = zError(se);
            break;

        default:
            ss = NULL;
    }

    return snprintf(buf, len, "%s%s%s",
                    zs, (ss ? ": " : ""), (ss ? ss : ""));
}

/*
   zip_extra_field.c -- manipulate extra fields
   Copyright (C) 2012-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



#include <errno.h>
#include <stdlib.h>
#include <string.h>


zip_extra_field_t *
_zip_ef_clone(const zip_extra_field_t *ef, zip_error_t *error) {
    zip_extra_field_t *head, *prev, *def;

    head = prev = NULL;

    while (ef) {
        if ((def = _zip_ef_new(ef->id, ef->size, ef->data, ef->flags)) == NULL) {
            zip_error_set(error, ZIP_ER_MEMORY, 0);
            _zip_ef_free(head);
            return NULL;
        }

        if (head == NULL)
            head = def;
        if (prev)
            prev->next = def;
        prev = def;

        ef = ef->next;
    }

    return head;
}

zip_extra_field_t *
_zip_ef_delete_by_id(zip_extra_field_t *ef, zip_uint16_t id, zip_uint16_t id_idx, zip_flags_t flags) {
    zip_extra_field_t *head, *prev;
    int i;

    i    = 0;
    head = ef;
    prev = NULL;
    for ( ; ef; ef = (prev ? prev->next : head)) {
        if ((ef->flags & flags & ZIP_EF_BOTH) && ((ef->id == id) || (id == ZIP_EXTRA_FIELD_ALL))) {
            if ((id_idx == ZIP_EXTRA_FIELD_ALL) || (i == id_idx)) {
                ef->flags &= ~(flags & ZIP_EF_BOTH);
                if ((ef->flags & ZIP_EF_BOTH) == 0) {
                    if (prev)
                        prev->next = ef->next;
                    else
                        head = ef->next;
                    ef->next = NULL;
                    _zip_ef_free(ef);

                    if (id_idx == ZIP_EXTRA_FIELD_ALL)
                        continue;
                }
            }

            i++;
            if (i > id_idx)
                break;
        }
        prev = ef;
    }

    return head;
}

void
_zip_ef_free(zip_extra_field_t *ef) {
    zip_extra_field_t *ef2;

    while (ef) {
        ef2 = ef->next;
        free(ef->data);
        free(ef);
        ef = ef2;
    }
}

const zip_uint8_t *
_zip_ef_get_by_id(const zip_extra_field_t *ef, zip_uint16_t *lenp, zip_uint16_t id, zip_uint16_t id_idx, zip_flags_t flags, zip_error_t *error) {
    static const zip_uint8_t empty[1] = { '\0' };

    int i;

    i = 0;
    for ( ; ef; ef = ef->next) {
        if ((ef->id == id) && (ef->flags & flags & ZIP_EF_BOTH)) {
            if (i < id_idx) {
                i++;
                continue;
            }

            if (lenp)
                *lenp = ef->size;
            if (ef->size > 0)
                return ef->data;
            else
                return empty;
        }
    }

    zip_error_set(error, ZIP_ER_NOENT, 0);
    return NULL;
}

zip_extra_field_t *
_zip_ef_merge(zip_extra_field_t *to, zip_extra_field_t *from) {
    zip_extra_field_t *ef2, *tt, *tail;
    int duplicate;

    if (to == NULL)
        return from;

    for (tail = to; tail->next; tail = tail->next)
        ;

    for ( ; from; from = ef2) {
        ef2 = from->next;

        duplicate = 0;
        for (tt = to; tt; tt = tt->next) {
            if ((tt->id == from->id) && (tt->size == from->size) && (memcmp(tt->data, from->data, tt->size) == 0)) {
                tt->flags |= (from->flags & ZIP_EF_BOTH);
                duplicate  = 1;
                break;
            }
        }

        from->next = NULL;
        if (duplicate)
            _zip_ef_free(from);
        else
            tail = tail->next = from;
    }

    return to;
}

zip_extra_field_t *
_zip_ef_new(zip_uint16_t id, zip_uint16_t size, const zip_uint8_t *data, zip_flags_t flags) {
    zip_extra_field_t *ef;

    if ((ef = (zip_extra_field_t *)malloc(sizeof(*ef))) == NULL)
        return NULL;

    ef->next  = NULL;
    ef->flags = flags;
    ef->id    = id;
    ef->size  = size;
    if (size > 0) {
        if ((ef->data = (zip_uint8_t *)_zip_memdup(data, size, NULL)) == NULL) {
            free(ef);
            return NULL;
        }
    } else
        ef->data = NULL;

    return ef;
}

zip_extra_field_t *
_zip_ef_parse(const zip_uint8_t *data, zip_uint16_t len, zip_flags_t flags, zip_error_t *error) {
    zip_buffer_t *buffer;
    zip_extra_field_t *ef, *ef2, *ef_head;

    if ((buffer = _zip_buffer_new((zip_uint8_t *)data, len)) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    ef_head = ef = NULL;

    while (_zip_buffer_ok(buffer) && !_zip_buffer_eof(buffer)) {
        zip_uint16_t fid, flen;
        zip_uint8_t *ef_data;

        fid     = _zip_buffer_get_16(buffer);
        flen    = _zip_buffer_get_16(buffer);
        ef_data = _zip_buffer_get(buffer, flen);

        if (ef_data == NULL) {
            break;
        }

        if ((ef2 = _zip_ef_new(fid, flen, ef_data, flags)) == NULL) {
            zip_error_set(error, ZIP_ER_MEMORY, 0);
            _zip_buffer_free(buffer);
            _zip_ef_free(ef_head);
            return NULL;
        }

        if (ef_head) {
            ef->next = ef2;
            ef       = ef2;
        } else
            ef_head = ef = ef2;
    }

    if (!_zip_buffer_eof(buffer)) {
        zip_error_set(error, ZIP_ER_INCONS, 0);
        _zip_buffer_free(buffer);
        _zip_ef_free(ef_head);
        return NULL;
    }

    _zip_buffer_free(buffer);

    return ef_head;
}

zip_extra_field_t *
_zip_ef_remove_internal(zip_extra_field_t *ef) {
    zip_extra_field_t *ef_head;
    zip_extra_field_t *prev, *next;

    ef_head = ef;
    prev    = NULL;

    while (ef) {
        if (ZIP_EF_IS_INTERNAL(ef->id)) {
            next = ef->next;
            if (ef_head == ef)
                ef_head = next;
            ef->next = NULL;
            _zip_ef_free(ef);
            if (prev)
                prev->next = next;
            ef = next;
        } else {
            prev = ef;
            ef   = ef->next;
        }
    }

    return ef_head;
}

zip_uint16_t
_zip_ef_size(const zip_extra_field_t *ef, zip_flags_t flags) {
    zip_uint16_t size;

    size = 0;
    for ( ; ef; ef = ef->next) {
        if (ef->flags & flags & ZIP_EF_BOTH)
            size = (zip_uint16_t)(size + 4 + ef->size);
    }

    return size;
}

int
_zip_ef_write(zip_t *za, const zip_extra_field_t *ef, zip_flags_t flags) {
    zip_uint8_t b[4];
    zip_buffer_t *buffer = _zip_buffer_new(b, sizeof(b));

    if (buffer == NULL) {
        return -1;
    }

    for ( ; ef; ef = ef->next) {
        if (ef->flags & flags & ZIP_EF_BOTH) {
            _zip_buffer_set_offset(buffer, 0);
            _zip_buffer_put_16(buffer, ef->id);
            _zip_buffer_put_16(buffer, ef->size);
            if (!_zip_buffer_ok(buffer)) {
                zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
                _zip_buffer_free(buffer);
                return -1;
            }
            if (_zip_write(za, b, 4) < 0) {
                _zip_buffer_free(buffer);
                return -1;
            }
            if (ef->size > 0) {
                if (_zip_write(za, ef->data, ef->size) < 0) {
                    _zip_buffer_free(buffer);
                    return -1;
                }
            }
        }
    }

    _zip_buffer_free(buffer);
    return 0;
}

int
_zip_read_local_ef(zip_t *za, zip_uint64_t idx) {
    zip_entry_t *e;
    unsigned char b[4];
    zip_buffer_t *buffer;
    zip_uint16_t fname_len, ef_len;

    if (idx >= za->nentry) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    e = za->entry + idx;

    if ((e->orig == NULL) || e->orig->local_extra_fields_read)
        return 0;

    if (e->orig->offset + 26 > ZIP_INT64_MAX) {
        zip_error_set(&za->error, ZIP_ER_SEEK, EFBIG);
        return -1;
    }

    if (zip_source_seek(za->src, (zip_int64_t)(e->orig->offset + 26), SEEK_SET) < 0) {
        _zip_error_set_from_source(&za->error, za->src);
        return -1;
    }

    if ((buffer = _zip_buffer_new_from_source(za->src, sizeof(b), b, &za->error)) == NULL) {
        return -1;
    }

    fname_len = _zip_buffer_get_16(buffer);
    ef_len    = _zip_buffer_get_16(buffer);

    if (!_zip_buffer_eof(buffer)) {
        _zip_buffer_free(buffer);
        zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
        return -1;
    }

    _zip_buffer_free(buffer);

    if (ef_len > 0) {
        zip_extra_field_t *ef;
        zip_uint8_t *ef_raw;

        if (zip_source_seek(za->src, fname_len, SEEK_CUR) < 0) {
            zip_error_set(&za->error, ZIP_ER_SEEK, errno);
            return -1;
        }

        ef_raw = _zip_read_data(NULL, za->src, ef_len, 0, &za->error);

        if (ef_raw == NULL)
            return -1;

        if ((ef = _zip_ef_parse(ef_raw, ef_len, ZIP_EF_LOCAL, &za->error)) == NULL) {
            free(ef_raw);
            return -1;
        }
        free(ef_raw);

        ef = _zip_ef_remove_internal(ef);
        e->orig->extra_fields = _zip_ef_merge(e->orig->extra_fields, ef);
    }

    e->orig->local_extra_fields_read = 1;

    if (e->changes && (e->changes->local_extra_fields_read == 0)) {
        e->changes->extra_fields            = e->orig->extra_fields;
        e->changes->local_extra_fields_read = 1;
    }

    return 0;
}

/*
   zip_extra_field_api.c -- public extra fields API functions
   Copyright (C) 2012-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_file_extra_field_delete(zip_t *za, zip_uint64_t idx, zip_uint16_t ef_idx, zip_flags_t flags) {
    zip_dirent_t *de;

    if ((flags & ZIP_EF_BOTH) == 0) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (((flags & ZIP_EF_BOTH) == ZIP_EF_BOTH) && (ef_idx != ZIP_EXTRA_FIELD_ALL)) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (_zip_get_dirent(za, idx, 0, NULL) == NULL)
        return -1;

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    if (_zip_file_extra_field_prepare_for_change(za, idx) < 0)
        return -1;

    de = za->entry[idx].changes;

    de->extra_fields = _zip_ef_delete_by_id(de->extra_fields, ZIP_EXTRA_FIELD_ALL, ef_idx, flags);
    return 0;
}

ZIP_EXTERN int
zip_file_extra_field_delete_by_id(zip_t *za, zip_uint64_t idx, zip_uint16_t ef_id, zip_uint16_t ef_idx, zip_flags_t flags) {
    zip_dirent_t *de;

    if ((flags & ZIP_EF_BOTH) == 0) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (((flags & ZIP_EF_BOTH) == ZIP_EF_BOTH) && (ef_idx != ZIP_EXTRA_FIELD_ALL)) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (_zip_get_dirent(za, idx, 0, NULL) == NULL)
        return -1;

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    if (_zip_file_extra_field_prepare_for_change(za, idx) < 0)
        return -1;

    de = za->entry[idx].changes;

    de->extra_fields = _zip_ef_delete_by_id(de->extra_fields, ef_id, ef_idx, flags);
    return 0;
}

ZIP_EXTERN const zip_uint8_t *
zip_file_extra_field_get(zip_t *za, zip_uint64_t idx, zip_uint16_t ef_idx, zip_uint16_t *idp, zip_uint16_t *lenp, zip_flags_t flags) {
    static const zip_uint8_t empty[1] = { '\0' };

    zip_dirent_t *de;
    zip_extra_field_t *ef;
    int i;

    if ((flags & ZIP_EF_BOTH) == 0) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    if ((de = _zip_get_dirent(za, idx, flags, &za->error)) == NULL)
        return NULL;

    if (flags & ZIP_FL_LOCAL)
        if (_zip_read_local_ef(za, idx) < 0)
            return NULL;

    i = 0;
    for (ef = de->extra_fields; ef; ef = ef->next) {
        if (ef->flags & flags & ZIP_EF_BOTH) {
            if (i < ef_idx) {
                i++;
                continue;
            }

            if (idp)
                *idp = ef->id;
            if (lenp)
                *lenp = ef->size;
            if (ef->size > 0)
                return ef->data;
            else
                return empty;
        }
    }

    zip_error_set(&za->error, ZIP_ER_NOENT, 0);
    return NULL;
}

ZIP_EXTERN const zip_uint8_t *
zip_file_extra_field_get_by_id(zip_t *za, zip_uint64_t idx, zip_uint16_t ef_id, zip_uint16_t ef_idx, zip_uint16_t *lenp, zip_flags_t flags) {
    zip_dirent_t *de;

    if ((flags & ZIP_EF_BOTH) == 0) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    if ((de = _zip_get_dirent(za, idx, flags, &za->error)) == NULL)
        return NULL;

    if (flags & ZIP_FL_LOCAL)
        if (_zip_read_local_ef(za, idx) < 0)
            return NULL;

    return _zip_ef_get_by_id(de->extra_fields, lenp, ef_id, ef_idx, flags, &za->error);
}

ZIP_EXTERN zip_int16_t
zip_file_extra_fields_count(zip_t *za, zip_uint64_t idx, zip_flags_t flags) {
    zip_dirent_t *de;
    zip_extra_field_t *ef;
    zip_uint16_t n;

    if ((flags & ZIP_EF_BOTH) == 0) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if ((de = _zip_get_dirent(za, idx, flags, &za->error)) == NULL)
        return -1;

    if (flags & ZIP_FL_LOCAL)
        if (_zip_read_local_ef(za, idx) < 0)
            return -1;

    n = 0;
    for (ef = de->extra_fields; ef; ef = ef->next)
        if (ef->flags & flags & ZIP_EF_BOTH)
            n++;

    return (zip_int16_t)n;
}

ZIP_EXTERN zip_int16_t
zip_file_extra_fields_count_by_id(zip_t *za, zip_uint64_t idx, zip_uint16_t ef_id, zip_flags_t flags) {
    zip_dirent_t *de;
    zip_extra_field_t *ef;
    zip_uint16_t n;

    if ((flags & ZIP_EF_BOTH) == 0) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if ((de = _zip_get_dirent(za, idx, flags, &za->error)) == NULL)
        return -1;

    if (flags & ZIP_FL_LOCAL)
        if (_zip_read_local_ef(za, idx) < 0)
            return -1;

    n = 0;
    for (ef = de->extra_fields; ef; ef = ef->next)
        if ((ef->id == ef_id) && (ef->flags & flags & ZIP_EF_BOTH))
            n++;

    return (zip_int16_t)n;
}

ZIP_EXTERN int
zip_file_extra_field_set(zip_t *za, zip_uint64_t idx, zip_uint16_t ef_id, zip_uint16_t ef_idx, const zip_uint8_t *data, zip_uint16_t len, zip_flags_t flags) {
    zip_dirent_t *de;
    zip_uint16_t ls, cs;
    zip_extra_field_t *ef, *ef_prev, *ef_new;
    int i, found, new_len;

    if ((flags & ZIP_EF_BOTH) == 0) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (_zip_get_dirent(za, idx, 0, NULL) == NULL)
        return -1;

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    if (ZIP_EF_IS_INTERNAL(ef_id)) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (_zip_file_extra_field_prepare_for_change(za, idx) < 0)
        return -1;

    de = za->entry[idx].changes;

    ef      = de->extra_fields;
    ef_prev = NULL;
    i       = 0;
    found   = 0;

    for ( ; ef; ef = ef->next) {
        if ((ef->id == ef_id) && (ef->flags & flags & ZIP_EF_BOTH)) {
            if (i == ef_idx) {
                found = 1;
                break;
            }
            i++;
        }
        ef_prev = ef;
    }

    if ((i < ef_idx) && (ef_idx != ZIP_EXTRA_FIELD_NEW)) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (flags & ZIP_EF_LOCAL)
        ls = _zip_ef_size(de->extra_fields, ZIP_EF_LOCAL);
    else
        ls = 0;
    if (flags & ZIP_EF_CENTRAL)
        cs = _zip_ef_size(de->extra_fields, ZIP_EF_CENTRAL);
    else
        cs = 0;

    new_len = ls > cs ? ls : cs;
    if (found)
        new_len -= ef->size + 4;
    new_len += len + 4;

    if (new_len > ZIP_UINT16_MAX) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if ((ef_new = _zip_ef_new(ef_id, len, data, flags)) == NULL) {
        zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
        return -1;
    }

    if (found) {
        if ((ef->flags & ZIP_EF_BOTH) == (flags & ZIP_EF_BOTH)) {
            ef_new->next = ef->next;
            ef->next     = NULL;
            _zip_ef_free(ef);
            if (ef_prev)
                ef_prev->next = ef_new;
            else
                de->extra_fields = ef_new;
        } else {
            ef->flags   &= ~(flags & ZIP_EF_BOTH);
            ef_new->next = ef->next;
            ef->next     = ef_new;
        }
    } else if (ef_prev) {
        ef_new->next  = ef_prev->next;
        ef_prev->next = ef_new;
    } else
        de->extra_fields = ef_new;

    return 0;
}

int
_zip_file_extra_field_prepare_for_change(zip_t *za, zip_uint64_t idx) {
    zip_entry_t *e;

    if (idx >= za->nentry) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    e = za->entry + idx;

    if (e->changes && (e->changes->changed & ZIP_DIRENT_EXTRA_FIELD))
        return 0;

    if (e->orig) {
        if (_zip_read_local_ef(za, idx) < 0)
            return -1;
    }

    if (e->changes == NULL) {
        if ((e->changes = _zip_dirent_clone(e->orig)) == NULL) {
            zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
            return -1;
        }
    }

    if (e->orig && e->orig->extra_fields) {
        if ((e->changes->extra_fields = _zip_ef_clone(e->orig->extra_fields, &za->error)) == NULL)
            return -1;
    }
    e->changes->changed |= ZIP_DIRENT_EXTRA_FIELD;

    return 0;
}

/*
   zip_fclose.c -- close file in zip archive
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>



ZIP_EXTERN int
zip_fclose(zip_file_t *zf) {
    int ret;

    if (zf->src)
        zip_source_free(zf->src);

    ret = 0;
    if (zf->error.zip_err)
        ret = zf->error.zip_err;

    zip_error_fini(&zf->error);
    free(zf);
    return ret;
}

/*
   zip_fdopen.c -- open read-only archive from file descriptor
   Copyright (C) 2009-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN zip_t *
zip_fdopen(int fd_orig, int _flags, int *zep) {
    int fd;
    FILE *fp;
    zip_t *za;
    zip_source_t *src;
    struct zip_error error;

    if ((_flags < 0) || (_flags & ZIP_TRUNCATE)) {
        _zip_set_open_error(zep, NULL, ZIP_ER_INVAL);
        return NULL;
    }

    /* We dup() here to avoid messing with the passed in fd.
       We could not restore it to the original state in case of error. */

    if ((fd = dup(fd_orig)) < 0) {
        _zip_set_open_error(zep, NULL, ZIP_ER_OPEN);
        return NULL;
    }

    if ((fp = fdopen(fd, "rb")) == NULL) {
        close(fd);
        _zip_set_open_error(zep, NULL, ZIP_ER_OPEN);
        return NULL;
    }

    zip_error_init(&error);
    if ((src = zip_source_filep_create(fp, 0, -1, &error)) == NULL) {
        _zip_set_open_error(zep, &error, 0);
        zip_error_fini(&error);
        return NULL;
    }

    if ((za = zip_open_from_source(src, _flags, &error)) == NULL) {
        _zip_set_open_error(zep, &error, 0);
        zip_error_fini(&error);
        return NULL;
    }

    zip_error_fini(&error);
    close(fd_orig);
    return za;
}

/*
   zip_file_add.c -- add file via callback function
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



/*
   NOTE: Return type is signed so we can return -1 on error.
        The index can not be larger than ZIP_INT64_MAX since the size
        of the central directory cannot be larger than
        ZIP_UINT64_MAX, and each entry is larger than 2 bytes.
 */

ZIP_EXTERN zip_int64_t
zip_file_add(zip_t *za, const char *name, zip_source_t *source, zip_flags_t flags) {
    if ((name == NULL) || (source == NULL)) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    return _zip_file_replace(za, ZIP_UINT64_MAX, name, source, flags);
}

/*
   zip_file_error_clear.c -- clear zip file error
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN void
zip_file_error_clear(zip_file_t *zf) {
    if (zf == NULL)
        return;

    _zip_error_clear(&zf->error);
}

/*
   zip_file_error_get.c -- get zip file error
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _ZIP_COMPILING_DEPRECATED



ZIP_EXTERN void
zip_file_error_get(zip_file_t *zf, int *zep, int *sep) {
    _zip_error_get(&zf->error, zep, sep);
}

/*
   zip_file_get_comment.c -- get file comment
   Copyright (C) 2006-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



/* lenp is 32 bit because converted comment can be longer than ZIP_UINT16_MAX */

ZIP_EXTERN const char *
zip_file_get_comment(zip_t *za, zip_uint64_t idx, zip_uint32_t *lenp, zip_flags_t flags) {
    zip_dirent_t *de;
    zip_uint32_t len;
    const zip_uint8_t *str;

    if ((de = _zip_get_dirent(za, idx, flags, NULL)) == NULL)
        return NULL;

    if ((str = _zip_string_get(de->comment, &len, flags, &za->error)) == NULL)
        return NULL;

    if (lenp)
        *lenp = len;

    return (const char *)str;
}

/*
   zip_file_get_external_attributes.c -- get opsys/external attributes
   Copyright (C) 2013-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



int
zip_file_get_external_attributes(zip_t *za, zip_uint64_t idx, zip_flags_t flags, zip_uint8_t *opsys, zip_uint32_t *attributes) {
    zip_dirent_t *de;

    if ((de = _zip_get_dirent(za, idx, flags, NULL)) == NULL)
        return -1;

    if (opsys)
        *opsys = (zip_uint8_t)((de->version_madeby >> 8) & 0xff);

    if (attributes)
        *attributes = de->ext_attrib;

    return 0;
}

/*
   zip_file_get_offset.c -- get offset of file data in archive.
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>



/* _zip_file_get_offset(za, ze):
   Returns the offset of the file data for entry ze.

   On error, fills in za->error and returns 0.
 */

zip_uint64_t
_zip_file_get_offset(const zip_t *za, zip_uint64_t idx, zip_error_t *error) {
    zip_uint64_t offset;
    zip_int32_t size;

    offset = za->entry[idx].orig->offset;

    if (zip_source_seek(za->src, (zip_int64_t)offset, SEEK_SET) < 0) {
        _zip_error_set_from_source(error, za->src);
        return 0;
    }

    /* TODO: cache? */
    if ((size = _zip_dirent_size(za->src, ZIP_EF_LOCAL, error)) < 0)
        return 0;

    if (offset + (zip_uint32_t)size > ZIP_INT64_MAX) {
        zip_error_set(error, ZIP_ER_SEEK, EFBIG);
        return 0;
    }

    return offset + (zip_uint32_t)size;
}

/*
   zip_file_rename.c -- rename file in zip archive
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <string.h>



ZIP_EXTERN int
zip_file_rename(zip_t *za, zip_uint64_t idx, const char *name, zip_flags_t flags) {
    const char *old_name;
    int old_is_dir, new_is_dir;

    if ((idx >= za->nentry) || ((name != NULL) && (strlen(name) > ZIP_UINT16_MAX))) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    if ((old_name = zip_get_name(za, idx, 0)) == NULL)
        return -1;

    new_is_dir = (name != NULL && name[strlen(name) - 1] == '/');
    old_is_dir = (old_name[strlen(old_name) - 1] == '/');

    if (new_is_dir != old_is_dir) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    return _zip_set_name(za, idx, name, flags);
}

/*
   zip_file_replace.c -- replace file via callback function
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_file_replace(zip_t *za, zip_uint64_t idx, zip_source_t *source, zip_flags_t flags) {
    if ((idx >= za->nentry) || (source == NULL)) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (_zip_file_replace(za, idx, NULL, source, flags) == -1)
        return -1;

    return 0;
}

/* NOTE: Signed due to -1 on error.  See zip_add.c for more details. */

zip_int64_t
_zip_file_replace(zip_t *za, zip_uint64_t idx, const char *name, zip_source_t *source, zip_flags_t flags) {
    zip_uint64_t za_nentry_prev;

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    za_nentry_prev = za->nentry;
    if (idx == ZIP_UINT64_MAX) {
        zip_int64_t i = -1;

        if (flags & ZIP_FL_OVERWRITE)
            i = _zip_name_locate(za, name, flags, NULL);

        if (i == -1) {
            /* create and use new entry, used by zip_add */
            if ((i = _zip_add_entry(za)) < 0)
                return -1;
        }
        idx = (zip_uint64_t)i;
    }

    if (name && (_zip_set_name(za, idx, name, flags) != 0)) {
        if (za->nentry != za_nentry_prev) {
            _zip_entry_finalize(za->entry + idx);
            za->nentry = za_nentry_prev;
        }
        return -1;
    }

    /* does not change any name related data, so we can do it here;
     * needed for a double add of the same file name */
    _zip_unchange_data(za->entry + idx);

    if ((za->entry[idx].orig != NULL) && ((za->entry[idx].changes == NULL) || ((za->entry[idx].changes->changed & ZIP_DIRENT_COMP_METHOD) == 0))) {
        if (za->entry[idx].changes == NULL) {
            if ((za->entry[idx].changes = _zip_dirent_clone(za->entry[idx].orig)) == NULL) {
                zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
                return -1;
            }
        }

        za->entry[idx].changes->comp_method = ZIP_CM_REPLACED_DEFAULT;
        za->entry[idx].changes->changed    |= ZIP_DIRENT_COMP_METHOD;
    }

    za->entry[idx].source = source;

    return (zip_int64_t)idx;
}

/*
   zip_file_set_comment.c -- set comment for file in archive
   Copyright (C) 2006-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>



ZIP_EXTERN int
zip_file_set_comment(zip_t *za, zip_uint64_t idx,
                     const char *comment, zip_uint16_t len, zip_flags_t flags) {
    zip_entry_t *e;
    zip_string_t *cstr;
    int changed;

    if (_zip_get_dirent(za, idx, 0, NULL) == NULL)
        return -1;

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    if ((len > 0) && (comment == NULL)) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (len > 0) {
        if ((cstr = _zip_string_new((const zip_uint8_t *)comment, len, flags, &za->error)) == NULL)
            return -1;
        if (((flags & ZIP_FL_ENCODING_ALL) == ZIP_FL_ENC_GUESS) && (_zip_guess_encoding(cstr, ZIP_ENCODING_UNKNOWN) == ZIP_ENCODING_UTF8_GUESSED))
            cstr->encoding = ZIP_ENCODING_UTF8_KNOWN;
    } else
        cstr = NULL;

    e = za->entry + idx;

    if (e->changes) {
        _zip_string_free(e->changes->comment);
        e->changes->comment  = NULL;
        e->changes->changed &= ~ZIP_DIRENT_COMMENT;
    }

    if (e->orig && e->orig->comment)
        changed = !_zip_string_equal(e->orig->comment, cstr);
    else
        changed = (cstr != NULL);

    if (changed) {
        if (e->changes == NULL) {
            if ((e->changes = _zip_dirent_clone(e->orig)) == NULL) {
                zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
                _zip_string_free(cstr);
                return -1;
            }
        }
        e->changes->comment  = cstr;
        e->changes->changed |= ZIP_DIRENT_COMMENT;
    } else {
        _zip_string_free(cstr);
        if (e->changes && (e->changes->changed == 0)) {
            _zip_dirent_free(e->changes);
            e->changes = NULL;
        }
    }

    return 0;
}

/*
   zip_file_set_external_attributes.c -- set external attributes for entry
   Copyright (C) 2013-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_file_set_external_attributes(zip_t *za, zip_uint64_t idx, zip_flags_t flags, zip_uint8_t opsys, zip_uint32_t attributes) {
    zip_entry_t *e;
    int changed;
    zip_uint8_t unchanged_opsys;
    zip_uint32_t unchanged_attributes;

    if (_zip_get_dirent(za, idx, 0, NULL) == NULL)
        return -1;

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    e = za->entry + idx;

    unchanged_opsys      = (e->orig ? (zip_uint8_t)(e->orig->version_madeby >> 8) : (zip_uint8_t)ZIP_OPSYS_DEFAULT);
    unchanged_attributes = e->orig ? e->orig->ext_attrib : ZIP_EXT_ATTRIB_DEFAULT;

    changed = (opsys != unchanged_opsys || attributes != unchanged_attributes);

    if (changed) {
        if (e->changes == NULL) {
            if ((e->changes = _zip_dirent_clone(e->orig)) == NULL) {
                zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
                return -1;
            }
        }
        e->changes->version_madeby = (zip_uint16_t)((opsys << 8) | (e->changes->version_madeby & 0xff));
        e->changes->ext_attrib     = attributes;
        e->changes->changed       |= ZIP_DIRENT_ATTRIBUTES;
    } else if (e->changes) {
        e->changes->changed &= ~ZIP_DIRENT_ATTRIBUTES;
        if (e->changes->changed == 0) {
            _zip_dirent_free(e->changes);
            e->changes = NULL;
        } else {
            e->changes->version_madeby = (zip_uint16_t)((unchanged_opsys << 8) | (e->changes->version_madeby & 0xff));
            e->changes->ext_attrib     = unchanged_attributes;
        }
    }

    return 0;
}

/*
   zip_file_set_mtime.c -- set modification time of entry.
   Copyright (C) 2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
   3. The names of the authors may not be used to endorse or promote
   products derived from this software without specific prior
   written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int zip_file_set_mtime(zip_t *za, zip_uint64_t idx, time_t mtime, zip_flags_t flags) {
    zip_entry_t *e;
    int changed;

    if (_zip_get_dirent(za, idx, 0, NULL) == NULL)
        return -1;

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    e = za->entry + idx;

    changed = e->orig == NULL || mtime != e->orig->last_mod;

    if (changed) {
        if (e->changes == NULL) {
            if ((e->changes = _zip_dirent_clone(e->orig)) == NULL) {
                zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
                return -1;
            }
        }
        e->changes->last_mod = mtime;
        e->changes->changed |= ZIP_DIRENT_LAST_MOD;
    } else {
        if (e->changes) {
            e->changes->changed &= ~ZIP_DIRENT_LAST_MOD;
            if (e->changes->changed == 0) {
                _zip_dirent_free(e->changes);
                e->changes = NULL;
            }
        }
    }

    return 0;
}

/*
   zip_file_sterror.c -- get string representation of zip file error
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN const char *
zip_file_strerror(zip_file_t *zf) {
    return zip_error_strerror(&zf->error);
}

/*
   zip_filerange_crc.c -- compute CRC32 for a range of a file
   Copyright (C) 2008-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <errno.h>



int
_zip_filerange_crc(zip_source_t *src, zip_uint64_t start, zip_uint64_t len, uLong *crcp, zip_error_t *error) {
    Bytef buf[BUFSIZE];
    zip_int64_t n;

    *crcp = crc32(0L, Z_NULL, 0);

    if (start > ZIP_INT64_MAX) {
        zip_error_set(error, ZIP_ER_SEEK, EFBIG);
        return -1;
    }

    if (zip_source_seek(src, (zip_int64_t)start, SEEK_SET) != 0) {
        _zip_error_set_from_source(error, src);
        return -1;
    }

    while (len > 0) {
        n = (zip_int64_t)(len > BUFSIZE ? BUFSIZE : len);
        if ((n = zip_source_read(src, buf, (zip_uint64_t)n)) < 0) {
            _zip_error_set_from_source(error, src);
            return -1;
        }
        if (n == 0) {
            zip_error_set(error, ZIP_ER_EOF, 0);
            return -1;
        }

        *crcp = crc32(*crcp, buf, (uInt)n);

        len -= (zip_uint64_t)n;
    }

    return 0;
}

/*
   zip_fopen.c -- open file in zip archive for reading
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN zip_file_t *
zip_fopen(zip_t *za, const char *fname, zip_flags_t flags) {
    zip_int64_t idx;

    if ((idx = zip_name_locate(za, fname, flags)) < 0)
        return NULL;

    return zip_fopen_index_encrypted(za, (zip_uint64_t)idx, flags, za->default_password);
}

/*
   zip_fopen_encrypted.c -- open file for reading with password
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN zip_file_t *
zip_fopen_encrypted(zip_t *za, const char *fname, zip_flags_t flags, const char *password) {
    zip_int64_t idx;

    if ((idx = zip_name_locate(za, fname, flags)) < 0)
        return NULL;

    return zip_fopen_index_encrypted(za, (zip_uint64_t)idx, flags, password);
}

/*
   zip_fopen_index.c -- open file in zip archive for reading by index
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>



ZIP_EXTERN zip_file_t *
zip_fopen_index(zip_t *za, zip_uint64_t index, zip_flags_t flags) {
    return zip_fopen_index_encrypted(za, index, flags, za->default_password);
}

/*
   zip_fopen_index_encrypted.c -- open file for reading by index w/ password
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>



static zip_file_t *_zip_file_new(zip_t *za);


ZIP_EXTERN zip_file_t *
zip_fopen_index_encrypted(zip_t *za, zip_uint64_t index, zip_flags_t flags,
                          const char *password) {
    zip_file_t *zf;
    zip_source_t *src;

    if ((src = _zip_source_zip_new(za, za, index, flags, 0, 0, password)) == NULL)
        return NULL;

    if (zip_source_open(src) < 0) {
        _zip_error_set_from_source(&za->error, src);
        zip_source_free(src);
        return NULL;
    }

    if ((zf = _zip_file_new(za)) == NULL) {
        zip_source_free(src);
        return NULL;
    }

    zf->src = src;

    return zf;
}

static zip_file_t *
_zip_file_new(zip_t *za) {
    zip_file_t *zf;

    if ((zf = (zip_file_t *)malloc(sizeof(struct zip_file))) == NULL) {
        zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    zf->za = za;
    zip_error_init(&zf->error);
    zf->eof = 0;
    zf->src = NULL;

    return zf;
}

/*
   zip_fread.c -- read from file
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN zip_int64_t
zip_fread(zip_file_t *zf, void *outbuf, zip_uint64_t toread) {
    zip_int64_t n;

    if (!zf)
        return -1;

    if (zf->error.zip_err != 0)
        return -1;

    if (toread > ZIP_INT64_MAX) {
        zip_error_set(&zf->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if ((zf->eof) || (toread == 0))
        return 0;

    if ((n = zip_source_read(zf->src, outbuf, toread)) < 0) {
        _zip_error_set_from_source(&zf->error, zf->src);
        return -1;
    }

    return n;
}

/*
   zip_get_archive_comment.c -- get archive comment
   Copyright (C) 2006-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <string.h>



ZIP_EXTERN const char *
zip_get_archive_comment(zip_t *za, int *lenp, zip_flags_t flags) {
    zip_string_t *comment;
    zip_uint32_t len;
    const zip_uint8_t *str;

    if ((flags & ZIP_FL_UNCHANGED) || (za->comment_changes == NULL))
        comment = za->comment_orig;
    else
        comment = za->comment_changes;

    if ((str = _zip_string_get(comment, &len, flags, &za->error)) == NULL)
        return NULL;

    if (lenp)
        *lenp = (int)len;

    return (const char *)str;
}

/*
   zip_get_archive_flag.c -- get archive global flag
   Copyright (C) 2008-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_get_archive_flag(zip_t *za, zip_flags_t flag, zip_flags_t flags) {
    unsigned int fl;

    fl = (flags & ZIP_FL_UNCHANGED) ? za->flags : za->ch_flags;

    return (fl & flag) ? 1 : 0;
}

/*
   zip_get_compression_implementation.c -- get compression implementation
   Copyright (C) 2009-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



zip_compression_implementation
_zip_get_compression_implementation(zip_int32_t cm) {
    if ((cm == ZIP_CM_DEFLATE) || ZIP_CM_IS_DEFAULT(cm))
        return zip_source_deflate;
    return NULL;
}

/*
   zip_get_encryption_implementation.c -- get encryption implementation
   Copyright (C) 2009-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



zip_encryption_implementation
_zip_get_encryption_implementation(zip_uint16_t em) {
    if (em == ZIP_EM_TRAD_PKWARE)
        return zip_source_pkware;
    return NULL;
}

/*
   zip_get_file_comment.c -- get file comment
   Copyright (C) 2006-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#define _ZIP_COMPILING_DEPRECATED



ZIP_EXTERN const char *
zip_get_file_comment(zip_t *za, zip_uint64_t idx, int *lenp, int flags) {
    zip_uint32_t len;
    const char *s;

    if ((s = zip_file_get_comment(za, idx, &len, (zip_flags_t)flags)) != NULL) {
        if (lenp)
            *lenp = (int)len;
    }

    return s;
}

/*
   zip_get_name.c -- get filename for a file in zip file
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <string.h>



ZIP_EXTERN const char *
zip_get_name(zip_t *za, zip_uint64_t idx, zip_flags_t flags) {
    return _zip_get_name(za, idx, flags, &za->error);
}

const char *
_zip_get_name(zip_t *za, zip_uint64_t idx, zip_flags_t flags, zip_error_t *error) {
    zip_dirent_t *de;
    const zip_uint8_t *str;

    if ((de = _zip_get_dirent(za, idx, flags, error)) == NULL)
        return NULL;

    if ((str = _zip_string_get(de->filename, NULL, flags, error)) == NULL)
        return NULL;

    return (const char *)str;
}

/*
   zip_get_num_entries.c -- get number of entries in archive
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN zip_int64_t
zip_get_num_entries(zip_t *za, zip_flags_t flags) {
    zip_uint64_t n;

    if (za == NULL)
        return -1;

    if (flags & ZIP_FL_UNCHANGED) {
        n = za->nentry;
        while (n > 0 && za->entry[n - 1].orig == NULL)
            --n;
        return (zip_int64_t)n;
    }
    return (zip_int64_t)za->nentry;
}

/*
   zip_get_num_files.c -- get number of files in archive
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#define _ZIP_COMPILING_DEPRECATED

#include <limits.h>


ZIP_EXTERN int
zip_get_num_files(zip_t *za) {
    if (za == NULL)
        return -1;

    if (za->nentry > INT_MAX) {
        zip_error_set(&za->error, ZIP_ER_OPNOTSUPP, 0);
        return -1;
    }

    return (int)za->nentry;
}

/*
   zip_io_util.c -- I/O helper functions
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
   3. The names of the authors may not be used to endorse or promote
   products derived from this software without specific prior
   written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>



int
_zip_read(zip_source_t *src, zip_uint8_t *b, zip_uint64_t length, zip_error_t *error) {
    zip_int64_t n;

    if (length > ZIP_INT64_MAX) {
        zip_error_set(error, ZIP_ER_INTERNAL, 0);
        return -1;
    }

    if ((n = zip_source_read(src, b, length)) < 0) {
        _zip_error_set_from_source(error, src);
        return -1;
    }

    if (n < (zip_int64_t)length) {
        zip_error_set(error, ZIP_ER_EOF, 0);
        return -1;
    }

    return 0;
}

zip_uint8_t *
_zip_read_data(zip_buffer_t *buffer, zip_source_t *src, size_t length, char nulp, zip_error_t *error) {
    zip_uint8_t *r;

    if ((length == 0) && !nulp) {
        return NULL;
    }

    r = (zip_uint8_t *)malloc(length + (nulp ? 1 : 0));
    if (!r) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    if (buffer) {
        zip_uint8_t *data = _zip_buffer_get(buffer, length);

        if (data == NULL) {
            zip_error_set(error, ZIP_ER_MEMORY, 0);
            free(r);
            return NULL;
        }
        memcpy(r, data, length);
    } else {
        if (_zip_read(src, r, length, error) < 0) {
            free(r);
            return NULL;
        }
    }

    if (nulp) {
        zip_uint8_t *o;
        /* replace any in-string NUL characters with spaces */
        r[length] = 0;
        for (o = r; o < r + length; o++)
            if (*o == '\0')
                *o = ' ';
    }

    return r;
}

zip_string_t *
_zip_read_string(zip_buffer_t *buffer, zip_source_t *src, zip_uint16_t len, char nulp, zip_error_t *error) {
    zip_uint8_t *raw;
    zip_string_t *s;

    if ((raw = _zip_read_data(buffer, src, len, nulp, error)) == NULL)
        return NULL;

    s = _zip_string_new(raw, len, ZIP_FL_ENC_GUESS, error);
    free(raw);
    return s;
}

int
_zip_write(zip_t *za, const void *data, zip_uint64_t length) {
    zip_int64_t n;

    if ((n = zip_source_write(za->src, data, length)) < 0) {
        _zip_error_set_from_source(&za->error, za->src);
        return -1;
    }
    if ((zip_uint64_t)n != length) {
        zip_error_set(&za->error, ZIP_ER_WRITE, EINTR);
        return -1;
    }

    return 0;
}

/*
   zip_memdup.c -- internal zip function, "strdup" with len
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <string.h>



void *
_zip_memdup(const void *mem, size_t len, zip_error_t *error) {
    void *ret;

    if (len == 0)
        return NULL;

    ret = malloc(len);
    if (!ret) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    memcpy(ret, mem, len);

    return ret;
}

/*
   zip_name_locate.c -- get index by name
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <string.h>
#ifdef HAVE_STRINGS_H
 #include <strings.h>
#endif



ZIP_EXTERN zip_int64_t
zip_name_locate(zip_t *za, const char *fname, zip_flags_t flags) {
    return _zip_name_locate(za, fname, flags, &za->error);
}

zip_int64_t
_zip_name_locate(zip_t *za, const char *fname, zip_flags_t flags, zip_error_t *error) {
    int (*cmp)(const char *, const char *);
    const char *fn, *p;
    zip_uint64_t i;

    if (za == NULL)
        return -1;

    if (fname == NULL) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return -1;
    }

    cmp = (flags & ZIP_FL_NOCASE) ? strcasecmp : strcmp;

    for (i = 0; i < za->nentry; i++) {
        fn = _zip_get_name(za, i, flags, error);

        /* newly added (partially filled) entry or error */
        if (fn == NULL)
            continue;

        if (flags & ZIP_FL_NODIR) {
            p = strrchr(fn, '/');
            if (p)
                fn = p + 1;
        }

        if (cmp(fname, fn) == 0) {
            _zip_error_clear(error);
            return (zip_int64_t)i;
        }
    }

    zip_error_set(error, ZIP_ER_NOENT, 0);
    return -1;
}

/*
   zip_new.c -- create and init struct zip
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>



/* _zip_new:
   creates a new zipfile struct, and sets the contents to zero; returns
   the new struct. */

zip_t *
_zip_new(zip_error_t *error) {
    zip_t *za;

    za = (zip_t *)malloc(sizeof(struct zip));
    if (!za) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    za->src        = NULL;
    za->open_flags = 0;
    zip_error_init(&za->error);
    za->flags            = za->ch_flags = 0;
    za->default_password = NULL;
    za->comment_orig     = za->comment_changes = NULL;
    za->comment_changed  = 0;
    za->nentry           = za->nentry_alloc = 0;
    za->entry            = NULL;
    za->nopen_source     = za->nopen_source_alloc = 0;
    za->open_source      = NULL;
    za->tempdir          = NULL;

    return za;
}

/*
   zip_open.c -- open zip archive by name
   Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef enum {
    EXISTS_ERROR = -1,
    EXISTS_NOT   = 0,
    EXISTS_EMPTY,
    EXISTS_NONEMPTY,
} exists_t;
static zip_t *_zip_allocate_new(zip_source_t *src, unsigned int flags, zip_error_t *error);
static zip_int64_t _zip_checkcons(zip_t *za, zip_cdir_t *cdir, zip_error_t *error);
static zip_cdir_t *_zip_find_central_dir(zip_t *za, zip_uint64_t len);
static exists_t _zip_file_exists(zip_source_t *src, zip_error_t *error);
static int _zip_headercomp(const zip_dirent_t *, const zip_dirent_t *);
static unsigned char *_zip_memmem(const unsigned char *, size_t, const unsigned char *, size_t);
static zip_cdir_t *_zip_read_cdir(zip_t *za, zip_buffer_t *buffer, zip_uint64_t buf_offset, zip_error_t *error);
static zip_cdir_t *_zip_read_eocd(zip_buffer_t *buffer, zip_uint64_t buf_offset, unsigned int flags, zip_error_t *error);
static zip_cdir_t *_zip_read_eocd64(zip_source_t *src, zip_buffer_t *buffer, zip_uint64_t buf_offset, unsigned int flags, zip_error_t *error);


ZIP_EXTERN zip_t *
zip_open(const char *fn, int _flags, int *zep) {
    zip_t *za;
    zip_source_t *src;
    struct zip_error error;

    zip_error_init(&error);
    if ((src = zip_source_file_create(fn, 0, -1, &error)) == NULL) {
        _zip_set_open_error(zep, &error, 0);
        zip_error_fini(&error);
        return NULL;
    }

    if ((za = zip_open_from_source(src, _flags, &error)) == NULL) {
        zip_source_free(src);
        _zip_set_open_error(zep, &error, 0);
        zip_error_fini(&error);
        return NULL;
    }

    zip_error_fini(&error);
    return za;
}

ZIP_EXTERN zip_t *
zip_open_from_source(zip_source_t *src, int _flags, zip_error_t *error) {
    static zip_int64_t needed_support_read  = -1;
    static zip_int64_t needed_support_write = -1;

    unsigned int flags;
    zip_int64_t supported;
    exists_t exists;

    if ((_flags < 0) || (src == NULL)) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }
    flags = (unsigned int)_flags;

    supported = zip_source_supports(src);
    if (needed_support_read == -1) {
        needed_support_read  = zip_source_make_command_bitmap(ZIP_SOURCE_OPEN, ZIP_SOURCE_READ, ZIP_SOURCE_CLOSE, ZIP_SOURCE_SEEK, ZIP_SOURCE_TELL, ZIP_SOURCE_STAT, -1);
        needed_support_write = zip_source_make_command_bitmap(ZIP_SOURCE_BEGIN_WRITE, ZIP_SOURCE_COMMIT_WRITE, ZIP_SOURCE_ROLLBACK_WRITE, ZIP_SOURCE_SEEK_WRITE, ZIP_SOURCE_TELL_WRITE, ZIP_SOURCE_REMOVE, -1);
    }
    if ((supported & needed_support_read) != needed_support_read) {
        zip_error_set(error, ZIP_ER_OPNOTSUPP, 0);
        return NULL;
    }
    if ((supported & needed_support_write) != needed_support_write) {
        flags |= ZIP_RDONLY;
    }

    if ((flags & (ZIP_RDONLY | ZIP_TRUNCATE)) == (ZIP_RDONLY | ZIP_TRUNCATE)) {
        zip_error_set(error, ZIP_ER_RDONLY, 0);
        return NULL;
    }

    exists = _zip_file_exists(src, error);
    switch (exists) {
        case EXISTS_ERROR:
            return NULL;

        case EXISTS_NOT:
            if ((flags & ZIP_CREATE) == 0) {
                zip_error_set(error, ZIP_ER_NOENT, 0);
                return NULL;
            }
            return _zip_allocate_new(src, flags, error);

        default:
            {
                zip_t *za;
                if (flags & ZIP_EXCL) {
                    zip_error_set(error, ZIP_ER_EXISTS, 0);
                    return NULL;
                }
                if (zip_source_open(src) < 0) {
                    _zip_error_set_from_source(error, src);
                    return NULL;
                }

                if (flags & ZIP_TRUNCATE) {
                    za = _zip_allocate_new(src, flags, error);
                } else {
                    /* ZIP_CREATE gets ignored if file exists and not ZIP_EXCL, just like open() */
                    za = _zip_open(src, flags, error);
                }

                if (za == NULL) {
                    zip_source_close(src);
                    return NULL;
                }
                return za;
            }
    }
}

ZIP_EXTERN int
zip_archive_set_tempdir(zip_t *za, const char *tempdir) {
    char *new_tempdir;

    if (tempdir) {
        if ((new_tempdir = strdup(tempdir)) == NULL) {
            zip_error_set(&za->error, ZIP_ER_MEMORY, errno);
            return -1;
        }
    } else
        new_tempdir = NULL;

    free(za->tempdir);
    za->tempdir = new_tempdir;

    return 0;
}

zip_t *
_zip_open(zip_source_t *src, unsigned int flags, zip_error_t *error) {
    zip_t *za;
    zip_cdir_t *cdir;
    struct zip_stat st;
    zip_uint64_t len;

    zip_stat_init(&st);
    if (zip_source_stat(src, &st) < 0) {
        _zip_error_set_from_source(error, src);
        return NULL;
    }
    if ((st.valid & ZIP_STAT_SIZE) == 0) {
        zip_error_set(error, ZIP_ER_SEEK, EOPNOTSUPP);
        return NULL;
    }
    len = st.size;

    /* treat empty files as empty archives */
    if (len == 0) {
        if ((za = _zip_allocate_new(src, flags, error)) == NULL) {
            zip_source_free(src);
            return NULL;
        }

        return za;
    }

    if ((za = _zip_allocate_new(src, flags, error)) == NULL) {
        return NULL;
    }

    if ((cdir = _zip_find_central_dir(za, len)) == NULL) {
        _zip_error_copy(error, &za->error);
        /* keep src so discard does not get rid of it */
        zip_source_keep(src);
        zip_discard(za);
        return NULL;
    }

    za->entry        = cdir->entry;
    za->nentry       = cdir->nentry;
    za->nentry_alloc = cdir->nentry_alloc;
    za->comment_orig = cdir->comment;

    za->ch_flags = za->flags;

    free(cdir);

    return za;
}

void
_zip_set_open_error(int *zep, const zip_error_t *err, int ze) {
    if (err) {
        ze = zip_error_code_zip(err);
        if (zip_error_system_type(err) == ZIP_ET_SYS) {
            errno = zip_error_code_system(err);
        }
    }

    if (zep)
        *zep = ze;
}

/* _zip_readcdir:
   tries to find a valid end-of-central-directory at the beginning of
   buf, and then the corresponding central directory entries.
   Returns a struct zip_cdir which contains the central directory
   entries, or NULL if unsuccessful. */

static zip_cdir_t *
_zip_read_cdir(zip_t *za, zip_buffer_t *buffer, zip_uint64_t buf_offset, zip_error_t *error) {
    zip_cdir_t *cd;
    zip_uint16_t comment_len;
    zip_uint64_t i, left;
    zip_uint64_t eocd_offset = _zip_buffer_offset(buffer);
    zip_buffer_t *cd_buffer;

    if (_zip_buffer_left(buffer) < EOCDLEN) {
        /* not enough bytes left for comment */
        zip_error_set(error, ZIP_ER_NOZIP, 0);
        return NULL;
    }

    /* check for end-of-central-dir magic */
    if (memcmp(_zip_buffer_get(buffer, 4), EOCD_MAGIC, 4) != 0) {
        zip_error_set(error, ZIP_ER_NOZIP, 0);
        return NULL;
    }

    if (_zip_buffer_get_32(buffer) != 0) {
        zip_error_set(error, ZIP_ER_MULTIDISK, 0);
        return NULL;
    }

    if ((eocd_offset >= EOCD64LOCLEN) && (memcmp(_zip_buffer_data(buffer) + eocd_offset - EOCD64LOCLEN, EOCD64LOC_MAGIC, 4) == 0)) {
        _zip_buffer_set_offset(buffer, eocd_offset - EOCD64LOCLEN);
        cd = _zip_read_eocd64(za->src, buffer, buf_offset, za->flags, error);
    } else {
        _zip_buffer_set_offset(buffer, eocd_offset);
        cd = _zip_read_eocd(buffer, buf_offset, za->flags, error);
    }

    if (cd == NULL)
        return NULL;

    _zip_buffer_set_offset(buffer, eocd_offset + 20);
    comment_len = _zip_buffer_get_16(buffer);

    if (cd->offset + cd->size > buf_offset + eocd_offset) {
        /* cdir spans past EOCD record */
        zip_error_set(error, ZIP_ER_INCONS, 0);
        _zip_cdir_free(cd);
        return NULL;
    }

    if (comment_len || (za->open_flags & ZIP_CHECKCONS)) {
        zip_uint64_t tail_len;

        _zip_buffer_set_offset(buffer, eocd_offset + EOCDLEN);
        tail_len = _zip_buffer_left(buffer);

        if ((tail_len < comment_len) || ((za->open_flags & ZIP_CHECKCONS) && (tail_len != comment_len))) {
            zip_error_set(error, ZIP_ER_INCONS, 0);
            _zip_cdir_free(cd);
            return NULL;
        }

        if (comment_len) {
            if ((cd->comment = _zip_string_new(_zip_buffer_get(buffer, comment_len), comment_len, ZIP_FL_ENC_GUESS, error)) == NULL) {
                _zip_cdir_free(cd);
                return NULL;
            }
        }
    }

    if (cd->offset >= buf_offset) {
        zip_uint8_t *data;
        /* if buffer already read in, use it */
        _zip_buffer_set_offset(buffer, cd->offset - buf_offset);

        if ((data = _zip_buffer_get(buffer, cd->size)) == NULL) {
            zip_error_set(error, ZIP_ER_INCONS, 0);
            _zip_cdir_free(cd);
            return NULL;
        }
        if ((cd_buffer = _zip_buffer_new(data, cd->size)) == NULL) {
            zip_error_set(error, ZIP_ER_MEMORY, 0);
            _zip_cdir_free(cd);
            return NULL;
        }
    } else {
        cd_buffer = NULL;

        if (zip_source_seek(za->src, (zip_int64_t)cd->offset, SEEK_SET) < 0) {
            _zip_error_set_from_source(error, za->src);
            _zip_cdir_free(cd);
            return NULL;
        }

        /* possible consistency check: cd->offset = len-(cd->size+cd->comment_len+EOCDLEN) ? */
        if (zip_source_tell(za->src) != (zip_int64_t)cd->offset) {
            zip_error_set(error, ZIP_ER_NOZIP, 0);
            _zip_cdir_free(cd);
            return NULL;
        }
    }

    left = (zip_uint64_t)cd->size;
    i    = 0;
    while (i < cd->nentry && left > 0) {
        zip_int64_t entry_size;
        if (((cd->entry[i].orig = _zip_dirent_new()) == NULL) || ((entry_size = _zip_dirent_read(cd->entry[i].orig, za->src, cd_buffer, ZIP_FALSE, error)) < 0)) {
            _zip_cdir_free(cd);
            _zip_buffer_free(cd_buffer);
            return NULL;
        }
        i++;
        left -= (zip_uint64_t)entry_size;
    }

    if (i != cd->nentry) {
        zip_error_set(error, ZIP_ER_INCONS, 0);
        _zip_buffer_free(cd_buffer);
        _zip_cdir_free(cd);
        return NULL;
    }

    if (za->open_flags & ZIP_CHECKCONS) {
        char ok;

        if (cd_buffer) {
            ok = _zip_buffer_eof(cd_buffer);
        } else {
            zip_int64_t offset = zip_source_tell(za->src);

            if (offset < 0) {
                _zip_error_set_from_source(error, za->src);
                _zip_buffer_free(cd_buffer);
                _zip_cdir_free(cd);
                return NULL;
            }
            ok = ((zip_uint64_t)offset == cd->offset + cd->size);
        }

        if (!ok) {
            zip_error_set(error, ZIP_ER_INCONS, 0);
            _zip_buffer_free(cd_buffer);
            _zip_cdir_free(cd);
            return NULL;
        }
    }

    _zip_buffer_free(cd_buffer);
    return cd;
}

/* _zip_checkcons:
   Checks the consistency of the central directory by comparing central
   directory entries with local headers and checking for plausible
   file and header offsets. Returns -1 if not plausible, else the
   difference between the lowest and the highest fileposition reached */

static zip_int64_t
_zip_checkcons(zip_t *za, zip_cdir_t *cd, zip_error_t *error) {
    zip_uint64_t i;
    zip_uint64_t min, max, j;
    struct zip_dirent temp;

    _zip_dirent_init(&temp);
    if (cd->nentry) {
        max = cd->entry[0].orig->offset;
        min = cd->entry[0].orig->offset;
    } else
        min = max = 0;

    for (i = 0; i < cd->nentry; i++) {
        if (cd->entry[i].orig->offset < min)
            min = cd->entry[i].orig->offset;
        if (min > (zip_uint64_t)cd->offset) {
            zip_error_set(error, ZIP_ER_NOZIP, 0);
            return -1;
        }

        j = cd->entry[i].orig->offset + cd->entry[i].orig->comp_size
            + _zip_string_length(cd->entry[i].orig->filename) + LENTRYSIZE;
        if (j > max)
            max = j;
        if (max > (zip_uint64_t)cd->offset) {
            zip_error_set(error, ZIP_ER_NOZIP, 0);
            return -1;
        }

        if (zip_source_seek(za->src, (zip_int64_t)cd->entry[i].orig->offset, SEEK_SET) < 0) {
            _zip_error_set_from_source(error, za->src);
            return -1;
        }

        if (_zip_dirent_read(&temp, za->src, NULL, ZIP_TRUE, error) == -1) {
            _zip_dirent_finalize(&temp);
            return -1;
        }

        if (_zip_headercomp(cd->entry[i].orig, &temp) != 0) {
            zip_error_set(error, ZIP_ER_INCONS, 0);
            _zip_dirent_finalize(&temp);
            return -1;
        }

        cd->entry[i].orig->extra_fields            = _zip_ef_merge(cd->entry[i].orig->extra_fields, temp.extra_fields);
        cd->entry[i].orig->local_extra_fields_read = 1;
        temp.extra_fields = NULL;

        _zip_dirent_finalize(&temp);
    }

    return (max - min) < ZIP_INT64_MAX ? (zip_int64_t)(max - min) : ZIP_INT64_MAX;
}

/* _zip_headercomp:
   compares a central directory entry and a local file header
   Return 0 if they are consistent, -1 if not. */

static int
_zip_headercomp(const zip_dirent_t *central, const zip_dirent_t *local) {
    if ((central->version_needed != local->version_needed)
#if 0

        /* some zip-files have different values in local
           and global headers for the bitflags */
        || (central->bitflags != local->bitflags)
#endif
        || (central->comp_method != local->comp_method) ||
        (central->last_mod != local->last_mod) ||
        !_zip_string_equal(central->filename, local->filename))
        return -1;

    if ((central->crc != local->crc) || (central->comp_size != local->comp_size) ||
        (central->uncomp_size != local->uncomp_size)) {
        /* InfoZip stores valid values in local header even when data descriptor is used.
           This is in violation of the appnote. */
        if ((((local->bitflags & ZIP_GPBF_DATA_DESCRIPTOR) == 0) ||
             (local->crc != 0) || (local->comp_size != 0) || (local->uncomp_size != 0)))
            return -1;
    }

    return 0;
}

static zip_t *
_zip_allocate_new(zip_source_t *src, unsigned int flags, zip_error_t *error) {
    zip_t *za;

    if ((za = _zip_new(error)) == NULL) {
        return NULL;
    }

    za->src        = src;
    za->open_flags = flags;
    if (flags & ZIP_RDONLY) {
        za->flags    |= ZIP_AFL_RDONLY;
        za->ch_flags |= ZIP_AFL_RDONLY;
    }
    return za;
}

/*
 * tests for file existence
 */
static exists_t
_zip_file_exists(zip_source_t *src, zip_error_t *error) {
    struct zip_stat st;

    zip_stat_init(&st);
    if (zip_source_stat(src, &st) != 0) {
        zip_error_t *src_error = zip_source_error(src);
        if ((zip_error_code_zip(src_error) == ZIP_ER_READ) && (zip_error_code_system(src_error) == ENOENT)) {
            return EXISTS_NOT;
        }
        _zip_error_copy(error, src_error);
        return EXISTS_ERROR;
    }

    return (st.valid & ZIP_STAT_SIZE) && st.size == 0 ? EXISTS_EMPTY : EXISTS_NONEMPTY;
}

static zip_cdir_t *
_zip_find_central_dir(zip_t *za, zip_uint64_t len) {
    zip_cdir_t *cdir, *cdirnew;
    zip_uint8_t *match;
    zip_int64_t buf_offset;
    zip_uint64_t buflen;
    zip_int64_t a;
    zip_int64_t best;
    zip_error_t error;
    zip_buffer_t *buffer;

    if (len < EOCDLEN) {
        zip_error_set(&za->error, ZIP_ER_NOZIP, 0);
        return NULL;
    }

    buflen = (len < CDBUFSIZE ? len : CDBUFSIZE);
    if (zip_source_seek(za->src, -(zip_int64_t)buflen, SEEK_END) < 0) {
        zip_error_t *src_error = zip_source_error(za->src);
        if ((zip_error_code_zip(src_error) != ZIP_ER_SEEK) || (zip_error_code_system(src_error) != EFBIG)) {
            /* seek before start of file on my machine */
            _zip_error_copy(&za->error, src_error);
            return NULL;
        }
    }
    if ((buf_offset = zip_source_tell(za->src)) < 0) {
        _zip_error_set_from_source(&za->error, za->src);
        return NULL;
    }

    if ((buffer = _zip_buffer_new_from_source(za->src, buflen, NULL, &za->error)) == NULL) {
        return NULL;
    }

    best = -1;
    cdir = NULL;
    if (buflen >= CDBUFSIZE) {
        /* EOCD64 locator is before EOCD, so leave place for it */
        _zip_buffer_set_offset(buffer, EOCD64LOCLEN);
    }
    zip_error_set(&error, ZIP_ER_NOZIP, 0);

    match = _zip_buffer_get(buffer, 0);
    while ((match = _zip_memmem(match, _zip_buffer_left(buffer) - (EOCDLEN - 4), (const unsigned char *)EOCD_MAGIC, 4)) != NULL) {
        _zip_buffer_set_offset(buffer, (zip_uint64_t)(match - _zip_buffer_data(buffer)));
        if ((cdirnew = _zip_read_cdir(za, buffer, (zip_uint64_t)buf_offset, &error)) != NULL) {
            if (cdir) {
                if (best <= 0) {
                    best = _zip_checkcons(za, cdir, &error);
                }

                a = _zip_checkcons(za, cdirnew, &error);
                if (best < a) {
                    _zip_cdir_free(cdir);
                    cdir = cdirnew;
                    best = a;
                } else {
                    _zip_cdir_free(cdirnew);
                }
            } else {
                cdir = cdirnew;
                if (za->open_flags & ZIP_CHECKCONS)
                    best = _zip_checkcons(za, cdir, &error);
                else {
                    best = 0;
                }
            }
            cdirnew = NULL;
        }

        match++;
        _zip_buffer_set_offset(buffer, (zip_uint64_t)(match - _zip_buffer_data(buffer)));
    }

    _zip_buffer_free(buffer);

    if (best < 0) {
        _zip_error_copy(&za->error, &error);
        _zip_cdir_free(cdir);
        return NULL;
    }

    return cdir;
}

static unsigned char *
_zip_memmem(const unsigned char *big, size_t biglen, const unsigned char *little, size_t littlelen) {
    const unsigned char *p;

    if ((biglen < littlelen) || (littlelen == 0))
        return NULL;
    p = big - 1;
    while ((p = (const unsigned char *)
                memchr(p + 1, little[0], (size_t)(big - (p + 1)) + (size_t)(biglen - littlelen) + 1)) != NULL) {
        if (memcmp(p + 1, little + 1, littlelen - 1) == 0)
            return (unsigned char *)p;
    }

    return NULL;
}

static zip_cdir_t *_zip_read_eocd(zip_buffer_t *buffer, zip_uint64_t buf_offset, unsigned int flags, zip_error_t *error) {
    zip_cdir_t *cd;
    zip_uint64_t i, nentry, size, offset, eocd_offset;

    if (_zip_buffer_left(buffer) < EOCDLEN) {
        zip_error_set(error, ZIP_ER_INCONS, 0);
        return NULL;
    }

    eocd_offset = _zip_buffer_offset(buffer);

    _zip_buffer_get(buffer, 8); /* magic and number of disks already verified */

    /* number of cdir-entries on this disk */
    i = _zip_buffer_get_16(buffer);
    /* number of cdir-entries */
    nentry = _zip_buffer_get_16(buffer);

    if (nentry != i) {
        zip_error_set(error, ZIP_ER_NOZIP, 0);
        return NULL;
    }

    size   = _zip_buffer_get_32(buffer);
    offset = _zip_buffer_get_32(buffer);

    if (offset + size < offset) {
        zip_error_set(error, ZIP_ER_SEEK, EFBIG);
        return NULL;
    }

    if (offset + size > buf_offset + eocd_offset) {
        /* cdir spans past EOCD record */
        zip_error_set(error, ZIP_ER_INCONS, 0);
        return NULL;
    }

    if ((flags & ZIP_CHECKCONS) && (offset + size != buf_offset + eocd_offset)) {
        zip_error_set(error, ZIP_ER_INCONS, 0);
        return NULL;
    }

    if ((cd = _zip_cdir_new(nentry, error)) == NULL)
        return NULL;

    cd->size   = size;
    cd->offset = offset;

    return cd;
}

static zip_cdir_t *
_zip_read_eocd64(zip_source_t *src, zip_buffer_t *buffer, zip_uint64_t buf_offset, unsigned int flags, zip_error_t *error) {
    zip_cdir_t *cd;
    zip_uint64_t offset;
    zip_uint8_t eocd[EOCD64LEN];
    zip_uint64_t eocd_offset;
    zip_uint64_t size, nentry, i, eocdloc_offset;
    char free_buffer;

    eocdloc_offset = _zip_buffer_offset(buffer);

    _zip_buffer_get(buffer, 8); /* magic and single disk already verified */
    eocd_offset = _zip_buffer_get_64(buffer);

    if ((eocd_offset > ZIP_INT64_MAX) || (eocd_offset + EOCD64LEN < eocd_offset)) {
        zip_error_set(error, ZIP_ER_SEEK, EFBIG);
        return NULL;
    }

    if (eocd_offset + EOCD64LEN > eocdloc_offset + buf_offset) {
        zip_error_set(error, ZIP_ER_INCONS, 0);
        return NULL;
    }

    if ((eocd_offset >= buf_offset) && (eocd_offset + EOCD64LEN <= buf_offset + _zip_buffer_size(buffer))) {
        _zip_buffer_set_offset(buffer, eocd_offset - buf_offset);
        free_buffer = ZIP_FALSE;
    } else {
        if (zip_source_seek(src, (zip_int64_t)eocd_offset, SEEK_SET) < 0) {
            _zip_error_set_from_source(error, src);
            return NULL;
        }
        if ((buffer = _zip_buffer_new_from_source(src, EOCD64LEN, eocd, error)) == NULL) {
            return NULL;
        }
        free_buffer = ZIP_TRUE;
    }

    if (memcmp(_zip_buffer_get(buffer, 4), EOCD64_MAGIC, 4) != 0) {
        zip_error_set(error, ZIP_ER_INCONS, 0);
        if (free_buffer) {
            _zip_buffer_free(buffer);
        }
        return NULL;
    }

    size = _zip_buffer_get_64(buffer);

    if ((flags & ZIP_CHECKCONS) && (size + eocd_offset + 12 != buf_offset + eocdloc_offset)) {
        zip_error_set(error, ZIP_ER_INCONS, 0);
        if (free_buffer) {
            _zip_buffer_free(buffer);
        }
        return NULL;
    }

    _zip_buffer_get(buffer, 12); /* skip version made by/needed and num disks */

    nentry = _zip_buffer_get_64(buffer);
    i      = _zip_buffer_get_64(buffer);

    if (nentry != i) {
        zip_error_set(error, ZIP_ER_MULTIDISK, 0);
        if (free_buffer) {
            _zip_buffer_free(buffer);
        }
        return NULL;
    }

    size   = _zip_buffer_get_64(buffer);
    offset = _zip_buffer_get_64(buffer);

    if (!_zip_buffer_ok(buffer)) {
        zip_error_set(error, ZIP_ER_INTERNAL, 0);
        if (free_buffer) {
            _zip_buffer_free(buffer);
        }
        return NULL;
    }

    if (free_buffer) {
        _zip_buffer_free(buffer);
    }

    if ((offset > ZIP_INT64_MAX) || (offset + size < offset)) {
        zip_error_set(error, ZIP_ER_SEEK, EFBIG);
        return NULL;
    }
    if ((flags & ZIP_CHECKCONS) && (offset + size != eocd_offset)) {
        zip_error_set(error, ZIP_ER_INCONS, 0);
        return NULL;
    }

    if ((cd = _zip_cdir_new(nentry, error)) == NULL)
        return NULL;


    cd->size   = size;
    cd->offset = offset;

    return cd;
}

/*
   zip_rename.c -- rename file in zip archive
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <string.h>

#define _ZIP_COMPILING_DEPRECATED



ZIP_EXTERN int
zip_rename(zip_t *za, zip_uint64_t idx, const char *name) {
    return zip_file_rename(za, idx, name, 0);
}

/*
   zip_replace.c -- replace file via callback function
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#define _ZIP_COMPILING_DEPRECATED



ZIP_EXTERN int
zip_replace(zip_t *za, zip_uint64_t idx, zip_source_t *source) {
    return zip_file_replace(za, idx, source, 0);
}

/*
   zip_set_archive_comment.c -- set archive comment
   Copyright (C) 2006-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>



ZIP_EXTERN int
zip_set_archive_comment(zip_t *za, const char *comment, zip_uint16_t len) {
    zip_string_t *cstr;

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    if ((len > 0) && (comment == NULL)) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (len > 0) {
        if ((cstr = _zip_string_new((const zip_uint8_t *)comment, len, ZIP_FL_ENC_GUESS, &za->error)) == NULL)
            return -1;

        if (_zip_guess_encoding(cstr, ZIP_ENCODING_UNKNOWN) == ZIP_ENCODING_CP437) {
            _zip_string_free(cstr);
            zip_error_set(&za->error, ZIP_ER_INVAL, 0);
            return -1;
        }
    } else
        cstr = NULL;

    _zip_string_free(za->comment_changes);
    za->comment_changes = NULL;

    if (((za->comment_orig && _zip_string_equal(za->comment_orig, cstr)) ||
         ((za->comment_orig == NULL) && (cstr == NULL)))) {
        _zip_string_free(cstr);
        za->comment_changed = 0;
    } else {
        za->comment_changes = cstr;
        za->comment_changed = 1;
    }

    return 0;
}

/*
   zip_get_archive_flag.c -- set archive global flag
   Copyright (C) 2008-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_set_archive_flag(zip_t *za, zip_flags_t flag, int value) {
    unsigned int new_flags;

    if (value)
        new_flags = za->ch_flags | flag;
    else
        new_flags = za->ch_flags & ~flag;

    if (new_flags == za->ch_flags)
        return 0;

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    if ((flag & ZIP_AFL_RDONLY) && value &&
        ((za->ch_flags & ZIP_AFL_RDONLY) == 0)) {
        if (_zip_changed(za, NULL)) {
            zip_error_set(&za->error, ZIP_ER_CHANGED, 0);
            return -1;
        }
    }

    za->ch_flags = new_flags;

    return 0;
}

/*
   zip_set_default_password.c -- set default password for decryption
   Copyright (C) 2009-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>
#include <string.h>



ZIP_EXTERN int
zip_set_default_password(zip_t *za, const char *passwd) {
    if (za == NULL)
        return -1;

    if (za->default_password)
        free(za->default_password);

    if (passwd) {
        if ((za->default_password = strdup(passwd)) == NULL) {
            zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
            return -1;
        }
    } else
        za->default_password = NULL;

    return 0;
}

/*
   zip_set_file_comment.c -- set comment for file in archive
   Copyright (C) 2006-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>

#define _ZIP_COMPILING_DEPRECATED



ZIP_EXTERN int
zip_set_file_comment(zip_t *za, zip_uint64_t idx, const char *comment, int len) {
    if ((len < 0) || (len > ZIP_UINT16_MAX)) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }
    return zip_file_set_comment(za, idx, comment, (zip_uint16_t)len, 0);
}

/*
   zip_set_file_compression.c -- set compression for file in archive
   Copyright (C) 2012-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_set_file_compression(zip_t *za, zip_uint64_t idx, zip_int32_t method, zip_uint32_t flags) {
    zip_entry_t *e;
    zip_int32_t old_method;

    if (idx >= za->nentry) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    if ((method != ZIP_CM_DEFAULT) && (method != ZIP_CM_STORE) && (method != ZIP_CM_DEFLATE)) {
        zip_error_set(&za->error, ZIP_ER_COMPNOTSUPP, 0);
        return -1;
    }

    e = za->entry + idx;

    old_method = (e->orig == NULL ? ZIP_CM_DEFAULT : e->orig->comp_method);

    /* TODO: revisit this when flags are supported, since they may require a recompression */

    if (method == old_method) {
        if (e->changes) {
            e->changes->changed &= ~ZIP_DIRENT_COMP_METHOD;
            if (e->changes->changed == 0) {
                _zip_dirent_free(e->changes);
                e->changes = NULL;
            }
        }
    } else {
        if (e->changes == NULL) {
            if ((e->changes = _zip_dirent_clone(e->orig)) == NULL) {
                zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
                return -1;
            }
        }

        e->changes->comp_method = method;
        e->changes->changed    |= ZIP_DIRENT_COMP_METHOD;
    }

    return 0;
}

/*
   zip_set_name.c -- rename helper function
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>
#include <string.h>



int
_zip_set_name(zip_t *za, zip_uint64_t idx, const char *name, zip_flags_t flags) {
    zip_entry_t *e;
    zip_string_t *str;
    int changed;
    zip_int64_t i;

    if (idx >= za->nentry) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (ZIP_IS_RDONLY(za)) {
        zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
        return -1;
    }

    if (name && (strlen(name) > 0)) {
        /* TODO: check for string too long */
        if ((str = _zip_string_new((const zip_uint8_t *)name, (zip_uint16_t)strlen(name), flags, &za->error)) == NULL)
            return -1;
        if (((flags & ZIP_FL_ENCODING_ALL) == ZIP_FL_ENC_GUESS) && (_zip_guess_encoding(str, ZIP_ENCODING_UNKNOWN) == ZIP_ENCODING_UTF8_GUESSED))
            str->encoding = ZIP_ENCODING_UTF8_KNOWN;
    } else
        str = NULL;

    /* TODO: encoding flags needed for CP437? */
    if (((i = _zip_name_locate(za, name, 0, NULL)) >= 0) && ((zip_uint64_t)i != idx)) {
        _zip_string_free(str);
        zip_error_set(&za->error, ZIP_ER_EXISTS, 0);
        return -1;
    }

    /* no effective name change */
    if ((i >= 0) && ((zip_uint64_t)i == idx)) {
        _zip_string_free(str);
        return 0;
    }

    e = za->entry + idx;

    if (e->changes) {
        _zip_string_free(e->changes->filename);
        e->changes->filename = NULL;
        e->changes->changed &= ~ZIP_DIRENT_FILENAME;
    }

    if (e->orig)
        changed = !_zip_string_equal(e->orig->filename, str);
    else
        changed = 1;

    if (changed) {
        if (e->changes == NULL) {
            if ((e->changes = _zip_dirent_clone(e->orig)) == NULL) {
                zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
                _zip_string_free(str);
                return -1;
            }
        }
        e->changes->filename = str;
        e->changes->changed |= ZIP_DIRENT_FILENAME;
    } else {
        _zip_string_free(str);
        if (e->changes && (e->changes->changed == 0)) {
            _zip_dirent_free(e->changes);
            e->changes = NULL;
        }
    }

    return 0;
}

/*
   zip_source_begin_write.c -- start a new file for writing
   Copyright (C) 2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_source_begin_write(zip_source_t *src) {
    if (ZIP_SOURCE_IS_OPEN_WRITING(src)) {
        zip_error_set(&src->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (_zip_source_call(src, NULL, 0, ZIP_SOURCE_BEGIN_WRITE) < 0) {
        return -1;
    }

    src->write_state = ZIP_SOURCE_WRITE_OPEN;

    return 0;
}

/*
   zip_source_buffer.c -- create zip data source from buffer
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>



#ifndef WRITE_FRAGMENT_SIZE
 #define WRITE_FRAGMENT_SIZE    64 * 1024
#endif

struct buffer {
    zip_uint64_t fragment_size;         /* size of each fragment */

    zip_uint8_t **fragments;            /* pointers to fragments */
    zip_uint64_t nfragments;            /* number of allocated fragments */
    zip_uint64_t fragments_capacity;    /* size of fragments (number of pointers) */
    zip_uint64_t size;                  /* size of data in bytes */
    zip_uint64_t offset;                /* current offset */
    int free_data;
};

typedef struct buffer   buffer_t;

struct read_data {
    zip_error_t error;
    time_t mtime;
    buffer_t *in;
    buffer_t *out;
};

static void buffer_free(buffer_t *buffer);
static buffer_t *buffer_new(zip_uint64_t fragment_size);
static buffer_t *buffer_new_read(const void *data, zip_uint64_t length, int free_data);
static buffer_t *buffer_new_write(zip_uint64_t fragment_size);
static zip_int64_t buffer_read(buffer_t *buffer, zip_uint8_t *data, zip_uint64_t length);
static int buffer_seek(buffer_t *buffer, void *data, zip_uint64_t len, zip_error_t *error);
static zip_int64_t buffer_write(buffer_t *buffer, const zip_uint8_t *data, zip_uint64_t length, zip_error_t *);

static zip_int64_t read_data(void *, void *, zip_uint64_t, zip_source_cmd_t);


ZIP_EXTERN zip_source_t *
zip_source_buffer(zip_t *za, const void *data, zip_uint64_t len, int freep) {
    if (za == NULL)
        return NULL;

    return zip_source_buffer_create(data, len, freep, &za->error);
}

ZIP_EXTERN zip_source_t *
zip_source_buffer_create(const void *data, zip_uint64_t len, int freep, zip_error_t *error) {
    struct read_data *ctx;
    zip_source_t *zs;

    if ((data == NULL) && (len > 0)) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    if ((ctx = (struct read_data *)malloc(sizeof(*ctx))) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    if ((ctx->in = buffer_new_read(data, len, freep)) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        free(ctx);
        return NULL;
    }

    ctx->out   = NULL;
    ctx->mtime = time(NULL);
    zip_error_init(&ctx->error);

    if ((zs = zip_source_function_create(read_data, ctx, error)) == NULL) {
        buffer_free(ctx->in);
        free(ctx);
        return NULL;
    }

    return zs;
}

static zip_int64_t
read_data(void *state, void *data, zip_uint64_t len, zip_source_cmd_t cmd) {
    struct read_data *ctx = (struct read_data *)state;

    switch (cmd) {
        case ZIP_SOURCE_BEGIN_WRITE:
            if ((ctx->out = buffer_new_write(WRITE_FRAGMENT_SIZE)) == NULL) {
                zip_error_set(&ctx->error, ZIP_ER_MEMORY, 0);
                return -1;
            }
            return 0;

        case ZIP_SOURCE_CLOSE:
            return 0;

        case ZIP_SOURCE_COMMIT_WRITE:
            buffer_free(ctx->in);
            ctx->in  = ctx->out;
            ctx->out = NULL;
            return 0;

        case ZIP_SOURCE_ERROR:
            return zip_error_to_data(&ctx->error, data, len);

        case ZIP_SOURCE_FREE:
            buffer_free(ctx->in);
            buffer_free(ctx->out);
            free(ctx);
            return 0;

        case ZIP_SOURCE_OPEN:
            ctx->in->offset = 0;
            return 0;

        case ZIP_SOURCE_READ:
            if (len > ZIP_INT64_MAX) {
                zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
                return -1;
            }
            return buffer_read(ctx->in, (zip_uint8_t *)data, len);

        case ZIP_SOURCE_REMOVE:
            {
                buffer_t *empty = buffer_new_read(NULL, 0, 0);
                if (empty == 0) {
                    zip_error_set(&ctx->error, ZIP_ER_MEMORY, 0);
                    return -1;
                }

                buffer_free(ctx->in);
                ctx->in = empty;
                return 0;
            }

        case ZIP_SOURCE_ROLLBACK_WRITE:
            buffer_free(ctx->out);
            ctx->out = NULL;
            return 0;

        case ZIP_SOURCE_SEEK:
            return buffer_seek(ctx->in, data, len, &ctx->error);

        case ZIP_SOURCE_SEEK_WRITE:
            return buffer_seek(ctx->out, data, len, &ctx->error);

        case ZIP_SOURCE_STAT:
            {
                zip_stat_t *st;

                if (len < sizeof(*st)) {
                    zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
                    return -1;
                }

                st = (zip_stat_t *)data;

                zip_stat_init(st);
                st->mtime             = ctx->mtime;
                st->size              = ctx->in->size;
                st->comp_size         = st->size;
                st->comp_method       = ZIP_CM_STORE;
                st->encryption_method = ZIP_EM_NONE;
                st->valid             = ZIP_STAT_MTIME | ZIP_STAT_SIZE | ZIP_STAT_COMP_SIZE | ZIP_STAT_COMP_METHOD | ZIP_STAT_ENCRYPTION_METHOD;

                return sizeof(*st);
            }

        case ZIP_SOURCE_SUPPORTS:
            return zip_source_make_command_bitmap(ZIP_SOURCE_OPEN, ZIP_SOURCE_READ, ZIP_SOURCE_CLOSE, ZIP_SOURCE_STAT, ZIP_SOURCE_ERROR, ZIP_SOURCE_FREE, ZIP_SOURCE_SEEK, ZIP_SOURCE_TELL, ZIP_SOURCE_BEGIN_WRITE, ZIP_SOURCE_COMMIT_WRITE, ZIP_SOURCE_REMOVE, ZIP_SOURCE_ROLLBACK_WRITE, ZIP_SOURCE_SEEK_WRITE, ZIP_SOURCE_TELL_WRITE, ZIP_SOURCE_WRITE, -1);

        case ZIP_SOURCE_TELL:
            if (ctx->in->offset > ZIP_INT64_MAX) {
                zip_error_set(&ctx->error, ZIP_ER_TELL, EOVERFLOW);
                return -1;
            }
            return (zip_int64_t)ctx->in->offset;


        case ZIP_SOURCE_TELL_WRITE:
            if (ctx->out->offset > ZIP_INT64_MAX) {
                zip_error_set(&ctx->error, ZIP_ER_TELL, EOVERFLOW);
                return -1;
            }
            return (zip_int64_t)ctx->out->offset;

        case ZIP_SOURCE_WRITE:
            if (len > ZIP_INT64_MAX) {
                zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
                return -1;
            }
            return buffer_write(ctx->out, (zip_uint8_t *)data, len, &ctx->error);

        default:
            zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
            return -1;
    }
}

static void
buffer_free(buffer_t *buffer) {
    if (buffer == NULL) {
        return;
    }

    if (buffer->free_data) {
        zip_uint64_t i;

        for (i = 0; i < buffer->nfragments; i++) {
            free(buffer->fragments[i]);
        }
    }
    free(buffer->fragments);
    free(buffer);
}

static buffer_t *
buffer_new(zip_uint64_t fragment_size) {
    buffer_t *buffer;

    if ((buffer = (buffer_t *)malloc(sizeof(*buffer))) == NULL) {
        return NULL;
    }

    buffer->fragment_size      = fragment_size;
    buffer->offset             = 0;
    buffer->free_data          = 0;
    buffer->nfragments         = 0;
    buffer->fragments_capacity = 0;
    buffer->fragments          = NULL;
    buffer->size = 0;

    return buffer;
}

static buffer_t *
buffer_new_read(const void *data, zip_uint64_t length, int free_data) {
    buffer_t *buffer;

    if ((buffer = buffer_new(length)) == NULL) {
        return NULL;
    }

    buffer->size = length;

    if (length > 0) {
        if ((buffer->fragments = (zip_uint8_t **)malloc(sizeof(*(buffer->fragments)))) == NULL) {
            buffer_free(buffer);
            return NULL;
        }
        buffer->fragments_capacity = 1;

        buffer->nfragments   = 1;
        buffer->fragments[0] = (zip_uint8_t *)data;
        buffer->free_data    = free_data;
    }

    return buffer;
}

static buffer_t *
buffer_new_write(zip_uint64_t fragment_size) {
    buffer_t *buffer;

    if ((buffer = buffer_new(fragment_size)) == NULL) {
        return NULL;
    }

    if ((buffer->fragments = (zip_uint8_t **)malloc(sizeof(*(buffer->fragments)))) == NULL) {
        buffer_free(buffer);
        return NULL;
    }
    buffer->fragments_capacity = 1;
    buffer->nfragments         = 0;
    buffer->free_data          = 1;

    return buffer;
}

static zip_int64_t
buffer_read(buffer_t *buffer, zip_uint8_t *data, zip_uint64_t length) {
    zip_uint64_t n, i, fragment_offset;

    length = ZIP_MIN(length, buffer->size - buffer->offset);

    if (length == 0) {
        return 0;
    }
    if (length > ZIP_INT64_MAX) {
        return -1;
    }

    i = buffer->offset / buffer->fragment_size;
    fragment_offset = buffer->offset % buffer->fragment_size;
    n = 0;
    while (n < length) {
        zip_uint64_t left = ZIP_MIN(length - n, buffer->fragment_size - fragment_offset);

        memcpy(data + n, buffer->fragments[i] + fragment_offset, left);

        n += left;
        i++;
        fragment_offset = 0;
    }

    buffer->offset += n;
    return (zip_int64_t)n;
}

static int
buffer_seek(buffer_t *buffer, void *data, zip_uint64_t len, zip_error_t *error) {
    zip_int64_t new_offset = zip_source_seek_compute_offset(buffer->offset, buffer->size, data, len, error);

    if (new_offset < 0) {
        return -1;
    }

    buffer->offset = (zip_uint64_t)new_offset;
    return 0;
}

static zip_int64_t
buffer_write(buffer_t *buffer, const zip_uint8_t *data, zip_uint64_t length, zip_error_t *error) {
    zip_uint64_t n, i, fragment_offset;

    if (buffer->offset + length + buffer->fragment_size - 1 < length) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return -1;
    }

    /* grow buffer if needed */
    if (buffer->offset + length > buffer->nfragments * buffer->fragment_size) {
        zip_uint64_t needed_fragments = (buffer->offset + length + buffer->fragment_size - 1) / buffer->fragment_size;

        if (needed_fragments > buffer->fragments_capacity) {
            zip_uint64_t new_capacity = buffer->fragments_capacity;

            while (new_capacity < needed_fragments) {
                new_capacity *= 2;
            }

            zip_uint8_t **fragments = (zip_uint8_t **)realloc(buffer->fragments, new_capacity * sizeof(*fragments));

            if (fragments == NULL) {
                zip_error_set(error, ZIP_ER_MEMORY, 0);
                return -1;
            }

            buffer->fragments          = fragments;
            buffer->fragments_capacity = new_capacity;
        }

        while (buffer->nfragments < needed_fragments) {
            if ((buffer->fragments[buffer->nfragments] = (zip_uint8_t *)malloc(buffer->fragment_size)) == NULL) {
                zip_error_set(error, ZIP_ER_MEMORY, 0);
                return -1;
            }
            buffer->nfragments++;
        }
    }

    i = buffer->offset / buffer->fragment_size;
    fragment_offset = buffer->offset % buffer->fragment_size;
    n = 0;
    while (n < length) {
        zip_uint64_t left = ZIP_MIN(length - n, buffer->fragment_size - fragment_offset);

        memcpy(buffer->fragments[i] + fragment_offset, data + n, left);

        n += left;
        i++;
        fragment_offset = 0;
    }

    buffer->offset += n;
    if (buffer->offset > buffer->size) {
        buffer->size = buffer->offset;
    }

    return (zip_int64_t)n;
}

/*
   zip_source_call.c -- invoke callback command on zip_source
   Copyright (C) 2009-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
   3. The names of the authors may not be used to endorse or promote
   products derived from this software without specific prior
   written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



zip_int64_t
_zip_source_call(zip_source_t *src, void *data, zip_uint64_t length, zip_source_cmd_t command) {
    zip_int64_t ret;

    if ((src->supports & ZIP_SOURCE_MAKE_COMMAND_BITMASK(command)) == 0) {
        zip_error_set(&src->error, ZIP_ER_OPNOTSUPP, 0);
        return -1;
    }

    if (src->src == NULL) {
        ret = src->cb.f(src->ud, data, length, command);
    } else {
        ret = src->cb.l(src->src, src->ud, data, length, command);
    }

    if (ret < 0) {
        if ((command != ZIP_SOURCE_ERROR) && (command != ZIP_SOURCE_SUPPORTS)) {
            int e[2];

            if (_zip_source_call(src, e, sizeof(e), ZIP_SOURCE_ERROR) < 0) {
                zip_error_set(&src->error, ZIP_ER_INTERNAL, 0);
            } else {
                zip_error_set(&src->error, e[0], e[1]);
            }
        }
    }

    return ret;
}

/*
   zip_source_close.c -- close zip_source (stop reading)
   Copyright (C) 2009-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



int
zip_source_close(zip_source_t *src) {
    if (!ZIP_SOURCE_IS_OPEN_READING(src)) {
        zip_error_set(&src->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    src->open_count--;
    if (src->open_count == 0) {
        _zip_source_call(src, NULL, 0, ZIP_SOURCE_CLOSE);

        if (ZIP_SOURCE_IS_LAYERED(src)) {
            if (zip_source_close(src->src) < 0) {
                zip_error_set(&src->error, ZIP_ER_INTERNAL, 0);
            }
        }
    }

    return 0;
}

/*
   zip_source_commit_write.c -- commit changes to file
   Copyright (C) 2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_source_commit_write(zip_source_t *src) {
    if (!ZIP_SOURCE_IS_OPEN_WRITING(src)) {
        zip_error_set(&src->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (src->open_count > 1) {
        zip_error_set(&src->error, ZIP_ER_INUSE, 0);
        return -1;
    } else if (ZIP_SOURCE_IS_OPEN_READING(src)) {
        if (zip_source_close(src) < 0) {
            return -1;
        }
    }

    if (_zip_source_call(src, NULL, 0, ZIP_SOURCE_COMMIT_WRITE) < 0) {
        src->write_state = ZIP_SOURCE_WRITE_FAILED;
        return -1;
    }

    src->write_state = ZIP_SOURCE_WRITE_CLOSED;

    return 0;
}

/*
   zip_source_crc.c -- pass-through source that calculates CRC32 and size
   Copyright (C) 2009-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>
#include <string.h>
#include <limits.h>



struct crc_context {
    int eof;
    int validate;
    zip_error_t error;
    zip_uint64_t size;
    zip_uint32_t crc;
};

static zip_int64_t crc_read(zip_source_t *, void *, void *, zip_uint64_t, zip_source_cmd_t);


zip_source_t *
zip_source_crc(zip_t *za, zip_source_t *src, int validate) {
    struct crc_context *ctx;

    if (src == NULL) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    if ((ctx = (struct crc_context *)malloc(sizeof(*ctx))) == NULL) {
        zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    ctx->eof      = 0;
    ctx->validate = validate;
    zip_error_init(&ctx->error);
    ctx->size = 0;
    ctx->crc  = 0;

    return zip_source_layered(za, src, crc_read, ctx);
}

static zip_int64_t
crc_read(zip_source_t *src, void *_ctx, void *data, zip_uint64_t len, zip_source_cmd_t cmd) {
    struct crc_context *ctx;
    zip_int64_t n;

    ctx = (struct crc_context *)_ctx;

    switch (cmd) {
        case ZIP_SOURCE_OPEN:
            ctx->eof  = 0;
            ctx->crc  = (zip_uint32_t)crc32(0, NULL, 0);
            ctx->size = 0;

            return 0;

        case ZIP_SOURCE_READ:
            if (ctx->eof || (len == 0))
                return 0;

            if ((n = zip_source_read(src, data, len)) < 0) {
                _zip_error_set_from_source(&ctx->error, src);
                return -1;
            }

            if (n == 0) {
                ctx->eof = 1;
                if (ctx->validate) {
                    struct zip_stat st;

                    if (zip_source_stat(src, &st) < 0) {
                        _zip_error_set_from_source(&ctx->error, src);
                        return -1;
                    }

                    if ((st.valid & ZIP_STAT_CRC) && (st.crc != ctx->crc)) {
                        zip_error_set(&ctx->error, ZIP_ER_CRC, 0);
                        return -1;
                    }
                    if ((st.valid & ZIP_STAT_SIZE) && (st.size != ctx->size)) {
                        zip_error_set(&ctx->error, ZIP_ER_INCONS, 0);
                        return -1;
                    }
                }
            } else {
                zip_uint64_t i, nn;

                for (i = 0; i < (zip_uint64_t)n; i += nn) {
                    nn = ZIP_MIN(UINT_MAX, (zip_uint64_t)n - i);

                    ctx->crc = (zip_uint32_t)crc32(ctx->crc, (const Bytef *)data + i, (uInt)nn);
                }
                ctx->size += (zip_uint64_t)n;
            }
            return n;

        case ZIP_SOURCE_CLOSE:
            return 0;

        case ZIP_SOURCE_STAT:
            {
                zip_stat_t *st;

                st = (zip_stat_t *)data;

                if (ctx->eof) {
                    /* TODO: Set comp_size, comp_method, encryption_method?
                            After all, this only works for uncompressed data. */
                    st->size              = ctx->size;
                    st->crc               = ctx->crc;
                    st->comp_size         = ctx->size;
                    st->comp_method       = ZIP_CM_STORE;
                    st->encryption_method = ZIP_EM_NONE;
                    st->valid            |= ZIP_STAT_SIZE | ZIP_STAT_CRC | ZIP_STAT_COMP_SIZE | ZIP_STAT_COMP_METHOD | ZIP_STAT_ENCRYPTION_METHOD;
                }
                return 0;
            }

        case ZIP_SOURCE_ERROR:
            return zip_error_to_data(&ctx->error, data, len);

        case ZIP_SOURCE_FREE:
            free(ctx);
            return 0;

        case ZIP_SOURCE_SUPPORTS:
            return zip_source_make_command_bitmap(ZIP_SOURCE_OPEN, ZIP_SOURCE_READ, ZIP_SOURCE_CLOSE, ZIP_SOURCE_STAT, ZIP_SOURCE_ERROR, ZIP_SOURCE_FREE, -1);

        default:
            zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
            return -1;
    }
}

/*
   zip_source_deflate.c -- deflate (de)compressoin routines
   Copyright (C) 2009-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>



struct deflate {
    zip_error_t error;

    char eof;
    char can_store;
    char is_stored;
    int mem_level;
    zip_uint64_t size;
    zip_uint8_t buffer[BUFSIZE];
    z_stream zstr;
};

static zip_int64_t compress_read(zip_source_t *, struct deflate *, void *, zip_uint64_t);
static zip_int64_t decompress_read(zip_source_t *, struct deflate *, void *, zip_uint64_t);
static zip_int64_t deflate_compress(zip_source_t *, void *, void *, zip_uint64_t, zip_source_cmd_t);
static zip_int64_t deflate_decompress(zip_source_t *, void *, void *, zip_uint64_t, zip_source_cmd_t);
static void deflate_free(struct deflate *);


zip_source_t *
zip_source_deflate(zip_t *za, zip_source_t *src, zip_int32_t cm, int flags) {
    struct deflate *ctx;
    zip_source_t *s2;

    if ((src == NULL) || ((cm != ZIP_CM_DEFLATE) && !ZIP_CM_IS_DEFAULT(cm))) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    if ((ctx = (struct deflate *)malloc(sizeof(*ctx))) == NULL) {
        zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    zip_error_init(&ctx->error);
    ctx->eof       = ZIP_FALSE;
    ctx->is_stored = ZIP_FALSE;
    ctx->can_store = ZIP_CM_IS_DEFAULT(cm);
    if (flags & ZIP_CODEC_ENCODE) {
        ctx->mem_level = MAX_MEM_LEVEL;
    }

    if ((s2 = zip_source_layered(za, src,
                                 ((flags & ZIP_CODEC_ENCODE)
                                  ? deflate_compress : deflate_decompress),
                                 ctx)) == NULL) {
        deflate_free(ctx);
        return NULL;
    }

    return s2;
}

static zip_int64_t
compress_read(zip_source_t *src, struct deflate *ctx, void *data, zip_uint64_t len) {
    int end, ret;
    zip_int64_t n;
    zip_uint64_t out_offset;
    uInt out_len;

    if (zip_error_code_zip(&ctx->error) != ZIP_ER_OK)
        return -1;

    if ((len == 0) || ctx->is_stored) {
        return 0;
    }

    out_offset          = 0;
    out_len             = (uInt)ZIP_MIN(UINT_MAX, len);
    ctx->zstr.next_out  = (Bytef *)data;
    ctx->zstr.avail_out = out_len;

    end = 0;
    while (!end) {
        ret = deflate(&ctx->zstr, ctx->eof ? Z_FINISH : 0);

        switch (ret) {
            case Z_STREAM_END:
                if (ctx->can_store && (ctx->zstr.total_in <= ctx->zstr.total_out)) {
                    ctx->is_stored = ZIP_TRUE;
                    ctx->size      = ctx->zstr.total_in;
                    memcpy(data, ctx->buffer, ctx->size);
                    return (zip_int64_t)ctx->size;
                }

            /* fallthrough */
            case Z_OK:
                /* all ok */

                if (ctx->zstr.avail_out == 0) {
                    out_offset += out_len;
                    if (out_offset < len) {
                        out_len             = (uInt)ZIP_MIN(UINT_MAX, len - out_offset);
                        ctx->zstr.next_out  = (Bytef *)data + out_offset;
                        ctx->zstr.avail_out = out_len;
                    } else {
                        ctx->can_store = ZIP_FALSE;
                        end            = 1;
                    }
                } else if (ctx->eof && (ctx->zstr.avail_in == 0))
                    end = 1;
                break;

            case Z_BUF_ERROR:
                if (ctx->zstr.avail_in == 0) {
                    if (ctx->eof) {
                        end = 1;
                        break;
                    }

                    if ((n = zip_source_read(src, ctx->buffer, sizeof(ctx->buffer))) < 0) {
                        _zip_error_set_from_source(&ctx->error, src);
                        end = 1;
                        break;
                    } else if (n == 0) {
                        ctx->eof = ZIP_TRUE;
                        /* TODO: check against stat of src? */
                        ctx->size = ctx->zstr.total_in;
                    } else {
                        if (ctx->zstr.total_in > 0) {
                            /* we overwrote a previously filled ctx->buffer */
                            ctx->can_store = ZIP_FALSE;
                        }
                        ctx->zstr.next_in  = (Bytef *)ctx->buffer;
                        ctx->zstr.avail_in = (uInt)n;
                    }
                    continue;
                }

            /* fallthrough */
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_STREAM_ERROR:
            case Z_MEM_ERROR:
                zip_error_set(&ctx->error, ZIP_ER_ZLIB, ret);

                end = 1;
                break;
        }
    }

    if (ctx->zstr.avail_out < len) {
        ctx->can_store = ZIP_FALSE;
        return (zip_int64_t)(len - ctx->zstr.avail_out);
    }

    return (zip_error_code_zip(&ctx->error) == ZIP_ER_OK) ? 0 : -1;
}

static zip_int64_t
decompress_read(zip_source_t *src, struct deflate *ctx, void *data, zip_uint64_t len) {
    int end, ret;
    zip_int64_t n;
    zip_uint64_t out_offset;
    uInt out_len;

    if (zip_error_code_zip(&ctx->error) != ZIP_ER_OK)
        return -1;

    if (len == 0)
        return 0;

    out_offset          = 0;
    out_len             = (uInt)ZIP_MIN(UINT_MAX, len);
    ctx->zstr.next_out  = (Bytef *)data;
    ctx->zstr.avail_out = out_len;

    end = 0;
    while (!end) {
        ret = inflate(&ctx->zstr, Z_SYNC_FLUSH);

        switch (ret) {
            case Z_OK:
                if (ctx->zstr.avail_out == 0) {
                    out_offset += out_len;
                    if (out_offset < len) {
                        out_len             = (uInt)ZIP_MIN(UINT_MAX, len - out_offset);
                        ctx->zstr.next_out  = (Bytef *)data + out_offset;
                        ctx->zstr.avail_out = out_len;
                    } else {
                        end = 1;
                    }
                }
                break;

            case Z_STREAM_END:
                ctx->eof = 1;
                end      = 1;
                break;

            case Z_BUF_ERROR:
                if (ctx->zstr.avail_in == 0) {
                    if (ctx->eof) {
                        end = 1;
                        break;
                    }

                    if ((n = zip_source_read(src, ctx->buffer, sizeof(ctx->buffer))) < 0) {
                        _zip_error_set_from_source(&ctx->error, src);
                        end = 1;
                        break;
                    } else if (n == 0) {
                        ctx->eof = 1;
                    } else {
                        ctx->zstr.next_in  = (Bytef *)ctx->buffer;
                        ctx->zstr.avail_in = (uInt)n;
                    }
                    continue;
                }

            /* fallthrough */
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_STREAM_ERROR:
            case Z_MEM_ERROR:
                zip_error_set(&ctx->error, ZIP_ER_ZLIB, ret);
                end = 1;
                break;
        }
    }

    if (ctx->zstr.avail_out < len)
        return (zip_int64_t)(len - ctx->zstr.avail_out);

    return (zip_error_code_zip(&ctx->error) == ZIP_ER_OK) ? 0 : -1;
}

static zip_int64_t
deflate_compress(zip_source_t *src, void *ud, void *data, zip_uint64_t len, zip_source_cmd_t cmd) {
    struct deflate *ctx;
    int ret;

    ctx = (struct deflate *)ud;

    switch (cmd) {
        case ZIP_SOURCE_OPEN:
            ctx->zstr.zalloc    = Z_NULL;
            ctx->zstr.zfree     = Z_NULL;
            ctx->zstr.opaque    = NULL;
            ctx->zstr.avail_in  = 0;
            ctx->zstr.next_in   = NULL;
            ctx->zstr.avail_out = 0;
            ctx->zstr.next_out  = NULL;

            /* negative value to tell zlib not to write a header */
            if ((ret = deflateInit2(&ctx->zstr, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, ctx->mem_level, Z_DEFAULT_STRATEGY)) != Z_OK) {
                zip_error_set(&ctx->error, ZIP_ER_ZLIB, ret);
                return -1;
            }

            return 0;

        case ZIP_SOURCE_READ:
            return compress_read(src, ctx, data, len);

        case ZIP_SOURCE_CLOSE:
            deflateEnd(&ctx->zstr);
            return 0;

        case ZIP_SOURCE_STAT:
            {
                zip_stat_t *st;

                st = (zip_stat_t *)data;

                st->comp_method = ctx->is_stored ? ZIP_CM_STORE : ZIP_CM_DEFLATE;
                st->valid      |= ZIP_STAT_COMP_METHOD;
                if (ctx->eof) {
                    st->comp_size = ctx->size;
                    st->valid    |= ZIP_STAT_COMP_SIZE;
                } else
                    st->valid &= ~ZIP_STAT_COMP_SIZE;
            }
            return 0;

        case ZIP_SOURCE_ERROR:
            return zip_error_to_data(&ctx->error, data, len);

        case ZIP_SOURCE_FREE:
            deflate_free(ctx);
            return 0;

        case ZIP_SOURCE_SUPPORTS:
            return ZIP_SOURCE_SUPPORTS_READABLE;

        default:
            zip_error_set(&ctx->error, ZIP_ER_INTERNAL, 0);
            return -1;
    }
}

static zip_int64_t
deflate_decompress(zip_source_t *src, void *ud, void *data,
                   zip_uint64_t len, zip_source_cmd_t cmd) {
    struct deflate *ctx;
    zip_int64_t n;
    int ret;

    ctx = (struct deflate *)ud;

    switch (cmd) {
        case ZIP_SOURCE_OPEN:
            if ((n = zip_source_read(src, ctx->buffer, sizeof(ctx->buffer))) < 0) {
                _zip_error_set_from_source(&ctx->error, src);
                return -1;
            }

            ctx->zstr.zalloc   = Z_NULL;
            ctx->zstr.zfree    = Z_NULL;
            ctx->zstr.opaque   = NULL;
            ctx->zstr.next_in  = (Bytef *)ctx->buffer;
            ctx->zstr.avail_in = (uInt)n;

            /* negative value to tell zlib that there is no header */
            if ((ret = inflateInit2(&ctx->zstr, -MAX_WBITS)) != Z_OK) {
                zip_error_set(&ctx->error, ZIP_ER_ZLIB, ret);
                return -1;
            }
            return 0;

        case ZIP_SOURCE_READ:
            return decompress_read(src, ctx, data, len);

        case ZIP_SOURCE_CLOSE:
            inflateEnd(&ctx->zstr);
            return 0;

        case ZIP_SOURCE_STAT:
            {
                zip_stat_t *st;

                st = (zip_stat_t *)data;

                st->comp_method = ZIP_CM_STORE;
                if ((st->comp_size > 0) && (st->size > 0))
                    st->comp_size = st->size;

                return 0;
            }

        case ZIP_SOURCE_ERROR:
            return zip_error_to_data(&ctx->error, data, len);

        case ZIP_SOURCE_FREE:
            free(ctx);
            return 0;

        case ZIP_SOURCE_SUPPORTS:
            return zip_source_make_command_bitmap(ZIP_SOURCE_OPEN, ZIP_SOURCE_READ, ZIP_SOURCE_CLOSE, ZIP_SOURCE_STAT, ZIP_SOURCE_ERROR, ZIP_SOURCE_FREE, -1);

        default:
            zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
            return -1;
    }
}

static void
deflate_free(struct deflate *ctx) {
    free(ctx);
}

/*
   zip_source_error.c -- get last error from zip_source
   Copyright (C) 2009-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



zip_error_t *
zip_source_error(zip_source_t *src) {
    return &src->error;
}

#ifndef _WIN32

/*
   zip_source_file.c -- create data source from file
   Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


 #include <errno.h>
 #include <stdio.h>



ZIP_EXTERN zip_source_t *
zip_source_file(zip_t *za, const char *fname, zip_uint64_t start, zip_int64_t len) {
    if (za == NULL)
        return NULL;

    return zip_source_file_create(fname, start, len, &za->error);
}

ZIP_EXTERN zip_source_t *
zip_source_file_create(const char *fname, zip_uint64_t start, zip_int64_t length, zip_error_t *error) {
    if ((fname == NULL) || (length < -1)) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    return _zip_source_file_or_p(fname, NULL, start, length, NULL, error);
}
#endif

/*
   zip_source_filep.c -- create data source from FILE *
   Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
/* WIN32 needs <fcntl.h> for _O_BINARY */
 #include <fcntl.h>
#endif

/* Windows sys/types.h does not provide these */
#ifndef S_ISREG
 #define S_ISREG(m)    (((m) & S_IFMT) == S_IFREG)
#endif
#ifndef S_IRWXG
 #define S_IRWXG    (S_IRWXU >> 3)
#endif
#ifndef S_IRWXO
 #define S_IRWXO    (S_IRWXG >> 3)
#endif

#ifdef _MSC_VER
/* MSVC doesn't have mode_t */
typedef int   mode_t;
#endif

struct read_file {
    zip_error_t error;      /* last error information */
    zip_int64_t supports;

    /* reading */
    char *fname;            /* name of file to read from */
    FILE *f;                /* file to read from */
    struct zip_stat st;     /* stat information passed in */
    zip_uint64_t start;     /* start offset of data to read */
    zip_uint64_t end;       /* end offset of data to read, 0 for up to EOF */
    zip_uint64_t current;   /* current offset */

    /* writing */
    char *tmpname;
    FILE *fout;
};

static zip_int64_t read_file(void *state, void *data, zip_uint64_t len, zip_source_cmd_t cmd);
static int create_temp_output(struct read_file *ctx);
static int _zip_fseek_u(FILE *f, zip_uint64_t offset, int whence, zip_error_t *error);
static int _zip_fseek(FILE *f, zip_int64_t offset, int whence, zip_error_t *error);


ZIP_EXTERN zip_source_t *
zip_source_filep(zip_t *za, FILE *file, zip_uint64_t start, zip_int64_t len) {
    if (za == NULL)
        return NULL;

    return zip_source_filep_create(file, start, len, &za->error);
}

ZIP_EXTERN zip_source_t *
zip_source_filep_create(FILE *file, zip_uint64_t start, zip_int64_t length, zip_error_t *error) {
    if ((file == NULL) || (length < -1)) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    return _zip_source_file_or_p(NULL, file, start, length, NULL, error);
}

zip_source_t *
_zip_source_file_or_p(const char *fname, FILE *file, zip_uint64_t start, zip_int64_t len, const zip_stat_t *st, zip_error_t *error) {
    struct read_file *ctx;
    zip_source_t *zs;

    if ((file == NULL) && (fname == NULL)) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    if ((ctx = (struct read_file *)malloc(sizeof(struct read_file))) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    ctx->fname = NULL;
    if (fname) {
        if ((ctx->fname = strdup(fname)) == NULL) {
            zip_error_set(error, ZIP_ER_MEMORY, 0);
            free(ctx);
            return NULL;
        }
    }
    ctx->f     = file;
    ctx->start = start;
    ctx->end   = (len < 0 ? 0 : start + (zip_uint64_t)len);
    if (st) {
        memcpy(&ctx->st, st, sizeof(ctx->st));
        ctx->st.name   = NULL;
        ctx->st.valid &= ~ZIP_STAT_NAME;
    } else {
        zip_stat_init(&ctx->st);
    }

    ctx->tmpname = NULL;
    ctx->fout    = NULL;

    zip_error_init(&ctx->error);

    ctx->supports = ZIP_SOURCE_SUPPORTS_READABLE | zip_source_make_command_bitmap(ZIP_SOURCE_SUPPORTS, ZIP_SOURCE_TELL, -1);
    if (ctx->fname) {
        struct stat sb;

        if ((stat(ctx->fname, &sb) < 0) || S_ISREG(sb.st_mode)) {
            ctx->supports = ZIP_SOURCE_SUPPORTS_WRITABLE;
        }
    } else if (fseeko(ctx->f, 0, SEEK_CUR) == 0) {
        ctx->supports = ZIP_SOURCE_SUPPORTS_SEEKABLE;
    }

    if ((zs = zip_source_function_create(read_file, ctx, error)) == NULL) {
        free(ctx->fname);
        free(ctx);
        return NULL;
    }

    return zs;
}

static int
create_temp_output(struct read_file *ctx) {
    char *temp;
    int tfd;
    mode_t mask;
    FILE *tfp;

    if ((temp = (char *)malloc(strlen(ctx->fname) + 8)) == NULL) {
        zip_error_set(&ctx->error, ZIP_ER_MEMORY, 0);
        return -1;
    }
    sprintf(temp, "%s.XXXXXX", ctx->fname);

    mask = umask(S_IXUSR | S_IRWXG | S_IRWXO);
    if ((tfd = mkstemp(temp)) == -1) {
        zip_error_set(&ctx->error, ZIP_ER_TMPOPEN, errno);
        umask(mask);
        free(temp);
        return -1;
    }
    umask(mask);

    if ((tfp = fdopen(tfd, "r+b")) == NULL) {
        zip_error_set(&ctx->error, ZIP_ER_TMPOPEN, errno);
        close(tfd);
        (void)remove(temp);
        free(temp);
        return -1;
    }

#ifdef _WIN32

    /*
       According to Pierre Joye, Windows in some environments per
       default creates text files, so force binary mode.
     */
    _setmode(_fileno(tfp), _O_BINARY);
#endif

    ctx->fout    = tfp;
    ctx->tmpname = temp;

    return 0;
}

static zip_int64_t
read_file(void *state, void *data, zip_uint64_t len, zip_source_cmd_t cmd) {
    struct read_file *ctx;
    char *buf;
    zip_uint64_t n;
    size_t i;

    ctx = (struct read_file *)state;
    buf = (char *)data;

    switch (cmd) {
        case ZIP_SOURCE_BEGIN_WRITE:
            if (ctx->fname == NULL) {
                zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
                return -1;
            }
            return create_temp_output(ctx);

        case ZIP_SOURCE_COMMIT_WRITE:
            {
                mode_t mask;

                if (fclose(ctx->fout) < 0) {
                    ctx->fout = NULL;
                    zip_error_set(&ctx->error, ZIP_ER_WRITE, errno);
                }
                ctx->fout = NULL;
                if (rename(ctx->tmpname, ctx->fname) < 0) {
                    zip_error_set(&ctx->error, ZIP_ER_RENAME, errno);
                    return -1;
                }
                mask = umask(022);
                umask(mask);
                /* not much we can do if chmod fails except make the whole commit fail */
                (void)chmod(ctx->fname, 0666 & ~mask);
                free(ctx->tmpname);
                ctx->tmpname = NULL;
                return 0;
            }

        case ZIP_SOURCE_CLOSE:
            if (ctx->fname) {
                fclose(ctx->f);
                ctx->f = NULL;
            }
            return 0;

        case ZIP_SOURCE_ERROR:
            return zip_error_to_data(&ctx->error, data, len);

        case ZIP_SOURCE_FREE:
            free(ctx->fname);
            free(ctx->tmpname);
            if (ctx->f)
                fclose(ctx->f);
            free(ctx);
            return 0;

        case ZIP_SOURCE_OPEN:
            if (ctx->fname) {
                if ((ctx->f = fopen(ctx->fname, "rb")) == NULL) {
                    zip_error_set(&ctx->error, ZIP_ER_OPEN, errno);
                    return -1;
                }
            }

            if (ctx->start > 0) {
                if (_zip_fseek_u(ctx->f, ctx->start, SEEK_SET, &ctx->error) < 0) {
                    return -1;
                }
            }
            ctx->current = ctx->start;
            return 0;

        case ZIP_SOURCE_READ:
            if (ctx->end > 0) {
                n = ctx->end - ctx->current;
                if (n > len) {
                    n = len;
                }
            } else {
                n = len;
            }

            if (n > SIZE_MAX)
                n = SIZE_MAX;

            if ((i = fread(buf, 1, (size_t)n, ctx->f)) == 0) {
                if (ferror(ctx->f)) {
                    zip_error_set(&ctx->error, ZIP_ER_READ, errno);
                    return -1;
                }
            }
            ctx->current += i;

            return (zip_int64_t)i;

        case ZIP_SOURCE_REMOVE:
            if (remove(ctx->fname) < 0) {
                zip_error_set(&ctx->error, ZIP_ER_REMOVE, errno);
                return -1;
            }
            return 0;

        case ZIP_SOURCE_ROLLBACK_WRITE:
            if (ctx->fout) {
                fclose(ctx->fout);
                ctx->fout = NULL;
            }
            (void)remove(ctx->tmpname);
            free(ctx->tmpname);
            ctx->tmpname = NULL;
            return 0;

        case ZIP_SOURCE_SEEK:
            {
                zip_int64_t new_current;
                int need_seek;
                zip_source_args_seek_t *args = ZIP_SOURCE_GET_ARGS(zip_source_args_seek_t, data, len, &ctx->error);

                if (args == NULL)
                    return -1;

                need_seek = 1;

                switch (args->whence) {
                    case SEEK_SET:
                        new_current = args->offset;
                        break;

                    case SEEK_END:
                        if (ctx->end == 0) {
                            if (_zip_fseek(ctx->f, args->offset, SEEK_END, &ctx->error) < 0) {
                                return -1;
                            }
                            if ((new_current = ftello(ctx->f)) < 0) {
                                zip_error_set(&ctx->error, ZIP_ER_SEEK, errno);
                                return -1;
                            }
                            need_seek = 0;
                        } else {
                            new_current = (zip_int64_t)ctx->end + args->offset;
                        }
                        break;

                    case SEEK_CUR:
                        new_current = (zip_int64_t)ctx->current + args->offset;
                        break;

                    default:
                        zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
                        return -1;
                }

                if ((new_current < 0) || ((zip_uint64_t)new_current < ctx->start) || ((ctx->end != 0) && ((zip_uint64_t)new_current > ctx->end))) {
                    zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
                    return -1;
                }

                ctx->current = (zip_uint64_t)new_current;

                if (need_seek) {
                    if (_zip_fseek_u(ctx->f, ctx->current, SEEK_SET, &ctx->error) < 0) {
                        return -1;
                    }
                }
                return 0;
            }

        case ZIP_SOURCE_SEEK_WRITE:
            {
                zip_source_args_seek_t *args;

                args = ZIP_SOURCE_GET_ARGS(zip_source_args_seek_t, data, len, &ctx->error);
                if (args == NULL) {
                    return -1;
                }

                if (_zip_fseek(ctx->fout, args->offset, args->whence, &ctx->error) < 0) {
                    return -1;
                }
                return 0;
            }

        case ZIP_SOURCE_STAT:
            {
                if (len < sizeof(ctx->st))
                    return -1;

                if (ctx->st.valid != 0)
                    memcpy(data, &ctx->st, sizeof(ctx->st));
                else {
                    zip_stat_t *st;
                    struct stat fst;
                    int err;

                    if (ctx->f)
                        err = fstat(fileno(ctx->f), &fst);
                    else
                        err = stat(ctx->fname, &fst);

                    if (err != 0) {
                        zip_error_set(&ctx->error, ZIP_ER_READ, errno);
                        return -1;
                    }

                    st = (zip_stat_t *)data;

                    zip_stat_init(st);
                    st->mtime  = fst.st_mtime;
                    st->valid |= ZIP_STAT_MTIME;
                    if (ctx->end != 0) {
                        st->size   = ctx->end - ctx->start;
                        st->valid |= ZIP_STAT_SIZE;
                    } else if ((fst.st_mode & S_IFMT) == S_IFREG) {
                        st->size   = (zip_uint64_t)fst.st_size;
                        st->valid |= ZIP_STAT_SIZE;
                    }
                }
                return sizeof(ctx->st);
            }

        case ZIP_SOURCE_SUPPORTS:
            return ctx->supports;

        case ZIP_SOURCE_TELL:
            return (zip_int64_t)ctx->current;

        case ZIP_SOURCE_TELL_WRITE:
            {
                off_t ret = ftello(ctx->fout);

                if (ret < 0) {
                    zip_error_set(&ctx->error, ZIP_ER_TELL, errno);
                    return -1;
                }
                return ret;
            }

        case ZIP_SOURCE_WRITE:
            {
                size_t ret;

                clearerr(ctx->fout);
                ret = fwrite(data, 1, len, ctx->fout);
                if ((ret != len) || ferror(ctx->fout)) {
                    zip_error_set(&ctx->error, ZIP_ER_WRITE, errno);
                    return -1;
                }

                return (zip_int64_t)ret;
            }

        default:
            zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
            return -1;
    }
}

static int
_zip_fseek_u(FILE *f, zip_uint64_t offset, int whence, zip_error_t *error) {
    if (offset > ZIP_INT64_MAX) {
        zip_error_set(error, ZIP_ER_SEEK, EOVERFLOW);
        return -1;
    }
    return _zip_fseek(f, (zip_int64_t)offset, whence, error);
}

static int
_zip_fseek(FILE *f, zip_int64_t offset, int whence, zip_error_t *error) {
    if ((offset > ZIP_FSEEK_MAX) || (offset < ZIP_FSEEK_MIN)) {
        zip_error_set(error, ZIP_ER_SEEK, EOVERFLOW);
        return -1;
    }
    if (fseeko(f, (off_t)offset, whence) < 0) {
        zip_error_set(error, ZIP_ER_SEEK, errno);
        return -1;
    }
    return 0;
}

/*
   zip_source_free.c -- free zip data source
   Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>



ZIP_EXTERN void
zip_source_free(zip_source_t *src) {
    if (src == NULL)
        return;

    if (src->refcount > 0) {
        src->refcount--;
    }
    if (src->refcount > 0) {
        return;
    }

    if (ZIP_SOURCE_IS_OPEN_READING(src)) {
        src->open_count = 1; /* force close */
        zip_source_close(src);
    }
    if (ZIP_SOURCE_IS_OPEN_WRITING(src)) {
        zip_source_rollback_write(src);
    }

    if (src->source_archive && !src->source_closed) {
        _zip_deregister_source(src->source_archive, src);
    }

    (void)_zip_source_call(src, NULL, 0, ZIP_SOURCE_FREE);

    if (src->src) {
        zip_source_free(src->src);
    }

    free(src);
}

/*
   zip_source_function.c -- create zip data source from callback function
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>



ZIP_EXTERN zip_source_t *
zip_source_function(zip_t *za, zip_source_callback zcb, void *ud) {
    if (za == NULL) {
        return NULL;
    }

    return zip_source_function_create(zcb, ud, &za->error);
}

ZIP_EXTERN zip_source_t *
zip_source_function_create(zip_source_callback zcb, void *ud, zip_error_t *error) {
    zip_source_t *zs;

    if ((zs = _zip_source_new(error)) == NULL)
        return NULL;

    zs->cb.f = zcb;
    zs->ud   = ud;

    zs->supports = zcb(ud, NULL, 0, ZIP_SOURCE_SUPPORTS);
    if (zs->supports < 0) {
        zs->supports = ZIP_SOURCE_SUPPORTS_READABLE;
    }

    return zs;
}

ZIP_EXTERN void
zip_source_keep(zip_source_t *src) {
    src->refcount++;
}

zip_source_t *
_zip_source_new(zip_error_t *error) {
    zip_source_t *src;

    if ((src = (zip_source_t *)malloc(sizeof(*src))) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    src->src            = NULL;
    src->cb.f           = NULL;
    src->ud             = NULL;
    src->open_count     = 0;
    src->write_state    = ZIP_SOURCE_WRITE_CLOSED;
    src->source_closed  = ZIP_FALSE;
    src->source_archive = NULL;
    src->refcount       = 1;
    zip_error_init(&src->error);

    return src;
}

/*
   zip_source_is_deleted.c -- was archive was removed?
   Copyright (C) 2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_source_is_deleted(zip_source_t *src) {
    return src->write_state == ZIP_SOURCE_WRITE_REMOVED;
}

/*
   zip_source_layered.c -- create layered source
   Copyright (C) 2009-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>



zip_source_t *
zip_source_layered(zip_t *za, zip_source_t *src, zip_source_layered_callback cb, void *ud) {
    if (za == NULL)
        return NULL;

    return zip_source_layered_create(src, cb, ud, &za->error);
}

zip_source_t *
zip_source_layered_create(zip_source_t *src, zip_source_layered_callback cb, void *ud, zip_error_t *error) {
    zip_source_t *zs;

    if ((zs = _zip_source_new(error)) == NULL)
        return NULL;

    zip_source_keep(src);
    zs->src  = src;
    zs->cb.l = cb;
    zs->ud   = ud;

    zs->supports = cb(src, ud, NULL, 0, ZIP_SOURCE_SUPPORTS);
    if (zs->supports < 0) {
        zs->supports = ZIP_SOURCE_SUPPORTS_READABLE;
    }

    return zs;
}

/*
   zip_source_open.c -- open zip_source (prepare for reading)
   Copyright (C) 2009-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_source_open(zip_source_t *src) {
    if (src->source_closed) {
        return -1;
    }
    if (src->write_state == ZIP_SOURCE_WRITE_REMOVED) {
        zip_error_set(&src->error, ZIP_ER_DELETED, 0);
        return -1;
    }

    if (ZIP_SOURCE_IS_OPEN_READING(src)) {
        if ((zip_source_supports(src) & ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_SEEK)) == 0) {
            zip_error_set(&src->error, ZIP_ER_INUSE, 0);
            return -1;
        }
    } else {
        if (ZIP_SOURCE_IS_LAYERED(src)) {
            if (zip_source_open(src->src) < 0) {
                _zip_error_set_from_source(&src->error, src->src);
                return -1;
            }
        }

        if (_zip_source_call(src, NULL, 0, ZIP_SOURCE_OPEN) < 0) {
            if (ZIP_SOURCE_IS_LAYERED(src)) {
                zip_source_close(src->src);
            }
            return -1;
        }
    }

    src->open_count++;

    return 0;
}

/*
   zip_source_pkware.c -- Traditional PKWARE de/encryption routines
   Copyright (C) 2009-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>
#include <string.h>



struct trad_pkware {
    zip_error_t error;
    zip_uint32_t key[3];
};

#define HEADERLEN    12
#define KEY0         305419896
#define KEY1         591751049
#define KEY2         878082192


static void decrypt(struct trad_pkware *, zip_uint8_t *,
                    const zip_uint8_t *, zip_uint64_t, int);
static int decrypt_header(zip_source_t *, struct trad_pkware *);
static zip_int64_t pkware_decrypt(zip_source_t *, void *, void *,
                                  zip_uint64_t, zip_source_cmd_t);
static void pkware_free(struct trad_pkware *);


zip_source_t *
zip_source_pkware(zip_t *za, zip_source_t *src,
                  zip_uint16_t em, int flags, const char *password) {
    struct trad_pkware *ctx;
    zip_source_t *s2;

    if ((password == NULL) || (src == NULL) || (em != ZIP_EM_TRAD_PKWARE)) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return NULL;
    }
    if (flags & ZIP_CODEC_ENCODE) {
        zip_error_set(&za->error, ZIP_ER_ENCRNOTSUPP, 0);
        return NULL;
    }

    if ((ctx = (struct trad_pkware *)malloc(sizeof(*ctx))) == NULL) {
        zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    zip_error_init(&ctx->error);

    ctx->key[0] = KEY0;
    ctx->key[1] = KEY1;
    ctx->key[2] = KEY2;
    decrypt(ctx, NULL, (const zip_uint8_t *)password, strlen(password), 1);

    if ((s2 = zip_source_layered(za, src, pkware_decrypt, ctx)) == NULL) {
        pkware_free(ctx);
        return NULL;
    }

    return s2;
}

static void
decrypt(struct trad_pkware *ctx, zip_uint8_t *out, const zip_uint8_t *in,
        zip_uint64_t len, int update_only) {
    zip_uint16_t tmp;
    zip_uint64_t i;
    Bytef b;

    for (i = 0; i < len; i++) {
        b = in[i];

        if (!update_only) {
            /* decrypt next byte */
            tmp = (zip_uint16_t)(ctx->key[2] | 2);
            tmp = (zip_uint16_t)((tmp * (tmp ^ 1)) >> 8);
            b  ^= (Bytef)tmp;
        }

        /* store cleartext */
        if (out)
            out[i] = b;

        /* update keys */
        ctx->key[0] = (zip_uint32_t)crc32(ctx->key[0] ^ 0xffffffffUL, &b, 1) ^ 0xffffffffUL;
        ctx->key[1] = (ctx->key[1] + (ctx->key[0] & 0xff)) * 134775813 + 1;
        b           = (Bytef)(ctx->key[1] >> 24);
        ctx->key[2] = (zip_uint32_t)crc32(ctx->key[2] ^ 0xffffffffUL, &b, 1) ^ 0xffffffffUL;
    }
}

static int
decrypt_header(zip_source_t *src, struct trad_pkware *ctx) {
    zip_uint8_t header[HEADERLEN];
    struct zip_stat st;
    zip_int64_t n;
    unsigned short dostime, dosdate;

    if ((n = zip_source_read(src, header, HEADERLEN)) < 0) {
        _zip_error_set_from_source(&ctx->error, src);
        return -1;
    }

    if (n != HEADERLEN) {
        zip_error_set(&ctx->error, ZIP_ER_EOF, 0);
        return -1;
    }

    decrypt(ctx, header, header, HEADERLEN, 0);

    if (zip_source_stat(src, &st) < 0) {
        /* stat failed, skip password validation */
        return 0;
    }

    _zip_u2d_time(st.mtime, &dostime, &dosdate);

    if ((header[HEADERLEN - 1] != st.crc >> 24) && (header[HEADERLEN - 1] != dostime >> 8)) {
        zip_error_set(&ctx->error, ZIP_ER_WRONGPASSWD, 0);
        return -1;
    }

    return 0;
}

static zip_int64_t
pkware_decrypt(zip_source_t *src, void *ud, void *data,
               zip_uint64_t len, zip_source_cmd_t cmd) {
    struct trad_pkware *ctx;
    zip_int64_t n;

    ctx = (struct trad_pkware *)ud;

    switch (cmd) {
        case ZIP_SOURCE_OPEN:
            if (decrypt_header(src, ctx) < 0)
                return -1;
            return 0;

        case ZIP_SOURCE_READ:
            if ((n = zip_source_read(src, data, len)) < 0) {
                _zip_error_set_from_source(&ctx->error, src);
                return -1;
            }

            decrypt((struct trad_pkware *)ud, (zip_uint8_t *)data, (zip_uint8_t *)data, (zip_uint64_t)n, 0);
            return n;

        case ZIP_SOURCE_CLOSE:
            return 0;

        case ZIP_SOURCE_STAT:
            {
                zip_stat_t *st;

                st = (zip_stat_t *)data;

                st->encryption_method = ZIP_EM_NONE;
                st->valid            |= ZIP_STAT_ENCRYPTION_METHOD;
                /* TODO: deduce HEADERLEN from size for uncompressed */
                if (st->valid & ZIP_STAT_COMP_SIZE)
                    st->comp_size -= HEADERLEN;

                return 0;
            }

        case ZIP_SOURCE_SUPPORTS:
            return zip_source_make_command_bitmap(ZIP_SOURCE_OPEN, ZIP_SOURCE_READ, ZIP_SOURCE_CLOSE, ZIP_SOURCE_STAT, ZIP_SOURCE_ERROR, ZIP_SOURCE_FREE, -1);

        case ZIP_SOURCE_ERROR:
            return zip_error_to_data(&ctx->error, data, len);

        case ZIP_SOURCE_FREE:
            pkware_free(ctx);
            return 0;

        default:
            zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
            return -1;
    }
}

static void
pkware_free(struct trad_pkware *ctx) {
    free(ctx);
}

/*
   zip_source_read.c -- read data from zip_source
   Copyright (C) 2009-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



zip_int64_t
zip_source_read(zip_source_t *src, void *data, zip_uint64_t len) {
    if (src->source_closed) {
        return -1;
    }
    if (!ZIP_SOURCE_IS_OPEN_READING(src) || (len > ZIP_INT64_MAX) || ((len > 0) && (data == NULL))) {
        zip_error_set(&src->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    return _zip_source_call(src, data, len, ZIP_SOURCE_READ);
}

/*
   zip_source_remove.c -- remove empty archive
   Copyright (C) 2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
   3. The names of the authors may not be used to endorse or promote
   products derived from this software without specific prior
   written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



int
zip_source_remove(zip_source_t *src) {
    if (src->write_state == ZIP_SOURCE_WRITE_REMOVED) {
        return 0;
    }

    if (ZIP_SOURCE_IS_OPEN_READING(src)) {
        if (zip_source_close(src) < 0) {
            return -1;
        }
    }
    if (src->write_state != ZIP_SOURCE_WRITE_CLOSED) {
        zip_source_rollback_write(src);
    }

    if (_zip_source_call(src, NULL, 0, ZIP_SOURCE_REMOVE) < 0) {
        return -1;
    }

    src->write_state = ZIP_SOURCE_WRITE_REMOVED;

    return 0;
}

/*
   zip_source_rollback_write.c -- discard changes
   Copyright (C) 2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN void
zip_source_rollback_write(zip_source_t *src) {
    if ((src->write_state != ZIP_SOURCE_WRITE_OPEN) && (src->write_state != ZIP_SOURCE_WRITE_FAILED)) {
        return;
    }

    _zip_source_call(src, NULL, 0, ZIP_SOURCE_ROLLBACK_WRITE);
    src->write_state = ZIP_SOURCE_WRITE_CLOSED;
}

/*
   zip_source_seek.c -- seek to offset
   Copyright (C) 2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_source_seek(zip_source_t *src, zip_int64_t offset, int whence) {
    zip_source_args_seek_t args;

    if (src->source_closed) {
        return -1;
    }
    if (!ZIP_SOURCE_IS_OPEN_READING(src) || ((whence != SEEK_SET) && (whence != SEEK_CUR) && (whence != SEEK_END))) {
        zip_error_set(&src->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    args.offset = offset;
    args.whence = whence;

    return _zip_source_call(src, &args, sizeof(args), ZIP_SOURCE_SEEK) < 0 ? -1 : 0;
}

zip_int64_t
zip_source_seek_compute_offset(zip_uint64_t offset, zip_uint64_t length, void *data, zip_uint64_t data_length, zip_error_t *error) {
    zip_int64_t new_offset;
    zip_source_args_seek_t *args = ZIP_SOURCE_GET_ARGS(zip_source_args_seek_t, data, data_length, error);

    if (args == NULL) {
        return -1;
    }

    switch (args->whence) {
        case SEEK_CUR:
            new_offset = (zip_int64_t)offset + args->offset;
            break;

        case SEEK_END:
            new_offset = (zip_int64_t)length + args->offset;
            break;

        case SEEK_SET:
            new_offset = args->offset;
            break;

        default:
            zip_error_set(error, ZIP_ER_INVAL, 0);
            return -1;
    }

    if ((new_offset < 0) || ((zip_uint64_t)new_offset > length)) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return -1;
    }

    return new_offset;
}

/*
   zip_source_seek_write.c -- seek to offset for writing
   Copyright (C) 2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_source_seek_write(zip_source_t *src, zip_int64_t offset, int whence) {
    zip_source_args_seek_t args;

    if (!ZIP_SOURCE_IS_OPEN_WRITING(src) || ((whence != SEEK_SET) && (whence != SEEK_CUR) && (whence != SEEK_END))) {
        zip_error_set(&src->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    args.offset = offset;
    args.whence = whence;

    return _zip_source_call(src, &args, sizeof(args), ZIP_SOURCE_SEEK_WRITE) < 0 ? -1 : 0;
}

/*
   zip_source_stat.c -- get meta information from zip_source
   Copyright (C) 2009-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



int
zip_source_stat(zip_source_t *src, zip_stat_t *st) {
    if (src->source_closed) {
        return -1;
    }
    if (st == NULL) {
        zip_error_set(&src->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    zip_stat_init(st);

    if (ZIP_SOURCE_IS_LAYERED(src)) {
        if (zip_source_stat(src->src, st) < 0) {
            _zip_error_set_from_source(&src->error, src->src);
            return -1;
        }
    }

    if (_zip_source_call(src, st, sizeof(*st), ZIP_SOURCE_STAT) < 0) {
        return -1;
    }

    return 0;
}

/*
   zip_source_supports.c -- check for supported functions
   Copyright (C) 2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdarg.h>



zip_int64_t
zip_source_supports(zip_source_t *src) {
    return src->supports;
}

zip_int64_t
zip_source_make_command_bitmap(zip_source_cmd_t cmd0, ...) {
    zip_int64_t bitmap;
    va_list ap;

    bitmap = ZIP_SOURCE_MAKE_COMMAND_BITMASK(cmd0);



    va_start(ap, cmd0);
    for ( ; ; ) {
        int cmd = va_arg(ap, int);
        if (cmd < 0) {
            break;
        }
        bitmap |= ZIP_SOURCE_MAKE_COMMAND_BITMASK(cmd);
    }
    va_end(ap);

    return bitmap;
}

/*
   zip_source_tell.c -- report current offset
   Copyright (C) 2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN zip_int64_t
zip_source_tell(zip_source_t *src) {
    if (src->source_closed) {
        return -1;
    }
    if (!ZIP_SOURCE_IS_OPEN_READING(src)) {
        zip_error_set(&src->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    return _zip_source_call(src, NULL, 0, ZIP_SOURCE_TELL);
}

/*
   zip_source_tell_write.c -- report current offset for writing
   Copyright (C) 2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN zip_int64_t
zip_source_tell_write(zip_source_t *src) {
    if (!ZIP_SOURCE_IS_OPEN_WRITING(src)) {
        zip_error_set(&src->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    return _zip_source_call(src, NULL, 0, ZIP_SOURCE_TELL_WRITE);
}

/*
   zip_source_win32a.c -- create data source from Windows file (ANSI)
   Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
   3. The names of the authors may not be used to endorse or promote
   products derived from this software without specific prior
   written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef _WIN32
 #ifndef _HAD_ZIPWIN32_H
  #define _HAD_ZIPWIN32_H

/*
   zipwin32.h -- internal declarations for Windows.
   Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
   3. The names of the authors may not be used to endorse or promote
   products derived from this software without specific prior
   written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

  #include <windows.h>

/* context for Win32 source */

struct _zip_source_win32_file_ops;

struct _zip_source_win32_read_file {
    zip_error_t error;      /* last error information */
    zip_int64_t supports;

    /* operations */
    struct _zip_source_win32_file_ops *ops;

    /* reading */
    void *fname;            /* name of file to read from - ANSI (char *) or Unicode (wchar_t *) */
    void *h;                /* HANDLE for file to read from */
    int closep;             /* whether to close f on ZIP_CMD_FREE */
    struct zip_stat st;     /* stat information passed in */
    zip_uint64_t start;     /* start offset of data to read */
    zip_uint64_t end;       /* end offset of data to read, 0 for up to EOF */
    zip_uint64_t current;   /* current offset */

    /* writing */
    void *tmpname;          /* name of temp file - ANSI (char *) or Unicode (wchar_t *) */
    void *hout;             /* HANDLE for output file */
};

typedef struct _zip_source_win32_read_file   _zip_source_win32_read_file_t;

/* internal operations for Win32 source */

struct _zip_source_win32_file_ops {
    void *(*op_strdup)(const void *);
    void *(*op_open)(_zip_source_win32_read_file_t *);
    void *(*op_create_temp)(_zip_source_win32_read_file_t *, void **, zip_uint32_t, PSECURITY_ATTRIBUTES);
    int (*op_rename_temp)(_zip_source_win32_read_file_t *);
    int (*op_remove)(const void *);
};

typedef struct _zip_source_win32_file_ops   _zip_source_win32_file_ops_t;

zip_source_t *_zip_source_win32_handle_or_name(const void *, void *, zip_uint64_t, zip_int64_t, int, const zip_stat_t *, _zip_source_win32_file_ops_t *, zip_error_t *);
 #endif /* zipwin32.h */

 #include <errno.h>
 #include <stdio.h>


static void *_win32_strdup_a(const void *str);
static HANDLE _win32_open_a(_zip_source_win32_read_file_t *ctx);
static HANDLE _win32_create_temp_a(_zip_source_win32_read_file_t *ctx, void **temp, zip_uint32_t value, PSECURITY_ATTRIBUTES sa);
static int _win32_rename_temp_a(_zip_source_win32_read_file_t *ctx);
static int _win32_remove_a(const void *fname);


static _zip_source_win32_file_ops_t win32_ops_a = { _win32_strdup_a, _win32_open_a, _win32_create_temp_a, _win32_rename_temp_a, _win32_remove_a };

ZIP_EXTERN zip_source_t *
zip_source_win32a(zip_t *za, const char *fname, zip_uint64_t start, zip_int64_t len) {
    if (za == NULL)
        return NULL;

    return zip_source_win32a_create(fname, start, len, &za->error);
}

ZIP_EXTERN zip_source_t *
zip_source_win32a_create(const char *fname, zip_uint64_t start, zip_int64_t length, zip_error_t *error) {
    if ((fname == NULL) || (length < -1)) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    return _zip_source_win32_handle_or_name(fname, INVALID_HANDLE_VALUE, start, length, 1, NULL, &win32_ops_a, error);
}

static void *
_win32_strdup_a(const void *str) {
    return strdup((const char *)str);
}

static HANDLE
_win32_open_a(_zip_source_win32_read_file_t *ctx) {
    return CreateFileA((char *)ctx->fname, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

static HANDLE
_win32_create_temp_a(_zip_source_win32_read_file_t *ctx, void **temp, zip_uint32_t value, PSECURITY_ATTRIBUTES sa) {
    int len;

    len = strlen((const char *)ctx->fname) + 10;
    if (*temp == NULL) {
        if ((*temp = malloc(sizeof(char) * len)) == NULL) {
            zip_error_set(&ctx->error, ZIP_ER_MEMORY, 0);
            return INVALID_HANDLE_VALUE;
        }
    }
    if (sprintf((char *)*temp, "%s.%08x", (const char *)ctx->fname, value) != len - 1) {
        return INVALID_HANDLE_VALUE;
    }

    return CreateFileA((const char *)*temp, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, sa, CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY, NULL);
}

static int
_win32_rename_temp_a(_zip_source_win32_read_file_t *ctx) {
    if (!MoveFileExA((char *)ctx->tmpname, (char *)ctx->fname, MOVEFILE_REPLACE_EXISTING))
        return -1;
    return 0;
}

static int
_win32_remove_a(const void *fname) {
    DeleteFileA((const char *)fname);
    return 0;
}

/*
   zip_source_win32file.c -- create data source from HANDLE (Win32)
   Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
   3. The names of the authors may not be used to endorse or promote
   products derived from this software without specific prior
   written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


 #include <wchar.h>
 #include <errno.h>
 #include <stdlib.h>
 #include <string.h>


static zip_int64_t _win32_read_file(void *state, void *data, zip_uint64_t len, zip_source_cmd_t cmd);
static int _win32_create_temp_file(_zip_source_win32_read_file_t *ctx);
static int _zip_filetime_to_time_t(FILETIME ft, time_t *t);
static int _zip_seek_win32_u(void *h, zip_uint64_t offset, int whence, zip_error_t *error);
static int _zip_seek_win32(void *h, zip_int64_t offset, int whence, zip_error_t *error);
static int _zip_win32_error_to_errno(unsigned long win32err);
static int _zip_stat_win32(void *h, zip_stat_t *st, _zip_source_win32_read_file_t *ctx);

ZIP_EXTERN zip_source_t *
zip_source_win32handle(zip_t *za, HANDLE h, zip_uint64_t start, zip_int64_t len) {
    if (za == NULL)
        return NULL;

    return zip_source_win32handle_create(h, start, len, &za->error);
}

ZIP_EXTERN zip_source_t *
zip_source_win32handle_create(HANDLE h, zip_uint64_t start, zip_int64_t length, zip_error_t *error) {
    if ((h == INVALID_HANDLE_VALUE) || (length < -1)) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    return _zip_source_win32_handle_or_name(NULL, h, start, length, 1, NULL, NULL, error);
}

zip_source_t *
_zip_source_win32_handle_or_name(const void *fname, HANDLE h, zip_uint64_t start, zip_int64_t len, int closep, const zip_stat_t *st, _zip_source_win32_file_ops_t *ops, zip_error_t *error) {
    _zip_source_win32_read_file_t *ctx;
    zip_source_t *zs;

    if ((h == INVALID_HANDLE_VALUE) && (fname == NULL)) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    if ((ctx = (_zip_source_win32_read_file_t *)malloc(sizeof(_zip_source_win32_read_file_t))) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    ctx->fname = NULL;
    if (fname) {
        if ((ctx->fname = ops->op_strdup(fname)) == NULL) {
            zip_error_set(error, ZIP_ER_MEMORY, 0);
            free(ctx);
            return NULL;
        }
    }

    ctx->ops    = ops;
    ctx->h      = h;
    ctx->start  = start;
    ctx->end    = (len < 0 ? 0 : start + (zip_uint64_t)len);
    ctx->closep = ctx->fname ? 1 : closep;
    if (st) {
        memcpy(&ctx->st, st, sizeof(ctx->st));
        ctx->st.name   = NULL;
        ctx->st.valid &= ~ZIP_STAT_NAME;
    } else {
        zip_stat_init(&ctx->st);
    }

    ctx->tmpname = NULL;
    ctx->hout    = INVALID_HANDLE_VALUE;

    zip_error_init(&ctx->error);

    ctx->supports = ZIP_SOURCE_SUPPORTS_READABLE | zip_source_make_command_bitmap(ZIP_SOURCE_SUPPORTS, ZIP_SOURCE_TELL, -1);
    if (ctx->fname) {
        HANDLE th;

        th = ops->op_open(ctx);
        if ((th == INVALID_HANDLE_VALUE) || (GetFileType(th) == FILE_TYPE_DISK)) {
            ctx->supports = ZIP_SOURCE_SUPPORTS_WRITABLE;
        }
        if (th != INVALID_HANDLE_VALUE) {
            CloseHandle(th);
        }
    } else if (GetFileType(ctx->h) == FILE_TYPE_DISK) {
        ctx->supports = ZIP_SOURCE_SUPPORTS_SEEKABLE;
    }

    if ((zs = zip_source_function_create(_win32_read_file, ctx, error)) == NULL) {
        free(ctx->fname);
        free(ctx);
        return NULL;
    }

    return zs;
}

static zip_int64_t
_win32_read_file(void *state, void *data, zip_uint64_t len, zip_source_cmd_t cmd) {
    _zip_source_win32_read_file_t *ctx;
    char *buf;
    zip_uint64_t n;
    DWORD i;

    ctx = (_zip_source_win32_read_file_t *)state;
    buf = (char *)data;

    switch (cmd) {
        case ZIP_SOURCE_BEGIN_WRITE:
            if (ctx->fname == NULL) {
                zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
                return -1;
            }
            return _win32_create_temp_file(ctx);

        case ZIP_SOURCE_COMMIT_WRITE:
            {
                if (!CloseHandle(ctx->hout)) {
                    ctx->hout = INVALID_HANDLE_VALUE;
                    zip_error_set(&ctx->error, ZIP_ER_WRITE, _zip_win32_error_to_errno(GetLastError()));
                }
                ctx->hout = INVALID_HANDLE_VALUE;
                if (ctx->ops->op_rename_temp(ctx) < 0) {
                    zip_error_set(&ctx->error, ZIP_ER_RENAME, _zip_win32_error_to_errno(GetLastError()));
                    return -1;
                }
                free(ctx->tmpname);
                ctx->tmpname = NULL;
                return 0;
            }

        case ZIP_SOURCE_CLOSE:
            if (ctx->fname) {
                CloseHandle(ctx->h);
                ctx->h = INVALID_HANDLE_VALUE;
            }
            return 0;

        case ZIP_SOURCE_ERROR:
            return zip_error_to_data(&ctx->error, data, len);

        case ZIP_SOURCE_FREE:
            free(ctx->fname);
            free(ctx->tmpname);
            if (ctx->closep && (ctx->h != INVALID_HANDLE_VALUE))
                CloseHandle(ctx->h);
            free(ctx);
            return 0;

        case ZIP_SOURCE_OPEN:
            if (ctx->fname) {
                if ((ctx->h = ctx->ops->op_open(ctx)) == INVALID_HANDLE_VALUE) {
                    zip_error_set(&ctx->error, ZIP_ER_OPEN, _zip_win32_error_to_errno(GetLastError()));
                    return -1;
                }
            }

            if (ctx->closep && (ctx->start > 0)) {
                if (_zip_seek_win32_u(ctx->h, ctx->start, SEEK_SET, &ctx->error) < 0) {
                    return -1;
                }
            }
            ctx->current = ctx->start;
            return 0;

        case ZIP_SOURCE_READ:
            if (ctx->end > 0) {
                n = ctx->end - ctx->current;
                if (n > len) {
                    n = len;
                }
            } else {
                n = len;
            }

            if (n > SIZE_MAX)
                n = SIZE_MAX;

            if (!ctx->closep) {
                if (_zip_seek_win32_u(ctx->h, ctx->current, SEEK_SET, &ctx->error) < 0) {
                    return -1;
                }
            }

            if (!ReadFile(ctx->h, buf, (DWORD)n, &i, NULL)) {
                zip_error_set(&ctx->error, ZIP_ER_READ, _zip_win32_error_to_errno(GetLastError()));
                return -1;
            }
            ctx->current += i;

            return (zip_int64_t)i;

        case ZIP_SOURCE_REMOVE:
            if (ctx->ops->op_remove(ctx->fname) < 0) {
                zip_error_set(&ctx->error, ZIP_ER_REMOVE, _zip_win32_error_to_errno(GetLastError()));
                return -1;
            }
            return 0;

        case ZIP_SOURCE_ROLLBACK_WRITE:
            if (ctx->hout) {
                CloseHandle(ctx->hout);
                ctx->hout = INVALID_HANDLE_VALUE;
            }
            ctx->ops->op_remove(ctx->tmpname);
            free(ctx->tmpname);
            ctx->tmpname = NULL;
            return 0;

        case ZIP_SOURCE_SEEK:
            {
                zip_int64_t new_current;
                int need_seek;
                zip_source_args_seek_t *args = ZIP_SOURCE_GET_ARGS(zip_source_args_seek_t, data, len, &ctx->error);

                if (args == NULL)
                    return -1;

                need_seek = ctx->closep;

                switch (args->whence) {
                    case SEEK_SET:
                        new_current = args->offset;
                        break;

                    case SEEK_END:
                        if (ctx->end == 0) {
                            LARGE_INTEGER zero;
                            LARGE_INTEGER new_offset;

                            if (_zip_seek_win32(ctx->h, args->offset, SEEK_END, &ctx->error) < 0) {
                                return -1;
                            }
                            zero.QuadPart = 0;
                            if (!SetFilePointerEx(ctx->h, zero, &new_offset, FILE_CURRENT)) {
                                zip_error_set(&ctx->error, ZIP_ER_SEEK, _zip_win32_error_to_errno(GetLastError()));
                                return -1;
                            }
                            new_current = new_offset.QuadPart;
                            need_seek   = 0;
                        } else {
                            new_current = (zip_int64_t)ctx->end + args->offset;
                        }
                        break;

                    case SEEK_CUR:
                        new_current = (zip_int64_t)ctx->current + args->offset;
                        break;

                    default:
                        zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
                        return -1;
                }

                if ((new_current < 0) || ((zip_uint64_t)new_current < ctx->start) || ((ctx->end != 0) && ((zip_uint64_t)new_current > ctx->end))) {
                    zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
                    return -1;
                }

                ctx->current = (zip_uint64_t)new_current;

                if (need_seek) {
                    if (_zip_seek_win32_u(ctx->h, ctx->current, SEEK_SET, &ctx->error) < 0) {
                        return -1;
                    }
                }
                return 0;
            }

        case ZIP_SOURCE_SEEK_WRITE:
            {
                zip_source_args_seek_t *args;

                args = ZIP_SOURCE_GET_ARGS(zip_source_args_seek_t, data, len, &ctx->error);
                if (args == NULL) {
                    return -1;
                }

                if (_zip_seek_win32(ctx->hout, args->offset, args->whence, &ctx->error) < 0) {
                    return -1;
                }
                return 0;
            }

        case ZIP_SOURCE_STAT:
            {
                if (len < sizeof(ctx->st))
                    return -1;

                if (ctx->st.valid != 0)
                    memcpy(data, &ctx->st, sizeof(ctx->st));
                else {
                    DWORD win32err;
                    zip_stat_t *st;
                    HANDLE h;
                    int success;

                    st = (zip_stat_t *)data;

                    if (ctx->h != INVALID_HANDLE_VALUE) {
                        h = ctx->h;
                    } else {
                        h = ctx->ops->op_open(ctx);
                        if ((h == INVALID_HANDLE_VALUE) && (GetLastError() == ERROR_FILE_NOT_FOUND)) {
                            zip_error_set(&ctx->error, ZIP_ER_READ, ENOENT);
                            return -1;
                        }
                    }

                    success  = _zip_stat_win32(h, st, ctx);
                    win32err = GetLastError();

                    /* We're done with the handle, so close it if we just opened it. */
                    if (h != ctx->h) {
                        CloseHandle(h);
                    }

                    if (success < 0) {
                        /* TODO: Is this the correct error to return in all cases? */
                        zip_error_set(&ctx->error, ZIP_ER_READ, _zip_win32_error_to_errno(win32err));
                        return -1;
                    }
                }
                return sizeof(ctx->st);
            }

        case ZIP_SOURCE_SUPPORTS:
            return ctx->supports;

        case ZIP_SOURCE_TELL:
            return (zip_int64_t)ctx->current;

        case ZIP_SOURCE_TELL_WRITE:
            {
                LARGE_INTEGER zero;
                LARGE_INTEGER offset;

                zero.QuadPart = 0;
                if (!SetFilePointerEx(ctx->hout, zero, &offset, FILE_CURRENT)) {
                    zip_error_set(&ctx->error, ZIP_ER_TELL, _zip_win32_error_to_errno(GetLastError()));
                    return -1;
                }

                return offset.QuadPart;
            }

        case ZIP_SOURCE_WRITE:
            {
                DWORD ret;
                if (!WriteFile(ctx->hout, data, (DWORD)len, &ret, NULL) || (ret != len)) {
                    zip_error_set(&ctx->error, ZIP_ER_WRITE, _zip_win32_error_to_errno(GetLastError()));
                    return -1;
                }

                return (zip_int64_t)ret;
            }

        default:
            zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
            return -1;
    }
}

static int
_win32_create_temp_file(_zip_source_win32_read_file_t *ctx) {
    /*
       Windows has GetTempFileName(), but it closes the file after
       creation, leaving it open to a horrible race condition. So
       we reinvent the wheel.
     */
    int i;
    HANDLE th  = INVALID_HANDLE_VALUE;
    void *temp = NULL;
    SECURITY_INFORMATION si;
    SECURITY_ATTRIBUTES sa;
    PSECURITY_DESCRIPTOR psd = NULL;
    PSECURITY_ATTRIBUTES psa = NULL;
    DWORD len;
    BOOL success;

    /*
       Read the DACL from the original file, so we can copy it to the temp file.
       If there is no original file, or if we can't read the DACL, we'll use the
       default security descriptor.
     */
    if ((ctx->h != INVALID_HANDLE_VALUE) && (GetFileType(ctx->h) == FILE_TYPE_DISK)) {
        si      = DACL_SECURITY_INFORMATION | UNPROTECTED_DACL_SECURITY_INFORMATION;
        len     = 0;
        success = GetUserObjectSecurity(ctx->h, &si, NULL, len, &len);
        if (!success && (GetLastError() == ERROR_INSUFFICIENT_BUFFER)) {
            if ((psd = (PSECURITY_DESCRIPTOR)malloc(len)) == NULL) {
                zip_error_set(&ctx->error, ZIP_ER_MEMORY, 0);
                return -1;
            }
            success = GetUserObjectSecurity(ctx->h, &si, psd, len, &len);
        }
        if (success) {
            sa.nLength              = sizeof(SECURITY_ATTRIBUTES);
            sa.bInheritHandle       = FALSE;
            sa.lpSecurityDescriptor = psd;
            psa = &sa;
        }
    }

    zip_uint32_t value = GetTickCount();
    for (i = 0; i < 1024 && th == INVALID_HANDLE_VALUE; i++) {
        th = ctx->ops->op_create_temp(ctx, &temp, value + i, psa);
        if ((th == INVALID_HANDLE_VALUE) && (GetLastError() != ERROR_FILE_EXISTS))
            break;
    }

    if (th == INVALID_HANDLE_VALUE) {
        free(temp);
        free(psd);
        zip_error_set(&ctx->error, ZIP_ER_TMPOPEN, _zip_win32_error_to_errno(GetLastError()));
        return -1;
    }

    free(psd);
    ctx->hout    = th;
    ctx->tmpname = temp;

    return 0;
}

static int
_zip_seek_win32_u(HANDLE h, zip_uint64_t offset, int whence, zip_error_t *error) {
    if (offset > ZIP_INT64_MAX) {
        zip_error_set(error, ZIP_ER_SEEK, EOVERFLOW);
        return -1;
    }
    return _zip_seek_win32(h, (zip_int64_t)offset, whence, error);
}

static int
_zip_seek_win32(HANDLE h, zip_int64_t offset, int whence, zip_error_t *error) {
    LARGE_INTEGER li;
    DWORD method;

    switch (whence) {
        case SEEK_SET:
            method = FILE_BEGIN;
            break;

        case SEEK_END:
            method = FILE_END;
            break;

        case SEEK_CUR:
            method = FILE_CURRENT;
            break;

        default:
            zip_error_set(error, ZIP_ER_SEEK, EINVAL);
            return -1;
    }

    li.QuadPart = (LONGLONG)offset;
    if (!SetFilePointerEx(h, li, NULL, method)) {
        zip_error_set(error, ZIP_ER_SEEK, _zip_win32_error_to_errno(GetLastError()));
        return -1;
    }

    return 0;
}

static int
_zip_win32_error_to_errno(DWORD win32err) {
    /*
       Note: This list isn't exhaustive, but should cover common cases.
     */
    switch (win32err) {
        case ERROR_INVALID_PARAMETER:
            return EINVAL;

        case ERROR_FILE_NOT_FOUND:
            return ENOENT;

        case ERROR_INVALID_HANDLE:
            return EBADF;

        case ERROR_ACCESS_DENIED:
            return EACCES;

        case ERROR_FILE_EXISTS:
            return EEXIST;

        case ERROR_TOO_MANY_OPEN_FILES:
            return EMFILE;

        case ERROR_DISK_FULL:
            return ENOSPC;

        default:
            return 0;
    }
}

static int
_zip_stat_win32(HANDLE h, zip_stat_t *st, _zip_source_win32_read_file_t *ctx) {
    FILETIME mtimeft;
    time_t mtime;
    LARGE_INTEGER size;
    int regularp;

    if (!GetFileTime(h, NULL, NULL, &mtimeft)) {
        zip_error_set(&ctx->error, ZIP_ER_READ, _zip_win32_error_to_errno(GetLastError()));
        return -1;
    }
    if (_zip_filetime_to_time_t(mtimeft, &mtime) < 0) {
        zip_error_set(&ctx->error, ZIP_ER_READ, ERANGE);
        return -1;
    }

    regularp = 0;
    if (GetFileType(h) == FILE_TYPE_DISK) {
        regularp = 1;
    }

    if (!GetFileSizeEx(h, &size)) {
        zip_error_set(&ctx->error, ZIP_ER_READ, _zip_win32_error_to_errno(GetLastError()));
        return -1;
    }

    zip_stat_init(st);
    st->mtime  = mtime;
    st->valid |= ZIP_STAT_MTIME;
    if (ctx->end != 0) {
        st->size   = ctx->end - ctx->start;
        st->valid |= ZIP_STAT_SIZE;
    } else if (regularp) {
        st->size   = (zip_uint64_t)size.QuadPart;
        st->valid |= ZIP_STAT_SIZE;
    }

    return 0;
}

static int
_zip_filetime_to_time_t(FILETIME ft, time_t *t) {
    /*
       Inspired by http://stackoverflow.com/questions/6161776/convert-windows-filetime-to-second-in-unix-linux
     */
    const zip_int64_t WINDOWS_TICK      = 10000000LL;
    const zip_int64_t SEC_TO_UNIX_EPOCH = 11644473600LL;
    ULARGE_INTEGER li;
    zip_int64_t secs;
    time_t temp;

    li.LowPart  = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    secs        = (li.QuadPart / WINDOWS_TICK - SEC_TO_UNIX_EPOCH);

    temp = (time_t)secs;
    if (secs != (zip_int64_t)temp)
        return -1;

    *t = temp;
    return 0;
}

/*
   zip_source_win32utf8.c -- create data source from Windows file (UTF-8)
   Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
   3. The names of the authors may not be used to endorse or promote
   products derived from this software without specific prior
   written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


 #include <errno.h>
 #include <stdio.h>

ZIP_EXTERN zip_source_t *
zip_source_file(zip_t *za, const char *fname, zip_uint64_t start, zip_int64_t len) {
    if (za == NULL)
        return NULL;

    return zip_source_file_create(fname, start, len, &za->error);
}

ZIP_EXTERN zip_source_t *
zip_source_file_create(const char *fname, zip_uint64_t start, zip_int64_t length, zip_error_t *error) {
    int size;
    wchar_t *wfname;
    zip_source_t *source;

    if ((fname == NULL) || (length < -1)) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    /* Convert fname from UTF-8 to Windows-friendly UTF-16. */
    size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, fname, -1, NULL, 0);
    if (size == 0) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }
    if ((wfname = (wchar_t *)malloc(sizeof(wchar_t) * size)) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, fname, -1, wfname, size);

    source = zip_source_win32w_create(wfname, start, length, error);

    free(wfname);
    return source;
}

static void *_win32_strdup_w(const void *str);
static HANDLE _win32_open_w(_zip_source_win32_read_file_t *ctx);
static HANDLE _win32_create_temp_w(_zip_source_win32_read_file_t *ctx, void **temp, zip_uint32_t value, PSECURITY_ATTRIBUTES sa);
static int _win32_rename_temp_w(_zip_source_win32_read_file_t *ctx);
static int _win32_remove_w(const void *fname);

static _zip_source_win32_file_ops_t win32_ops_w = { _win32_strdup_w, _win32_open_w, _win32_create_temp_w, _win32_rename_temp_w, _win32_remove_w };

ZIP_EXTERN zip_source_t *
zip_source_win32w(zip_t *za, const wchar_t *fname, zip_uint64_t start, zip_int64_t len) {
    if (za == NULL)
        return NULL;

    return zip_source_win32w_create(fname, start, len, &za->error);
}

ZIP_EXTERN zip_source_t *
zip_source_win32w_create(const wchar_t *fname, zip_uint64_t start, zip_int64_t length, zip_error_t *error) {
    if ((fname == NULL) || (length < -1)) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    return _zip_source_win32_handle_or_name(fname, INVALID_HANDLE_VALUE, start, length, 1, NULL, &win32_ops_w, error);
}

static void *
_win32_strdup_w(const void *str) {
    return _wcsdup((const wchar_t *)str);
}

static HANDLE
_win32_open_w(_zip_source_win32_read_file_t *ctx) {
    return CreateFileW((WCHAR *)ctx->fname, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

static HANDLE
_win32_create_temp_w(_zip_source_win32_read_file_t *ctx, void **temp, zip_uint32_t value, PSECURITY_ATTRIBUTES sa) {
    int len;

    len = wcslen((const wchar_t *)ctx->fname) + 10;
    if (*temp == NULL) {
        if ((*temp = malloc(sizeof(wchar_t) * len)) == NULL) {
            zip_error_set(&ctx->error, ZIP_ER_MEMORY, 0);
            return INVALID_HANDLE_VALUE;
        }
    }
    //if (swprintf((wchar_t *)*temp, len, L"%s.%08x", (const wchar_t *)ctx->fname, value) != len - 1) {
    if (swprintf((wchar_t *)*temp, L"%s.%08x", (const wchar_t *)ctx->fname, value) != len - 1) {
        return INVALID_HANDLE_VALUE;
    }

    return CreateFileW((const wchar_t *)*temp, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, sa, CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY, NULL);
}

static int
_win32_rename_temp_w(_zip_source_win32_read_file_t *ctx) {
    if (!MoveFileExW((WCHAR *)ctx->tmpname, (WCHAR *)ctx->fname, MOVEFILE_REPLACE_EXISTING))
        return -1;
    return 0;
}

static int
_win32_remove_w(const void *fname) {
    DeleteFileW((const wchar_t *)fname);
    return 0;
}
#endif

/*
   zip_source_window.c -- return part of lower source
   Copyright (C) 2012-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>
#include <string.h>



struct window {
    zip_uint64_t start;
    zip_uint64_t end;
    zip_uint64_t offset;
    zip_stat_t stat;
    zip_error_t error;
    zip_int64_t supports;
    char needs_seek;
};

static zip_int64_t window_read(zip_source_t *, void *, void *, zip_uint64_t, zip_source_cmd_t);


zip_source_t *
zip_source_window(zip_t *za, zip_source_t *src, zip_uint64_t start, zip_uint64_t len) {
    return _zip_source_window_new(src, start, len, NULL, &za->error);
}

zip_source_t *
_zip_source_window_new(zip_source_t *src, zip_uint64_t start, zip_uint64_t length, zip_stat_t *st, zip_error_t *error) {
    struct window *ctx;

    if ((src == NULL) || (start + length < start)) {
        zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    if ((ctx = (struct window *)malloc(sizeof(*ctx))) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    ctx->start = start;
    ctx->end   = start + length;
    zip_stat_init(&ctx->stat);
    zip_error_init(&ctx->error);
    ctx->supports   = (zip_source_supports(src) & ZIP_SOURCE_SUPPORTS_SEEKABLE) | (zip_source_make_command_bitmap(ZIP_SOURCE_SUPPORTS, ZIP_SOURCE_TELL, -1));
    ctx->needs_seek = (ctx->supports & ZIP_SOURCE_MAKE_COMMAND_BITMASK(ZIP_SOURCE_SEEK)) ? ZIP_TRUE : ZIP_FALSE;

    if (st) {
        if (_zip_stat_merge(&ctx->stat, st, error) < 0) {
            free(ctx);
            return NULL;
        }
    }

    return zip_source_layered_create(src, window_read, ctx, error);
}

int
_zip_source_set_source_archive(zip_source_t *src, zip_t *za) {
    src->source_archive = za;
    return _zip_register_source(za, src);
}

/* called by zip_discard to avoid operating on file from closed archive */
void
_zip_source_invalidate(zip_source_t *src) {
    src->source_closed = 1;

    if (zip_error_code_zip(&src->error) == ZIP_ER_OK) {
        zip_error_set(&src->error, ZIP_ER_ZIPCLOSED, 0);
    }
}

static zip_int64_t
window_read(zip_source_t *src, void *_ctx, void *data, zip_uint64_t len, zip_source_cmd_t cmd) {
    struct window *ctx;
    zip_int64_t ret;
    zip_uint64_t n, i;
    char b[8192];

    ctx = (struct window *)_ctx;

    switch (cmd) {
        case ZIP_SOURCE_CLOSE:
            return 0;

        case ZIP_SOURCE_ERROR:
            return zip_error_to_data(&ctx->error, data, len);

        case ZIP_SOURCE_FREE:
            free(ctx);
            return 0;

        case ZIP_SOURCE_OPEN:
            if (!ctx->needs_seek) {
                for (n = 0; n < ctx->start; n += (zip_uint64_t)ret) {
                    i = (ctx->start - n > sizeof(b) ? sizeof(b) : ctx->start - n);
                    if ((ret = zip_source_read(src, b, i)) < 0) {
                        _zip_error_set_from_source(&ctx->error, src);
                        return -1;
                    }
                    if (ret == 0) {
                        zip_error_set(&ctx->error, ZIP_ER_EOF, 0);
                        return -1;
                    }
                }
            }
            ctx->offset = ctx->start;
            return 0;

        case ZIP_SOURCE_READ:
            if (len > ctx->end - ctx->offset)
                len = ctx->end - ctx->offset;

            if (len == 0)
                return 0;

            if (ctx->needs_seek) {
                if (zip_source_seek(src, (zip_int64_t)ctx->offset, SEEK_SET) < 0) {
                    _zip_error_set_from_source(&ctx->error, src);
                    return -1;
                }
            }

            if ((ret = zip_source_read(src, data, len)) < 0) {
                zip_error_set(&ctx->error, ZIP_ER_EOF, 0);
                return -1;
            }

            ctx->offset += (zip_uint64_t)ret;

            if (ret == 0) {
                if (ctx->offset < ctx->end) {
                    zip_error_set(&ctx->error, ZIP_ER_EOF, 0);
                    return -1;
                }
            }
            return ret;

        case ZIP_SOURCE_SEEK:
            {
                zip_int64_t new_offset = zip_source_seek_compute_offset(ctx->offset - ctx->start, ctx->end - ctx->start, data, len, &ctx->error);

                if (new_offset < 0) {
                    return -1;
                }

                ctx->offset = (zip_uint64_t)new_offset + ctx->start;
                return 0;
            }

        case ZIP_SOURCE_STAT:
            {
                zip_stat_t *st;

                st = (zip_stat_t *)data;

                if (_zip_stat_merge(st, &ctx->stat, &ctx->error) < 0) {
                    return -1;
                }
                return 0;
            }

        case ZIP_SOURCE_SUPPORTS:
            return ctx->supports;

        case ZIP_SOURCE_TELL:
            return (zip_int64_t)(ctx->offset - ctx->start);

        default:
            zip_error_set(&ctx->error, ZIP_ER_OPNOTSUPP, 0);
            return -1;
    }
}

void
_zip_deregister_source(zip_t *za, zip_source_t *src) {
    unsigned int i;

    for (i = 0; i < za->nopen_source; i++) {
        if (za->open_source[i] == src) {
            za->open_source[i] = za->open_source[za->nopen_source - 1];
            za->nopen_source--;
            break;
        }
    }
}

int
_zip_register_source(zip_t *za, zip_source_t *src) {
    zip_source_t **open_source;

    if (za->nopen_source + 1 >= za->nopen_source_alloc) {
        unsigned int n;
        n           = za->nopen_source_alloc + 10;
        open_source = (zip_source_t **)realloc(za->open_source, n * sizeof(zip_source_t *));
        if (open_source == NULL) {
            zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
            return -1;
        }
        za->nopen_source_alloc = n;
        za->open_source        = open_source;
    }

    za->open_source[za->nopen_source++] = src;

    return 0;
}

/*
   zip_source_write.c -- start a new file for writing
   Copyright (C) 2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN zip_int64_t
zip_source_write(zip_source_t *src, const void *data, zip_uint64_t length) {
    if (!ZIP_SOURCE_IS_OPEN_WRITING(src) || (length > ZIP_INT64_MAX)) {
        zip_error_set(&src->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    return _zip_source_call(src, (void *)data, length, ZIP_SOURCE_WRITE);
}

/*
   zip_source_zip.c -- create data source from zip file
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>
#include <string.h>



ZIP_EXTERN zip_source_t *
zip_source_zip(zip_t *za, zip_t *srcza, zip_uint64_t srcidx,
               zip_flags_t flags, zip_uint64_t start, zip_int64_t len) {
    if (len < -1) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    if (len == -1)
        len = 0;

    if ((start == 0) && (len == 0))
        flags |= ZIP_FL_COMPRESSED;
    else
        flags &= ~ZIP_FL_COMPRESSED;

    return _zip_source_zip_new(za, srcza, srcidx, flags, start, (zip_uint64_t)len, NULL);
}

/*
   zip_source_zip_new.c -- prepare data structures for zip_fopen/zip_source_zip
   Copyright (C) 2012-2015 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>



zip_source_t *
_zip_source_zip_new(zip_t *za, zip_t *srcza, zip_uint64_t srcidx, zip_flags_t flags, zip_uint64_t start, zip_uint64_t len, const char *password) {
    zip_compression_implementation comp_impl;
    zip_encryption_implementation enc_impl;
    zip_source_t *src, *s2;
    zip_uint64_t offset;
    struct zip_stat st;

    if (za == NULL)
        return NULL;

    if ((srcza == NULL) || (srcidx >= srcza->nentry)) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    if (((flags & ZIP_FL_UNCHANGED) == 0) &&
        (ZIP_ENTRY_DATA_CHANGED(srcza->entry + srcidx) || srcza->entry[srcidx].deleted)) {
        zip_error_set(&za->error, ZIP_ER_CHANGED, 0);
        return NULL;
    }

    if (zip_stat_index(srcza, srcidx, flags | ZIP_FL_UNCHANGED, &st) < 0) {
        zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
        return NULL;
    }

    if (flags & ZIP_FL_ENCRYPTED)
        flags |= ZIP_FL_COMPRESSED;

    if (((start > 0) || (len > 0)) && (flags & ZIP_FL_COMPRESSED)) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    /* overflow or past end of file */
    if (((start > 0) || (len > 0)) && ((start + len < start) || (start + len > st.size))) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return NULL;
    }

    enc_impl = NULL;
    if (((flags & ZIP_FL_ENCRYPTED) == 0) && (st.encryption_method != ZIP_EM_NONE)) {
        if (password == NULL) {
            zip_error_set(&za->error, ZIP_ER_NOPASSWD, 0);
            return NULL;
        }
        if ((enc_impl = _zip_get_encryption_implementation(st.encryption_method)) == NULL) {
            zip_error_set(&za->error, ZIP_ER_ENCRNOTSUPP, 0);
            return NULL;
        }
    }

    comp_impl = NULL;
    if ((flags & ZIP_FL_COMPRESSED) == 0) {
        if (st.comp_method != ZIP_CM_STORE) {
            if ((comp_impl = _zip_get_compression_implementation(st.comp_method)) == NULL) {
                zip_error_set(&za->error, ZIP_ER_COMPNOTSUPP, 0);
                return NULL;
            }
        }
    }

    if ((offset = _zip_file_get_offset(srcza, srcidx, &za->error)) == 0)
        return NULL;

    if (st.comp_size == 0) {
        return zip_source_buffer(za, NULL, 0, 0);
    }

    if ((start + len > 0) && (enc_impl == NULL) && (comp_impl == NULL)) {
        struct zip_stat st2;

        st2.size        = len ? len : st.size - start;
        st2.comp_size   = st2.size;
        st2.comp_method = ZIP_CM_STORE;
        st2.mtime       = st.mtime;
        st2.valid       = ZIP_STAT_SIZE | ZIP_STAT_COMP_SIZE | ZIP_STAT_COMP_METHOD | ZIP_STAT_MTIME;

        if ((src = _zip_source_window_new(srcza->src, offset + start, st2.size, &st2, &za->error)) == NULL) {
            return NULL;
        }
    } else {
        if ((src = _zip_source_window_new(srcza->src, offset, st.comp_size, &st, &za->error)) == NULL) {
            return NULL;
        }
    }

    if (_zip_source_set_source_archive(src, srcza) < 0) {
        zip_source_free(src);
        return NULL;
    }

    /* creating a layered source calls zip_keep() on the lower layer, so we free it */

    if (enc_impl) {
        s2 = enc_impl(za, src, st.encryption_method, 0, password);
        zip_source_free(src);
        if (s2 == NULL) {
            return NULL;
        }
        src = s2;
    }
    if (comp_impl) {
        s2 = comp_impl(za, src, st.comp_method, 0);
        zip_source_free(src);
        if (s2 == NULL) {
            return NULL;
        }
        src = s2;
    }
    if ((((flags & ZIP_FL_COMPRESSED) == 0) || (st.comp_method == ZIP_CM_STORE)) && ((len == 0) || (len == st.comp_size))) {
        /* when reading the whole file, check for CRC errors */
        s2 = zip_source_crc(za, src, 1);
        zip_source_free(src);
        if (s2 == NULL) {
            return NULL;
        }
        src = s2;
    }

    if ((start + len > 0) && (comp_impl || enc_impl)) {
        s2 = zip_source_window(za, src, start, len ? len : st.size - start);
        zip_source_free(src);
        if (s2 == NULL) {
            return NULL;
        }
        src = s2;
    }

    return src;
}

/*
   zip_stat.c -- get information about file by name
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_stat(zip_t *za, const char *fname, zip_flags_t flags, zip_stat_t *st) {
    zip_int64_t idx;

    if ((idx = zip_name_locate(za, fname, flags)) < 0)
        return -1;

    return zip_stat_index(za, (zip_uint64_t)idx, flags, st);
}

/*
   zip_stat_index.c -- get information about file by index
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN int
zip_stat_index(zip_t *za, zip_uint64_t index, zip_flags_t flags,
               zip_stat_t *st) {
    const char *name;
    zip_dirent_t *de;

    if ((de = _zip_get_dirent(za, index, flags, NULL)) == NULL)
        return -1;

    if ((name = zip_get_name(za, index, flags)) == NULL)
        return -1;


    if (((flags & ZIP_FL_UNCHANGED) == 0) &&
        ZIP_ENTRY_DATA_CHANGED(za->entry + index)) {
        if (zip_source_stat(za->entry[index].source, st) < 0) {
            zip_error_set(&za->error, ZIP_ER_CHANGED, 0);
            return -1;
        }
    } else {
        zip_stat_init(st);

        st->crc         = de->crc;
        st->size        = de->uncomp_size;
        st->mtime       = de->last_mod;
        st->comp_size   = de->comp_size;
        st->comp_method = (zip_uint16_t)de->comp_method;
        if (de->bitflags & ZIP_GPBF_ENCRYPTED) {
            if (de->bitflags & ZIP_GPBF_STRONG_ENCRYPTION) {
                /* TODO */
                st->encryption_method = ZIP_EM_UNKNOWN;
            } else
                st->encryption_method = ZIP_EM_TRAD_PKWARE;
        } else
            st->encryption_method = ZIP_EM_NONE;
        st->valid = ZIP_STAT_CRC | ZIP_STAT_SIZE | ZIP_STAT_MTIME
                    | ZIP_STAT_COMP_SIZE | ZIP_STAT_COMP_METHOD | ZIP_STAT_ENCRYPTION_METHOD;
    }

    st->index  = index;
    st->name   = name;
    st->valid |= ZIP_STAT_INDEX | ZIP_STAT_NAME;

    return 0;
}

/*
   zip_stat_init.c -- initialize struct zip_stat.
   Copyright (C) 2006-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>



ZIP_EXTERN void
zip_stat_init(zip_stat_t *st) {
    st->valid             = 0;
    st->name              = NULL;
    st->index             = ZIP_UINT64_MAX;
    st->crc               = 0;
    st->mtime             = (time_t)-1;
    st->size              = 0;
    st->comp_size         = 0;
    st->comp_method       = ZIP_CM_STORE;
    st->encryption_method = ZIP_EM_NONE;
}

int
_zip_stat_merge(zip_stat_t *dst, const zip_stat_t *src, zip_error_t *error) {
    /* name is not merged, since zip_stat_t doesn't own it, and src may not be valid as long as dst */
    if (src->valid & ZIP_STAT_INDEX) {
        dst->index = src->index;
    }
    if (src->valid & ZIP_STAT_SIZE) {
        dst->size = src->size;
    }
    if (src->valid & ZIP_STAT_COMP_SIZE) {
        dst->comp_size = src->comp_size;
    }
    if (src->valid & ZIP_STAT_MTIME) {
        dst->mtime = src->mtime;
    }
    if (src->valid & ZIP_STAT_CRC) {
        dst->crc = src->crc;
    }
    if (src->valid & ZIP_STAT_COMP_METHOD) {
        dst->comp_method = src->comp_method;
    }
    if (src->valid & ZIP_STAT_ENCRYPTION_METHOD) {
        dst->encryption_method = src->encryption_method;
    }
    if (src->valid & ZIP_STAT_FLAGS) {
        dst->flags = src->flags;
    }
    dst->valid |= src->valid;

    return 0;
}

/*
   zip_sterror.c -- get string representation of zip error
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



ZIP_EXTERN const char *
zip_strerror(zip_t *za) {
    return zip_error_strerror(&za->error);
}

/*
   zip_string.c -- string handling (with encoding)
   Copyright (C) 2012-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>
#include <string.h>



zip_uint32_t
_zip_string_crc32(const zip_string_t *s) {
    zip_uint32_t crc;

    crc = (zip_uint32_t)crc32(0L, Z_NULL, 0);

    if (s != NULL)
        crc = (zip_uint32_t)crc32(crc, s->raw, s->length);

    return crc;
}

int
_zip_string_equal(const zip_string_t *a, const zip_string_t *b) {
    if ((a == NULL) || (b == NULL))
        return a == b;

    if (a->length != b->length)
        return 0;

    /* TODO: encoding */

    return memcmp(a->raw, b->raw, a->length) == 0;
}

void
_zip_string_free(zip_string_t *s) {
    if (s == NULL)
        return;

    free(s->raw);
    free(s->converted);
    free(s);
}

const zip_uint8_t *
_zip_string_get(zip_string_t *string, zip_uint32_t *lenp, zip_flags_t flags, zip_error_t *error) {
    static const zip_uint8_t empty[1] = "";

    if (string == NULL) {
        if (lenp)
            *lenp = 0;
        return empty;
    }

    if ((flags & ZIP_FL_ENC_RAW) == 0) {
        /* start guessing */
        if (string->encoding == ZIP_ENCODING_UNKNOWN)
            _zip_guess_encoding(string, ZIP_ENCODING_UNKNOWN);

        if (((flags & ZIP_FL_ENC_STRICT) &&
             (string->encoding != ZIP_ENCODING_ASCII) && (string->encoding != ZIP_ENCODING_UTF8_KNOWN)) ||
            (string->encoding == ZIP_ENCODING_CP437)) {
            if (string->converted == NULL) {
                if ((string->converted = _zip_cp437_to_utf8(string->raw, string->length,
                                                            &string->converted_length, error)) == NULL)
                    return NULL;
            }
            if (lenp)
                *lenp = string->converted_length;
            return string->converted;
        }
    }

    if (lenp)
        *lenp = string->length;
    return string->raw;
}

zip_uint16_t
_zip_string_length(const zip_string_t *s) {
    if (s == NULL)
        return 0;

    return s->length;
}

zip_string_t *
_zip_string_new(const zip_uint8_t *raw, zip_uint16_t length, zip_flags_t flags, zip_error_t *error) {
    zip_string_t *s;
    zip_encoding_type_t expected_encoding;

    if (length == 0)
        return NULL;

    switch (flags & ZIP_FL_ENCODING_ALL) {
        case ZIP_FL_ENC_GUESS:
            expected_encoding = ZIP_ENCODING_UNKNOWN;
            break;

        case ZIP_FL_ENC_UTF_8:
            expected_encoding = ZIP_ENCODING_UTF8_KNOWN;
            break;

        case ZIP_FL_ENC_CP437:
            expected_encoding = ZIP_ENCODING_CP437;
            break;

        default:
            zip_error_set(error, ZIP_ER_INVAL, 0);
            return NULL;
    }

    if ((s = (zip_string_t *)malloc(sizeof(*s))) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    if ((s->raw = (zip_uint8_t *)malloc((size_t)(length + 1))) == NULL) {
        free(s);
        return NULL;
    }

    memcpy(s->raw, raw, length);
    s->raw[length]      = '\0';
    s->length           = length;
    s->encoding         = ZIP_ENCODING_UNKNOWN;
    s->converted        = NULL;
    s->converted_length = 0;

    if (expected_encoding != ZIP_ENCODING_UNKNOWN) {
        if (_zip_guess_encoding(s, expected_encoding) == ZIP_ENCODING_ERROR) {
            _zip_string_free(s);
            zip_error_set(error, ZIP_ER_INVAL, 0);
            return NULL;
        }
    }

    return s;
}

int
_zip_string_write(zip_t *za, const zip_string_t *s) {
    if (s == NULL)
        return 0;

    return _zip_write(za, s->raw, s->length);
}

/*
   zip_unchange.c -- undo changes to file in zip archive
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>



ZIP_EXTERN int
zip_unchange(zip_t *za, zip_uint64_t idx) {
    return _zip_unchange(za, idx, 0);
}

int
_zip_unchange(zip_t *za, zip_uint64_t idx, int allow_duplicates) {
    zip_int64_t i;

    if (idx >= za->nentry) {
        zip_error_set(&za->error, ZIP_ER_INVAL, 0);
        return -1;
    }

    if (!allow_duplicates && za->entry[idx].changes && (za->entry[idx].changes->changed & ZIP_DIRENT_FILENAME)) {
        i = _zip_name_locate(za, _zip_get_name(za, idx, ZIP_FL_UNCHANGED, NULL), 0, NULL);
        if ((i >= 0) && ((zip_uint64_t)i != idx)) {
            zip_error_set(&za->error, ZIP_ER_EXISTS, 0);
            return -1;
        }
    }

    _zip_dirent_free(za->entry[idx].changes);
    za->entry[idx].changes = NULL;

    _zip_unchange_data(za->entry + idx);

    return 0;
}

/*
   zip_unchange.c -- undo changes to all files in zip archive
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>



ZIP_EXTERN int
zip_unchange_all(zip_t *za) {
    int ret;
    zip_uint64_t i;

    ret = 0;
    for (i = 0; i < za->nentry; i++)
        ret |= _zip_unchange(za, i, 1);

    ret |= zip_unchange_archive(za);

    return ret;
}

/*
   zip_unchange_archive.c -- undo global changes to ZIP archive
   Copyright (C) 2006-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>



ZIP_EXTERN int
zip_unchange_archive(zip_t *za) {
    if (za->comment_changed) {
        _zip_string_free(za->comment_changes);
        za->comment_changes = NULL;
        za->comment_changed = 0;
    }

    za->ch_flags = za->flags;

    return 0;
}

/*
   zip_unchange_data.c -- undo helper function
   Copyright (C) 1999-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



void
_zip_unchange_data(zip_entry_t *ze) {
    if (ze->source) {
        zip_source_free(ze->source);
        ze->source = NULL;
    }

    if ((ze->changes != NULL) && (ze->changes->changed & ZIP_DIRENT_COMP_METHOD) && (ze->changes->comp_method == ZIP_CM_REPLACED_DEFAULT)) {
        ze->changes->changed &= ~ZIP_DIRENT_COMP_METHOD;
        if (ze->changes->changed == 0) {
            _zip_dirent_free(ze->changes);
            ze->changes = NULL;
        }
    }

    ze->deleted = 0;
}

/*
   zip_utf-8.c -- UTF-8 support functions for libzip
   Copyright (C) 2011-2014 Dieter Baron and Thomas Klausner

   This file is part of libzip, a library to manipulate ZIP archives.
   The authors can be contacted at <libzip@nih.at>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



#include <stdlib.h>


static const zip_uint16_t _cp437_to_unicode[256] = {
    /* 0x00 - 0x0F */
    0x2007, 0x263A, 0x263B, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,
    0x25D8, 0x25CB, 0x25D9, 0x2642, 0x2640, 0x266A, 0x266B, 0x263C,

    /* 0x10 - 0x1F */
    0x25BA, 0x25C4, 0x2195, 0x203C, 0x00B6, 0x00A7, 0x25AC, 0x21A8,
    0x2191, 0x2193, 0x2192, 0x2190, 0x221F, 0x2194, 0x25B2, 0x25BC,

    /* 0x20 - 0x2F */
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,

    /* 0x30 - 0x3F */
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,

    /* 0x40 - 0x4F */
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,

    /* 0x50 - 0x5F */
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,

    /* 0x60 - 0x6F */
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,

    /* 0x70 - 0x7F */
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x2302,

    /* 0x80 - 0x8F */
    0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
    0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,

    /* 0x90 - 0x9F */
    0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
    0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,

    /* 0xA0 - 0xAF */
    0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
    0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,

    /* 0xB0 - 0xBF */
    0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
    0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,

    /* 0xC0 - 0xCF */
    0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
    0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,

    /* 0xD0 - 0xDF */
    0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
    0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,

    /* 0xE0 - 0xEF */
    0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4,
    0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,

    /* 0xF0 - 0xFF */
    0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248,
    0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0
};

#define UTF_8_LEN_2_MASK        0xe0
#define UTF_8_LEN_2_MATCH       0xc0
#define UTF_8_LEN_3_MASK        0xf0
#define UTF_8_LEN_3_MATCH       0xe0
#define UTF_8_LEN_4_MASK        0xf8
#define UTF_8_LEN_4_MATCH       0xf0
#define UTF_8_CONTINUE_MASK     0xc0
#define UTF_8_CONTINUE_MATCH    0x80


zip_encoding_type_t
_zip_guess_encoding(zip_string_t *str, zip_encoding_type_t expected_encoding) {
    zip_encoding_type_t enc;
    const zip_uint8_t *name;
    zip_uint32_t i, j, ulen;

    if (str == NULL)
        return ZIP_ENCODING_ASCII;

    name = str->raw;

    if (str->encoding != ZIP_ENCODING_UNKNOWN)
        enc = str->encoding;
    else {
        enc = ZIP_ENCODING_ASCII;
        for (i = 0; i < str->length; i++) {
            if (((name[i] > 31) && (name[i] < 128)) || (name[i] == '\r') || (name[i] == '\n') || (name[i] == '\t'))
                continue;

            enc = ZIP_ENCODING_UTF8_GUESSED;
            if ((name[i] & UTF_8_LEN_2_MASK) == UTF_8_LEN_2_MATCH)
                ulen = 1;
            else if ((name[i] & UTF_8_LEN_3_MASK) == UTF_8_LEN_3_MATCH)
                ulen = 2;
            else if ((name[i] & UTF_8_LEN_4_MASK) == UTF_8_LEN_4_MATCH)
                ulen = 3;
            else {
                enc = ZIP_ENCODING_CP437;
                break;
            }

            if (i + ulen >= str->length) {
                enc = ZIP_ENCODING_CP437;
                break;
            }

            for (j = 1; j <= ulen; j++) {
                if ((name[i + j] & UTF_8_CONTINUE_MASK) != UTF_8_CONTINUE_MATCH) {
                    enc = ZIP_ENCODING_CP437;
                    goto done;
                }
            }
            i += ulen;
        }
    }

done:
    str->encoding = enc;

    if (expected_encoding != ZIP_ENCODING_UNKNOWN) {
        if ((expected_encoding == ZIP_ENCODING_UTF8_KNOWN) && (enc == ZIP_ENCODING_UTF8_GUESSED))
            str->encoding = enc = ZIP_ENCODING_UTF8_KNOWN;

        if ((expected_encoding != enc) && (enc != ZIP_ENCODING_ASCII))
            return ZIP_ENCODING_ERROR;
    }

    return enc;
}

static zip_uint32_t
_zip_unicode_to_utf8_len(zip_uint32_t codepoint) {
    if (codepoint < 0x0080)
        return 1;
    if (codepoint < 0x0800)
        return 2;
    if (codepoint < 0x10000)
        return 3;
    return 4;
}

static zip_uint32_t
_zip_unicode_to_utf8(zip_uint32_t codepoint, zip_uint8_t *buf) {
    if (codepoint < 0x0080) {
        buf[0] = codepoint & 0xff;
        return 1;
    }
    if (codepoint < 0x0800) {
        buf[0] = (zip_uint8_t)(UTF_8_LEN_2_MATCH | ((codepoint >> 6) & 0x1f));
        buf[1] = (zip_uint8_t)(UTF_8_CONTINUE_MATCH | (codepoint & 0x3f));
        return 2;
    }
    if (codepoint < 0x10000) {
        buf[0] = (zip_uint8_t)(UTF_8_LEN_3_MATCH | ((codepoint >> 12) & 0x0f));
        buf[1] = (zip_uint8_t)(UTF_8_CONTINUE_MATCH | ((codepoint >> 6) & 0x3f));
        buf[2] = (zip_uint8_t)(UTF_8_CONTINUE_MATCH | (codepoint & 0x3f));
        return 3;
    }
    buf[0] = (zip_uint8_t)(UTF_8_LEN_4_MATCH | ((codepoint >> 18) & 0x07));
    buf[1] = (zip_uint8_t)(UTF_8_CONTINUE_MATCH | ((codepoint >> 12) & 0x3f));
    buf[2] = (zip_uint8_t)(UTF_8_CONTINUE_MATCH | ((codepoint >> 6) & 0x3f));
    buf[3] = (zip_uint8_t)(UTF_8_CONTINUE_MATCH | (codepoint & 0x3f));
    return 4;
}

zip_uint8_t *
_zip_cp437_to_utf8(const zip_uint8_t *const _cp437buf, zip_uint32_t len,
                   zip_uint32_t *utf8_lenp, zip_error_t *error) {
    zip_uint8_t *cp437buf = (zip_uint8_t *)_cp437buf;
    zip_uint8_t *utf8buf;
    zip_uint32_t buflen, i, offset;

    if (len == 0) {
        if (utf8_lenp)
            *utf8_lenp = 0;
        return NULL;
    }

    buflen = 1;
    for (i = 0; i < len; i++)
        buflen += _zip_unicode_to_utf8_len(_cp437_to_unicode[cp437buf[i]]);

    if ((utf8buf = (zip_uint8_t *)malloc(buflen)) == NULL) {
        zip_error_set(error, ZIP_ER_MEMORY, 0);
        return NULL;
    }

    offset = 0;
    for (i = 0; i < len; i++)
        offset += _zip_unicode_to_utf8(_cp437_to_unicode[cp437buf[i]],
                                       utf8buf + offset);

    utf8buf[buflen - 1] = 0;
    if (utf8_lenp)
        *utf8_lenp = buflen - 1;
    return utf8buf;
}
