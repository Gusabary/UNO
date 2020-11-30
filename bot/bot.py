from subprocess import Popen, PIPE
import os, sys
import re
import util

class Bot:
    def __init__(self, endpoint, username="bot", player_num=2, debug=False):
        project_path = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        if os.name == 'nt':
            # windows
            uno_path = project_path + "\\build\\src\\Debug\\uno.exe"
        else:
            # unix
            uno_path = project_path + "/build/src/uno"
        args = [uno_path, "-c", endpoint, "-u", username]
        
        self.game = Popen(args, stdin=PIPE, stdout=PIPE)
        self.player_num = player_num
        
        self.last_played_card = None
        self.handcards = []
        self.cursor_index = 0

        self.hint_stat = -1
        self.last_hint_stat = -1
        self.time_to_think = -1

        self.debug = debug

    def press_keyboard(self, action=' '):
        if self.debug:
            if action == ' ':
                print("press: <space>")
            elif action == '\n':
                print("press: \\n")
            else:
                print("press:", action)

        action = bytes(action, "UTF-8")
        self.game.stdin.write(action)
        self.game.stdin.flush()
    
    def single_loop(self):
        if self.debug:
            print('--------------------------------------------------------------------------')
        frame_info = util.next_frame(self.game, self.player_num, self.debug)
        if self.debug:
            print(frame_info)
        self.hint_stat, self.last_played_card, self.handcards, self.cursor_index = frame_info
        if self.hint_stat > 0:
            # it's my turn
            if self.hint_stat != self.last_hint_stat:
                # see something new, need time to think
                self.time_to_think = 2
                self.last_hint_stat = self.hint_stat
            
            if self.hint_stat == 4:
                # always want to play again
                self.press_keyboard('y')
                return

            if self.time_to_think == 0:
                # ok, do some action
                if self.hint_stat == 1:
                    self.try_play_card()
                elif self.hint_stat == 2:
                    self.try_play_card_immediately()
                elif self.hint_stat == 3:
                    self.specify_next_color()
                else:
                    assert False
            else:
                self.time_to_think -= 1
        else:
            # not my turn
            self.last_hint_stat = -1
    
    def game_loop(self):
        while True:
            # abstract out single loop method for better test
            self.single_loop()

    def try_play_card(self):
        # press keyboard only once for each frame
        index_of_card_to_play = -1
        for i in range(len(self.handcards)):
            if util.can_be_played_after(self.handcards[i], self.last_played_card, len(self.handcards)):
                index_of_card_to_play = i
                break
        if self.debug:
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
    
    def try_play_card_immediately(self):
        card_to_play = self.handcards[self.cursor_index]
        if util.can_be_played_after(card_to_play, self.last_played_card, len(self.handcards)):
            self.press_keyboard('\n')
        else:
            self.press_keyboard(' ')
        
    def specify_next_color(self):
        first_handcard = self.handcards[0]
        if first_handcard in ['W', '+4']:
            self.press_keyboard('r')
        else:
            self.press_keyboard(first_handcard[0])

if __name__ == "__main__":
    endpoint = sys.argv[1]
    player_num = int(sys.argv[2])
    if len(sys.argv) > 3:
        username = sys.argv[3]
    else:
        username = "bot"
    bot = Bot(endpoint, player_num=player_num, username=username, debug=True)
    bot.game_loop()