#!/bin/bash

clear
g++ -std=c++11 -pthread $1.cpp lib/*.cpp -o bin
./bin

# rm -r database/