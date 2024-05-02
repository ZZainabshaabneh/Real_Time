#!/bin/sh


gcc -g main.c -o main -lpthread -lrt
gcc -g WrapTorus.c -o WrapTorus  -lpthread -lrt
gcc -g triangle.c -o triangle  -lpthread -lrt
gcc -g tetrahedron.c -o tetrahedron  -lpthread -lrt
gcc -g SimpleAnim.c -o person -lpthread -lrt
gcc -g SimpleDraw.c -o SimpleDraw  -lglut -lGLU -lGL -lm -lpthread -lrt
gcc -g SimpleLight.c -o SimpleLight  -lpthread -lrt
gcc -g SimpleNurbs.c -o SimpleNurbs  -lpthread -lrt
gcc -g Solar.c -o Solar -lpthread -lrt
gcc -g new_ex1.c -o new_ex1 -lpthread -lrt
gcc -g ConnectDots.c -o ConnectDots -lpthread -lrt
gcc -g GluCylinders.c -o GluCylinders -lpthread -lrt
gcc -g spinningsquare.c -o spinningsquare  -lpthread -lrt
gcc -g robotarm.c -o robotarm -lpthread -lrt
./test