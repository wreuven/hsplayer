#! /bin/sh
gcc *.cpp -g -o hsplayer  \
-I /usr/local/include -L /usr/local/lib -lSDL2main -lSDL2 -lavformat -lavcodec -lavutil -lswscale -pthread
