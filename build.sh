#!/bin/bash
progDir='/home/jacobhampton/Prog05/Programs/*.cpp'
vr=$1
count=0
for file in $progDir
do
    count=$((count+1))
    f="${file##*/}"
    ex="${f%.cpp}"
    g++ -std=c++14 -Wall $file -lstdc++ -lpthread -o $ex
    mv $ex "${progDir%/*}"
    if (($count == $vr)); then
        ./$ex "/home/jacobhampton/Prog05/Data/smallMap_Horiz.map" "sajhbd" 1 1 1
    fi 
done
