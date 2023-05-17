################################################################
# Imports
################################################################

import os 
import sys
sys.path.append("/u/group/clas12/packages/clas12root/1.8.0/ccdb/python")
import pandas as pd
import pymysql 
pymysql.install_as_MySQLdb()
import ccdb
from ccdb import Directory, TypeTable, Assignment, ConstantSet

################################################################
# CCDB Provider
################################################################

ccdb_home = "/group/clas12/packages/clas12root/1.8.0/ccdb"
sqlite_connect_str = "mysql://clas12reader@clasdb.jlab.org/clas12"

# create CCDB api class
provider = ccdb.AlchemyProvider()                        # this class has all CCDB manipulation functions
provider.connect(sqlite_connect_str)                     # use usual connection string to connect to database
provider.authentication.current_user_name = "anonymous"  # to have a name in logs

################################################################
# Read from /runcontrol/fcup
################################################################
table = provider.get_type_table("/runcontrol/fcup") # Get FCup table

# Initialize empty lists
runMin_list = []
runMax_list = []
slope_list = []
offset_list = []
atten_list = []

# Iterate over constant sets
for constant_set in table.constant_sets:
    runMin = constant_set.assignment.run_range.min
    runMax = constant_set.assignment.run_range.max
    _, _, _, slope, offset, atten = constant_set.data_list

    # Append values to respective lists
    runMin_list.append(runMin)
    runMax_list.append(runMax)
    slope_list.append(slope)
    offset_list.append(offset)
    atten_list.append(atten)

################################################################
# Save the data to a csv
################################################################

# Create dataframe
df = pd.DataFrame({
    'runMin': runMin_list,
    'runMax': runMax_list,
    'slope': slope_list,
    'offset': offset_list,
    'atten': atten_list
})


# # Sort the dataframe by 'runMin' column
# df = df.sort_values('runMin')


# ################################################################
# # For some reason, runMax is set to MAX_INT in random parts
# # of the document. Only the last entry should have runMax = MAX_INT
# # To remedy this, we set those 'mis-behaving' runMax's to one less than
# # the next entry's runMin
# ################################################################
# # Loop over the 'runMax' column
# for i in range(len(df) - 1):
#     if df.loc[i, 'runMax'] >= 99999:
#         df.loc[i, 'runMax'] = df.loc[i + 1, 'runMin'] - 1


################################################################
# Sanity check the dataframe by making sure that each row's runMax is
# one less than the following entry's runMin
################################################################
# Loop over the 'runMax' column
for i in range(len(df) - 1):
    current_run_max = df.loc[i, 'runMax']
    next_run_min = df.loc[i + 1, 'runMin']

    if current_run_max != next_run_min - 1:
        print(f"Warning: Table jump between rows {i} and {i + 1}")
        print(f"runMax at row {i}: {current_run_max}")
        print(f"runMin at row {i + 1}: {next_run_min}")
        print(25*"---")

# Save dataframe as CSV
df.to_csv('runcontrol_data.csv', index=False)

################################################################
# Display the CSV
################################################################
print(df)
print("Created <runcontrol_data.csv>")