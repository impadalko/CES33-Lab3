#!/bin/bash

clear
g++ -std=c++11 -pthread synchronized.cpp lib/*.cpp -o bin
./bin
