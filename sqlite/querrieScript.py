import sqlite3

import serial
import serial.tools.list_ports
import sys
import readline

Countries = [(0, "Asia"),(1,"Americas"),(2,"Europe"),(3,"Africa")]
Types = [(0,"Flood"),(1, "Drought"),(2, "Storm")]
Decades = [(0,1950),(1,1960),(2,1970),(3,1980),(4,1990),(5,2000),(6,2010)]



def input_with_prefill(prompt, text):
    def hook():
        readline.insert_text(text)
        readline.redisplay()
    readline.set_pre_input_hook(hook)
    result = input(prompt)
    readline.set_pre_input_hook()
    return result

def int_or_n1(num):
    try: 
        return int(num)
    except: 
        return -1

class Serial:

    def __init__(self):
        # set up connection
        if sys.platform == 'windows' or sys.platform == 'cygwin':
            self.serialInst = self.connectionWin()
        else: 
            self.serialInst = self.connectionLinux()

    def connectionWin(self):
        ports = serial.tools.list_ports.comports()
        print("== Detected ports: ==")
        for port in ports: 
            print("-->\t", str(port.device))

        comport = input_with_prefill("== Select Com port: ==\n> ", str(ports[0].device))

        for x in range(0, len(portList)):
            if portList[x].startswith('COM' + str(val)):
                portVar = 'COM' + str(val)

        ser = serial.serial()
        ser.baudrate = 9600  # same as arduino
        ser.timeout = 1
        ser.port = comport
        ser.open()
        return ser

    def connectionLinux(self):
        ports = serial.tools.list_ports.comports()
        print("== Detected ports: ==")
        for port in ports: 
            print("-->\t", str(port.device))
        
        path = input_with_prefill("== Give device path: ==\n> ", str(ports[0].device))
        return serial.Serial(path,9600)

    def get_data(self):
        dec = None 
        dis = None 

        while True:
            l,v = self.wait_for_line_val()
            if l == 'a': 
                dec = int_or_n1(v)
                print(f"{dec=}")
            elif l == 'b': 
                dis = int_or_n1(v)
                print(f"{dis=}")

            if dec is not None and dis is not None: 
                return dec,dis 
                
    def wait_for_line_val(self):
        line = []
        while True:
             if len(line) and ('\n' in line[-1] or '\r' in line[-1]):
                 break
             
             line.append(str(self.serialInst.read(1), 'utf-8'))

        try:
            val = ''.join(line[1:])
            label = line[0]
        except:
            return 'z', -1
        
        print(f"READ: {label=} , {val=}")
        return label,val 
    
    def close(self):
        self.serialInst.close()

if __name__ == "__main__":
    con = sqlite3.connect("emdat_public3.db")
    cur = con.cursor()
    s = Serial()

    decade, disaster = s.get_data()
    if decade == -1  or disaster == -1:
        print("no_data")
        s.get_data()

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
                      SELECT d.Year, c.continent, dt.type, AVG(d.Deaths), AVG(d.Injured), AVG(d.TotalCost) 
                      FROM Disaster d, Country c, DisasterTypes dt 
                      WHERE d.ISO = c.ISO 
                           AND dt.disasterTypeId = d.dtype_id 
                           AND d.Year >= ?
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
