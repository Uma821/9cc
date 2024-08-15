#!/bin/bash
./9cc codegen_helper.c > tmp.s
cc -static -o 9cc codegen.o tmp.s main.o parse.o tokenize.o type.o
