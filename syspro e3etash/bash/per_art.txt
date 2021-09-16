#!/bin/bash
if test -f "$1"; then
    echo "$1 exist"
fi
let total=0;
let art=0;
let per=0;
while read -r onoma sdi email hw1 hw2 hw3 exam; do

	let total=total+1;
	let temp="${sdi: -1}"%2
	if [ $temp -eq 0 ]; then
		let art=art+1
		echo $sdi >> grades1art
	fi
	if [ $temp -eq 1 ]; then
		let per=per+1
		echo $sdi >> grades1per
	fi
	let sum=0;
done < "$1"
echo per $per
echo art $art
echo total $total