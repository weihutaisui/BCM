# 
# Format of files in merge-igd.txt:
# All blank lines and lines beginning with # are ignored.
# All other lines must contain one of the commands described below.
# Each file can contain one or more commands, although for the sake
# of modularity, usually we only put one command per file.


# The 6 commands supported are:
#
# base <data-model-file>
# addObjBelowObj <data-model-file> <target-object>
# addObjBelowObj <data-model-file> <target-object>
# addFirstChildObjToObj <data-model-file> <target-object>
# addLastChildObjToObj <data-model-file> <target-object>
# deleteObj <target-object>


# The name of each file must begin with 4 digits, and end with .txt.
# The number ranges are defined as follows (note this applies to merge-igd.d only):
# 1000      : must be the first file and contain the "base" command.  This is
#             the only file that can contain the base command.  This file
#             creates the complete TR98 data model.
# 1001-1999 : Broadcom defined objects
# 2000-2999 : Other Broadband Forum data models
# 3000-3999 : PON data models
# 4000-4999 : Hybrid TR181 data model objects
# 5000-8000 : Reserved
# 8000-9999 : For customer use

