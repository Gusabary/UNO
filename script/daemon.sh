#!/bin/bash

script_dir=$(cd "$(dirname "$0")" >/dev/null 2>&1; pwd -P)
proj_dir=$(dirname $script_dir)
log_dir=$proj_dir"/logs"

server_path=$proj_dir"/build/src/uno"
bot_path=$proj_dir"/bot/bot.py"
conf_path=$script_dir"/server.conf"

declare -A player_num_by_port
declare -A bot_num_by_port

print_log()
{
    echo "[$(date '+%Y/%m/%d %H:%M:%S')] $1"
}

prepare_room()
{
    port=$1
    totol_player_num=$2
    bot_num=$3
    log_path=$log_dir"/server-$port.log"
    print_log "starting server... [port: $port, player_num: $totol_player_num, bot_num: $bot_num]"
    $server_path -l $port -n $totol_player_num --log $log_path >/dev/null 2>&1 &
    sleep 0.2
    print_log "server started."
    for ((i = 0; i < $bot_num; i++)) 
    do
        print_log "starting bot... [port: $port, bot_index: $i]"
        python3 $bot_path localhost:$port $totol_player_num bot$i >/dev/null 2>&1 &
        print_log "bot started."
    done
    print_log "server and bot started, room prepared."
    print_log "-------------------------------------"
}

init_all_rooms()
{
    for port in "${!player_num_by_port[@]}";
    do
        prepare_room $port ${player_num_by_port[$port]} ${bot_num_by_port[$port]}
    done
}

load_config()
{
    while read port player_num bot_num
    do
        player_num_by_port[$port]=$player_num
        bot_num_by_port[$port]=$bot_num
    done < $conf_path
}

daemonize()
{
    while :
    do
        sleep 10
        for port in "${!player_num_by_port[@]}";
        do
            if [ -z $(netstat -nlt | awk '{print $4}' | grep $port) ]; then
                print_log "server on port $port crashed, restarting..."
                prepare_room $port ${player_num_by_port[$port]} ${bot_num_by_port[$port]}
                print_log "server restarted."
            fi
            # maybe need to check bot num ?
        done
        print_log "check complete"
    done
}

load_config
init_all_rooms
daemonize
