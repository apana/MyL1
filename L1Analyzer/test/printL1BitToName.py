import FWCore.ParameterSet.Config as cms


overRideL1=True  # override the L1 menu
isMC = False

process = cms.Process("L1BitToName")


### Input source ###################################################

inputfile="/store/data/Run2011B/L1JetHPF/RAW/v1/000/178/208/2AC71D39-5AF3-E011-9DEE-003048D3756A.root"
# inputfile="/store/data/Run2011B/MinimumBias/RAW/v1/000/180/250/007DE4B2-FB02-E111-A378-00215AEDFD74.root"
# inputfile="file:L1AlgoSkim_MinimumBiasMC.root"
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
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')

process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = autoCond['hltonline']
if isMC:
    process.GlobalTag.globaltag = autoCond['startup']

# GLOBALTAG = 'GR_H_V22::All'
# process.GlobalTag.globaltag = GLOBALTAG

if overRideL1:
    luminosityDirectory = "startup"
    useXmlFile = 'L1Menu_Collisions2012_v0_L1T_Scales_20101224_Imp0_0x1027.xml'

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
