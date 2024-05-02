#include "local.h"

int main(){
    key_t key = ftok(".",'M');
    int msgid = msgget(key,0666|IPC_CREAT);
    pos.msg_type = 1;
    while(1){
        int x,y;
        printf("type x position : ");
        scanf("%d",&x);
        printf("type y position : ");
        scanf("%d",&y);
        pos.x=x;
        pos.y=y;
        msgsnd(msgid,&pos,sizeof(pos),0);
    }
    return 0;
}
