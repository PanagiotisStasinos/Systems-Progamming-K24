#!/bin/bash
let sum=0;
let total=0;
let fail=0;
let pass=0;
while read -r onoma eponumo email hw1 hw2 hw3 exam; do
	let sum=15*$hw1+15*$hw2+15*$hw3+55*$exam;
	let sum=sum/100
	echo "$sum";

	let total=total+1;
	if [ $sum -lt 50 ]; then
		let fail=fail+1
		#echo "Fail" | mail -s "Grades" "$email"
	fi
	if [ $sum -gt 49 ]; then
		let pass=pass+1
		#echo "Pass" | mail -s "Grades" "$email"
	fi
	let sum=0;
done < "$1"
echo pass $pass
echo fail $fail
echo total $total