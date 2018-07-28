#!/bin/bash
gcc `pkg-config --cflags gtk+-3.0` -o $2 $1 `pkg-config --libs gtk+-3.0`