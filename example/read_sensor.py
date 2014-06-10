#!/usr/bin/python

import RPi.GPIO as GPIO
import OSC
import time
GPIO.setmode(GPIO.BCM)

buttonPin = 9

GPIO.setup(buttonPin,GPIO.IN)
GPIO.setup(buttonPin,GPIO.IN)

# setup OSC
client = OSC.OSCClient()
client.connect( ('127.0.0.1', 9002) )
msgPlay = OSC.OSCMessage()
msgPlay.setAddress("/play")
msgPlay.append(1)
msgStop = OSC.OSCMessage()
msgStop.setAddress("/play")
msgStop.append(0)

prev_input = 0
print("start polling")
while True:
  #take a reading
  input = GPIO.input(buttonPin)
  #if the last reading was low and this one high, print
  if (prev_input != input):
    print(input)
    #update previous input
    prev_input = input
    if (input==0):
      try:
        client.send(msgPlay)
      except:
        print("can't connect to OSC")
      else:
        print("play")
      time.sleep(0.5) # 1s debounce
    else:
      client.send(msgStop)
      time.sleep(0.5) # 5s debounce
