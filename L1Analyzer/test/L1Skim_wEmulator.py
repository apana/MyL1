import FWCore.ParameterSet.Config as cms

process= cms.Process('L1SKIM')

######## User options ############################################## 

nevts=100
overRideL1=True  # override the L1 menu

OutputFile= "L1AlgoSkim.root"

## GLOBALTAG = 'PRE_LS172_V11::All'
## inputfile="root://cms-xrd-global.cern.ch//store/relval/CMSSW_7_2_0_pre8/RelValTTbar_13/GEN-SIM-DIGI-RAW-HLTDEBUG/PU25ns_PRE_LS172_V15-v1/00000/128408A7-F74F-E411-99FB-002618943854.root"

GLOBALTAG = 'PHYS14_25_V3'
inputfile=["root://xrootd.ba.infn.it//store/mc/Phys14DR/Neutrino_Pt-2to20_gun/GEN-SIM-RAW/AVE20BX25_tsg_PHYS14_25_V3-v1/00000/00128B2A-C88E-E411-AFB9-0025905A48D6.root","root://xrootd.ba.infn.it//store/mc/Phys14DR/Neutrino_Pt-2to20_gun/GEN-SIM-RAW/AVE20BX25_tsg_PHYS14_25_V3-v1/00000/0012B7B5-DE8E-E411-99B1-003048FF9AC6.root"]
#inputfile="root://xrootd.ba.infn.it//store/mc/Fall13dr/QCD_Pt-50to80_Tune4C_13TeV_pythia8/GEN-SIM-RAW/castor_tsg_PU40bx25_POSTLS162_V2-v1/20000/FE742333-E480-E311-B65F-20CF305B055B.root"

## rawDataLabel="source"  ## DATA
rawDataLabel="rawDataCollector"  ## MC

############# import of standard configurations ####################

process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_condDBv2_cff')

process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_CONDITIONS'
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

## L1 Menu

if overRideL1:
    luminosityDirectory = "startup"
    useXmlFile = 'L1Menu_Collisions2015_25ns_v2_L1T_Scales_20141121_Imp0_0x1030.xml'

    process.load('L1TriggerConfig.L1GtConfigProducers.l1GtTriggerMenuXml_cfi')
    process.l1GtTriggerMenuXml.TriggerMenuLuminosity = luminosityDirectory
    process.l1GtTriggerMenuXml.DefXmlFile = useXmlFile

    process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMenuConfig_cff')
    process.es_prefer_l1GtParameters = cms.ESPrefer('L1GtTriggerMenuXmlProducer','l1GtTriggerMenuXml')

### Apply precales #################################################

process.load("L1TriggerConfig.L1GtConfigProducers.L1GtPrescaleFactorsAlgoTrigConfig_cff")
process.es_prefer_l1GtPrescaleFactorsAlgoTrig = cms.ESPrefer("L1GtPrescaleFactorsAlgoTrigTrivialProducer","l1GtPrescaleFactorsAlgoTrig")

process.load("L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskAlgoTrigConfig_cff")
process.es_prefer_l1GtTriggerMaskAlgoTrig = cms.ESPrefer("L1GtTriggerMaskAlgoTrigTrivialProducer","l1GtTriggerMaskAlgoTrig")

process.load("L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff")
process.es_prefer_l1GtTriggerMaskTechTrig = cms.ESPrefer("L1GtTriggerMaskTechTrigTrivialProducer","l1GtTriggerMaskTechTrig")

process.load("L1TriggerConfig.L1GtConfigProducers.L1GtPrescaleFactorsTechTrigConfig_cff")
process.es_prefer_l1GtPrescaleFactorsTechTrig = cms.ESPrefer("L1GtPrescaleFactorsTechTrigTrivialProducer","l1GtPrescaleFactorsTechTrig")


####################################################################

# Unpack GT, GCT,
# make l1extras
# run GMT,Stage1, and GT Emulator
###

process.load('Configuration/StandardSequences/SimL1Emulator_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration/StandardSequences/DigiToRaw_cff')

process.raw2digi_step= cms.Path(process.RawToDigi)
process.digi2raw_step = cms.Path(process.DigiToRaw)
process.L1simulation_step = cms.Path(process.SimL1Emulator)


from L1Trigger.L1TCommon.customsPostLS1 import customiseSimL1EmulatorForPostLS1
process=customiseSimL1EmulatorForPostLS1(process)

#####  Redo RCT Digis
process.simRctDigis.ecalDigis = cms.VInputTag( cms.InputTag( 'ecalDigis:EcalTriggerPrimitives' ) )
process.simRctDigis.hcalDigis = cms.VInputTag( cms.InputTag( 'simHcalTriggerPrimitiveDigis' ) )

# HCAL TP hack
process.load("L1Trigger.L1TCalorimeter.L1TRerunHCALTP_FromRaw_cff")
process.reRunHCALTP=cms.Path(process.L1TRerunHCALTP_FromRAW)

### Global Trigger Emulator
import L1Trigger.GlobalTrigger.gtDigis_cfi
process.gtDigisFromSkim = L1Trigger.GlobalTrigger.gtDigis_cfi.gtDigis.clone()
process.gtDigisFromSkim.GmtInputTag = 'simGmtDigis'
process.gtDigisFromSkim.GctInputTag = 'simCaloStage1LegacyFormatDigis'
process.gtDigisFromSkim.TechnicalTriggersInputTags = cms.VInputTag(cms.InputTag('simBscDigis'), 
                                                       cms.InputTag('simRpcTechTrigDigis'))

