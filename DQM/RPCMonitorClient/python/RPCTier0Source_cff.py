import FWCore.ParameterSet.Config as cms

from DQM.RPCMonitorDigi.RPCDigiMonitoring_cfi import rpcdigidqm
rpcdigidqm.UseMuon =  cms.untracked.bool(True)

from DQM.RPCMonitorDigi.RPCFEDIntegrity_cfi import rpcFEDIntegrity
from DQM.RPCMonitorDigi.rpcMonitorRecoMuon_cfi import rpcMonitorRecoMuon

# DQM Services
from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
rpcEventInfo = DQMEDAnalyzer('DQMEventInfo',
    subSystemFolder = cms.untracked.string('RPC')
)

rpcTier0Source = cms.Sequence(
    rpcdigidqm
  * rpcMonitorRecoMuon
  * rpcEventInfo
  * rpcFEDIntegrity
)

