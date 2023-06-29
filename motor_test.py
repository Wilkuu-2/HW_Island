#!/usr/bin/env python3 
from python_common import motor
import argparse
from time import sleep


if __name__ == "__main__": 
    parser = argparse.ArgumentParser( prog = "Motor tester",
                                     description="Tests the big lift motor",
                                     epilog='Part of Hybrid worlds project of 2023'
                                    )
    parser.add_argument("serial_port", help="The serial interface of the motor")
    parser.add_argument("baudrate", help="The serial baudrate of the motor interface", default=9600)
    parser.add_argument("-c","--count", help="Amount of test loops", default=-99)

    args = vars(parser.parse_args())
    print("Arguments:\n", args)
    
    try:
        count = int(args["count"])
    except Exception as e:
        print("Could not parse loop count:\n", e)
        exit(1)

    try:
        m = motor.Motor.from_path(args["serial_port"], int(args["baudrate"]))
    except Exception as e:
        print("Unable to open motor interface:\n", e)
        exit(1)

    iterc = 0

    while count < 0 or iterc < count:
        m.sendCMD("ROR 0, 50000")
        sleep(1)

        m.sendCMD("MST 0")
        sleep(1)

        m.sendCMD("ROL 0, 50000")
        sleep(1)

        iterc += 1
