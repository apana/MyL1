import FWCore.ParameterSet.Config as cms

overRideL1=False  # override the L1 menu
isMC = False
GLOBALTAG = "74X_dataRun2_HLT_v1"

process = cms.Process("L1BitToName")

### Input source ###################################################

inputfile="/store/data/Run2015C/HLTPhysicspart0/RAW/v1/000/254/790/00000/FE944ECF-5148-E511-96F3-02163E012BBB.root"
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(inputfile)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

############# import of standard configurations ####################

process.load('Configuration/StandardSequences/Services_cff')
process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_38T_cff')
## process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
## process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
## process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')
process.GlobalTag.globaltag = GLOBALTAG

if overRideL1:
    luminosityDirectory = "startup"
    useXmlFile = 'L1Menu_Collisions2015_25ns_v1_L1T_Scales_20101224_Imp0_0x102f.xml'

    process.load('L1TriggerConfig.L1GtConfigProducers.l1GtTriggerMenuXml_cfi')
    process.l1GtTriggerMenuXml.TriggerMenuLuminosity = luminosityDirectory
    process.l1GtTriggerMenuXml.DefXmlFile = useXmlFile

    process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMenuConfig_cff')
    process.es_prefer_l1GtParameters = cms.ESPrefer('L1GtTriggerMenuXmlProducer','l1GtTriggerMenuXml')


###################################################################

# unpack the GT

# import EventFilter.L1GlobalTriggerRawToDigi.l1GtUnpack_cfi
# process.hltGtDigis = EventFilter.L1GlobalTriggerRawToDigi.l1GtUnpack_cfi.l1GtUnpack.clone()

process.load("L1Trigger.GlobalTriggerAnalyzer.l1GtTrigReport_cfi")
process.load('Configuration/StandardSequences/RawToDigi_Data_cff')
process.l1GtTrigReport.L1GtRecordInputTag = "gtDigis"

###################################################################

process.l1bittoname = cms.EDAnalyzer("BitNumbertoName",
                                     BitsAndPrescales=cms.string("BitsAndPrescales.txt")
                                     )

process.p = cms.Path(process.RawToDigi + process.l1GtTrigReport + process.l1bittoname)


process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1
process.MessageLogger.categories.append('L1GtTrigReport')

##process.MessageLogger.debugModules = ['l1bittoname']
##process.MessageLogger.cout = cms.untracked.PSet(
##    INFO = cms.untracked.PSet(
##        limit = cms.untracked.int32(-1)
##    ),
##    threshold = cms.untracked.string('DEBUG'), ## DEBUG 
##
##    DEBUG = cms.untracked.PSet( ## DEBUG, all messages  
##
##        limit = cms.untracked.int32(-1)
##    )
##)
