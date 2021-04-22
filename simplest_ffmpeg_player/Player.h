#pragma once

#if defined WIN32
# include <Windows.h>
# include <io.h>
# define sleep(x) Sleep(x*1000)
//DWORD WINAPI ThreadFunc(void* data)
inline void start_thread(void (*function)()) {
    HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)function, NULL, 0, NULL);
}
#endif

#define DEFAULT_PORT            5200
#define MAX_BUFFER              100000
#define MAX_ENCODED_FRAME_SIZE  (1000*1000)

#define U8  unsigned char
#define U16 unsigned short
#define U32 unsigned int
#define S8    signed char
#define S16   signed short
#define S32   signed int

// input comes as UDP packets over INPUT_PORT 
// [1 byte]packet_type, [4 bytes]pts_milli, [4 bytes]metadata_len, [metadata_len]metadata, [4 bytes]payload_len, [payload_len]payload

#define NUM_ELEMS(x) (sizeof(x)/sizeof(x[0]))

typedef struct packet_t {
    U8  type;
    U32 pts_milli;
    U32 metadata_len;
    U8* metadata;
    U32 payload_len;
    U8* payload;
} PACKET_T;

void InputThread();
void OutputThread();