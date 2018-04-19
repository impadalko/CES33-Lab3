#!/bin/bash

clear
g++ -std=c++11 -pthread not_synchronized.cpp lib/*.cpp -o bin
./bin
