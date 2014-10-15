import FWCore.ParameterSet.Config as cms

process= cms.Process('L1SKIMSKIM')

######## User options ############################################## 

nevts=100
overRideL1=True  # override the L1 menu
GLOBALTAG = 'PRE_LS172_V11::All'

OutputFile= "L1AlgoSkim_tst.root"
inputfile="root://cms-xrd-global.cern.ch//store/relval/CMSSW_7_2_0_pre8/RelValTTbar_13/GEN-SIM-DIGI-RAW-HLTDEBUG/PU25ns_PRE_LS172_V15-v1/00000/128408A7-F74F-E411-99FB-002618943854.root"

## rawDataLabel="source"  ## DATA
rawDataLabel="rawDataCollector"  ## MC

############# import of standard configurations ####################

process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')

process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')
process.GlobalTag.globaltag = GLOBALTAG


process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
    )

process.load('FWCore/MessageService/MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.categories.append('L1GtTrigReport')


### Input source ###################################################

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(inputfile)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(nevts)
)

if overRideL1:
    luminosityDirectory = "startup"
    useXmlFile = 'L1Menu_Collisions2015_25ns_v1_L1T_Scales_20101224_Imp0_0x102f.xml'

    process.load('L1TriggerConfig.L1GtConfigProducers.l1GtTriggerMenuXml_cfi')
    process.l1GtTriggerMenuXml.TriggerMenuLuminosity = luminosityDirectory
    process.l1GtTriggerMenuXml.DefXmlFile = useXmlFile

    process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMenuConfig_cff')
    process.es_prefer_l1GtParameters = cms.ESPrefer('L1GtTriggerMenuXmlProducer','l1GtTriggerMenuXml')

### Apply precales #################################################

process.load("L1TriggerConfig.L1GtConfigProducers.L1GtPrescaleFactorsAlgoTrigConfig_cff")
process.es_prefer_l1GtPrescaleFactorsAlgoTrig = cms.ESPrefer("L1GtPrescaleFactorsAlgoTrigTrivialProducer","l1GtPrescaleFactorsAlgoTrig")

####################################################################

# Unpack GT, GCT,
# make l1extras
# run GT Emulator
###

import EventFilter.L1GlobalTriggerRawToDigi.l1GtUnpack_cfi
process.hltGtDigis = EventFilter.L1GlobalTriggerRawToDigi.l1GtUnpack_cfi.l1GtUnpack.clone()
process.hltGtDigis.DaqGtInputTag = rawDataLabel
process.hltGtDigis.UnpackBxInEvent = 5
 
import EventFilter.GctRawToDigi.l1GctHwDigis_cfi
process.hltGctDigis = EventFilter.GctRawToDigi.l1GctHwDigis_cfi.l1GctHwDigis.clone()
process.hltGctDigis.inputLabel = rawDataLabel
process.hltGctDigis.hltMode = True

import L1Trigger.GlobalTrigger.gtDigis_cfi
process.hltGtDigisPrescaled = L1Trigger.GlobalTrigger.gtDigis_cfi.gtDigis.clone()
process.hltGtDigisPrescaled.GmtInputTag = 'hltGtDigis'
process.hltGtDigisPrescaled.GctInputTag = 'hltGctDigis'
process.hltGtDigisPrescaled.TechnicalTriggersInputTags = cms.VInputTag(cms.InputTag('simBscDigis'), 
                                                       cms.InputTag('simRpcTechTrigDigis'))

import L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi
process.hltL1extraParticles = L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi.l1extraParticles.clone()
process.hltL1extraParticles.muonSource = cms.InputTag( "hltGtDigis" )
process.hltL1extraParticles.isolatedEmSource = cms.InputTag( 'hltGctDigis','isoEm' )
process.hltL1extraParticles.nonIsolatedEmSource = cms.InputTag( 'hltGctDigis','nonIsoEm' )
process.hltL1extraParticles.centralJetSource = cms.InputTag( 'hltGctDigis','cenJets' )
process.hltL1extraParticles.forwardJetSource = cms.InputTag( 'hltGctDigis','forJets' )
process.hltL1extraParticles.tauJetSource = cms.InputTag( 'hltGctDigis','tauJets' )
process.hltL1extraParticles.etTotalSource = cms.InputTag( "hltGctDigis" )
process.hltL1extraParticles.etHadSource = cms.InputTag( "hltGctDigis" )
process.hltL1extraParticles.etMissSource = cms.InputTag( "hltGctDigis" )
process.hltL1extraParticles.htMissSource = cms.InputTag( "hltGctDigis" )
process.hltL1extraParticles.hfRingEtSumsSource = cms.InputTag( "hltGctDigis" )
process.hltL1extraParticles.hfRingBitCountsSource = cms.InputTag( "hltGctDigis" )


process.HLTL1UnpackerSequence = cms.Sequence( process.hltGtDigis + process.hltGctDigis + process.hltGtDigisPrescaled + process.hltL1extraParticles )


####################################################################

import HLTrigger.HLTfilters.hltLevel1GTSeed_cfi
process.L1Algos = HLTrigger.HLTfilters.hltLevel1GTSeed_cfi.hltLevel1GTSeed.clone()
process.L1Algos.L1GtReadoutRecordTag = 'hltGtDigisPrescaled'
process.L1Algos.L1GtObjectMapTag = 'hltGtDigisPrescaled'
process.L1Algos.L1CollectionsTag = cms.InputTag("hltL1extraParticles")
process.L1Algos.L1MuonCollectionTag = cms.InputTag("hltL1extraParticles")
process.L1Algos.L1SeedsLogicalExpression = "L1_DummyBit"

### Trigger report ################################################################

process.load("L1Trigger.GlobalTriggerAnalyzer.l1GtTrigReport_cfi")
process.l1GtTrigReport.L1GtRecordInputTag = "hltGtDigisPrescaled"
process.l1GtTrigReport.PrintVerbosity = 1

#####################################################################################
#### run the paths and output

process.skimL1Algos = cms.Path(process.HLTL1UnpackerSequence +  process.L1Algos + process.l1GtTrigReport)
## process.skimL1Algos = cms.Path(process.L1Algos + process.l1GtTrigReport)

process.out = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring('keep *',
                                           'drop *_*_*_L1SKIMSKIM',
                                           ),
                               
    SelectEvents = cms.untracked.PSet(  SelectEvents = cms.vstring( 'skimL1Algos' ) ),
    fileName = cms.untracked.string(OutputFile)
)
process.o = cms.EndPath( process.out )

# process.schedule = cms.Schedule(process.skimL1Algos, process.o)

