/*
 * dull but useful.
 */
#ifndef STD_DEFS
#define STD_DEFS

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iterator>
#include <unordered_map>

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

#define MAJORLY_BIG	2147483648

#ifndef M_PI
#define M_PI	3.1415926535897932384626433832795
#endif
#ifndef M_E
#define M_E		2.7182818284590452353602874713527
#endif

#define	B_ERROR -1
#define	B_OK 0
#define	B_NO_ERROR 0

char *errorStr(int32 no);
void tragicError(char *str, ...);
void reportError(char *str, ...);
int retryError(char *str, ...);
bool abortError(char *str, ...);
bool continueError(char *str, ...);
int optionWin(int, char *str, ...);

status_t tab(FILE *f, short in);
long writeChunkId(FILE *f, uint32, uint32);
long readChunkId(FILE *f, uint32&, uint32&);

char *uintstr(uint32);
void memfill(void *, long, uchar);
void memswab(void *, long);

void beep();
std::string getLeaf(std::string path);
std::string getParent(std::string path);
std::string getBase(std::string path);
std::string getExt(std::string path);
std::string getSupertype(std::string path);  // of a mime type

namespace qut {
	template<typename T> typename std::vector<T>::iterator find(std::vector<T> a, T i) {
		for (auto ai = a.begin(); ai != a.end(); ++ai) {
			if (*ai == i) {
				return ai;
			}
		}
		return a.end();
	}

	template<typename T> bool del(std::vector<T> a, T i) {
		for (auto ai = a.begin(); ai != a.end(); ++ai) {
			if (*ai == i) {
				a.erase(ai);
				return true;
			}
		}
		return false;
	}

	template<typename K, typename V> K unfind(typename std::unordered_map<K, V> m, typename V v) {
		for (auto mi = m.begin(); mi != m.end(); ++mi) {
			if (mi->second == v) {
				return mi->first;
			}
		}
		return K();
	}
}
#ifdef WIN32
std::string wc2string(WCHAR []);
#endif
#endif
