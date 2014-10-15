import FWCore.ParameterSet.Config as cms

process= cms.Process('L1SKIMSKIM')

######## User options ############################################## 

nevts=100

isMC = True
# isMC = True

OutputFile= "L1AlgoSkim_tst.root"

inputfile="/store/data/Run2010B/Jet/RAW/v1/000/149/181/0C21A23E-3DE2-DF11-9008-0030487C778E.root"
rawDataLabel="source"
GLOBALTAG = 'GR10_H_V9::All'

if isMC:
    inputfile="dcap://cmsdca.fnal.gov:24136/pnfs/fnal.gov/usr/cms/WAX/11/store/user/apana/l1skims/7e31/MB_PU10/L1AlgoSkim_MinimumBiasMC_7e31_17_1_BLQ.root"
    rawDataLabel="rawDataCollector"
    GLOBALTAG = 'L1HLTST311_V0::All'

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

### Apply precales #################################################

# uncomment the following 2 lines if using a custom L1 menu (xml file)
# process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMenuConfig_cff')
# process.l1GtTriggerMenuXml.DefXmlFile = 'L1Menu_Commissioning2010_v2_test0_L1T_Scales_20080926_startup_Imp0_0x0000.xml'

#process.load("L1TriggerConfig.L1GtConfigProducers.L1GtPrescaleFactorsAlgoTrigConfig_cff")
#process.es_prefer_l1GtPrescaleFactorsAlgoTrig = cms.ESPrefer("L1GtPrescaleFactorsAlgoTrigTrivialProducer","l1GtPrescaleFactorsAlgoTrig")

####################################################################

# Unpack GT, GCT,
# make l1extras
# run GT Emulator
###

# import EventFilter.L1GlobalTriggerRawToDigi.l1GtUnpack_cfi
# process.hltGtDigis = EventFilter.L1GlobalTriggerRawToDigi.l1GtUnpack_cfi.l1GtUnpack.clone()
# process.hltGtDigis.DaqGtInputTag = rawDataLabel
# process.hltGtDigis.UnpackBxInEvent = 5
# 
# import EventFilter.GctRawToDigi.l1GctHwDigis_cfi
# process.hltGctDigis = EventFilter.GctRawToDigi.l1GctHwDigis_cfi.l1GctHwDigis.clone()
# process.hltGctDigis.inputLabel = rawDataLabel
# process.hltGctDigis.hltMode = True
# 
# import L1Trigger.GlobalTrigger.gtDigis_cfi
# process.newL1GtObjectMap = L1Trigger.GlobalTrigger.gtDigis_cfi.gtDigis.clone()
# process.newL1GtObjectMap.GmtInputTag = 'hltGtDigis'
# process.newL1GtObjectMap.GctInputTag = 'hltGctDigis'
# process.newL1GtObjectMap.TechnicalTriggersInputTags = cms.VInputTag(cms.InputTag('simBscDigis'), 
#                                                        cms.InputTag('simRpcTechTrigDigis'))
# 
# import L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi
# process.hltL1extraParticles = L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi.l1extraParticles.clone()
# process.hltL1extraParticles.muonSource = cms.InputTag( "hltGtDigis" )
# process.hltL1extraParticles.isolatedEmSource = cms.InputTag( 'hltGctDigis','isoEm' )
# process.hltL1extraParticles.nonIsolatedEmSource = cms.InputTag( 'hltGctDigis','nonIsoEm' )
# process.hltL1extraParticles.centralJetSource = cms.InputTag( 'hltGctDigis','cenJets' )
# process.hltL1extraParticles.forwardJetSource = cms.InputTag( 'hltGctDigis','forJets' )
# process.hltL1extraParticles.tauJetSource = cms.InputTag( 'hltGctDigis','tauJets' )
# process.hltL1extraParticles.etTotalSource = cms.InputTag( "hltGctDigis" )
# process.hltL1extraParticles.etHadSource = cms.InputTag( "hltGctDigis" )
# process.hltL1extraParticles.etMissSource = cms.InputTag( "hltGctDigis" )
# process.hltL1extraParticles.htMissSource = cms.InputTag( "hltGctDigis" )
# process.hltL1extraParticles.hfRingEtSumsSource = cms.InputTag( "hltGctDigis" )
# process.hltL1extraParticles.hfRingBitCountsSource = cms.InputTag( "hltGctDigis" )
# 
# 
# process.HLTL1UnpackerSequence = cms.Sequence( process.hltGtDigis + process.hltGctDigis + process.newL1GtObjectMap + process.hltL1extraParticles )


####################################################################

