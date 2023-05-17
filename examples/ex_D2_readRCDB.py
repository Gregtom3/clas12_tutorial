import sys
sys.path.append("/work/clas12/users/gmat/packages/clas12root/rcdb/python/")
import rcdb
import os
import re

from rcdb.provider import RCDBProvider
from rcdb.model import ConditionType

db = RCDBProvider("mysql://rcdb@clasdb/rcdb")

# Check if the number of command-line arguments is correct
if len(sys.argv) != 4:
    print("Usage: python script.py runMin runMax <RCDB Condition>")
    sys.exit(1)

# Get the command-line arguments
run_min = int(sys.argv[1])
run_max = int(sys.argv[2])
condition = str(sys.argv[3])

# Initialize the list to store the output
output = []

# Iterate over the range of runs
for run in range(run_min, run_max + 1):
    # Get run from database
    db_run = db.get_run(run)
    
    # Get the condition value
    try:
        condition_value = db_run.get_condition(condition).value
    except:
        print("Run", run, "failed to get data from RCDB...skipping...")
        continue
    
    if condition == "target":
        condition_value = condition_value.strip()
        if condition_value == "12C":
            condition_value = "C"
    
    # Append to output
    output.append(str(run) + "," + str(condition_value))

# Print the output
for item in output:
    print(item)
