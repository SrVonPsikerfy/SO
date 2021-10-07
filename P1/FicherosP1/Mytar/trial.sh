#!/bin/sh
if [ ! -f "./mytar" ]; then
    echo "File mytar doesn't exists"
    exit 1
fi

if [ ! -x "./mytar" ]; then
    echo "File mytar isn't executable"
    exit 1
fi

if [ -d "./tmp" ]; then 
    rm -r "./tmp"
fi

mkdir "./tmp"
cd "./tmp"

touch "file1.txt"
echo "Hello World!" > file1.txt

touch "file2.txt"
head "/etc/passwd" > file2.txt # -n [number] limits reading to that specific number of lines

touch "file3.dat"
head -c 1024 "/dev/urandom" > file3.dat

touch "file4.txt"
echo "Ay me cago en todo" > file4.txt

../mytar -cf "filetar.mtar" "file1.txt" "file2.txt" "file3.dat" "file4.txt"

mkdir "./out"
cp "filetar.mtar" "./out"
cd "./out"

../../mytar -xf "filetar.mtar"

diff -q "file1.txt" "../file1.txt" 1>/dev/null # dev null erases output
if [ $? != "0" ]; then # exit status of the last executed command [in this case, diff]
  echo "file1.txt was not extracted correctly"
  exit 1
fi

echo "Correct!"
exit 0