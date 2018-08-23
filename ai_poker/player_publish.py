#! /usr/bin/env python
# -*- coding:utf-8 -*-

import sys
import time
import json
from websocket import create_connection

import event as EVNETNAME
import action as ACTION
# pip install websocket-client
import hashlib
#from pypokerengine.api.game import start_poker, setup_config

from keras.models import load_model
import numpy as np

import signal
signal.signal(signal.SIGINT, signal.SIG_DFL)
from time import sleep

player = ""
address = "you need to put URL"

#ws = create_connection(address)
ws = ""
RETRY = 3

m = hashlib.md5()
m.update(playerName)

cards = [];
chips = 0;
board = [];
isBlind = 0;


Dlearner = load_model('model/Dlearner')
flower = ['D', 'S', 'C', 'H']
num = ['A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K']
allcards = {}
n = 1
for f in flower:
    for c in num:
        allcards[c+f] = n
        n+=1

def predict(hand, board):
    input_card = []
    for h in hand:
        input_card.append(allcards[h])
    for b in board:
        input_card.append(allcards[b])
    zero = 7 - len(input_card)
    for z in range(zero):
        input_card.append(0)
    print input_card
    input_card.append(5)
    inp = np.array(input_card).reshape(1,8)
    result = Dlearner.predict(inp)
    return np.argmax(result)
    
# ws = ""

def __send_event(event_name, data={}):
    retry = 0
    payload = {"eventName": event_name}
    if data:
        payload["data"] = data

    while 1:
        try:
            ws.send(json.dumps(payload))
            return True
        except:
            if retry < RETRY:
                continue
            else:
                return False
        retry += 1

def search(players):
    for p in players:
        if p['playerName'] == m.hexdigest():
            return p

def transform(cards):
    n_c = []
    for c in cards:
        n_c.append(c[::-1])
    return n_c
        
def takeAction(action, data):
    if action == EVNETNAME.NEW_PEER:
        print len(data)
        print "Game Stat: {}".format(action)
        pass
    elif action == EVNETNAME.NEW_ROUNT:
        print "Game Stat: {}".format(action)
        m.hexdigest();
        p = search(data["players"]);
        global cards
        global chips
        cards = p["cards"];
        chips = p["chips"];
        pass  
    elif action == EVNETNAME.DEAL:
        print "Game Stat: {}".format(action)
        t = data["table"];
        global board
        global isBlind
        board = t["board"];
        sb = t["smallBlind"];
        if sb["playerName"] == m.hexdigest():
            isBlind = 1
        bb = t["bigBlind"];
        if bb["playerName"] == m.hexdigest():
            isBlind = 1
               
        pass
    elif action == EVNETNAME.ACTION:
        global cards
        global board
        # botttttttt
        action = predict(cards, board)
        amount = data['self']['minBet']
        print action
        if action == 0:
            a = { "action": 'fold', "amount": amount}
        elif action == 1:
            a = { "action": 'check', "amount": amount}
        elif action == 2:
            a = { "action": 'bet', "amount": amount}
        elif action == 3:
            a = { "action": 'raise', "amount": amount}
        elif action == 4:
            a = { "action": 'call', "amount": amount}
        else:
            a = { "action": 'all in', "amount": amount}
        return __send_event(EVNETNAME.ACTION, a)

    elif action == EVNETNAME.BET:
        global cards
        global board
        action = predict(cards, board)
        amount = data['self']['minBet']
        print action
        if action == 0:
            a = { "action": 'fold', "amount": amount}
        elif action == 1:
            a = { "action": 'check', "amount": amount}
        elif action == 2:
            a = { "action": 'bet', "amount": amount}
        elif action == 3:
            a = { "action": 'raise', "amount": amount}
        elif action == 4:
            a = { "action": 'call', "amount": amount}
        else:
            a = { "action": 'all in', "amount": amount}
        return __send_event(EVNETNAME.ACTION, a)

    elif action == EVNETNAME.START_RELOAD:
        if data["self"]["chips"] == 0:
        #if True:
            reload()
    elif action == EVNETNAME.SHOW_ACTION:
        # record behavior
        pass
    elif action == EVNETNAME.ROUND_END:
        print "Game Stat: {}".format(action)
        pass 
    elif action == EVNETNAME.GAME_OVER:
        print "Game Stat: {}".format(action)
        #sys.exit(1)
    else:
        print "Not Handle state : {}".format(action)
        pass

def join():
    return __send_event(EVNETNAME.JOIN, {"playerName": playerName})

def reload():
    return __send_event(EVNETNAME.RELOAD)

def doListen():    
    try:
        # if join():
        #     print "connected!!"
        # else:
        #     print "connecton fail!!"
        #     #sys.exit(1)

        global ws
        ws = create_connection(address)
        print "creation"
        ws.send(json.dumps({"eventName":"__join", "data":{"playerName":player}}))
        while 1:
            result = ws.recv()
            msg = json.loads(result)
            event_name = msg["eventName"]
            data = msg["data"]
#            print event_name
#            print data
            takeAction(event_name, data)

    except Exception as e:
        print(e.args)
        ws.close()
        # doListen()
        #print e


if __name__ == '__main__':
    print "start-------------------"
    while 1:
        doListen()
        sleep(1)
        print("retry doListen()")

