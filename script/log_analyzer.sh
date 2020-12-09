#!/bin/bash

script_dir=$(cd "$(dirname "$0")" >/dev/null 2>&1; pwd -P)
proj_dir=$(dirname $script_dir)
log_dir=$proj_dir"/logs"
conf_path=$script_dir"/server.conf"

declare -A restart_num
declare -A start_num
declare -A end_num

declare -A player_num_by_port
declare -A bot_num_by_port

load_config()
{
    while read port player_num bot_num
    do
        player_num_by_port[$port]=$player_num
        bot_num_by_port[$port]=$bot_num
    done < $conf_path
}

load_hint()
{
    for port in "${!player_num_by_port[@]}";
    do
        # prepare_room $port ${player_num_by_port[$port]} ${bot_num_by_port[$port]}
        log_path=$log_dir"/server-"$port".log"
        hint_list=$(cat $log_path | awk '{print $6}')
        for hint in $hint_list
        do
            if [[ $hint == "spdlog" ]]; then
                ((restart_num[$port]+=1))
            elif [[ $hint == "Starts." ]]; then
                ((start_num[$port]+=1))
            elif [[ $hint == "Ends." ]]; then
                ((end_num[$port]+=1))
            fi
        done
    done
}

print()
{
    echo "Times of service restart: $restart_num"
    echo "Times of game start: $start_num"
    echo "Times of game over: $end_num"

    echo -e "  port\t      restart  start    end"
    echo -e "-----------------------------------"
    for port in "${!player_num_by_port[@]}";
    do
        bot_num=${bot_num_by_port[$port]}
        player_num=${player_num_by_port[$port]}
        _restart_num=${restart_num[$port]}
        _start_num=${start_num[$port]}
        _end_num=${end_num[$port]}
        echo -e "$port ($bot_num/$player_num)\t$_restart_num\t$_start_num\t $_end_num"
    done |
    sort
}

load_config
load_hint
print
