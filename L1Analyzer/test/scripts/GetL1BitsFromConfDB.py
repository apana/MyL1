#!/usr/bin/env python

# GetL1BitsForSkim.py
#
# Simple script for getting all L1 seeds used by a particular HLT menu

import os, string, sys, posix, tokenize, array, getopt
from pkgutil import extend_path
import FWCore.ParameterSet.Config as cms

def main(argv):

    # Keep track to avoid double-counting bits used in >1 path
    usedl1bits = []

    # Get commandline arguments
    # Name of configuration and option to use orcoff or HLTDEV
    opts, args = getopt.getopt(sys.argv[1:], "c:oh", ["configName=","orcoff","help"])

    doorcoff = " "
    configname = ""
    for o, a in opts:
        if o in ("-c","configName"):
            configname = str(a)
            # Try to be helpful and guess when orcoff should be used automatically
            if(configname.startswith('/cdaq')):
               doorcoff = " --orcoff"
        if o in ("-o","orcoff"):
            doorcoff = " --orcoff"
        if o in ("-h","help"):
            print "Options:"
            print "\t-c <name> (Select the name of the HLT key to use)"
            print "\t-o (Retrieve menu from ORCOFF. By default keys starting with /cdaq use ORCOFF, others use HLTDEV)"
            print "\t-h (Print the help menu)"
            return
            
    configcommand = "edmConfigFromDB" + doorcoff + " --configName " + configname + " --cff >& temphltmenu.py"
    # Use edmConfigFromDB to get a temporary HLT configuratio
    os.system(configcommand)

    # Setup a fake process and load the HLT configuration
    importcommand = "import temphltmenu"
    process = cms.Process("MyProcess")
    exec importcommand
    theextend = "process.extend(temphltmenu)"
    exec theextend

    # Find all EDFilters and iterate through them looking for L1 seeding modules 
    myfilters = process.filters_()

    for name, value in myfilters.iteritems():
        myfilter = value.type_()

        if(myfilter == "HLTLevel1GTSeed"):
            # Get parameters of the L1 seeding module and look for the L1
            params = value.parameters_()

            for paramname, paramval in params.iteritems():

                if(paramname == "L1SeedsLogicalExpression"):
                    seeds = paramval.value()
                    # Now get the value of the logical expression, and strip off any OR/AND/NOT/etc. 
                    # Insert each L1 seed in the tuple, after checking that it hasn't already been used 
                    tmpindividualseeds = seeds.split(' OR ')

                    for tmpindividualseed in tmpindividualseeds:

                        if(tmpindividualseed.find(' AND ') == -1):
                            theseed = str(tmpindividualseed).strip('NOT ').strip('(').strip(')')

                            if (not theseed in usedl1bits):
                                usedl1bits.append(theseed)
                        else:
                            individualseeds = tmpindividualseed.split(' AND ')

                            for individualseed in individualseeds:
                                theseed = str(individualseed).strip('NOT ').strip('(').strip(')')

                                if (not theseed in usedl1bits):
                                    usedl1bits.append(theseed)

    # Print the results
    for usedl1bit in usedl1bits:
        print usedl1bit

    # Cleanup the temporary HLT configuration
    os.system('rm temphltmenu.py temphltmenu.pyc')

if __name__ == "__main__":
    main(sys.argv[1:])
