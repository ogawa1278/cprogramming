#!/bin/sh
gcc comp.c -lm

for i in {1..6}
do
    mv ${i}.bmp input.bmp
    ./a.out
    mv input.bmp ${i}.bmp
    mv output.bmp output${i}.bmp
done
