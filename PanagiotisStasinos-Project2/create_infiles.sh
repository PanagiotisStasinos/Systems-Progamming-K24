#!/bin/bash

let a=$#

if [ $a -gt 4 ] 
then
	echo more arguments given $a
	exit -1
elif [ $a -lt 4 ]
then
	echo less arguments given $a
	exit -1
fi

echo Number of input parameters = $a
echo Program Name = $0
echo dir name = $1
echo num of files = $2
echo num of dirs = $3
echo levels = $4


# check if dir name exists and if not make it
if [ -d "$1" ]
then
	echo $1 exists
else
	echo does not exists
	mkdir -p "$1"
fi


# produce #num_of_dirs names
for ((i = 0 ; i < $3 ; i++))
	do dirs[i]=''
	let b=$(($RANDOM%8))
	let b=b+1
	for ((j = 0 ; j < $b ; j++))	#produce a random name with length b
		do dirs[i]+=$(printf "%x" $(($RANDOM%16)) );
	done
	for ((j = 0 ; j < i ; j++))	#check if this dir name allready exists
		do if [ ${dirs[i]} == ${dirs[j]} ]
			then echo ${dirs[i]} dir allready exists
			exit -1
		fi
	done
	echo ${dirs[i]}	
done


# make dirs level by level
path="$1"/
temp_lvl=$4
for ((i = 0 ; i < $3 ; i++))
	do
	if [ $temp_lvl -ne 0 ]
	then
		path+="${dirs[i]}"
		echo $path
		mkdir -p "$path"
		path+=/
		let temp_lvl=temp_lvl-1
	else
		path="$1"/
		temp_lvl=$4
		path+="${dirs[i]}"
		echo $path
		mkdir -p "$path"
		path+=/
		let temp_lvl=temp_lvl-1
	fi	
done


#create files
for ((i = 0 ; i < $2 ; i++))
	do files[i]=''
	let b=$(($RANDOM%8))
	let b=b+1
	for ((j = 0 ; j < $b ; j++))	#produce a random name with length b
		do files[i]+=$(printf "%x" $(($RANDOM%16)) );
	done
	for ((j = 0 ; j < i ; j++))	#check if this dir name allready exists
		do if [ ${files[i]} == ${files[j]} ]
			then echo ${files[i]} file allready exists
			exit -1
		fi
	done
	# echo ${files[i]}	
done
echo
path="$1"/
temp_lvl=$4+1
num_of_folders=$3+1
for ((f = 0 ; f < $2 ; f++))
	do
	temp_path=$path
	let m=f%num_of_folders
	if [ $m -eq 0 ]
		then
			printf "%s" $path
			temp_path+=${files[f]}
			touch $temp_path
	else
		let m=m-1
		let p=m%$4
		printf "%s" $path
		for ((n = p ; n >= 0 ; n--))
		do
			printf "%s/" ${dirs[m-n]}
			temp_path+=${dirs[m-n]}/
		done
		temp_path+=${files[f]}
		touch $temp_path
	fi
	printf "%s\n" ${files[f]}

	let r=$(($RANDOM%128))
	let r=r+1
	echo $r kb
	str=""
	str=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $((r*1024)) | head -n 1)

	echo $str >> $temp_path
done