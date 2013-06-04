#! /bin/bash

if [ -z "$2" ]
    then
    max=64
else
    max=$2
fi

if [ -z "$3" ]
	then
	FLGS = ""
else
	FLGS=$3
fi

OF=pqvm$(date +%s).data

echo "#P  T" > $OF
echo "#pqvm -s $FLGS" >> $OF

echo "Increasing threads from 1 to $max"

for ((i=1;i<=max;i++))
do
    echo "$i threads"
    /usr/bin/time -f"$i %e" -o$OF -a ./pqvm -sp$i $FLGS $1
done