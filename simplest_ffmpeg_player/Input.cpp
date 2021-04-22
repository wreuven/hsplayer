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

static int	g_listen_sock;
static struct sockaddr_in addr;
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
	printf("%s %s\n", packet->metadata, packet->payload);
}

static void HandleConnection(int sock) {

#	define RECV(sock,ptr,len,flags) if (recv(sock,ptr,len,flags) != len) goto fail;
#   define CHK(a) if (!a) goto fail;

	while (1) {
		PACKET_T packet;
		// [1 byte]packet_type, [4 bytes]pts_milli, [4 bytes]metadata_len, [metadata_len]metadata, [4 bytes]payload_len, [payload_len]payload
		RECV(sock, (char*)&(packet.type), sizeof(packet.type), MSG_WAITALL);
		RECV(sock, (char*)&(packet.pts_milli), sizeof(packet.pts_milli), MSG_WAITALL);
		RECV(sock, (char*)&(packet.metadata_len), sizeof(packet.metadata_len), MSG_WAITALL);
		packet.metadata = (U8*)malloc(packet.metadata_len); CHK(packet.metadata);
		RECV(sock, (char*)packet.metadata, packet.metadata_len, MSG_WAITALL);
		RECV(sock, (char*)&(packet.payload_len), sizeof(packet.payload_len), MSG_WAITALL);
		packet.payload = (U8*)malloc(packet.payload_len); CHK(packet.payload);
		RECV(sock, (char*)packet.payload, packet.payload_len, MSG_WAITALL);
		HandlePacket(&packet);
	}

fail:
	closesocket(sock);
}

static void WaitForAndHandleConnections() {
	int sin_size = sizeof(struct sockaddr_in);
	int sock;
	int finished = 0;

	while (!finished) {
		if ((sock = accept(g_listen_sock, (struct sockaddr*)&addr, &sin_size)) == -1)
			perror("accept");
		HandleConnection(sock);
		closesocket(sock);
	}
}

static void Terminate() {
	closesocket(g_listen_sock);
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

	if ((g_listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;         
	addr.sin_port = htons(DEFAULT_PORT);
	addr.sin_addr.s_addr = INADDR_ANY; 

	if (bind(g_listen_sock, (struct sockaddr*)&addr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(1);
	}

	if (listen(g_listen_sock, 1) == -1) {
		perror("listen");
		exit(1);
	}

	init = 1;
	return;
}

void InputThread() {
	Initialize();
	WaitForAndHandleConnections();
	Terminate();
}

