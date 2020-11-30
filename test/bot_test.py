import os, sys, time
from subprocess import Popen, PIPE

proj_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(proj_dir + "/bot")
from bot import Bot

player_num = 2
if len(sys.argv) > 1:
    player_num = int(sys.argv[1])

start_time = time.time()

if os.name == 'nt':
    # windows
    uno_path = proj_dir + "\\build\\src\\Debug\\uno.exe"
else:
    # unix
    uno_path = proj_dir + "/build/src/uno"

server = Popen([uno_path, "-l", "9091", "-n", str(player_num)], stdout=PIPE)
time.sleep(0.2)

bots = []

for i in range(player_num):
    _username = "bot" + str(i)
    bots.append(Bot("localhost:9091", username=_username, player_num=player_num, debug=True))
    time.sleep(0.2)

for i in range(player_num):
    for j in range(player_num - i + 1):
        bots[i].single_loop()

cur_player = -1

for i in range(player_num):
    if bots[i].hint_stat == 1:
        cur_player = i
        break
assert cur_player != -1

def wrap_with(num_to_wrap, wrap_range):
    # -1 % 3 == 2 in Python
    return num_to_wrap % wrap_range

def update_cur_player(last_played_card, cur_player, is_in_clockwise):
    if last_played_card[1:] == 'R':
        is_in_clockwise = not is_in_clockwise
    if is_in_clockwise:
        cur_player = wrap_with(cur_player + 1, player_num)
    else:
        cur_player = wrap_with(cur_player - 1, player_num)
    return cur_player, is_in_clockwise

# 0 -> 1 -> 2 -> 3 -> 0
# 0 -> 1 -> 2 -> 0
# 0 -> 1 -> 3 -> 0
# 0 -> 1 -> 0

def test_end():
    global start_time, server
    end_time = time.time()
    print("time consumed: " + format(end_time - start_time, '.2f') + " seconds")
    print("\033[92mtest success\033[0m")
    server.kill()
    exit(0)

old_last_played_card = bots[cur_player].last_played_card
last_played_card = bots[cur_player].last_played_card
is_in_clockwise = (last_played_card[1:] != 'R')
while True:
    print("cur_player:", cur_player)
    has_done_action = (bots[cur_player].hint_stat > 0)
    while True:
        bots[cur_player].single_loop()
        has_done_action = has_done_action or bots[cur_player].hint_stat > 0
        if (has_done_action and bots[cur_player].hint_stat == 0):
            print("[TURN ENDS]")
            # turn ends
            print("old last played card:", last_played_card)
            old_last_played_card = last_played_card
            last_played_card = bots[cur_player].last_played_card
            print("new last played card:", last_played_card)
            cur_player, is_in_clockwise = \
                update_cur_player(last_played_card, cur_player, is_in_clockwise)
            for i in range(player_num):
                while True:
                    print("[loop in]", i)
                    bots[i].single_loop()
                    if bots[i].hint_stat == 4:
                        test_end()
                    assert bots[i].last_played_card == old_last_played_card \
                        or bots[i].last_played_card == last_played_card
                    if bots[i].last_played_card == last_played_card:
                        break
                print("bots", i, "last_played_card", bots[i].last_played_card)
                assert bots[i].last_played_card == last_played_card
            break
