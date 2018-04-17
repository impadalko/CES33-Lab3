#!/bin/bash

g++ -pthread main.cpp lib/*.cpp -o bin
./bin