####
import L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi
process.l1extraParticlesFromSkim = L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi.l1extraParticles.clone()
process.l1extraParticlesFromSkim.muonSource = cms.InputTag( "simGmtDigis" )
process.l1extraParticlesFromSkim.isolatedEmSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','isoEm' )
process.l1extraParticlesFromSkim.nonIsolatedEmSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','nonIsoEm' )
process.l1extraParticlesFromSkim.centralJetSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','cenJets' )
process.l1extraParticlesFromSkim.forwardJetSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','forJets' )
process.l1extraParticlesFromSkim.tauJetSource = cms.InputTag( 'simCaloStage1LegacyFormatDigis','tauJets' )
process.l1extraParticlesFromSkim.etTotalSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" )
process.l1extraParticlesFromSkim.etHadSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" )
process.l1extraParticlesFromSkim.etMissSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" )
process.l1extraParticlesFromSkim.htMissSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" )
process.l1extraParticlesFromSkim.hfRingEtSumsSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" )
process.l1extraParticlesFromSkim.hfRingBitCountsSource = cms.InputTag( "simCaloStage1LegacyFormatDigis" ) 


####################################################################

import HLTrigger.HLTfilters.hltLevel1GTSeed_cfi
process.L1Algos = HLTrigger.HLTfilters.hltLevel1GTSeed_cfi.hltLevel1GTSeed.clone()
process.L1Algos.L1GtReadoutRecordTag = 'gtDigisFromSkim'
process.L1Algos.L1GtObjectMapTag = 'gtDigisFromSkim'
process.L1Algos.L1CollectionsTag = cms.InputTag("l1extraParticlesFromSkim")
process.L1Algos.L1MuonCollectionTag = cms.InputTag("l1extraParticlesFromSkim")
process.L1Algos.L1SeedsLogicalExpression = "L1GlobalDecision"   ## OR of all L1 bits
#process.L1Algos.L1SeedsLogicalExpression = "L1_DoubleEG_15_10 OR L1_SingleEG35 OR L1_SingleIsoEG25er OR L1_DoubleTauJet36er OR L1_DoubleMu_10_0_HighQ_WdEta18 OR L1_ZeroBias OR L1_SingleJetC32_NotBptxOR OR L1_ETM60 OR L1_SingleMu16er OR L1_SingleEG35er OR L1_IsoEG20er_TauJet20er OR L1_SingleIsoEG30er OR L1_DoubleTauJet68er OR L1_Mu5_EG20 OR L1_HTT150 OR L1_SingleEG20 OR L1_SingleJetC20_NotBptxOR OR L1_DoubleMu0_Eta1p6_HighQ_WdEta18_OS OR L1_SingleMu20er OR L1_TripleEG_14_10_8 OR L1_Mu20_EG10 OR L1_DoubleMu_10_3p5 OR L1_ETM70 OR L1_SingleMu16 OR L1_Mu5_IsoEG18 OR L1_Mu16er_TauJet20er OR L1_SingleMu6_NotBptxOR OR L1_SingleJet176"

### Trigger report ################################################################

process.load("L1Trigger.GlobalTriggerAnalyzer.l1GtTrigReport_cfi")
process.l1GtTrigReport.L1GtRecordInputTag = "gtDigisFromSkim"
process.l1GtTrigReport.PrintVerbosity = 1

#####################################################################################
#### run the paths and output

process.skimL1Algos = cms.Path(process.l1extraParticlesFromSkim + process.gtDigisFromSkim + process.L1Algos + process.l1GtTrigReport)

process.out = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring('keep *',
                                           'drop *_*_*_SIM',
                                           'drop *_*_*_HLT',
                                           'drop *_*_*_L1SKIM',
                                           'keep *_rawDataCollector_*_HLT',
                                           'keep *_l1extraParticlesFromSkim_*_*',
                                           'keep *_simCaloStage1LegacyFormatDigis_*_*',
                                           'keep L1MuGMTReadoutCollection_simGmtDigis__L1SKIM',
                                           'keep *_gtDigisFromSkim_*_*',
                                           'keep *_simMuonCSCDigis_*_*',
                                           'keep *_*_MuonCSCHits_*',
                                           ),                               
    SelectEvents = cms.untracked.PSet(  SelectEvents = cms.vstring( 'skimL1Algos' ) ),
    fileName = cms.untracked.string(OutputFile)
)
process.o = cms.EndPath( process.out )


process.L1simulation_step.remove(process.simRpcTechTrigDigis)

process.schedule = cms.Schedule( process.raw2digi_step,
                                 process.reRunHCALTP,
                                 process.L1simulation_step,
                                 process.skimL1Algos, 
                                 ## process.digi2raw_step,
                                 process.o
                                 )
                    
if 'GlobalTag' in process.__dict__:
    from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag as customiseGlobalTag
    process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'auto:run2_mc_GRun')
    process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_CONDITIONS'
    process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')
    for pset in process.GlobalTag.toGet.value():
        pset.connect = pset.connect.value().replace('frontier://FrontierProd/', 'frontier://FrontierProd/')
    # fix for multi-run processing
    process.GlobalTag.RefreshEachRun = cms.untracked.bool( False )
    process.GlobalTag.ReconnectEachRun = cms.untracked.bool( False )
