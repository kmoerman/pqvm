#! /bin/bash

echo "#P  T" > pqvm.data

if [ -z "$2" ]
    then
    max=64
else
    max=$2
fi

echo "Increasing threads from 1 to $max"

for ((i=1;i<=max;i++))
do
    echo "$i threads"
    /usr/bin/time -f"$i %e" -opqvm.data -a ./pqvm -st$i $1
done