#!/bin/bash
set -e

if [[ ! "$1" =~ ^[12]$ ]]; then
    echo "Неверно выбранное задание"
    exit 1
fi

CFLAGS="-O0 --std=c++23 -g"
LDFLAGS="-fopenmp -I/home/hixac/Projects/school/Zotin/"

echo "Компилируется..."
g++ $CFLAGS $LDFLAGS $1/main.cpp -o main

echo "Запускается..."
if [ "$1" -eq 2 ]; then
    > info.txt
    "./main" | tee info.txt
else
    "./main"
fi
