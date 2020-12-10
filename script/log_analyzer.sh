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

bot_num_filter=-1
player_num_filter=-1
days_filter=100

parse_arg()
{
    # -v: show version
    # -n x/y: x is bot num, y is total player num
    # -d x: just show logs of recent x days
    while getopts ":vn:d:" options; do
        case "${options}" in
            v)
                echo "log_analyzer version 1.0"
                exit 0
                ;;
            n)
                bot_num_filter=$(echo $OPTARG | cut -d'/' -f1)
                player_num_filter=$(echo $OPTARG | cut -d'/' -f2)
                if [[ -z $bot_num_filter ]]; then
                    bot_num_filter=-1
                fi
                if [[ -z $player_num_filter ]]; then
                    player_num_filter=-1
                fi
                ;;
            d)
                days_filter=$OPTARG
                ;;
            :)
                echo "Error: -${OPTARG} requires an argument."
                exit -1
                ;;
            *)
                exit -1
                ;;
        esac
    done
}

satisfy_n_filter()
{
    if [[ $bot_num_filter != -1 && $bot_num_filter != $1 ]]; then
        echo 0
    elif [[ $player_num_filter != -1 && $player_num_filter != $2 ]]; then
        echo 0
    else
        echo 1
    fi
}

load_config()
{
    while read port player_num bot_num
    do
        if [[ $(satisfy_n_filter $bot_num $player_num) == 1 ]]; then
            player_num_by_port[$port]=$player_num
            bot_num_by_port[$port]=$bot_num
        fi
    done < $conf_path
}

load_hint()
{
    for port in "${!player_num_by_port[@]}";
    do
        log_path=$log_dir"/server-"$port".log"
        while read date _ _ _ _ hint
        do
            date=$(echo $date | cut -d'[' -f2)
            # date is in format of yyyy-mm-dd
            seconds_diff=$(( $(date -d 'now' +%s) - $(date -d $date +%s) ))
            day_diff=$(( $seconds_diff / (60*60*24) ))
            if [[ $day_diff -lt $days_filter ]]; then
                if [[ $hint == "spdlog" ]]; then
                    ((restart_num[$port]+=1))
                elif [[ $hint == "Starts." ]]; then
                    ((start_num[$port]+=1))
                elif [[ $hint == "Ends." ]]; then
                    ((end_num[$port]+=1))
                fi
            fi
        done < $log_path
    done
}

print()
{
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

parse_arg $@
load_config
load_hint
print
