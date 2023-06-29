#!/bin/env python3
from python_common import comms
from python_common import database
import argparse
import time 
import sqlite3
from playsound import playsound

SOUNDS_PATH="../sounds/"

def play_disaster_news(disaster,decade,continent,block=True):
    if continent == "Americas":
        continent = "NorthAmerica"
    playsound(f"{SOUNDS_PATH}{decade}s/{continent}{disaster}s{decade}.wav",block=block)

def play_disaster_sounds(disaster,block=True):
    playsound(f"{SOUNDS_PATH}/disaster sfx/{disaster.lower()}_final.wav",block=block)
    

# util function, might have to find a home somewhere else  
def int_or_n1(num):
    try: 
        return int(num)
    except: 
        return -1

def int_or_0(num):
    try: 
        return int(num)
    except: 
        return 0

# Mappings 
Countries = {'0': "Asia",
             '1': "Americas",
             '2': "Europe",
             '3': "Africa"}

Types = {0: "Drought",
		 1: "Storm",
		 2: "Flood"}

Decades = {0 : 1980,
		   1 : 1990,
		   2 : 2000,
		   3 : 2010}

def map_safe(mdict,val,default=None): 
    try: 
        return mdict[val] 
    except Exception as e: 
        print(f"[Map] Mapping failed: {e}")
        return default

def mapint(i_min, i_max,value,o_min=0,o_max=1023):
    # map function for arduinos
    return int(ceil(((value - i_min) / i_max) * (o_max - o_min) + o_min ))

# Data getter function
def get_data(ser):
    dec = None 
    dis = None 
    uuid = None 

    # Listen until found all of the inputs 
    while True:
        # Reads from the arduino 
        l,v = ser.wait_for_message()

        # Decade 
        if l == 'a': 
            dec = int_or_n1(v)
            print(f"{dec=}")
        # Disaster type 
        elif l == 'b': 
            dis = int_or_n1(v)
            print(f"{dis=}")

        # Continent UUID  
        elif l == 'c':
            uuid = v
            print(f"{uuid=}")

        if dec is not None and dis is not None and uuid is not None: 
            return dec,dis, uuid

def list_connected_outputs(outputs):
    print("============== OUTPUTS ================")
    for id,ser in outputs.items(): 
        print(f"\t--> {id} => {ser}")

    print("=======================================")

def set_stats(conns,deaths,injuries,damages):
        if "deaths" in connections: 
            connections["deaths"].send_message('a',
                                               str(mapint(0,3_000_000,int_or_0(deaths))))  

        if "injuries" in connections: 
            connections["injuries"].send_message('a',
                                             str(mapint(0,3_000_000,int_or_0(injuries))))

        if "bank" in connections: 
            connections["bank"].send_message('a',
                                         str(mapint(0,10_000_000,int_or_0(damages))))


# All the inputs 
#OUTPUT_IDS = ["bank","deaths","injuries","lcd","fan","_MOTOR","droughts"]


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
                    prog='Main Control Script',
                    description='Controls the installation',
                    epilog='Part of Hybrid worlds project of 2023')

    parser.add_argument("database", help="Database used in the installation")
    parser.add_argument('-s', '--self_activate' , 
            help="Sends an message to the input panel to emulate pressing the start button",
            action='store_true')

    args = vars(parser.parse_args())
    print(f"[RPI] Starting with args: {args}")
    
    con = sqlite3.connect(args["database"])

    #outputs = comms.Serial.scan_all()
    #list_connected_outputs(outputs)
    #connections = comms.Serial.connect_to_all(outputs)
    connections = {}

    #if "input" not in connections:
    #    print("[ERROR] Input panel not found, exiting")
    #    exit(1)

    while True: # Main loop 
        if args["self_activate"]:
            time.sleep(5)
            connections["input"].send_message('X',999)
            time.sleep(0.2)
        
        #decade, disaster, uuid = get_data(connections["input"])
        decade = int(0) 
        disaster = int(0) 
        uuid = str('0')

        time.sleep(1)
        in_disaster = map_safe(Types, disaster, default = "Flood")
        in_decade = map_safe(Decades, decade, default = 2010)
        in_continent = map_safe(Countries, uuid, default = "Asia")
        
        data = database.query_data(con, in_disaster, in_decade, in_continent)[0]
        print(f"[RPI]: {data=}")


        print("Pre")
        wait_time = 5 
        #playsound(SOUNDS_PATH+'disaster sfx/alarm_final.wav', block=True) 
        if in_disaster == "Flood" and "_MOTOR" in connections:
            position = 100000
            turn_time = 1.5
            print("[Motor] START")
            connections["_MOTOR"].sendCMD("MST 0")
            connections["_MOTOR"].sendCMD(f"ROR 0,{position}")
            time.sleep(turn_time)
            connections["_MOTOR"].sendCMD("MST 0")

            print("[Motor] STOP")
            set_stats(connections,data[3],data[4],data[5])
            set_stats(connections,0,0,0)
            play_disaster_sounds(in_disaster)

            print("[Motor] START")
            connections["_MOTOR"].sendCMD(f"ROL 0,{position}")
            time.sleep(turn_time)
            connections["_MOTOR"].sendCMD("MST 0"),
            print("[Motor] STOP")
            play_disaster_news(in_disaster,in_decade,in_continent)


        elif True or (in_disaster == "Drought" and "drought" in connections):
            print("Start")
            #connections["drought"].send_message('a',str(999)) 
            set_stats(connections,data[3],data[4],data[5])
            print("Mid")
            play_disaster_sounds(in_disaster)
            set_stats(connections,0,0,0)
            #connections["drought"].send_message('a',str(0)) 
            play_disaster_news(in_disaster,in_decade,in_continent)
            print("Finished")
        

        elif in_disaster == "Storm" and "fan" in connections:
            connections["fan"].send_message('a',str(999)) 
            set_stats(connections,data[3],data[4],data[5])
            play_disaster_sounds(in_disaster)
            set_stats(connections,0,0,0)
            connections["fan"].send_message('a',str(0)) 
            play_disaster_news(in_disaster,in_decade,in_continent)
        
