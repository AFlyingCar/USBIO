#!/bin/bash
g++ ./src/*.cpp ./test/AudioTest/*.cpp -lusb-1.0 -o usbiotest -std=c++11

