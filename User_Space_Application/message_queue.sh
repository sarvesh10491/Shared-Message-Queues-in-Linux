#!/bin/bash
rm -rf /tmp/clk_freq.lst
rm -rf /tmp/makelog.lst
touch /tmp/clk_freq.lst
touch /tmp/makelog.lst

val=`cat /proc/cpuinfo |grep "cpu MHz"|uniq|cut -d ":" -f2`
multi=1000000
clk_freq=$(echo "${val}*${multi}" |bc)
echo $clk_freq > /tmp/clk_freq.lst
chmod 775 /tmp/clk_freq.lst
clk_freq_cal=`cat /tmp/clk_freq.lst|cut -d "." -f1`

if [ "$clk_freq_cal" -eq 0 ]
then
  echo "Error capturing clock frequency."
  exit 0
else
sed -i "s/CLK_FREQUENCY_VAR/$clk_freq_cal/" ./queue_ops.h
fi

make >> /tmp/makelog.lst

if [ -f msg_queue ]
then
    ./msg_queue
else
    echo "Error. Object file not found."
fi