"""
Script: RCDB Data Extraction
Description: This script retrieves condition values from the RCDB (Run Conditions Database) for a given range of runs
             and specified conditions. It builds a table with the run numbers and their corresponding condition values,
             and prints the table in CSV format.

Usage: python ex_D2_readRCDB.py runMin runMax condition1 condition2 ...

Arguments:
    - runMin: Minimum run number (inclusive) in the range.
    - runMax: Maximum run number (inclusive) in the range.
    - condition1, condition2, ...: Names of the conditions to retrieve from the RCDB.

Example: python ex_D2_readRCDB.py 16400 16500 events_rate target half_wave_plate

Output: The script prints the table of run numbers and condition values in CSV format, with column headers.

"""



import sys
sys.path.append("/work/clas12/users/gmat/packages/clas12root/rcdb/python/")
import rcdb
import os
import re

from rcdb.provider import RCDBProvider
from rcdb.model import ConditionType

db = RCDBProvider("mysql://rcdb@clasdb/rcdb")

# Check if the number of command-line arguments is correct
if len(sys.argv) < 4:
    print("Usage: python script.py runMin runMax condition1 condition2 ...")
    sys.exit(1)

# Get the command-line arguments
run_min = int(sys.argv[1])
run_max = int(sys.argv[2])
conditions = sys.argv[3:]

# Initialize the table dictionary to store the output
table = {}

# Iterate over the range of runs
for run in range(run_min, run_max + 1):
    # Get run from database
    db_run = db.get_run(run)
    
    # Initialize the row dictionary for the current run
    row = {'Run': run}
    
    # Get the condition values
    for condition in conditions:
        try:
            condition_value = db_run.get_condition(condition).value
        except:
            print("Run", run, "failed to get data for condition", condition, "...skipping...")
            continue
        
        if condition == "target":
            condition_value = condition_value.strip()
            if condition_value == "12C":
                condition_value = "C"
        
        # Add condition value to the row dictionary
        row[condition] = condition_value
    
    # Add the row dictionary to the table dictionary
    table[run] = row

# Print the table header
header = ['Run'] + conditions
print(','.join(header))

# Print the table rows
for run in table:
    row = [str(table[run].get(condition, '')) for condition in header]
    print(','.join(row))
    
