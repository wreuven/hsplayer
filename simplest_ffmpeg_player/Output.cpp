#define _WINSOCK_DEPRECATED_NO_WARNINGS

#if defined WIN32
# include <WinSock2.h>
# pragma comment(lib,"ws2_32.lib") //Winsock Library
# include <io.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include "Player.h"

static int	g_sock;
static struct sockaddr_in addr;
static struct hostent* host;
static char buffer[MAX_BUFFER];

/* 
	input comes as UDP packets over INPUT_PORT 
	[1 byte]packet_type, [4 bytes]pts_milli, [4 bytes]metadata_len, [metadata_len]metadata, [4 bytes]payload_len, [payload_len]payload

typedef struct packet_t {
	U8  type;
	U32 pts_milli;
	U32 metadata_len;
	U8* metadata;
	U32 payload_len;
	U8* payload;
} PACKET_T;
*/

static void ErrorHandler(const char* errorMessage) {
	printf(errorMessage);
}

static void HandlePacket(PACKET_T* packet) {

}

static void HandleConnection(int sock) {

}

static void SendPackets(int sock) {

#	define SEND(sock,ptr,len,flags) if (send(sock,ptr,len,flags) != len) goto fail;

	PACKET_T packet;
	packet.type = 1;
	packet.pts_milli = 1;
	packet.metadata_len = 6;
	packet.metadata = (U8*)"HELLO";
	packet.payload_len = 6;
	packet.payload = (U8*)"WORLD";

	while (1) {

		// [1 byte]packet_type, [4 bytes]pts_milli, [4 bytes]metadata_len, [metadata_len]metadata, [4 bytes]payload_len, [payload_len]payload
		SEND(sock, (char*)&(packet.type), sizeof(packet.type), 0);
		SEND(sock, (char*)&(packet.pts_milli), sizeof(packet.pts_milli), 0);
		SEND(sock, (char*)&(packet.metadata_len), sizeof(packet.metadata_len), 0);
		SEND(sock, (char*)packet.metadata, packet.metadata_len, 0);
		SEND(sock, (char*)&(packet.payload_len), sizeof(packet.payload_len), 0);
		SEND(sock, (char*)packet.payload, packet.payload_len, 0);
	}

fail:
	closesocket(sock);
}

static void Terminate() {
#if defined WIN32
	WSACleanup();
#endif
}

static void Initialize() {

	static int init = 0;

	if (init)
		return;

#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("error at WSAStartup\n");
		return;
	}
#endif

	if ((g_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	if ((host = gethostbyname("localhost")) == NULL) {
		perror("gethostbyname");
		exit(1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;         
	addr.sin_port = htons(DEFAULT_PORT);     
	addr.sin_addr = *((struct in_addr*)host->h_addr);

	while (connect(g_sock, (struct sockaddr*)&addr, sizeof(struct sockaddr)) == -1) {
		perror("connect: will retry in -5- seconds");
		sleep(5);
	}

	init = 1;
	return;
}

void OutputThread() {
	Initialize();
	SendPackets(g_sock);
	Terminate();
}

