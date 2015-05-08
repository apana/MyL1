import FWCore.ParameterSet.Config as cms

process = cms.PSet()


## from MyL1.L1Analyzer.files_r240657 import inputFiles
from MyL1.L1Analyzer.files_r240923 import inputFiles

# from Analysis.FWLite.files_r179828_ZeroBias_PFCorJets_v2 import inputFiles

myTrigger="Any"  # reference trigger
# myTrigger="HLT_L1SingleJet92_v4"  # reference trigger
Prescl=1  # prescale factor of reference trigger

# myTrigger="HLT_ZeroBias_part0_v1"  # reference trigger
# Prescl=1  # prescale factor of reference trigger

myOutputFile="Histograms_CompStage1_" + myTrigger + "_240923.root"
# myOutputFile="Debug.root"

nevts=-1
# nevts=100
process.fwliteInput = cms.PSet(
    fileNames   = cms.vstring(inputFiles), ## mandatory
    maxEvents   = cms.int32(nevts),           ## optional
    outputEvery = cms.uint32(10000),       ## optional
)

process.fwliteOutput = cms.PSet(
    fileName  = cms.string('CompStage1.root'),  ## mandatory
)
process.fwliteOutput.fileName=myOutputFile


process.compStage1 = cms.PSet(
    ## input specific for this analyzer
    GtDigis1 = cms.InputTag('gtDigisFED813::L1TEMULATION'),
    GtDigis2 = cms.InputTag('gtDigisFED809::L1TEMULATION'),
    TriggerResults = cms.InputTag('TriggerResults::HLT'),
    ## TriggerResults = cms.InputTag('TriggerResults::TEST'),
    Trigger   = cms.string(myTrigger),
    TriggerBitMap = cms.string("../test/BitsAndPrescales.txt"),
    TriggerPS = cms.int32(Prescl), 
    Debug     = cms.bool(False),
    BeginLumi = cms.uint32(0),
    EndLumi   = cms.uint32(1000)
)
