#include <stdio.h>

#include "Player.h"

struct input_type_to_func_map_t {
    U8 type;
    int (*function)(PACKET_T* packet);
};

enum _packet_types {
    TYPE_NULL = 0,
    TYPE_VIDEO_INPUT,
    TYPE_AUDIO_INPUT,
    TYPE_VIDEO_CONFIG,
    TYPE_AUDIO_CONFIG,
    TYPE_VIDEO_FRAME,
    TYPE_AUDIO_FRAME
};

int video_input_cb(PACKET_T *packet);
int audio_input_cb(PACKET_T* packet);
int video_input_config_cb(PACKET_T* packet);
int audio_input_config_cb(PACKET_T* packet);
int video_frame_cb(PACKET_T* packet);
int audio_frame_cb(PACKET_T* packet);

struct input_type_to_func_map_t input_type_to_func_map[] = {
    { TYPE_VIDEO_INPUT, video_input_cb },
    { TYPE_AUDIO_INPUT, audio_input_cb },
    { TYPE_VIDEO_CONFIG, video_input_config_cb },
    { TYPE_AUDIO_CONFIG, audio_input_config_cb },
};

int video_input_cb(PACKET_T* packet) {
    return 0;
}
int audio_input_cb(PACKET_T* packet) {
    return 0;
}
int video_input_config_cb(PACKET_T* packet) {
    return 0;
}
int audio_input_config_cb(PACKET_T* packet) {
    return 0;
}
int video_frame_cb(PACKET_T* packet) {
    return 0;
}
int audio_frame_cb(PACKET_T* packet) {
    return 0;
}

// config changes on input thread forwarded to playout thread to be handled at the correct point

void input_thread() {
}

void playout_thread() {
}

int main()
{
    start_thread(OutputThread);
    start_thread(InputThread);
    sleep(10000);

    return 0;
}
