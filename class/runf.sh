#!/bin/bash
cc=$1

function a_sub {
	echo "Start run $i at `date`">>run.txt
	$cc $i > xx.txt 2>&1
}

starttime=`date +%s`
export starttime

tmp_fifofile="/tmp/$$.fifo"

mkfifo $tmp_fifofile

exec 6<>$tmp_fifofile

rm -f $tmp_fifofile
rm -f run.txt
thread=15

for ((i=0;i<$thread;i++));
#for ((i -e 0; i-le $thread;i++));
do
    echo >&6
done

for ((i=$2;i<=$3;i++))
do
#		if [$i -ge 0 -a $i -le 0]
		if (($i >= 0 & $i <= 0))
		then
			echo "The file of run $i is broken !">>run.txt
#		elif [$i -ge 0 -a $i -le 0]
		elif (($i >= 0 & $i <= 0))
		then
			echo "Run $i has done !">>run.txt
		else
    		read -u6
    		{
        		(a_sub && echo "Finish run $i at `date`">>run.txt) || (echo "Run$i is failed">>run.txt)
        		echo >&6
			} &
		fi
done
wait

exec 6>&-
