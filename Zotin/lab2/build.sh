#!/bin/bash

read -p "Введите номер задания (1/2) " choose

if [[ ! "$choose" =~ ^[12]$ ]]; then
    echo "Неверно выбранное задание"
    exit 1
fi

CFLAGS="-O0 --std=c++23"
LDFLAGS="-fopenmp"

g++ $CFLAGS $LDFLAGS $choose/main.cpp -o main

read -p "Запустить? (y/n) " run

if [[ "$run" == "y" ]]; then
    if [ "$choose" -eq 2 ]; then
        > info.txt
        "./main" | tee info.txt
    else
        "./main"
    fi
fi