import HLTrigger.HLTfilters.hltLevel1GTSeed_cfi
process.L1Algos = HLTrigger.HLTfilters.hltLevel1GTSeed_cfi.hltLevel1GTSeed.clone()
process.L1Algos.L1GtReadoutRecordTag = 'simGtDigis'
process.L1Algos.L1GtObjectMapTag = 'simGtDigis'
process.L1Algos.L1CollectionsTag = cms.InputTag("hltL1extraParticles")
process.L1Algos.L1MuonCollectionTag = cms.InputTag("hltL1extraParticles")
# process.L1Algos.L1SeedsLogicalExpression = "L1_SingleEG2"
# process.L1Algos.L1SeedsLogicalExpression = "L1_SingleIsoEG10 OR L1_SingleJet36 OR L1_SingleMu7 OR L1_DoubleMu_0_5 OR L1_SingleTauJet68 OR L1_SingleMu3 OR L1_SingleJet16 OR L1_ETM30 OR L1_SingleJet20_NotBptxOR OR L1_SingleJet80_Central OR L1_SingleIsoEG12 OR L1_SingleIsoEG15 OR L1_DoubleJet36_Central OR L1_SingleEG30 OR L1_SingleMu5_Eta1p5_Q80 OR L1_SingleIsoEG5 OR L1_EG5_HTT100 OR L1_ETT180 OR L1_DoubleIsoEG5 OR L1_SingleIsoEG12_Eta2p17 OR L1_DoubleEG3 OR L1_EG5_HTT125 OR L1_DoubleEG5 OR L1_SingleEG15 OR L1_DoubleEG8 OR L1_ETM20 OR L1_DoubleEG_12_5 OR L1_DoubleIsoEG8 OR L1_DoubleForJet36_EtaOpp OR L1_Mu3_Jet16 OR L1_TripleEG5 OR L1_TripleEG7 OR L1_EG5_HTT75 OR L1_SingleJet68 OR L1_SingleMu25 OR L1_DoubleEG10 OR L1_SingleJet128 OR L1_SingleMu20 OR L1_SingleEG12 OR L1_DoubleEG2_FwdVeto OR L1_DoubleTauJet28 OR L1_SingleJet20_NotBptxOR_NotMuBeamHalo OR L1_HTT100 OR L1_HTT50 OR L1_Mu0_HTT50 OR L1_SingleEG12_Eta2p17 OR L1_Mu7_Jet20_Central OR L1_Mu3_EG5 OR L1_Mu5_EG8 OR L1_Mu3_EG8 OR L1_QuadJet20_Central OR L1_SingleEG20 OR L1_DoubleMu3 OR L1_DoubleIsoEG10 OR L1_SingleJet36_FwdVeto OR L1_DoubleMu0 OR L1_DoubleEG5_HTT50 OR L1_DoubleMu5 OR L1_DoubleEG5_HTT75 OR L1_SingleMuOpen OR L1_DoubleForJet20_EtaOpp OR L1_SingleJet92 OR L1_SingleMu10 OR L1_SingleMu16 OR L1_SingleMu12 OR L1_DoubleJet52 OR L1_Mu3_Jet20 OR L1_TripleJet28 OR L1_SingleEG5 OR L1_SingleJet52 OR L1_SingleTauJet52"
process.L1Algos.L1SeedsLogicalExpression = "L1_SingleIsoEG10 OR L1_SingleJet36 OR L1_SingleMu7 OR L1_DoubleMu_0_5 OR L1_SingleTauJet68 OR L1_SingleMu3 OR L1_SingleJet16 OR L1_ETM30 OR L1_SingleJet20_NotBptxOR OR L1_SingleJet80_Central OR L1_SingleIsoEG12 OR L1_SingleIsoEG15 OR L1_DoubleJet36_Central OR L1_SingleEG30 OR L1_SingleMu5_Eta1p5_Q80 OR L1_EG5_HTT100 OR L1_ETT180 OR L1_DoubleIsoEG5 OR L1_SingleIsoEG12_Eta2p17 OR L1_DoubleEG3 OR L1_EG5_HTT125 OR L1_DoubleEG5 OR L1_SingleEG15 OR L1_DoubleEG8 OR L1_ETM20 OR L1_DoubleEG_12_5 OR L1_DoubleIsoEG8 OR L1_DoubleForJet36_EtaOpp OR L1_Mu3_Jet16 OR L1_TripleEG5 OR L1_TripleEG7 OR L1_EG5_HTT75 OR L1_SingleJet68 OR L1_SingleMu25 OR L1_DoubleEG10 OR L1_SingleJet128 OR L1_SingleMu20 OR L1_SingleEG12 OR L1_DoubleEG2_FwdVeto OR L1_DoubleTauJet28 OR L1_SingleJet20_NotBptxOR_NotMuBeamHalo OR L1_HTT100 OR L1_HTT50 OR L1_Mu0_HTT50 OR L1_SingleEG12_Eta2p17 OR L1_Mu7_Jet20_Central OR L1_Mu3_EG5 OR L1_Mu5_EG8 OR L1_Mu3_EG8 OR L1_QuadJet20_Central OR L1_SingleEG20 OR L1_DoubleMu3 OR L1_DoubleIsoEG10 OR L1_SingleJet36_FwdVeto OR L1_DoubleMu0 OR L1_DoubleEG5_HTT50 OR L1_DoubleMu5 OR L1_DoubleEG5_HTT75 OR L1_SingleMuOpen OR L1_DoubleForJet20_EtaOpp OR L1_SingleJet92 OR L1_SingleMu10 OR L1_SingleMu16 OR L1_SingleMu12 OR L1_DoubleJet52 OR L1_Mu3_Jet20 OR L1_TripleJet28 OR L1_SingleEG5 OR L1_SingleJet52 OR L1_SingleTauJet52"

### Trigger report ################################################################

process.load("L1Trigger.GlobalTriggerAnalyzer.l1GtTrigReport_cfi")
process.l1GtTrigReport.L1GtRecordInputTag = "simGtDigis"
process.l1GtTrigReport.PrintVerbosity = 1

#####################################################################################
#### run the paths and output

# process.skimL1Algos = cms.Path(process.HLTL1UnpackerSequence +  process.L1Algos + process.l1GtTrigReport)
process.skimL1Algos = cms.Path(process.L1Algos + process.l1GtTrigReport)

process.out = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring('keep *',
                                           'drop *_*_*_L1SKIMSKIM',
                                           ),
                               
    SelectEvents = cms.untracked.PSet(  SelectEvents = cms.vstring( 'skimL1Algos' ) ),
    fileName = cms.untracked.string(OutputFile)
)
process.o = cms.EndPath( process.out )

# process.schedule = cms.Schedule(process.skimL1Algos, process.o)

