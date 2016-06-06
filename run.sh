#!usr/local/bin/bash

last_command=$(sudo cat ~/.bash_history | awk -v n=2 '{x[NR%n]=$0}END{print x[(NR+1)%n]}')
echo $last_command

#c=$(IFS=' |'; for word in $last_command; do echo "$word"; done)

IFS=' |' read -r -a array <<< "$last_command"
s=${array[0]}
l=${#array[0]}

for ((i=0; i < (${#s}-1); i++)); do
    echo $(compgen -ac | grep -o -w "\w\{$l\}" | grep "${s:$i:2}" |sort -u)
    echo
done
