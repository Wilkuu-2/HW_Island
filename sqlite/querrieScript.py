import sqlite3

import comms

Countries = [(0, "Asia"),(1,"Americas"),(2,"Europe"),(3,"Africa")]
Types = [(0,"Flood"),(1, "Drought"),(2, "Storm")]
Decades = [(0,1950),(1,1960),(2,1970),(3,1980),(4,1990),(5,2000),(6,2010)]



def int_or_n1(num):
    try: 
        return int(num)
    except: 
        return -1

def get_data(ser):
    dec = None 
    dis = None 
    uuid = None 

    while True:
        l,v = ser.wait_for_line_val()
        if l == 'a': 
            dec = int_or_n1(v)
            print(f"{dec=}")
        elif l == 'b': 
            dis = int_or_n1(v)
            print(f"{dis=}")
        elif l == 'c':
            uuid = v
            print(f"{uuid=}")

        if dec is not None and dis is not None and uuid is not None: 
            return dec,dis, uuid

if __name__ == "__main__":
    con = sqlite3.connect("emdat_public5.db")
    cur = con.cursor()
    s = comms.Serial(auto_id="input")

    decade, disaster, uuid = get_data(s)
    if decade == -1  or disaster == -1:
        print("no_data")
        get_data(s)

    #print("input a decade(so 1900, tot 1980, 1990, 2000 or 2010")
    #x = input()
    #x = int(x)
    #y = int(x+10)
    print("input a number with the buttonsliderthing for the continent between 0 and 3")
    c = input()
    c = int(c)
    #print("input a number for the disaster type, (0 = flood, 1 = drought, 2 = storm")
    #t = input()
    #t = int(t)

    #decade = x
    #disaster = t
    in_disaster = Types[disaster][1]
    in_year_start = Decades[decade][1]
    in_year_end = in_year_start + 10 
    in_continent = Countries[c][1] 
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

          
    print(res.fetchall())
    s.close()
