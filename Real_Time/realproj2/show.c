#include "local.h"
//gcc deettest.c -o deek -lglut -lGLU -lGL -lm

int xpos,ypos;

int flag = 0;
void time(int){
    glutPostRedisplay();
    glutTimerFunc(1000/60,time,0);
    if(pos.x<350 && flag == 0){
        pos.x++;
        if(pos.x == 350)
            flag = 1;
    }
    else if(pos.x > 50 && flag==1){
        pos.x--;
        if(pos.x == xpos)
            flag=0;
    }

}

void reshape(int w, int h){
    glViewport(0,0,(GLsizei)w,(GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 400.0, 0.0, 400.0);
    glMatrixMode(GL_MODELVIEW);
}

void dot(){
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1,0,0);
    glPointSize(25);

    glBegin(GL_POINTS);
    //parnet
    glVertex2i(200,50);
    glColor3f(0,0,0);
    glVertex2i(50,50);
    glVertex2i(350,50);

    glColor3f(0,1,1);

    //////////////////////

    //glPointSize(13);
    glVertex2i(70,90);
    glVertex2i(30,70);
    glVertex2i(50,100);
    glVertex2i(60,70);
    glVertex2i(30,90);


    //master


    glVertex2i(190,70);
    glVertex2i(220,100);
    glVertex2i(210,80);
    glVertex2i(190,90);
    glVertex2i(230,80);





    glEnd();


    glColor3f(0,0,1.0);

    glPointSize(9);
    glBegin(GL_POINTS);
    glVertex2f(pos.x,pos.y);

    glEnd();

    glutSwapBuffers();

    //glFlush();
}

void myInit (void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);

}

int main(int argc, char **argv){
    //-----------------------------
    key_t key = ftok(".",'M');
    int msgid = msgget(key,0666|IPC_CREAT);

    while(1){
        msgrcv(msgid,&pos,sizeof(pos),1,0);
        printf("the x is : %d\nthe y is : %d\n",pos.x,pos.y);
        xpos = pos.x;
        ypos = pos.y;
        msgctl(msgid,IPC_RMID,NULL);
        glutInit(&argc,argv);
        glutInitWindowSize(600,600);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
        glutInitWindowPosition(20, 20);
        glutCreateWindow("message encoding");
        glutDisplayFunc(dot);

        glutReshapeFunc(reshape);
        glutTimerFunc(0,time,0);
        myInit();
        glutMainLoop();
        //msgrcv(msgid,&pos,sizeof(pos),1,0);
        //printf("the x is : %d\nthe y is : %d\n",pos.x,pos.y);
        return EXIT_SUCCESS;
    }

    //-----------------------------

}

