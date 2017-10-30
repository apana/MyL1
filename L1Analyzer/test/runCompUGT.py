#!/usr/bin/env python

import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras
process = cms.Process('COMPUGT',eras.Run2_2017)

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
## process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('L1Trigger.Configuration.L1TRawToDigi_cff')

process.load('L1Trigger.L1TGlobal.GlobalParameters_cff')

## process.load('L1Trigger/L1TGlobal/debug_messages_cfi')
## # process.MessageLogger.l1t_debug.l1t.limit = cms.untracked.int32(100000)
## process.MessageLogger.categories.append('l1t|Global')
## process.MessageLogger.categories.append('L1TGlobal')
## process.MessageLogger.debugModules = cms.untracked.vstring('*')
## process.MessageLogger.cerr.threshold = cms.untracked.string('DEBUG')
## process.MessageLogger.cerr.default.limit = cms.untracked.int32(0)
process.MessageLogger.cerr.FwkReport.reportEvery = 1000


process.maxEvents = cms.untracked.PSet(
    ## input = cms.untracked.int32(2)
    input = cms.untracked.int32(-100)
    )

# Input source
process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    fileNames = cms.untracked.vstring(
        "root://cmseos.fnal.gov//store/user/lpctrig/apana/uGT/Validation/run304777_ErrorStream_RAW.root",
        "root://cmseos.fnal.gov//store/user/lpctrig/apana/uGT/Validation/run304776_ErrorStream_RAW.root"
        ## "/store/data/Run2017E/ZeroBias/RAW/v1/000/304/740/00000/5E28E7A0-9BAD-E711-98FF-02163E019B69.root"
	),
    skipEvents = cms.untracked.uint32(78)
    )

# use TFileService for output histograms
process.TFileService = cms.Service("TFileService",
                              fileName = cms.string("comp_ugt.root")
                              ##      fileName = cms.string("comp_ugt_run304777_ErrorStream.root")
                              )

process.output =cms.OutputModule("PoolOutputModule",
        outputCommands = cms.untracked.vstring('keep *'),
	fileName = cms.untracked.string('poolout_compUGT.root')
	)

process.options = cms.untracked.PSet()
## process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
## process.GlobalTag = GlobalTag(process.GlobalTag, '90X_dataRun2_v0', '')
process.GlobalTag = GlobalTag(process.GlobalTag, '92X_dataRun2_HLT_v7', '')


## from CondCore.DBCommon.CondDBSetup_cfi import CondDBSetup
## process.l1conddb = cms.ESSource("PoolDBESSource",
##        CondDBSetup,
##        connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
##        toGet   = cms.VPSet(
##             cms.PSet(
##                  #record = cms.string('L1TGlobalPrescalesVetosRcd'),
##                  #tag = cms.string("L1TGlobalPrescalesVetos_Stage2v0_hlt")
##                 record = cms.string('L1TUtmTriggerMenuRcd'),
##                 ## tag = cms.string("L1Menu_Collisions2017_dev_r5_m4_patch_921")
##                 tag = cms.string("L1Menu_Collisions2017_v4_m6")
##             )
##        )
## )
## process.es_prefer_l1conddb = cms.ESPrefer( "PoolDBESSource","l1conddb")

##### needed until prescales go into GlobalTag ########################
## from CondCore.DBCommon.CondDBSetup_cfi import CondDBSetup
## process.l1conddb = cms.ESSource("PoolDBESSource",
##        CondDBSetup,
##        connect = cms.string('frontier://FrontierPrep/CMS_CONDITIONS'),
##        toGet   = cms.VPSet(
##             cms.PSet(
##                  record = cms.string('L1TGlobalPrescalesVetosRcd'),
##                  tag = cms.string("L1TGlobalPrescalesVetos_Stage2v0_hlt")
##             )
##        )
## )
## process.es_prefer_l1conddb = cms.ESPrefer( "PoolDBESSource","l1conddb")
#### done ##############################################################

