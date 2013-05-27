#! /bin/bash

## Test kronecker
echo "test kronecker
"

for i in {10..12}
do
    p=$((2**i))
    #./kronecker $p
    ./kronecker $p seq
done

## Test sigma-x
echo "test sigma-x
"

for i in {10..22}
do
    for (( j = 1 ; j<i; j++))
    do
	p=$((2**i))
      #./sigma-x $p $j
      ./sigma-x $p $j seq
    done
done

## Test sigma-z
echo "test sigma-z
"

for i in {10..22}
do
    for (( j = 1 ; j<i; j++))
    do
	p=$((2**i))
	#./sigma-z $p $j
	./sigma-z $p $j seq
    done
done

## Test normalize
echo "test normalize
"

for i in {10..22}
do
    p=$((2**i))
    #./normalize $p
    ./normalize $p seq
done

echo "generating output
"

## Make output
R --no-save < graph-tests.r
