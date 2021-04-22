#include <stdio.h>

#define VIDEO_CODEC AV_CODEC_ID_H264

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
 //Windows
extern "C"
{
#include "libavcodec/avcodec.h"
};
#else
 //Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#ifdef __cplusplus
};
#endif

#endif
#include "Player.h"

static AVCodec *pCodec;
static AVCodecContext *pCodecCtx = NULL;
static AVCodecParserContext *pCodecParserCtx = NULL;
static AVFrame* pFrame;
static AVPacket encodedFrame;
static int height=0;
static int width=0;

static int VideoInit() {

	static int init = 0;

	if (init)
		return 0;

	init = 1;

	avcodec_register_all();

	pCodec = avcodec_find_decoder(VIDEO_CODEC);
	if (!pCodec) {
		printf("Codec not found\n");
		return -1;
	}

	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (!pCodecCtx) {
		printf("Could not allocate video codec context\n");
		return -1;
	}

	pCodecParserCtx = av_parser_init(VIDEO_CODEC);
	if (!pCodecParserCtx) {
		printf("Could not allocate video parser context\n");
		return -1;
	}

	//if(pCodec->capabilities & CODEC_CAP_TRUNCATED)
	//    pCodecCtx->flags|= CODEC_FLAG_TRUNCATED; 

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("Could not open codec\n");
		return -1;
	}

	av_new_packet(&encodedFrame, MAX_ENCODED_FRAME_SIZE);

	return 0;
}

static void VideoInjectPacket(PACKET_T *packet) {

	U32 payload_len = packet->payload_len;
	U8 *payload = (U8*)packet->payload;
	int len;
	int ret;

	while (payload_len > 0) {

		len = av_parser_parse2(
				pCodecParserCtx, 
				pCodecCtx,
				&encodedFrame.data,
				&encodedFrame.size,
				payload,
				payload_len,
				AV_NOPTS_VALUE, 
				AV_NOPTS_VALUE, 
				AV_NOPTS_VALUE);

		payload		+= len;
		payload_len -= len;

		// no full frame yet ?

		if (encodedFrame.size == 0)
			continue;

		//Some Info from AVCodecParserContext

		printf("[Encoded Frame Size]:%6d\t", encodedFrame.size);

		switch (pCodecParserCtx->pict_type) {
			case AV_PICTURE_TYPE_I: printf("Type:I\t"); break;
			case AV_PICTURE_TYPE_P: printf("Type:P\t"); break;
			case AV_PICTURE_TYPE_B: printf("Type:B\t"); break;
			default: printf("Type:Other\t"); break;
		}

		printf("Number:%4d\n", pCodecParserCtx->output_picture_number);

		int got_picture = 0;

		ret = avcodec_decode_video2(
				pCodecCtx, 
				pFrame, 
				&got_picture, 
				&encodedFrame);

		// reset the encodedFrame length (needed??)
		encodedFrame.size = 0;

		if (ret < 0) {
			printf("Decode Error.\n");
			return;
		}

		if (got_picture) {
			if (height != pCodecCtx->height || width != pCodecCtx->width) {
				printf("\nCodec Full Name:%s\n", pCodecCtx->codec->long_name);
				printf("width:%d\nheight:%d\n\n", pCodecCtx->width, pCodecCtx->height);
			}
			printf("Succeed to decode 1 frame!\n");
		}
	}
}

static void VideoTerm() {
	av_free_packet(&encodedFrame);
	av_parser_close(pCodecParserCtx);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	av_free(pCodecCtx);
}
