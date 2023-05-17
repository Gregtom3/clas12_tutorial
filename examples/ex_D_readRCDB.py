import sys
sys.path.append("/work/clas12/users/gmat/packages/clas12root/rcdb/python/")
import rcdb
# Import necessary modules
import os
import re

from rcdb.provider import RCDBProvider
from rcdb.model import ConditionType

db = RCDBProvider("mysql://rcdb@clasdb/rcdb")


#########################################################
input_hipos = sys.argv[1:-1]
condition   = str(sys.argv[-1])
runs = []
output = []


# Iterate over the file names
for hipo in input_hipos:
    # Extract the number after sidisdvcs
    runs.append(int(re.findall(r'sidisdvcs_([0-9]+)\.hipo', hipo)[0]))
    

# Iterate over the runs
for run in runs:
    # Get run from database
    DB = db.get_run(run)
    cnd_val = DB.get_condition(condition).value
    if(condition == "target"):
            cnd_val=cnd_val.strip()
            if(cnd_val=="12C"):
                cnd_val="C"
    # Append to output
    output.append(str(cnd_val))

# Return output
print(output)


