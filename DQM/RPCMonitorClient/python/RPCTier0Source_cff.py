import FWCore.ParameterSet.Config as cms

from DQM.RPCMonitorDigi.RPCDigiMonitoring_cfi import rpcdigidqm
rpcdigidqm.UseMuon =  cms.untracked.bool(True)

# FED integrity
from DQM.RPCMonitorClient.RPCFEDIntegrity_cfi import rpcFEDIntegrity

# DQM Services
from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
rpcEventInfo = DQMEDAnalyzer('DQMEventInfo',
    subSystemFolder = cms.untracked.string('RPC')
)

rpcTier0Source = cms.Sequence(rpcdigidqm*rpcEventInfo*rpcFEDIntegrity)

