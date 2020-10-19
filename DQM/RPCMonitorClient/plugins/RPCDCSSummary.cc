#include "DQM/RPCMonitorClient/interface/RPCDCSSummary.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "CondFormats/RunInfo/interface/RunInfo.h"
#include "CondFormats/DataRecord/interface/RunSummaryRcd.h"

RPCDCSSummary::RPCDCSSummary(const edm::ParameterSet& ps):
  minFEDId_(ps.getUntrackedParameter<unsigned int>("MinimumRPCFEDId", 790)),
  maxFEDId_(ps.getUntrackedParameter<unsigned int>("MaximumRPCFEDId", 792)),
  nDisks_(ps.getUntrackedParameter<int>("NumberOfEndcapDisks", 4)),
  isOfflineDQM_(ps.getUntrackedParameter<bool>("OfflineDQM", true))
{
  isFilled_ = false;
  fracDCS_ = 1.;
}

void RPCDCSSummary::dqmEndLuminosityBlock(DQMStore::IBooker& ibooker,
                                          DQMStore::IGetter& igetter,
                                          edm::LuminosityBlock const& lumiB,
                                          edm::EventSetup const& setup)
{
  if ( isFilled_ ) return;

  checkDCSbit(setup);
  if (!isOfflineDQM_) myBooker(ibooker);
}

void RPCDCSSummary::dqmEndJob(DQMStore::IBooker& ibooker, DQMStore::IGetter& igetter)
{
  if (isOfflineDQM_) myBooker(ibooker);
}

void RPCDCSSummary::checkDCSbit(edm::EventSetup const& setup)
{
  fracDCS_ = 1.;

  if ( auto runInfoRec = setup.tryToGet<RunInfoRcd>() ) {
    fracDCS_ = -1.;
    //get fed summary information
    edm::ESHandle<RunInfo> sumFED;
    runInfoRec->get(sumFED);

    for ( const int fedID : sumFED->m_fed_in ) {
      //make sure fed id is in allowed range
      if ( fedID >= minFEDId_ and fedID <= maxFEDId_ ) {
        fracDCS_ = 1.;
        break;
      }
    }
  }

  isFilled_ = true;
}

void RPCDCSSummary::myBooker(DQMStore::IBooker& ibooker)
{
  ibooker.setCurrentFolder("RPC/EventInfo");
  // global fraction
  MonitorElement* totalDCSFraction = ibooker.bookFloat("DCSSummary");
  totalDCSFraction->Fill(fracDCS_);

  MonitorElement* meDCSMap = ibooker.book2D("DCSSummaryMap", "RPC DCS Summary Map", 15, -7.5, 7.5, 12, 0, 12);

  //customize the 2d histo
  std::stringstream binLabel;
  for (int i=1; i<=12; ++i) {
    binLabel.str("");
    binLabel << "Sec" << i;
    meDCSMap->setBinLabel(i, binLabel.str(), 2);
  }

  for (int i = -2; i <= 2; ++i) {
    binLabel.str("");
    binLabel << "Wheel" << i;
    meDCSMap->setBinLabel((i + 8), binLabel.str(), 1);
  }

  for (int i = 1; i <= nDisks_; ++i) {
    binLabel.str("");
    binLabel << "Disk" << i;
    meDCSMap->setBinLabel((i + 11), binLabel.str(), 1);
    binLabel.str("");
    binLabel << "Disk" << -i;
    meDCSMap->setBinLabel((-i + 5), binLabel.str(), 1);
  }

  //fill histogram bins with -1 to indicate invalid area
  for ( int i=1; i<=15; ++i ) {
    for (int j=1; j<=12; ++j) {
      meDCSMap->setBinContent(i, j, -1);
    }
  }

  //fill DCS fractions for valid cells
  //First go through the wheel
  for (int i=-2; i<=2; ++i) {
    for (int j=1; j<=12; ++j) {
      meDCSMap->setBinContent(i+8, j, fracDCS_);
    }
  }

  for (int i=1; i<nDisks_; ++i ) {
    for (int j=1; j<6; ++j ) {
      meDCSMap->setBinContent(11+i, j, fracDCS_); // Disk+1 to Disk+4
      meDCSMap->setBinContent( 5-i, j, fracDCS_); // Disk-1 to Disk-4
    }
  }

  // book the ME
  ibooker.setCurrentFolder("RPC/EventInfo/DCSContents");
  TH2F* h = meDCSMap->getTH2F();
  for ( int i=1; i<h->GetNbinsX(); ++i ) {
    const std::string label = h->GetXaxis()->GetBinLabel(i);
    if ( label.empty() ) continue;

    MonitorElement* me = ibooker.bookFloat("RPC_"+label);
    me->Fill(fracDCS_);
  }
}

