#include "DQM/RPCMonitorDigi/interface/RPCMonitorRaw.h"
#include "DQM/RPCMonitorDigi/interface/RPCRawDataCountsHistoMaker.h"

#include "DQMServices/Core/interface/DQMStore.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/Event.h"

#include "CondFormats/RPCObjects/interface/LinkBoardElectronicIndex.h"
#include "DataFormats/RPCDigi/interface/ReadoutError.h"

#include "TH1F.h"
#include "TH2F.h"

RPCMonitorRaw::RPCMonitorRaw(const edm::ParameterSet& cfg)
{
  rpcRawDataCountsTag_ = consumes<RPCRawDataCounts>(cfg.getParameter<edm::InputTag>("rpcRawDataCountsTag"));

  for ( unsigned int i = 0; i < 10; ++i ) theWatchedErrorHistoPos[i] = 0;
  std::vector<int> algos = cfg.getUntrackedParameter<std::vector<int> >("watchedErrors");
  for ( auto it = algos.begin(); it != algos.end(); ++it ) {
    unsigned int ialgo = *it;
    if (ialgo < 10) theWatchedErrorHistoPos[ialgo] = 1;  // real position initialisain is in begin job. here mark just switched on.
  }
}

void RPCMonitorRaw::bookHistograms(DQMStore::IBooker& ibooker, edm::Run const&, edm::EventSetup const&)
{
  ibooker.cd();
  ibooker.setCurrentFolder("RPC/LinkMonitor");

  me_t[0] = ibooker.book1D("recordType_790", RPCRawDataCountsHistoMaker::emptyRecordTypeHisto(790));
  me_t[1] = ibooker.book1D("recordType_791", RPCRawDataCountsHistoMaker::emptyRecordTypeHisto(791));
  me_t[2] = ibooker.book1D("recordType_792", RPCRawDataCountsHistoMaker::emptyRecordTypeHisto(792));
  for (int i = 0; i < 3; ++i) me_t[i]->getTH1F()->SetStats(false);

  me_e[0] = ibooker.book1D("readoutErrors_790", RPCRawDataCountsHistoMaker::emptyReadoutErrorHisto(790));
  me_e[1] = ibooker.book1D("readoutErrors_791", RPCRawDataCountsHistoMaker::emptyReadoutErrorHisto(791));
  me_e[2] = ibooker.book1D("readoutErrors_792", RPCRawDataCountsHistoMaker::emptyReadoutErrorHisto(792));
  for (int i = 0; i < 3; ++i) me_e[i]->getTH1F()->SetStats(false);

  me_mapGoodEvents = ibooker.book2D("mapGoodRecords", "mapGoodRecords", 36, -0.5, 35.5, 3, 789.5, 792.5);
  me_mapGoodEvents->getTH2F()->SetNdivisions(3, "y");
  me_mapGoodEvents->setAxisTitle("rmb");
  me_mapGoodEvents->getTH2F()->SetYTitle("fed");
  me_mapGoodEvents->getTH2F()->SetStats(false);
  me_mapBadEvents = ibooker.book2D("mapErrorRecords", "mapErrorRecords", 36, -0.5, 35.5, 3, 789.5, 792.5);
  me_mapBadEvents->setAxisTitle("fed");
  me_mapBadEvents->getTH2F()->SetYTitle("rmb");
  me_mapBadEvents->getTH2F()->SetNdivisions(3, "y");
  me_mapBadEvents->getTH2F()->SetStats(false);

  for (unsigned int i = 0; i <= 9; ++i) {
    if ( !theWatchedErrorHistoPos[i] ) continue;
    for (unsigned int fed = 790; fed <= 792; ++fed) {
      TH2F* histo = RPCRawDataCountsHistoMaker::emptyReadoutErrorMapHisto(fed, i);
      MonitorElement* watched = ibooker.book2D(histo->GetName(), histo);
      theWatchedErrorHistos[fed - 790].push_back(watched);
      theWatchedErrorHistoPos[i] = theWatchedErrorHistos[fed - 790].size();
    }
  }
}

void RPCMonitorRaw::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
  edm::Handle<RPCRawDataCounts> rawCounts;
  ev.getByToken(rpcRawDataCountsTag_, rawCounts);
  const RPCRawDataCounts& counts = *rawCounts.product();

  // record type
  for ( auto t : counts.theRecordTypes ) me_t[t.first.first-790]->Fill(t.first.second, t.second);

  // good events topology
  for ( auto t : counts.theGoodEvents ) me_mapGoodEvents->Fill(t.first.second, t.first.first, t.second);

  // bad events topology
  for ( auto t : counts.theBadEvents ) me_mapBadEvents->Fill(t.first.second, t.first.first, t.second);

  // readout errors
  for ( auto t : counts.theReadoutErrors ) {
    rpcrawtodigi::ReadoutError error(t.first.second);
    LinkBoardElectronicIndex ele = error.where();
    rpcrawtodigi::ReadoutError::ReadoutErrorType type = error.type();

    const int fed = t.first.first;
    me_e[fed - 790]->Fill(type, t.second);

    // in addition fill location map for selected errors
    const int idx = theWatchedErrorHistoPos[type] - 1;
    if (idx < 0) continue;

    std::vector<MonitorElement*>& wh = theWatchedErrorHistos[fed - 790];
    MonitorElement* me = wh[idx];
    me->Fill(ele.dccInputChannelNum, ele.tbLinkInputNum, t.second);
  }

}
