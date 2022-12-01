#!/bin/bash 
gcc main.c util.c -O2 -lGL -lGLEW -lglut -lm -std=c99 -Wall -Wextra -o sand
./sand
