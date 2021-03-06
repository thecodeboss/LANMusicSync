#ifndef Buffer_h__
#define Buffer_h__

#include <deque>
#include <vector>
#define BUFFER_SIZE 512
#define MIN_BUFFER_COUNT 2
#define MAX_BUFFER_COUNT 8
typedef unsigned char Sample_t;
typedef std::vector<Sample_t> Buffer;

#endif // Buffer_h__
