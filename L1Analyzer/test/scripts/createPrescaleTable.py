#!/usr/bin/python
#
import sys,string,math,os

NumberOfColumns=5
BeginString="PrescaleFactors = cms.vint32"

def usage():
    """ Usage: createPrescaleTable <infile> <outfile>
    Create list of prescales suitable for use in
    l1GtPrescaleFactorsAlgoTrig_cfi.py from <infile>
    <infile> is a list of prescale factors formatted as:
       Bit Number : Trigger Name : prescale factor
    """
    pass

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

def formatString(outlist,i,ilast):

    outstring="   "
    if (i<=NumberOfColumns):
        outstring=BeginString+ "(" + "\n" + outstring 
    for item in outlist:
        outstring = outstring +str(item) + ","

    if i == ilast:
        outstring=outstring[:-1]
        outstring=outstring + ")"
    return outstring


def writePrescaleTable(mylist,file):

    print "Writing prescale table to: ", file
    infile=OpenFile(file,'w')

    i=0
    ibatch=0
    outlist=[]
    for item in mylist:
        i=i+1
        outlist.append(item)
        outstring=""
        if i%NumberOfColumns == 0:
            outstring=formatString(outlist,i,len(mylist))
            infile.write(outstring+"\n")
            outlist=[]
        if (i == len(mylist) and len(outlist)>0): # get the last batch
            outstring=formatString(outlist,i,len(mylist))
            infile.write(outstring+"\n")

    infile.close()
    
def getL1Prescales(file):

    infile=OpenFile(file,'r')
    iline=0

    x = infile.readline()

    prescales=[]
    while x != "":
        iline+=1
        xx=string.strip(x)
        if xx[0] != "#":
            inputLine=string.split(xx,":")
            # print xx
            # print len(inputLine), inputLine[2]
            prescale=int(inputLine[2])
            prescales.append(prescale)
        x = infile.readline()
        
    return prescales

if __name__ == '__main__':


    narg=len(sys.argv)
    if narg < 2 :
        print usage.__doc__
        sys.exit(1)


    inFile=sys.argv[1]
    
    L1prescales=getL1Prescales(inFile)
    print "Number of l1 seeds read: ",len(L1prescales)
    # print L1prescales
    # print l1seeds        
    writePrescaleTable(L1prescales,"prescale_Snippet.py")
    
