#include<GL/glut.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>

struct position{
    long msg_type;
    int x;
    int y;
} pos;
