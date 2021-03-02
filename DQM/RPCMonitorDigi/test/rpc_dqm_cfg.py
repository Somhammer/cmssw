import FWCore.ParameterSet.Config as cms

#from Configuration.Eras.Era_Run2_2017_cff import Run2_2017
#process = cms.Process('DQM',Run2_2017)
process = cms.Process('DQM')

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.load('DQMOffline.Configuration.DQMOfflineMC_cff')
process.load('DQMServices.Core.DQMStoreNonLegacy_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/relval/CMSSW_11_2_0_pre5/RelValZMM_14/GEN-SIM-RECO/112X_mcRun3_2021_realistic_v7-v1/20000/0950B6DE-D002-CE4D-8E05-9E5302CA0A92.root',
        '/store/relval/CMSSW_11_2_0_pre5/RelValZMM_14/GEN-SIM-RECO/112X_mcRun3_2021_realistic_v7-v1/20000/61C845BB-DCAB-644B-858A-C8C607D46047.root',
        '/store/relval/CMSSW_11_2_0_pre5/RelValZMM_14/GEN-SIM-RECO/112X_mcRun3_2021_realistic_v7-v1/20000/8217A887-1C53-E241-A985-3A386FE84F57.root',
    ),
)

process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.options = cms.untracked.PSet()
process.options.numberOfThreads=cms.untracked.uint32(8)
process.options.numberOfStreams=cms.untracked.uint32(0)

process.DQMoutput = cms.OutputModule("DQMRootOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('DQMIO'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('file:step3_inDQM.root'),
    outputCommands = process.DQMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

process.mix.digitizers = cms.PSet()
for a in process.aliases: delattr(process, a)
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

process.raw2digi_step = cms.Path(process.RawToDigi)
process.dqmoffline_step = cms.EndPath(process.DQMOfflineFakeHLT)
process.rpcDQM_step = cms.EndPath(process.rpcRecHitsDQM)
process.DQMoutput_step = cms.EndPath(process.DQMoutput)

process.schedule = cms.Schedule(
    process.raw2digi_step,
    #process.dqmoffline_step,
    process.rpcDQM_step,
    process.DQMoutput_step,
)

from SimGeneral.MixingModule.fullMixCustomize_cff import setCrossingFrameOn
process = setCrossingFrameOn(process)
#process.mix.playback = True
#process.RandomNumberGeneratorService.restoreStateLabel=cms.untracked.string("randomEngineStateProducer")
