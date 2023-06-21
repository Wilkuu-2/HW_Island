# =======================================================*
# Arduino driven sqlite querying 
# Reads input from serial and prints query based on it
# uses comms.py as it's communication protocol
# 
# Copyright 2023, Group Tantor (CreaTe M8 2023)
# You may use the code under the terms of the MIT license
# =======================================================*/

import sqlite3
import comms

# Mappings 
Countries = [(0, "Asia"),(1,"Americas"),(2,"Europe"),(3,"Africa")]
Types = [(0,"Flood"),(1, "Drought"),(2, "Storm")]
Decades = [(0,1950),(1,1960),(2,1970),(3,1980),(4,1990),(5,2000),(6,2010)]
# TODO: TAG UUID's for continents 

# util function, might have to find a home somewhere else  
def int_or_n1(num):
    try: 
        return int(num)
    except: 
        return -1

# Data getter function
def get_data(ser):
    dec = None 
    dis = None 
    uuid = None 

    # Listen until found all of the inputs 
    while True:
        # Reads from the arduino 
        l,v = ser.wait_for_line_val()

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
    # Connect to database 
    con = sqlite3.connect("emdat_public5.db")
    cur = con.cursor()
    
    # Connect to serial 
    s = comms.Serial(auto_id="input")

    decade, disaster, uuid = get_data(s)
    
    # TODO: Replace with arduino input 
    print("input a number with the buttonsliderthing for the continent between 0 and 3")
    c = input()
    c = int(c)
    
    # Map the values 
    in_disaster = Types[disaster][1]
    in_year_start = Decades[decade][1]
    in_year_end = in_year_start + 10 
    in_continent = Countries[c][1] 

    # Debug
    print(f"{in_disaster}s from {in_year_start} to {in_year_end} in {in_continent}")

    res = cur.execute("""
                      SELECT d.Year, c.continent, dt.type, AVG(d.Deaths), AVG(d.Injured), AVG(d.TotalCost), w.Temperature 
                      FROM "Disaster" d
                            INNER JOIN "DisasterTypes" AS dt ON dt.disasterTypeId = d.dtype_id
                            INNER JOIN "Country" AS c ON d.ISO = c.ISO
                            FULL OUTER JOIN Warming AS w ON w.Continent = c.Continent AND w.Decade BETWEEN d.Year -1  AND d.Year + 11
                      WHERE d.Year >= ?
                           AND d.Year < ? 
                           AND c.Continent = ? 
                           AND dt.type = ? 
                      """,
                      (str(in_year_start),
                       str(in_year_end),
                       str(in_continent),
                       str(in_disaster)))

    # Get the results of the query 
    print(res.fetchall())
    s.close()
