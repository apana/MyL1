import FWCore.ParameterSet.Config as cms

nevts=-100
histofile="l1jetplots.root"
# inputfile="/store/data/Commissioning08/Calo/RECO/v1/000/068/288/92E74EA6-63A8-DD11-8628-001617DBCF1E.root"
# inputfile="file:/uscmst1b_scratch/lpc1/lpctrig/apana/dev/CMSSW_3_1_0_pre9/src/run/RelVal_HLT2_8E29_HF07_v1_HF07_500ev_QCD.root"
inputfile="dcache:/pnfs/cms/WAX/11/store/user/lpctrig/apana/GCT/r193336_MinBias_GCT_5GeV/L1Skim_GCT_jet_1_1_WgP.root"

process= cms.Process('L1JetsAnalysis')

process.load('FWCore/MessageService/MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

GLOBAL_TAG='GR_R_50_V3::All'
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = GLOBAL_TAG
process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(nevts)
)

# use TFileService for output histograms
process.TFileService = cms.Service("TFileService",
                              fileName = cms.string(histofile)
                              )
# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(inputfile)
)
process.l1extraParticlesNewGCT = cms.EDProducer("L1ExtraParticlesProd",
    muonSource = cms.InputTag("gtDigis"),
    etTotalSource = cms.InputTag("gctReEmulDigis"),
    nonIsolatedEmSource = cms.InputTag("gctReEmulDigis","nonIsoEm"),
    etMissSource = cms.InputTag("gctReEmulDigis"),
    htMissSource = cms.InputTag("gctReEmulDigis"),
    produceMuonParticles = cms.bool(True),
    forwardJetSource = cms.InputTag("gctReEmulDigis","forJets"),
    centralJetSource = cms.InputTag("gctReEmulDigis","cenJets"),
    produceCaloParticles = cms.bool(True),
    tauJetSource = cms.InputTag("gctReEmulDigis","tauJets"),
    isolatedEmSource = cms.InputTag("gctReEmulDigis","isoEm"),
    etHadSource = cms.InputTag("gctReEmulDigis"),
    hfRingEtSumsSource = cms.InputTag("gctReEmulDigis"),
    hfRingBitCountsSource = cms.InputTag("gctReEmulDigis"),
    centralBxOnly = cms.bool(True),
    ignoreHtMiss = cms.bool(False)
)


######################################################################

process.hltGtDigis = cms.EDProducer( "L1GlobalTriggerRawToDigi",
    DaqGtFedId = cms.untracked.int32( 813 ),
    DaqGtInputTag = cms.InputTag( "rawDataCollector" ),
    UnpackBxInEvent = cms.int32( 5 ),
    ActiveBoardsMask = cms.uint32( 0xffff )
)
process.hltGctDigis = cms.EDProducer( "GctRawToDigi",
    unpackSharedRegions = cms.bool( False ),
    numberOfGctSamplesToUnpack = cms.uint32( 1 ),
    verbose = cms.untracked.bool( False ),
    numberOfRctSamplesToUnpack = cms.uint32( 1 ),
    inputLabel = cms.InputTag( "rawDataCollector" ),
    unpackerVersion = cms.uint32( 0 ),
    gctFedId = cms.untracked.int32( 745 ),
    hltMode = cms.bool( True )
)
process.hltL1GtObjectMap = cms.EDProducer( "L1GlobalTrigger",
    TechnicalTriggersUnprescaled = cms.bool( True ),
    ProduceL1GtObjectMapRecord = cms.bool( True ),
    AlgorithmTriggersUnmasked = cms.bool( False ),
    EmulateBxInEvent = cms.int32( 1 ),
    AlgorithmTriggersUnprescaled = cms.bool( True ),
    ProduceL1GtDaqRecord = cms.bool( False ),
    ReadTechnicalTriggerRecords = cms.bool( True ),
    RecordLength = cms.vint32( 3, 0 ),
    TechnicalTriggersUnmasked = cms.bool( False ),
    ProduceL1GtEvmRecord = cms.bool( False ),
    GmtInputTag = cms.InputTag( "hltGtDigis" ),
    TechnicalTriggersVetoUnmasked = cms.bool( True ),
    AlternativeNrBxBoardEvm = cms.uint32( 0 ),
    TechnicalTriggersInputTags = cms.VInputTag( 'simBscDigis' ),
    CastorInputTag = cms.InputTag( "castorL1Digis" ),
    GctInputTag = cms.InputTag( "hltGctDigis" ),
    AlternativeNrBxBoardDaq = cms.uint32( 0 ),
    WritePsbL1GtDaqRecord = cms.bool( False ),
    BstLengthBytes = cms.int32( -1 )
)
process.hltL1extraParticles = cms.EDProducer( "L1ExtraParticlesProd",
    tauJetSource = cms.InputTag( 'hltGctDigis','tauJets' ),
    etHadSource = cms.InputTag( "hltGctDigis" ),
    etTotalSource = cms.InputTag( "hltGctDigis" ),
    centralBxOnly = cms.bool( True ),
    centralJetSource = cms.InputTag( 'hltGctDigis','cenJets' ),
    etMissSource = cms.InputTag( "hltGctDigis" ),
    hfRingEtSumsSource = cms.InputTag( "hltGctDigis" ),
    produceMuonParticles = cms.bool( True ),
    forwardJetSource = cms.InputTag( 'hltGctDigis','forJets' ),
    ignoreHtMiss = cms.bool( False ),
    htMissSource = cms.InputTag( "hltGctDigis" ),
    produceCaloParticles = cms.bool( True ),
    muonSource = cms.InputTag( "hltGtDigis" ),
    isolatedEmSource = cms.InputTag( 'hltGctDigis','isoEm' ),
    nonIsolatedEmSource = cms.InputTag( 'hltGctDigis','nonIsoEm' ),
    hfRingBitCountsSource = cms.InputTag( "hltGctDigis" )
)
###########################################################################################

process.l1jetsOldGCT = cms.EDAnalyzer("L1JetPlots",
                                 CaloJetAlgorithm = cms.InputTag("ak5CaloJets"),
                                 PFJetAlgorithm   = cms.InputTag("ak5PFJets"),
                                 GenJetAlgorithm  = cms.InputTag("iterativeCone5GenJets"),
                                 recmet           = cms.InputTag("hltMet::HLT"),
                                 genmet           = cms.InputTag("genMetCalo"),
                                 l1collections    = cms.InputTag("hltL1extraParticles")
                               )

process.l1jetsNewGCT = cms.EDAnalyzer("L1JetPlots",
                                 CaloJetAlgorithm = cms.InputTag("ak5CaloJets"),
                                 PFJetAlgorithm   = cms.InputTag("ak5PFJets"),
                                 GenJetAlgorithm  = cms.InputTag("iterativeCone5GenJets"),
                                 recmet           = cms.InputTag("hltMet::HLT"),
                                 genmet           = cms.InputTag("genMetCalo"),
                                 l1collections    = cms.InputTag("l1extraParticlesNewGCT")
                               )

# paths to run
process.HLTL1UnpackerSequence = cms.Sequence( process.hltGtDigis + process.hltGctDigis + process.hltL1GtObjectMap)

process.p1 = cms.Path(process.l1extraParticlesNewGCT + process.l1jetsNewGCT)
process.p2 = cms.Path(process.HLTL1UnpackerSequence + process.hltL1extraParticles + process.l1jetsOldGCT)

# process.schedule = cms.Schedule(process.p1)
