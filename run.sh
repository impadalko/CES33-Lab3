#!/bin/bash

g++ -pthread db.cpp db_lib.cpp -o bin
./bin
