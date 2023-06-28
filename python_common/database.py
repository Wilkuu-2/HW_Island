# =======================================================*
# Arduino driven sqlite querying 
# 
# Copyright 2023, Group Tantor (CreaTe M8 2023)
# You may use the code under the terms of the MIT license
# =======================================================*/



# con = sqlite3.connect("emdat_public5.db")
def query_data(con, in_disaster, in_decade, in_continent):
    cur = con.cursor()

    # Map the values 
    in_year_end = in_decade + 9 
    # Debug
    print(f"[QUERY] {in_disaster}s from {in_decade} to {in_year_end} in {in_continent}")

    res = cur.execute("""
                      SELECT d.Year, c.continent, dt.type, AVG(d.Deaths), AVG(d.Injured), AVG(d.TotalCost), w.Temperature 
                      FROM "Disaster" d
                            INNER JOIN "DisasterTypes" AS dt ON dt.disasterTypeId = d.dtype_id
                            INNER JOIN "Country" AS c ON d.ISO = c.ISO
                            INNER JOIN Warming AS w ON w.Continent = c.Continent AND w.Decade BETWEEN d.Year -1  AND d.Year + 11
                      WHERE d.Year >= ?
                           AND d.Year < ? 
                           AND c.Continent = ? 
                           AND dt.type = ? 
                      """,
                      (str(in_decade),
                       str(in_year_end),
                       str(in_continent),
                       str(in_disaster)))

    # Get the results of the query 
    data = res.fetchall()
    return data
    


