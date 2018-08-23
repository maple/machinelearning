#! /usr/bin/env python
# -*- coding:utf-8 -*-

import json
import random
import datetime
from hashlib import md5

from websocket import create_connection

from mcbot import MonteCarloBot
from mcbot import PreflopBot

import signal
signal.signal(signal.SIGINT, signal.SIG_DFL)

from time import sleep

""" 
action decision based on self card strength calcalated from 10000 simulation
    > 0.9: raise
    > 0.8: bet
    > 0.5: check
    <= 0.5: fold
"""
ws = ""

player = ""
address = "URL"

player_md5 = ""
bot = MonteCarloBot()
preflop = PreflopBot()

# thresholds for standard
std_thresholds = {"check": 0.5, "allin": 0.95, "bet": 0.75, "raise": 0.9, "chipsguard": 0.55}

# thresholds for pre-flop
pf_thresholds = {"check": 0.43, "allin": 0.95, "bet": 0.75, "raise": 0.9, "chipsguard": 0.55}


# 1=enable, 0=disable
enable_auto_fold = 1 
auto_fold_chips = 15000

last_action = "fold"
timing_to_last_action = "pre-flop"
dynamic_std_thresholds = 0.0
dynamic_pf_thresholds = 0.0

def get_thresholds(action, data):

    th = std_thresholds

    if "board" in data["game"].keys() and len(data["game"]["board"]) > 0:
        # not pre-flop
        th = std_thresholds

    else:
        # pre-flop
        th = pf_thresholds

    return th


def takeAction(action, data):
    global caution_allin_called
    global last_action
    global timing_to_last_action
    if action == "__new_round":
        bot.reset()
        last_action = "None. because chip is none."  # initialize

    elif action == "__action" or action == "__bet":
        # get hand cards
        bot.assign_hand(format_cards(data["self"]["cards"]))

        if "board" in data["game"].keys() and len(data["game"]["board"]) > 0:
            # get board cards
            bot.assign_board(format_cards(data["game"]["board"]))
            # get win rate
            win_rate = bot.estimate_winrate()
            print "est. win rate: {}".format(win_rate)
        else:
            win_rate = preflop.estimate_winrate(format_cards(data["self"]["cards"]))
            print "est. pre-flop strength: {}".format(win_rate)

            # for adding win_rate in preflop stage
            if enable_auto_fold == 1 and data["self"]["chips"] >= auto_fold_chips and win_rate >= 0.8:
                print "raise win_rate calculation for catching a chance to get more chips"
                win_rate = win_rate + float("0.11")   # to add possibility of "allin"

        th = get_thresholds(action,data)
        resp, amount = decide_action(th, win_rate, data["self"]["chips"], data["self"]["minBet"])
        print "chips = ", data["self"]["chips"], "============== action = ", resp
        last_action = resp
        if th == pf_thresholds:
            timing_to_last_action = "pre-flop"
        else:
            timing_to_last_action = "after_flop"

        ws.send(json.dumps({"eventName":"__action", "data":{"action":resp,"amount":amount, "playerName":player}}))
    elif action == "__start_reload":
        ws.send(json.dumps({"eventName" : "__reload"}))

