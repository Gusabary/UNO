import conf

def get_ui_line_nums(player_num, handcards_num):
    base_line_num = conf.base_scale_of_view[player_num][0]
    # -1 // 8 == 0 in python
    extra_line_num = max((handcards_num - 1) // 8, 0)
    return base_line_num + extra_line_num