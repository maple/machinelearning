import numpy as np
np.random.seed(1337)  # for reproducibility

from keras.datasets import mnist
from keras.models import Sequential
from keras.layers.core import Dense, Dropout, Activation
from keras.optimizers import SGD, Adam, RMSprop
from keras.utils import np_utils
import keras.backend as K
from itertools import product
from functools import partial
import tensorflow as tf
from keras.models import Model
import time
import sys 


flower = ['D', 'S', 'C', 'H']
num = ['A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K']
allcards = {}
n = 1 
for f in flower:
    for c in num:
        allcards[c+f] = n 
        n+=1

layer = 3

model = Sequential()
# node: 8, output 30 arrows from each node of 8 nodes then, next node is 30.
#model.add(Dense(30, input_dim = 8, init="uniform"))
model.add(Dense(60, input_dim = 8, init="uniform"))
model.add(Activation('relu'))
for i in range(layer-1):
    #model.add(Dense(30, init="uniform"))
    model.add(Dense(60, init="uniform"))
    model.add(Activation('relu'))
model.add(Dense(6, init="uniform"))
model.add(Activation('softmax'))
    
model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
#model.compile(loss=my_loss, optimizer='adam', metrics=['accuracy'])

data_x = np.loadtxt('sample_data', delimiter=',')
#print data_x
data_y = np.loadtxt('sample_ans', delimiter=',')
#model.fit(data_x, data_y, nb_epoch=200, batch_size=2000, verbose=0, shuffle=True)
model.fit(data_x, data_y, nb_epoch=600, batch_size=9000, verbose=0, shuffle=True)

model.save('model/Dlearner')

