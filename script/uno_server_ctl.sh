#!/bin/bash

script_dir=$(cd "$(dirname "$0")" >/dev/null 2>&1; pwd -P)
daemon_script_path=$script_dir"/daemon.sh"
log_path=$script_dir"/uno.log"
conf_path=$script_dir"/server.conf"

start_daemon()
{
    $daemon_script_path >$log_path 2>&1 & 
}

# target can be daemon or a port
get_target()
{
    if [[ $1 == "daemon" ]];
    then
        if [ ! -z "$(ps aux | grep daemon.sh | grep -v grep)" ]; then
            echo "daemon status: running"
        else
            echo "daemon status: shutdown"
        fi
    else
        if [ ! -z "$(netstat -nlt | awk '{print $4}' | grep $1)" ]; then
            echo "server-$1 status: running"
        else
            echo "server-$1 status: shutdown"
        fi
    fi
}

list_ports()
{
    while read port player_num bot_num
    do
        if [ ! -z "$(netstat -nlt | awk '{print $4}' | grep $port)" ]; then
            echo "server-$port status: running"
        else
            echo "server-$port status: shutdown"
        fi
    done < $conf_path
}

# mode can be [start, get, list]
mode=$1

# target can be [daemon, $port]
target=$2

if [[ $mode == "start" ]]; then
    start_daemon
elif [[ $mode == "get" ]]; then
    get_target $target
elif [[ $mode == "list" ]]; then
    list_ports
fi