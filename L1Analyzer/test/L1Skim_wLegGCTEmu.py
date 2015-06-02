import FWCore.ParameterSet.Config as cms

process= cms.Process('L1SKIM')

######## User options ############################################## 

nevts=500
overRideL1=True  # override the L1 menu
GLOBALTAG = 'MCRUN2_74_V6'

OutputFile= "L1AlgoSkim_tst.root"
inputfile="/store/mc/RunIISpring15Digi74/QCD_Pt_80to120_TuneCUETP8M1_13TeV_pythia8/GEN-SIM-RAW/AVE_30_BX_50ns_tsg_MCRUN2_74_V6-v1/60000/04F08357-DAEC-E411-BB03-0025905C94D0.root"

## rawDataLabel="source"  ## DATA
rawDataLabel="rawDataCollector"  ## MC

############# import of standard configurations ####################

process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('L1Trigger.Configuration.CaloTriggerPrimitives_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
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
    useXmlFile = 'L1Menu_Collisions2015_50nsGct_v1_L1T_Scales_20141121_Imp0_0x1030.xml'

    process.load('L1TriggerConfig.L1GtConfigProducers.l1GtTriggerMenuXml_cfi')
    process.l1GtTriggerMenuXml.TriggerMenuLuminosity = luminosityDirectory
    process.l1GtTriggerMenuXml.DefXmlFile = useXmlFile

    process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMenuConfig_cff')
    process.es_prefer_l1GtParameters = cms.ESPrefer('L1GtTriggerMenuXmlProducer','l1GtTriggerMenuXml')

### Apply precales #################################################

process.load("L1TriggerConfig.L1GtConfigProducers.L1GtPrescaleFactorsAlgoTrigConfig_cff")
process.es_prefer_l1GtPrescaleFactorsAlgoTrig = cms.ESPrefer("L1GtPrescaleFactorsAlgoTrigTrivialProducer","l1GtPrescaleFactorsAlgoTrig")

####################################################################


process.simEcalTriggerPrimitiveDigis.Label = 'ecalDigis'
process.simHcalTriggerPrimitiveDigis.inputLabel = cms.VInputTag(
    cms.InputTag('hcalDigis'),
    cms.InputTag('hcalDigis')
    )

process.load("L1TriggerConfig.GctConfigProducers.L1GctConfig_cff")
process.L1GctConfigProducers.JetFinderCentralJetSeed = 10.0
process.L1GctConfigProducers.JetFinderForwardJetSeed = 10.0    
process.es_prefer_gct = cms.ESPrefer("L1GctConfigProducers")


#Unpack original GT
import EventFilter.L1GlobalTriggerRawToDigi.l1GtUnpack_cfi
process.hltGtDigis = EventFilter.L1GlobalTriggerRawToDigi.l1GtUnpack_cfi.l1GtUnpack.clone()
process.hltGtDigis.DaqGtInputTag = rawDataLabel
process.hltGtDigis.UnpackBxInEvent = 5
 

gctDigis="simGctDigis";

import L1Trigger.GlobalTrigger.gtDigis_cfi
process.gtDigisFromSkim = L1Trigger.GlobalTrigger.gtDigis_cfi.gtDigis.clone()
process.gtDigisFromSkim.GmtInputTag = 'hltGtDigis'
process.gtDigisFromSkim.GctInputTag = gctDigis
process.gtDigisFromSkim.TechnicalTriggersInputTags = cms.VInputTag(cms.InputTag('simBscDigis'), 
                                                       cms.InputTag('simRpcTechTrigDigis'))

import L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi
process.l1extraParticlesFromSkim = L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi.l1extraParticles.clone()
process.l1extraParticlesFromSkim.muonSource = cms.InputTag( "hltGtDigis" )
process.l1extraParticlesFromSkim.isolatedEmSource = cms.InputTag( gctDigis,'isoEm' )
process.l1extraParticlesFromSkim.nonIsolatedEmSource = cms.InputTag( gctDigis,'nonIsoEm' )
process.l1extraParticlesFromSkim.centralJetSource = cms.InputTag( gctDigis,'cenJets' )
process.l1extraParticlesFromSkim.forwardJetSource = cms.InputTag( gctDigis,'forJets' )
process.l1extraParticlesFromSkim.tauJetSource = cms.InputTag( gctDigis,'tauJets' )
process.l1extraParticlesFromSkim.etTotalSource = cms.InputTag( gctDigis )
process.l1extraParticlesFromSkim.etHadSource = cms.InputTag( gctDigis )
process.l1extraParticlesFromSkim.etMissSource = cms.InputTag( gctDigis )
process.l1extraParticlesFromSkim.htMissSource = cms.InputTag( gctDigis )
process.l1extraParticlesFromSkim.hfRingEtSumsSource = cms.InputTag( gctDigis )
process.l1extraParticlesFromSkim.hfRingBitCountsSource = cms.InputTag( gctDigis )

## process.raw2digi_step = cms.Path(process.RawToDigi)
process.HLTL1UnpackerSequence = cms.Sequence( process.RawToDigi 
                                                                                               + process.simEcalTriggerPrimitiveDigis+process.simHcalTriggerPrimitiveDigis 
                                                                                               + process.simRctDigis + process.simGctDigis 
                                                                                               + process.hltGtDigis 
                                                                                               + process.gtDigisFromSkim + process.l1extraParticlesFromSkim )


####################################################################

import HLTrigger.HLTfilters.hltLevel1GTSeed_cfi
process.L1Algos = HLTrigger.HLTfilters.hltLevel1GTSeed_cfi.hltLevel1GTSeed.clone()
process.L1Algos.L1GtReadoutRecordTag = 'gtDigisFromSkim'
process.L1Algos.L1GtObjectMapTag = 'gtDigisFromSkim'
process.L1Algos.L1CollectionsTag = cms.InputTag("l1extraParticlesFromSkim")
process.L1Algos.L1MuonCollectionTag = cms.InputTag("l1extraParticlesFromSkim")
process.L1Algos.L1SeedsLogicalExpression = "L1GlobalDecision"

### Trigger report ################################################################

process.load("L1Trigger.GlobalTriggerAnalyzer.l1GtTrigReport_cfi")
process.l1GtTrigReport.L1GtRecordInputTag = "gtDigisFromSkim"
process.l1GtTrigReport.PrintVerbosity = 1

#####################################################################################
#### run the paths and output

process.skimL1Algos = cms.Path(process.HLTL1UnpackerSequence +  process.L1Algos + process.l1GtTrigReport)

process.out = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring('keep *',
                                           'drop *_*_*_SIM',
                                           'drop *_*_*_HLT',
                                           'drop *_*_*_L1SKIM',
                                           'keep *_l1extraParticlesFromSkim_*_*',
                                           'keep *_gtDigisFromSkim_*_*',
                                           'keep *_simGctDigis_*_L1SKIM',
                                           # 'keep *_hltGtDigis_*_*',
                                           # 'keep *_TriggerResults_*_*',
                                           'keep *_rawDataCollector_*_HLT'
                                           ),
                               
    SelectEvents = cms.untracked.PSet(  SelectEvents = cms.vstring( 'skimL1Algos' ) ),
    fileName = cms.untracked.string(OutputFile)
)
process.o = cms.EndPath( process.out )
