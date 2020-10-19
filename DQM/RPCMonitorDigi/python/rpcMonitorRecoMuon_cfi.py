import FWCore.ParameterSet.Config as cms

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
rpcMonitorRecoMuon = DQMEDAnalyzer('RPCMonitorRecoMuon',
    MuonPtCut = cms.untracked.double(3.0),
    MuonEtaCut= cms.untracked.double(2.1),
    ScalersRawToDigiLabel = cms.InputTag('scalersRawToDigi'),
    MuonLabel =  cms.InputTag('muons'),
    RPCFolder = cms.untracked.string('RPC'),
    GlobalFolder = cms.untracked.string('SummaryHistograms/Muon'),
)

