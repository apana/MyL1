import FWCore.ParameterSet.Config as cms

l1bits = cms.EDAnalyzer("L1Bits",
                        Outfile   = cms.string("l1results_8e29.dat"),
                        L1GtRecordInputTag  = cms.InputTag("gtDigis"),
                        # L1GtRecordInputTag  = cms.InputTag("hltGtDigis::HLT"),
                        L1GtObjectMapRecord = cms.InputTag("hltL1GtObjectMap::HLT"),
                        L1AlgoName = cms.string("L1_SingleJet15"),
                        CaloJetAlgorithm = cms.InputTag("iterativeCone5CaloJets")
                        )
