import FWCore.ParameterSet.Config as cms

process = cms.PSet()


## from MyL1.L1Analyzer.files_r240657 import inputFiles
## from MyL1.L1Analyzer.files_r240923 import inputFiles
from MyL1.L1Analyzer.files_r254790 import inputFiles

# from Analysis.FWLite.files_r179828_ZeroBias_PFCorJets_v2 import inputFiles

# myTrigger="Any"  # reference trigger
myTrigger="HLT_Mu17_v2"  # reference trigger
Prescl=1  # prescale factor of reference trigger

# myTrigger="HLT_ZeroBias_part0_v1"  # reference trigger
# Prescl=1  # prescale factor of reference trigger

myOutputFile="Histograms_CompL1Extra_" + myTrigger + "_254790.root"
# myOutputFile="Debug.root"

nevts=-1
# nevts=100000
process.fwliteInput = cms.PSet(
    fileNames   = cms.vstring(inputFiles), ## mandatory
    maxEvents   = cms.int32(nevts),           ## optional
    outputEvery = cms.uint32(100000),       ## optional
)

process.fwliteOutput = cms.PSet(
    fileName  = cms.string(myOutputFile)  ## mandatory
)

process.compL1Extra = cms.PSet(
    ## input specific for this analyzer
    GtDigis1 = cms.InputTag('gtDigis::L1NTUPLE'),
    GtDigis2 = cms.InputTag('gtReEmulDigis::L1NTUPLE'),
    
    L1ExtraEMIsol1 = cms.InputTag('l1ExtraLayer2:Isolated:L1NTUPLE'),
    L1ExtraEMIsol2 = cms.InputTag('l1ExtraReEmul:Isolated:L1NTUPLE'),
    L1ExtraEMNonIsol1 = cms.InputTag('l1ExtraLayer2:NonIsolated:L1NTUPLE'),
    L1ExtraEMNonIsol2 = cms.InputTag('l1ExtraReEmul:NonIsolated:L1NTUPLE'),    
    L1ExtraTauIsol1 = cms.InputTag('l1ExtraLayer2:IsoTau:L1NTUPLE'),
    L1ExtraTauIsol2 = cms.InputTag('l1ExtraReEmul:IsoTau:L1NTUPLE'),
    L1ExtraTauRlx1 = cms.InputTag('l1ExtraLayer2:Tau:L1NTUPLE'),
    L1ExtraTauRlx2 = cms.InputTag('l1ExtraReEmul:Tau:L1NTUPLE'),
    L1ExtraMET1 = cms.InputTag('l1ExtraLayer2:MET:L1NTUPLE'),
    L1ExtraMET2 = cms.InputTag('l1ExtraReEmul:MET:L1NTUPLE'),
    L1ExtraMHT1 = cms.InputTag('l1ExtraLayer2:MHT:L1NTUPLE'),
    L1ExtraMHT2 = cms.InputTag('l1ExtraReEmul:MHT:L1NTUPLE'),
    L1ExtraCenJet1 = cms.InputTag('l1ExtraLayer2:Central:L1NTUPLE'),
    L1ExtraCenJet2 = cms.InputTag('l1ExtraReEmul:Central:L1NTUPLE'),
    L1ExtraForJet1 = cms.InputTag('l1ExtraLayer2:Forward:L1NTUPLE'),
    L1ExtraForJet2 = cms.InputTag('l1ExtraReEmul:Forward:L1NTUPLE'),
    
    TriggerResults = cms.InputTag('TriggerResults::HLT'),
    Trigger   = cms.string(myTrigger),
    TriggerBitMap = cms.string("../test/BitsAndPrescales_run254790.txt"),
    TriggerPS = cms.int32(Prescl), 
    Debug     = cms.bool(False),
    BeginLumi = cms.uint32(0),
    EndLumi   = cms.uint32(1000)
)
