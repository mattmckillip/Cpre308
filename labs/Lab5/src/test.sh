#! /bin/bash
# pipe this script into the stdin of the print server
# ./test.sh | ./printserver -o log_file.log --n1 2 --n2 4

echo "#device=pdf1"
echo "#name=test-output-file1"
echo "#description=This is a test file to use with the print server program"
cat samplec.ps
echo "#device=pdf1"
echo "#name=test-output-file2"
echo "#description=This is a test file to use with the print server program"
cat samplec.ps
echo "#device=pdf1"
echo "#name=test-output-file3"
echo "#description=This is a test file to use with the print server program"
cat samplec.ps
echo "#device=pdf2"
echo "#name=test-output-file4"
echo "#description=This is a test file to use with the print server program"
cat samplec.ps
sleep 10
echo "#device=pdf1"
echo "#name=test-output-file5"
echo "#description=This is a test file to use with the print server program"
cat samplec.ps
echo "#device=pdf2"
echo "#name=test-output-file6"
echo "#description=This is a test file to use with the print server program"
cat samplec.ps
echo "#device=pdf2"
echo "#name=test-output-file7"
echo "#description=This is a test file to use with the print server program"
cat samplec.ps
sleep 10
echo "#device=pdf1"
echo "#name=test-output-file8"
echo "#description=This is a test file to use with the print server program"
cat samplec.ps
sleep 5
echo "#device=pdf2"
echo "#name=test-output-file9"
echo "#description=This is a test file to use with the print server program"
cat samplec.ps
echo "#device=pdf1"
echo "#name=test-output-file10"
echo "#description=This is a test file to use with the print server program"
cat samplec.ps
sleep 3
echo "#device=pdf1"
echo "#name=test-output-file11"
echo "#description=This is a test file to use with the print server program"
cat samplec.ps
sleep 1
echo "#device=pdf2"
echo "#name=test-output-file12"
echo "#description=This is a test file to use with the print server program"
cat samplec.ps
echo "#exit"

