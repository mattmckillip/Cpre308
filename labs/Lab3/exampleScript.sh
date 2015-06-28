#! /bin/bash
# this is a comment
echo Running test script

#change directory
cd ../
pwd

#do a few things
echo Filling up disk quota...
touch a b c d e

#having some fun
echo testing export
export HELLO="Allo!"

#
echo printing environment variables
echo $HELLO

#
echo deleting file system...
rm a b c d e
