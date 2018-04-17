#!/bin/bash

g++ -std=c++11 -pthread main.cpp lib/*.cpp -o bin
./bin
