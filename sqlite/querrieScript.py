import sqlite3

import serial
import serial.tools.list_ports

Countries = [(0, "Asia"),(1,"Americas"),(2,"Europe"),(3,"Africa")]
Types = [(0,"Flood"),(1, "Drought"),(2, "Storm")]
Decades = [(0,1950),(1,1960),(2,1970),(3,1980),(4,1990),(5,2000),(6,2010)]



class Serial:

    def __init__():
        # set up connection
        ports = serial.tools.list_ports.comports()
        serialInst = serial.Serial()
        portList = []
        portVar = ''
        data = []
        for onePort in ports:
            portList.append(str(onePort))
        print(portList)

        val = input("select portnumber: ")
        for x in range(0, len(portList)):
            if portList[x].startswith('COM' + str(val)):
                portVar = 'COM' + str(val)

        serialInst.baudrate = 9600  # same as arduino
        serialInst.timeout = 1
        serialInst.port = portVar
        serialInst.open()

    def get_data():
        if serialInst.in_waiting:
            value = serialInst.readline()
            value = value.decode()
            #decade
            if value[0] == "a":
                value1 = value.translate({ord('a'): None})
                value1 = int(value1)
            else:
                value1 = -1
        if serialInst.in_waiting:
            value = serialInst.readline()
            value = value.decode()
            #disaster
            if value[0] == "b":
                value2 = value.translate({ord('b'): None})
                value2 = int(value2)
            else:
                value2 = -1
        else:
            return -1, -1

        return value1, value2

con = sqlite3.connect("emdat_public3.db")
cur = con.cursor()
Serial.__init__()
decade, disaster = Serial.get_data()
if decade or disaster == -1:
    print("no_data")
    Serial.get_data()
#print("input a decade(so 1900, tot 1980, 1990, 2000 or 2010")
#x = input()
#x = int(x)
y = int(Decade+10)
print("input a number with the buttonsliderthing for the continent between 0 and 3")
c = input()
c = int(c)
#print("input a number for the disaster type, (0 = flood, 1 = drought, 2 = storm")
#t = input()
#t = int(t)
res = cur.execute("SELECT d.Year, c.continent, dt.type, AVG(d.Deaths), AVG(d.Injured), AVG(d.TotalCost) FROM Disaster d, Country c, DisasterTypes dt WHERE d.ISO = c.ISO AND dt.disasterTypeId = d.dtype_id AND d.Year >= ? AND d.Year < ? AND c.Continent = ? AND dt.type = ? ORDER BY d.Year, c.country, dt.type" ,(str(Decades[decade][1]),str(y),str(Countries[c][1]), str(Types[disaster][1])))
print(res.fetchall())

