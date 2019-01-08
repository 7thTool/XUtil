#include "XThread.h"

#ifdef WIN32
#else
// size_t GetTickCount()
// {
// 	struct timespec ts;
// 	clock_gettime(CLOCK_MONOTONIC, &ts);
// 	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
// }
#endif//

