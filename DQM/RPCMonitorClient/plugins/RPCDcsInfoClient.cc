#include "DQM/RPCMonitorClient/interface/RPCDcsInfoClient.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"

RPCDcsInfoClient::RPCDcsInfoClient(const edm::ParameterSet& ps):
  dcsinfofolder_(ps.getUntrackedParameter<std::string>("dcsInfoFolder", "RPC/DCSInfo")),
  //eventinfofolder_(ps.getUntrackedParameter<std::string>("eventInfoFolder", "RPC/EventInfo")),
  dqmprovinfofolder_(ps.getUntrackedParameter<std::string>("dqmProvInfoFolder", "Info/EventInfo"))
{
}

void RPCDcsInfoClient::dqmEndJob(DQMStore::IBooker& ibooker, DQMStore::IGetter& igetter)
{
  // book
  ibooker.cd();
  ibooker.setCurrentFolder(dcsinfofolder_);

  MonitorElement* reportSummaryMap = igetter.get(dqmprovinfofolder_ + "/reportSummaryMap");
  //MonitorElement* lumiNumber = igetter.get(eventinfofolder_ + "/iLumiSection");
  if ( !reportSummaryMap ) return;
  TH2F* h2 = reportSummaryMap->getTH2F();
  if ( !h2 ) return;
  const int binRPC = h2->GetYaxis()->FindFixBin("RPC");
  if ( binRPC < 0 ) return;
  //const int nLS = lumiNumber->getIntValue();
  const int nLS = h2->GetNbinsX();

  MonitorElement* rpcHVStatus = ibooker.book2D("rpcHVStatus", "RPC HV Status", nLS, 1, nLS+1, 1, 0, 1);
  rpcHVStatus->setAxisTitle("Luminosity Section", 1);
  rpcHVStatus->setBinLabel(1, "", 2);

  int nHVOn = 0;
  for ( int i=1; i<=nLS; ++i ) {
    const int dcsBit = (h2->GetBinContent(i, binRPC) != -1) ? 1 : 0;
    if ( dcsBit == 1 ) ++nHVOn;
    rpcHVStatus->setBinContent(i, 1, dcsBit);
  }

  MonitorElement* rpcHV = ibooker.bookInt("rpcHV");
  rpcHV->Fill(nHVOn);

}
