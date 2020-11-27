from subprocess import Popen, PIPE
import os
import util

class Bot:
    def __init__(self, endpoint, username="bot", player_num=2):
        project_path = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        uno_path = project_path + "/build/src/uno"
        args = [uno_path, "-c", endpoint, "-u", username]
        
        self.game = Popen(args, stdin=PIPE, stdout=PIPE)
        self.player_num = player_num
        self.handcards = []
    
    def Input(self, action):
        action = bytes(action, "UTF-8")
        self.game.stdin.write(action)
        self.game.stdin.flush()
    
    def Output(self):
        line_num = util.get_ui_line_nums(self.player_num, len(self.handcards))
        # print(line_num)
        for row in range(line_num):
            line = self.game.stdout.readline().decode("UTF-8") \
                    .replace("\x1b[31m", "") \
                    .replace("\x1b[32m", "") \
                    .replace("\x1b[33m", "") \
                    .replace("\x1b[34m", "") \
                    .replace("\x1b[0m", "") \
                    .replace("\n", "")
            print(line)

bot = Bot("localhost:9091")
while True:
    bot.Input(' ')
    bot.Output()
    # print("*******")
