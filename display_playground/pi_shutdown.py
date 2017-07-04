#!/usr/bin/env python   
    
import RPi.GPIO as GPIO  
import os  
from time import sleep  
  
def initiate_shutdown(channel):  
  os.system("shutdown now -h") 
  
BUTTON_PIN = 18  
  
GPIO.setmode(GPIO.BCM) # Sets GPIO pins to BCM numbering  
GPIO.setup(BUTTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP) # Sets pin to input  
GPIO.add_event_detect(BUTTON_PIN, GPIO.FALLING, callback=initiate_shutdown, bouncetime=500) # Sets edge detection on pin  
  
# infinite loop  
while True:  
  sleep(60)  
