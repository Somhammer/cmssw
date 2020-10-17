import FWCore.ParameterSet.Config as cms
from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer

rpcFEDIntegrity = DQMEDAnalyzer('RPCFEDIntegrity',
    prefixDir =  cms.untracked.string('RPC/FEDIntegrity'),
    rawCountsInputTag = cms.untracked.InputTag('muonRPCDigis'),
)

