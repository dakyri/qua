/*
 * dull but useful.
 */
#ifndef __STD_DEFS
#define __STD_DEFS

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iterator>
#include <unordered_map>
#include <limits>
#include <algorithm>

using namespace std;

typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uin16;
typedef short int16;
typedef unsigned char uint8;
typedef	char int8;
typedef unsigned short uint16;
typedef	short int16;
typedef long long int64;
typedef unsigned long long uint64;
typedef int64 bigtime_t;

typedef unsigned long	ulong;
typedef unsigned short	ushort;
typedef unsigned char	uchar;
typedef unsigned char	flag;

typedef int32 status_t;

#define Min(X,Y)	((X)>(Y)?(Y):(X))
#define Max(X,Y)	((X)>(Y)?(X):(Y))
#define Sign(X,Y)	((Y)>=0? Abs(X) : -Abs(X))
#define Abs(X)	((X)>0?(X):-(X))

inline int32 min32(int32 x, int32 y) { return x<y? x : y; }
inline int32 max32(int32 x, int32 y) { return x>y? x : y; }
inline int32 abs32(int32 x) { return x>=0? x : -x; }
inline int32 round32(float x) { return ((int32)(x+0.5)); }
inline string tab(int n) { return string(n, '\t'); }

#define MAJORLY_BIG	2147483648

#ifndef M_PI
#define M_PI	3.1415926535897932384626433832795
#endif
#ifndef M_E
#define M_E		2.7182818284590452353602874713527
#endif

status_t tab(FILE *f, short in);
long writeChunkId(FILE *f, uint32, uint32);
long readChunkId(FILE *f, uint32&, uint32&);

std::string uintstr(uint32);
void memfill(void *, long, uchar);
void memswab(void *, long);

void beep();
std::string getLeaf(const std::string &path);
std::string getParent(const std::string &path);
std::string getBase(const std::string &path);
std::string getExt(const std::string &path);
std::string getSupertype(const std::string &path);  // of a mime type


namespace qut {
	template<typename T> bool del(vector<T> &a, T &i) {
		auto ai = std::find(a.begin(), a.end(), i);
		if (ai == a.end()) return false;
		a.erase(ai);
		return true;
	}

	template<typename T> T maxel(vector<T> &a) {
		auto cp = std::max_element(a.begin(), a.end());
		if (cp == a.end()) return std::numeric_limits<T>::min();
		return *cp;
	}

	template<typename K, typename V> K unfind(typename unordered_map<K, V> &m, typename V &v) {
		for (auto mi = m.begin(); mi != m.end(); ++mi) {
			if (mi->second == v) {
				return mi->first;
			}
		}
		return K();
	}
}

#define B_PAGE_SIZE	4096

#define ERRNO_BASE -10000
#define ERRNO_STORAGE_ERROR_BASE (ERRNO_BASE + 1000)

#define	B_ERROR -1
#define	B_OK 0
#define	B_NO_ERROR 0


std::string errorStr(int no);
// general erorrs
enum {
	ERRNO_NO_MEMORY = ERRNO_BASE,
	ERRNO_IO_ERROR,
	ERRNO_PERMISSION_DENIED,
	ERRNO_BAD_INDEX,
	ERRNO_BAD_TYPE,
	ERRNO_BAD_VALUE,
	ERRNO_MISMATCHED_VALUES,
	ERRNO_NAME_NOT_FOUND,
	ERRNO_NAME_IN_USE,
	ERRNO_TIMED_OUT,
	ERRNO_INTERRUPTED,
	ERRNO_WOULD_BLOCK,
	ERRNO_CANCELED,
	ERRNO_NO_INIT,
	ERRNO_BUSY,
	ERRNO_NOT_ALLOWED
};

// file errors
enum {
	ERRNO_FILE_ERROR = ERRNO_STORAGE_ERROR_BASE,
	ERRNO_FILE_NOT_FOUND,	/* discouraged; use B_ENTRY_NOT_FOUND in new code*/
	ERRNO_FILE_EXISTS,
	ERRNO_ENTRY_NOT_FOUND,
	ERRNO_NAME_TOO_LONG,
	ERRNO_NOT_A_DIRECTORY,
	ERRNO_DIRECTORY_NOT_EMPTY,
	ERRNO_DEVICE_FULL,
	ERRNO_READ_ONLY_DEVICE,
	ERRNO_IS_A_DIRECTORY,
	ERRNO_NO_MORE_FDS,
	ERRNO_CROSS_DEVICE_LINK,
	ERRNO_LINK_LIMIT,
	ERRNO_BUSTED_PIPE,
	ERRNO_UNSUPPORTED,
	ERRNO_PARTITION_TOO_SMALL
};


extern void internalError(char *str, ...);
std::string wc2string(wchar_t  wa[]);

#endif