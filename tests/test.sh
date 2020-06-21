#!/bin/bash

../sorted-diff normal ./test1/base.txt ./test1/branch.txt ./additions_tmp.txt ./deletions_tmp.txt > /dev/null
cmp ./test1/additions.txt ./additions_tmp.txt
ADDITIONS_CMP=$?
cmp ./test1/deletions.txt ./deletions_tmp.txt
DELETIONS_CMP=$?
rm ./additions_tmp.txt ./deletions_tmp.txt
if [ $ADDITIONS_CMP -ne 0 ]
then
    exit 1
fi
if [ $DELETIONS_CMP -ne 0 ]
then
    exit 1
fi
