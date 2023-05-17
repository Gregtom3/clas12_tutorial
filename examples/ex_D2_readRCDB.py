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


import pandas as pd
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

# Define the maximum number of warning messages
max_warnings = 5

# Initialize the warning counter
warning_count = 0

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
            warning_count += 1
            if warning_count <= max_warnings:
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

# Print the total number of warning messages
if warning_count > max_warnings:
    print("Total number of warning messages:", warning_count)

# Define the header
header = ['Run'] + conditions

# Create a list to store the rows
rows = []

# Append the header to the rows list
rows.append(header)

# Iterate over the table
for run in table:
    # Create a list for the current row
    row = [str(run)] + [str(table[run].get(condition, '')) for condition in conditions]
    # Append the row to the rows list
    rows.append(row)

# Create a DataFrame from the rows
df = pd.DataFrame(rows)

# Save the DataFrame as a CSV file
df.to_csv('RCDB_table.csv', index=False)
print("Created < RCDB_table.csv >")