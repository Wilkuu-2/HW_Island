
from python_common import comms
from python_common import database
import argparse
import time 
import sqlite3

# util function, might have to find a home somewhere else  
def int_or_n1(num):
    try: 
        return int(num)
    except: 
        return -1

# Mappings 
Countries = {'0': "Asia",
             '1': "Americas",
             '2': "Europe",
             '3': "Africa"}

Types = {0: "Flood",
		 1: "Drought",
		 2: "Storm"}

Decades = {0 : 1950,
		   1 : 1960,
		   2 : 1970,
		   3 : 1980,
		   4 : 1990,
		   5 : 2000,
		   6 : 2010}

def map_safe(mdict,val,default=None): 
    try: 
        return mdict[val] 
    except Exception as e: 
        print(f"[Map] Mapping failed: {e}")
        return default

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



if __name__ == "__main__":
    parser = argparse.ArgumentParser(
                    prog='Main Control Script',
                    description='Controls the installation',
                    epilog='Part of Hybrid worlds project of 2023')

    parser.add_argument("database", help="Database used in the installation")
    args = vars(parser.parse_args())
    print(f"[RPI] Starting with args: {args}")
    
    con = sqlite3.connect(args["database"])

    input_ser = comms.Serial(auto_id="input")

    while True: # Main loop 
        input_ser.send_message('X',9999999)
        time.sleep(0.2)
        decade, disaster, uuid = get_data(input_ser)
        in_disaster = map_safe(Types, disaster)
        in_decade = map_safe(Decades, decade)
        in_continent = map_safe(Countries, uuid, default = "Asia")
        
        data = database.query_data(con, in_disaster, in_decade, in_continent)
        print(f"[RPI]: {data=}")
        time.sleep(5)

    input_ser.close()
        
