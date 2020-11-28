from subprocess import Popen, PIPE
import os
import re
import util

class Bot:
    def __init__(self, endpoint, username="bot", player_num=2):
        project_path = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        uno_path = project_path + "/build/src/uno"
        args = [uno_path, "-c", endpoint, "-u", username]
        
        self.game = Popen(args, stdin=PIPE, stdout=PIPE)
        self.player_num = player_num
        
        # self.hint_stat = 0
        self.last_played_card = None
        self.handcards = []
        self.cursor_index = 0

        self.game_loop()
    
    def press_keyboard(self, action=' '):
        if action == ' ':
            print("press: <space>")
        elif action == '\n':
            print("press: \\n")
        else:
            print("press:", action)

        action = bytes(action, "UTF-8")
        self.game.stdin.write(action)
        self.game.stdin.flush()
    
    def game_loop(self):
        hint_stat = -1
        last_hint_stat = -1
        while True:
            frame_info = util.next_frame(self.game, self.player_num)
            print(frame_info)
            if frame_info:
                # it's my turn
                hint_stat, self.last_played_card, self.handcards, self.cursor_index = frame_info
                if hint_stat != last_hint_stat:
                    # see something new, need time to think
                    time_to_think = 2
                    last_hint_stat = hint_stat
                
                if hint_stat == 4:
                    # always want to play again
                    self.press_keyboard('y')
                    continue

                if time_to_think == 0:
                    # ok, do some action
                    if hint_stat == 1:
                        self.try_play_card()
                    elif hint_stat == 2:
                        self.press_keyboard(' ')
                    elif hint_stat == 3:
                        self.press_keyboard('r')
                    else:
                        assert False
                else:
                    time_to_think -= 1

    def try_play_card(self):
        # press keyboard only once for each frame
        index_of_card_to_play = -1
        for i in range(len(self.handcards)):
            if util.can_be_played_after(self.handcards[i], self.last_played_card):
                index_of_card_to_play = i
                break
        print("index of card to play:", index_of_card_to_play, "\tcursor index:", self.cursor_index)
        if index_of_card_to_play == -1:
            # no card can be played
            self.press_keyboard(' ')
        else:
            # there is a card to play
            if index_of_card_to_play < self.cursor_index:
                self.press_keyboard(',')
            elif index_of_card_to_play > self.cursor_index:
                self.press_keyboard('.')
            else:
                self.press_keyboard('\n')
        

bot = Bot("localhost:9091", player_num=3)
