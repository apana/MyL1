#!/usr/bin/env python

import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras
process = cms.Process('L1GTSUMMARY',eras.Run2_2016)

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')

process.load('L1Trigger.L1TGlobal.GlobalParameters_cff')

#process.load('L1Trigger/L1TGlobal/debug_messages_cfi')
## process.MessageLogger.l1t_debug.l1t.limit = cms.untracked.int32(100000)
#process.MessageLogger.categories.append('l1t|Global')
#process.MessageLogger.debugModules = cms.untracked.vstring('*')
#process.MessageLogger.cerr.threshold = cms.untracked.string('DEBUG')

process.maxEvents = cms.untracked.PSet(
    ##input = cms.untracked.int32(100000)
    input = cms.untracked.int32(10000)
    )

# Input source
process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    fileNames = cms.untracked.vstring(
        ## '/store/data/Run2016H/ZeroBias/RAW/v1/000/283/946/00000/94A3398F-239E-E611-94A7-FA163EE85157.root'
    '/store/data/Run2017A/HLTPhysics/RAW/v1/000/295/317/00000/80CDF7B6-AF42-E711-90BC-02163E01A2BB.root'
        ## '/store/data/Run2017A/L1Accept/RAW/v1/000/295/317/00000/7643BA28-A142-E711-9B2D-02163E01A30C.root'
        ## '/store/data/Run2017A/L1Accept/RAW/v1/000/295/317/00000/5AC44ADD-A142-E711-B5F8-02163E01A4E6.root'
	),
    skipEvents = cms.untracked.uint32(0)
    )

process.output =cms.OutputModule("PoolOutputModule",
        outputCommands = cms.untracked.vstring('keep *'),
	fileName = cms.untracked.string('poolout.root')
	)

process.options = cms.untracked.PSet()
## process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
## process.GlobalTag = GlobalTag(process.GlobalTag, '90X_dataRun2_v0', '')
process.GlobalTag = GlobalTag(process.GlobalTag, '90X_dataRun2_HLT_v2', '')

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
##process.simGtStage2Digis.PrescaleCSVFile = cms.string('prescale_L1TGlobal.csv')
## process.simGtStage2Digis.PrescaleSet = cms.uint32(1)
process.simGtStage2Digis.EmulateBxInEvent = cms.int32(1)
## process.simGtStage2Digis.ExtInputTag = cms.InputTag("simGtExtFakeProd")
## process.simGtStage2Digis.MuonInputTag = cms.InputTag("gmtStage2Digis","Muon")
## process.simGtStage2Digis.EGammaInputTag = cms.InputTag("caloStage2Digis","EGamma")
## process.simGtStage2Digis.TauInputTag = cms.InputTag("caloStage2Digis","Tau")
## process.simGtStage2Digis.JetInputTag = cms.InputTag("caloStage2Digis","Jet")
## process.simGtStage2Digis.EtSumInputTag = cms.InputTag("caloStage2Digis","EtSum")

## gtInputTag="hltGtStage2Digis" ## for l1Accept events
gtInputTag="gtStage2Digis" ## for RAW events

process.simGtStage2Digis.ExtInputTag = cms.InputTag(gtInputTag)
process.simGtStage2Digis.MuonInputTag = cms.InputTag(gtInputTag,"Muon")
process.simGtStage2Digis.EGammaInputTag = cms.InputTag(gtInputTag,"EGamma")
process.simGtStage2Digis.TauInputTag = cms.InputTag(gtInputTag,"Tau")
process.simGtStage2Digis.JetInputTag = cms.InputTag(gtInputTag,"Jet")
process.simGtStage2Digis.EtSumInputTag = cms.InputTag(gtInputTag,"EtSum")



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


process.raw2digi_step = cms.Path(process.RawToDigi)
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