## Fill External conditions
process.load('L1Trigger.L1TGlobal.simGtExtFakeProd_cfi')
process.simGtExtFakeProd.bxFirst = cms.int32(-2)
process.simGtExtFakeProd.bxLast = cms.int32(2)
process.simGtExtFakeProd.setBptxAND   = cms.bool(True)
process.simGtExtFakeProd.setBptxPlus  = cms.bool(True)
process.simGtExtFakeProd.setBptxMinus = cms.bool(True)
process.simGtExtFakeProd.setBptxOR    = cms.bool(True)

process.load('L1Trigger.L1TGlobal.simGtStage2Digis_cfi')
process.simGtStage2Digis.EmulateBxInEvent = cms.int32(1)
## process.simGtStage2Digis.ExtInputTag = cms.InputTag("simGtExtFakeProd")
process.simGtStage2Digis.ExtInputTag = cms.InputTag("gtStage2Digis")
process.simGtStage2Digis.MuonInputTag = cms.InputTag("gmtStage2Digis","Muon")
## process.simGtStage2Digis.MuonInputTag = cms.InputTag("gtStage2Digis","Muon")

process.simGtStage2Digis.EGammaInputTag = cms.InputTag("caloStage2Digis","EGamma")
process.simGtStage2Digis.TauInputTag = cms.InputTag("caloStage2Digis","Tau")
process.simGtStage2Digis.JetInputTag = cms.InputTag("caloStage2Digis","Jet")
process.simGtStage2Digis.EtSumInputTag = cms.InputTag("caloStage2Digis","EtSum")

## process.simGtStage2Digis.EGammaInputTag = cms.InputTag("gtStage2Digis","EGamma")
## process.simGtStage2Digis.TauInputTag = cms.InputTag("gtStage2Digis","Tau")
## process.simGtStage2Digis.JetInputTag = cms.InputTag("gtStage2Digis","Jet")
## process.simGtStage2Digis.EtSumInputTag = cms.InputTag("gtStage2Digis","EtSum")


## gtInputTag="hltGtStage2Digis" ## for l1Accept events
## gtInputTag="gtStage2Digis" ## for RAW events
##
## process.simGtStage2Digis.ExtInputTag = cms.InputTag(gtInputTag)
## process.simGtStage2Digis.MuonInputTag = cms.InputTag(gtInputTag,"Muon")
## process.simGtStage2Digis.EGammaInputTag = cms.InputTag(gtInputTag,"EGamma")
## process.simGtStage2Digis.TauInputTag = cms.InputTag(gtInputTag,"Tau")
## process.simGtStage2Digis.JetInputTag = cms.InputTag(gtInputTag,"Jet")
## process.simGtStage2Digis.EtSumInputTag = cms.InputTag(gtInputTag,"EtSum")



################################################################################

process.compugt = cms.EDAnalyzer("l1t::CompUGT",
                                 uGtAlgInputTag1 = cms.InputTag("gtStage2Digis"),
                                 uGtExtInputTag1 = cms.InputTag("gtExtFakeProd"),
                                 uGtAlgInputTag2 = cms.InputTag("simGtStage2Digis"),
                                 uGtExtInputTag2 = cms.InputTag("simGtExtFakeProd"),
                                 ## uGtAlgInputTag1 = cms.InputTag("hltGtStage2Digis"),
                                 ## uGtExtInputTag1 = cms.InputTag("simGtExtFakeProd"),
                                 ## uGtAlgInputTag2 = cms.InputTag("simGtStage2Digis"),
                                 ## uGtExtInputTag2 = cms.InputTag("simGtExtFakeProd"),
                                 )

################################################################################


## process.raw2digi_step = cms.Path(process.RawToDigi)
process.raw2digi_step = cms.Path(process.L1TRawToDigi_Stage2)

process.gtEmul = cms.Path(process.simGtExtFakeProd
                          *process.simGtStage2Digis)
process.p = cms.Path(process.compugt)

process.schedule = cms.Schedule(process.raw2digi_step,
                                process.gtEmul,
                                process.p)
rootout=False
## rootout=True
if rootout:
    process.outpath = cms.EndPath(process.output)
    process.schedule.append(process.outpath)

dump=False
## dump=True
if dump:
    outfile = open('dump_config.py','w')
    print >> outfile,process.dumpPython()
    outfile.close()
