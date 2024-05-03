#!/bin/bash

make clean
flex lexical.l
gcc lex.yy.c -lfl -o scanner