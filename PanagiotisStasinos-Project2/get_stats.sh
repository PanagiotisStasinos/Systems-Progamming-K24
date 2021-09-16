#!/bin/bash
# Check to see if a pipe exists on stdin.
if [ -p /dev/stdin ]; then
	let client_count=0
	let prev_receiver=0
	let min=0	#min client id
	let max=0	#max client id
	let total_bytes=0
	let num_of_files=0
	printf "Clients id :"
	while IFS=$'\t' read receiver sender file size; do
		# echo "${receiver} - ${sender} - ${file} - ${size}"
		if [ $prev_receiver -ne $receiver ]; then
			let client_count=client_count+1
			let prev_receiver=${receiver}
			printf " %s" ${receiver}
		fi
		if [ $min -eq 0 ]; then #to avoid 0
			let min=$receiver
		fi
		if [ $min -gt $receiver ]; then
			let min=$receiver
		fi
		if [ $max -lt $receiver ]; then
			let max=$receiver
		fi

		let total_bytes=total_bytes+$size
		let num_of_files=num_of_files+1
	done
	echo
	echo $client_count clients
	echo min $min
	echo max $max
	echo $total_bytes bytes sended/received
	echo $num_of_files files sended/received
else
	echo "No input was found on stdin"
fi