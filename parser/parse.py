#!/usr/bin/env python3
import argparse
import csv
import sqlite3
import pathlib
import os

class DataRows:
    def __init__(self, top, rows):
        self.rows = rows
        self.top  = top


    def get(self, rindex, variable):
        cindex = self.top.index(variable) 
        return self.rows[rindex][cindex]

    def __len__(self):
        return len(self.rows)

def int_or_null(inp):
    try:
        return int(inp)
    except: 
        return None

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
                    prog='Data Parser',
                    description='Parses disaster data into a sqlite database file',
                    epilog='Part of Hybrid worlds project of 2023')

    parser.add_argument("in_file", nargs=1, help="The CSV file used as the source")
    parser.add_argument("out_file", nargs=1, help="The sqlite .db file result")

    args = vars(parser.parse_args())
    print(args)

    data = None 

    with open(args["in_file"][0], 'r') as f:
        input = list(csv.reader(f, delimiter=",", quotechar='"'))

        rlimit = 20000

        top_row = input[:1][0] 
        print(top_row)
        rows = input[1:]
        data = DataRows(top_row,rows)

    pathstr = f"./{args['out_file'][0]}"

    if(os.path.exists(pathstr)):
        os.remove(pathstr)

    pathlib.Path(pathstr).touch()

    db = sqlite3.connect(pathstr)
    
    c = db.cursor()
    c.execute("""CREATE TABLE "DisasterData_InitialFilter"(
    "Year"    INTEGER,
    "DisasterType"    TEXT,
    "DisasterSubtype"    TEXT,
    "Country"    TEXT,
    "ISO"    TEXT,
    "Region"    TEXT,
    "Continent"    TEXT,
    "TotalDeaths"    INTEGER,
    "NoInjured"        INTEGER,
    "NoAffected"    INTEGER,
    "NoHomeless"    INTEGER,
    "TotalAffected"    INTEGER,
    "ReconstructionCosts"    INTEGER,
    "InsuredDamages"    INTEGER,
    "TotalDamages"    INTEGER
    )""")

    for i in range(0,min(len(data),rlimit)):
        sqldata = (int(data.get(i, "Year")), 
                   data.get(i, "Disaster Type"),
                   data.get(i, "Disaster Subtype"),
                   data.get(i, "Country"),
                   data.get(i, "ISO"),
                   data.get(i, "Region"),
                   data.get(i, "Continent"),
                   int_or_null(data.get(i, "Total Deaths")),
                   int_or_null(data.get(i, "No Injured")),
                   int_or_null(data.get(i, "No Affected")),
                   int_or_null(data.get(i, "No Homeless")),
                   int_or_null(data.get(i, "Total Affected")),
                   int_or_null(data.get(i, "Reconstruction Costs ('000 US$)")),
                   int_or_null(data.get(i, "Insured Damages ('000 US$)")),
                   int_or_null(data.get(i, "Total Damages ('000 US$)")),)

        cl = db.cursor()
        c.execute("""
INSERT INTO "DisasterData_InitialFilter" (
    Year,
    DisasterType,
    DisasterSubtype,
    Country,
    ISO,
    Region,
    Continent,
    TotalDeaths,
    NoInjured,
    NoAffected,
    NoHomeless,
    TotalAffected,
    ReconstructionCosts,
    InsuredDamages,
    TotalDamages)
VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)
                  """, sqldata)

    c.executescript("""
BEGIN;

CREATE TABLE Country(Continent TEXT, Region TEXT, Country TEXT, ISO TEXT, PRIMARY KEY(ISO));

CREATE TABLE "DisasterTypes" (
    "disasterTypeId"	INTEGER NOT NULL UNIQUE,
    "type"	TEXT,
    "subtype"	TEXT,
    PRIMARY KEY("disasterTypeId" AUTOINCREMENT)
);

CREATE TABLE Disaster (
    id    INTEGER NOT NULL UNIQUE,
    Year    INTEGER NOT NULL,
    dtype_id    INTEGER,
    ISO    TEXT,
    Deaths    INTEGER,
    Injured    INTEGER,
    TotalCost    INTEGER,
    PRIMARY KEY(id AUTOINCREMENT),
    FOREIGN KEY(ISO) REFERENCES Country(ISO),
    FOREIGN KEY(dtype_id) REFERENCES DisasterTypes(disasterTypeId)
);


INSERT INTO Country SELECT DISTINCT Continent, Region, Country, ISO
FROM DisasterData_InitialFilter;


INSERT INTO DisasterTypes (type, subtype) SELECT DISTINCT DisasterType, DisasterSubtype 
FROM DisasterData_InitialFilter;

INSERT INTO Disaster (dtype_id, Year,ISO, Deaths, Injured, TotalCost)
SELECT dt.disasterTypeId, di.Year, di.ISO, di.TotalDeaths, di.NoInjured, di.TotalDamages FROM  DisasterTypes dt, DisasterData_InitialFilter di
WHERE dt.type = di.DisasterType AND dt.subtype = di.DisasterSubtype;

COMMIT;
""")

    db.commit()