def OutputLog(action, data):
    global dynamic_std_thresholds
    global dynamic_pf_thresholds
    if action == "__new_round":
        print "--------------------------------"
    elif action == "__round_end":
        print "         ", action
        print "board=", data["table"]["board"]
        winner_highest_card_rank = 0.0
        lose_flag = 0
        my_card_rank = 0
        my_last_action = "call or raise"
        for gameplayer in data["players"]:
            if gameplayer["winMoney"] > 0:
                print gameplayer["playerName"], gameplayer["hand"]["message"], gameplayer["hand"]["rank"], "<<=== Winner"
                if winner_highest_card_rank < gameplayer["hand"]["rank"]:
                    winner_highest_card_rank = gameplayer["hand"]["rank"]
            else:
                if "hand" in gameplayer:                
                    print gameplayer["playerName"], gameplayer["hand"]["message"], gameplayer["hand"]["rank"]
                else:
                    print gameplayer["playerName"], "S/He is already died. no chip , no rank."

            if gameplayer["playerName"] == player_md5:
                if "cards" in gameplayer:
                    print "my card = ", gameplayer["cards"]
                    if "hand" in gameplayer:
                        my_card_rank = gameplayer["hand"]["rank"]
                        print "my_card_rank = ", my_card_rank
                        if "message" in gameplayer["hand"]:
                            print "My Hand Ranking=", gameplayer["hand"]["message"]
                        print "player current chips=", gameplayer["chips"]
                if gameplayer["winMoney"] > 0:
                    print "==== Round end : Win money!! ==== ", gameplayer["winMoney"], " chips"
                else:
                    print "==== Round end : Cheer up!"
                    lose_flag = 1

        if lose_flag > 0:
            print "Winner Highest rank === ", winner_highest_card_rank
            print "My card rank === ", my_card_rank, ", My last action is ", last_action
            if winner_highest_card_rank <= my_card_rank:
                print ">>>>>>>>>>>>>>>>  Check threshold down to avoid fold, then we'll win next time !!! "
                if timing_to_last_action == "pre-flop":
                    dynamic_pf_thresholds = dynamic_pf_thresholds - 0.03
                else:
                    dynamic_std_thresholds = dynamic_std_thresholds - 0.03
                print "dynamic_std_thresholds is updated std= ", dynamic_std_thresholds, "pf = ", dynamic_pf_thresholds
            else:
                if last_action != "fold" and last_action != "None. because chip is none.":
                    print ">>>>>>>>>>>>> Check threshold up to avoid lost-money........ Don't challenge too aggressive"
                    if timing_to_last_action == "pre-flop":
                        dynamic_pf_thresholds = dynamic_pf_thresholds + 0.03
                    else:
                        dynamic_std_thresholds = dynamic_std_thresholds + 0.03
                    print "dynamic_std_thresholds is updated std= ", dynamic_std_thresholds, "pf = ", dynamic_pf_thresholds

                
    elif action == "__game_over":
        max_chips = 0
        my_chips = 0
        for winner in data["winners"]:
            if winner["chips"] > max_chips:
                max_chips = winner["chips"]
            if winner["playerName"] == player_md5:
                my_chips = winner["chips"]
        if my_chips == max_chips:
            print ("==== Game over : YOU ARE THE WINNER!! ==== Final chips %d" % max_chips)
        else:
            print ("==== Game over : So close... ==== my chips %d vs max chips %d" % (my_chips, max_chips))
        
    else:
       print "current event is ", action

def doListen():
    global caution_allin_called
    try:
        global ws
        ws = create_connection(address)
        ws.send(json.dumps({"eventName":"__join", "data":{"playerName":player}}))
        while 1:
            result = ws.recv()
            msg = json.loads(result)
            event_name = msg["eventName"]
            data = msg["data"]
            OutputLog(event_name, data)
            # print data
            start_time = datetime.datetime.now()
            takeAction(event_name, data)
            end_time = datetime.datetime.now()
            if event_name == "__action" or event_name == "__bet":
                delta_time = end_time - start_time
                print "calc_time=", delta_time.total_seconds(), "sec"

    except Exception, e:
        print(e.args)
        ws.close()
        # doListen()


def decide_action(th, win_rate, chips, betamount):
    action = "fold"
    global caution_allin_called    
    print "chips=", chips

    if enable_auto_fold == 1 and chips >= auto_fold_chips and win_rate <= th["raise"]:
        print "auto fold"
    elif enable_auto_fold == 1 and chips >= auto_fold_chips and win_rate > th["raise"]:
        print "raise during auto fold mode."
        action = "raise"
        newamount = betamount * random.uniform(1.0, 2.0)
        if newamount <= chips*th["chipsguard"]:
            betamount = int(newamount)
        else:
            action = "call"
    elif win_rate > th["allin"]:
        action = "allin"
    elif win_rate > th["raise"]:
        action = "raise"
        newamount = betamount * random.uniform(1.0, 2.0)
        if newamount <= chips*th["chipsguard"]:
            betamount = int(newamount)
        else:
            action = "call"
    elif win_rate > th["bet"]:
        if betamount <= chips*th["chipsguard"]:
            action = "bet"
        else:
            action = "check"
    elif betamount > chips*th["chipsguard"] and chips >= 3000 :
        print "choose fold action due to not enough chips and not enough win rate...oh my god."
    if th == std_thresholds:
        if win_rate > th["check"] + dynamic_std_thresholds:
            action = "check"
    else:
        if win_rate > th["check"] + dynamic_pf_thresholds:
            action = "check"

    print "action=", action, " : betamount=", betamount

    return action, betamount


def format_cards(card_strs):
    ret = list()
    for s in card_strs:
        ret.append(last_lower(s))

    return ret


def last_lower(s):
    if len(s) == 0:
        return s
    else:
        return s[:-1] + s[-1].lower()


if __name__ == '__main__':
    m = md5()
    m.update(player)
    player_md5 = m.hexdigest()

    while 1:
        doListen()
        sleep(1)
        print("retry doListen()")
