#!/usr/bin/python
#
import sys,string,math,os
from optparse import OptionParser

def OpenFile(file_in,iodir):
    """  file_in -- Input file name
         iodir   -- 'r' readonly  'r+' read+write """
    try:
        ifile=open(file_in, iodir)
        # print "Opened file: ",file_in," iodir ",iodir
    except:
        print "Could not open file: ",file_in
        sys.exit(1)
    return ifile

def CloseFile(ifile):
    ifile.close()


def getL1AlgoBits(file):

    StringToFind="L1_"
    
    infile=OpenFile(file,'r')
    iline=0

    x = infile.readline()

    totcount=0

    seeds=[]
    while x != "":
        iline+=1
        xx=string.rstrip(x)
        xx=xx.translate(None,"()")
        if xx[0] != "#":
            inputLine=string.split(xx)
            for iblock in inputLine:
                # print iblock
                iblock=string.strip(iblock)
                if (string.find(iblock,"HLT_") >-1 ): continue
                index=string.find(iblock,StringToFind)
                if (index>-1):
                    seed=iblock[index:]
                    seeds.append(seed)
                    #print seed
                            
        x = infile.readline()
    
    # remove duplicate seeds from list
    d = {}
    for x in seeds:
        d[x] = x
    newseeds = d.values()
    print "## Number of used L1 Algorithm bits in HLT menu: ", len(newseeds)
    return newseeds


def getL1TechBits(file):

    StringToFind="L1_"
    # StringToFind="technical bits:"
    
    infile=OpenFile(file,'r')
    iline=0

    x = infile.readline()

    totcount=0

    seeds=[]
    while x != "":
        iline+=1
        xx=string.rstrip(x)
        # xx=xx.translate(None,"()")  # remove parenthesis
        # xx=xx.replace("AND","") # remove ANDs and NOTs
        # xx=xx.replace("NOT","")
        if xx[0] != "#":
            index=string.find(xx,StringToFind)
            if (index==-1):
                # skip if NOT l1tech
                if string.find(xx,"NOT")>-1:
                    x = infile.readline()
                    continue

                substring=string.strip(xx)
                org=substring
                # print "org= ",org
                substring=substring.translate(None,"()")  # remove parenthesis
                substring=substring.replace("AND","") # remove ANDs and NOTs
                substring=substring.replace("NOT","")                
                # print "TT string: ", substring
                # remove any l1 algo bits from string if present
                index=string.find(substring,"L1_")
                if (index>-1):
                    print "There could have be problems parsing this line: "
                    print " ", org
                    print "\tPlease cross-check that any technical bits on this line are included in the final list"
                    print ""
                    #i2=string.find(substring," ")
                    rmstring=substring[index:]
                    #if (i2>-1):rmstring=substring[index:i2]
                    #print "\t\t ", rmstring
                    substring=substring.replace(rmstring,"")
                    #print "\tYYY: ", substring
                inputLine=string.split(substring,"OR")
                for iblock in inputLine:
                    try:
                        ttbit=int(iblock)
                        seeds.append(string.strip(iblock))
                        # print ttbit
                    except ValueError:
                        print "Trouble parsing: ", inputLine
                            
        x = infile.readline()
    
    # remove duplicate seeds from list
    d = {}
    for x in seeds:
        d[x] = x
    newseeds = d.values()
    print "Number of used technical trigger bits: ", len(newseeds)
    return newseeds

def writeORofBits(listofSeeds,modulename,file):

    infile=OpenFile(file,'w')

    initstring="process." + modulename + ".L1SeedsLogicalExpression = \""
    finalstring="\""
    
    iostring=initstring
    for seed in listofSeeds:
        if (seed==listofSeeds[0]):
            iostring=iostring+ seed
        else:
            iostring=iostring+ " OR " + seed
            
    iostring=iostring+finalstring
    infile.write(iostring + "\n")
    sys.stdout.write(iostring + "\n")
    #print iostring
        
    #CloseFile(infile)

def usage():
        """
    L1Bits.txt should be a one-column list of L1 bits.
    Lines beginning with a "#" are treated as comment lines.
    Output can be optionally passed to file
    Type -h to get full options
               """
        pass
        
if __name__ == '__main__':


    verbose=False
    usage = "usage: %prog [options] L1Bits.txt [outfile]" + "\n" + usage.__doc__ 
    parser = OptionParser(usage=usage)
    parser.add_option("-v","--verbose",
                      action="store_true",dest="verbose",default=False,
                      help="be verbose, default=false")

    (options, args) = parser.parse_args()

    narg=len(sys.argv)
    if narg < 2 :
        print usage
        sys.exit(1)

    inFile=args[0]
    outFile="/dev/null"
    if len(args) > 1:
        outFile=args[1]
    
    verbose=options.verbose

    l1seeds=getL1AlgoBits(inFile)
    if (verbose): print l1seeds        
    writeORofBits(l1seeds,"L1Algos",outFile)
    

    # techseeds=getL1TechBits(inFile)
    # writeORofBits(techseeds,"L1TechBits","l1TechBits_cffSnippet.py")
    # if (verbose): print techseeds

