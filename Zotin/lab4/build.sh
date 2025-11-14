#!/bin/bash
set -e

if [[ ! "$1" =~ ^[123]$ ]]; then
    echo "Неверно выбранное задание"
    exit 1
fi

CFLAGS="-O0 --std=c++23 -g"
LDFLAGS="-ltbb -fopenmp -I/home/hixac/Projects/school/Zotin/"

echo "Компилируется..."
g++ $CFLAGS $LDFLAGS $1/main.cpp -o main

echo "Запускается..."
"./main"
