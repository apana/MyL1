CXX       = g++
CXXFLAGS  = $(shell root-config --cflags)
CXXFLAGS  += -Wall -fPIC -O3
CXXFLAGS  += $(subst -I, -isystem , $(shell root-config --cflags))

LIBS  = $(shell root-config --libs)

ifneq ($(shell echo $$CMSSW_BASE), )
  CXXFLAGS  += -isystem $(CMSSW_BASE)/src/
  CXXFLAGS  += -isystem $(CMSSW_RELEASE_BASE)/src
  CXXFLAGS  += -isystem $(shell scram tool info boost | awk -F"=" '/INCLUDE=(.*)/{print $$NF}')
  LIBS += -L$(CMSSW_BASE)/lib/$(SCRAM_ARCH)/ -L$(CMSSW_RELEASE_BASE)/lib/$(SCRAM_ARCH)/ \
		  -lFWCoreFWLite -lDataFormatsL1TGlobal
endif

.PHONY: all clean

all: plotDecisions plotBX

clean:
	rm -f plotDecisions plotBX

plotDecisions: plotDecisions.cc
	$(CXX) plotDecisions.cc -o plotDecisions $(CXXFLAGS) $(LIBS)

plotBX: plotBX.cc
	$(CXX) plotBX.cc -o plotBX $(CXXFLAGS) $(LIBS)
