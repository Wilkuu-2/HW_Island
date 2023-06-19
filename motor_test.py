#!/usr/bin/env python3 
import motor
from time import sleep

m = motor.Motor("/dev/ttyACM0", 9600)

m.sendCMD("ROR 0, 10000")
sleep(5)

m.sendCMD("MST 0")
