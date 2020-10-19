#include "DQM/RPCMonitorClient/interface/RPCDaqInfo.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "CondFormats/RunInfo/interface/RunInfo.h"
#include "CondFormats/RunInfo/interface/RunSummary.h"
#include "CondFormats/DataRecord/interface/RunSummaryRcd.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

RPCDaqInfo::RPCDaqInfo(const edm::ParameterSet& ps):
  minFEDId_(ps.getUntrackedParameter<unsigned int>("MinimumRPCFEDId", 790)),
  maxFEDId_(ps.getUntrackedParameter<unsigned int>("MaximumRPCFEDId", 792)),
  nDisks_(ps.getUntrackedParameter<int>("NumberOfEndcapDisks", 4))
{
  isBooked_ = false;
}

void RPCDaqInfo::dqmEndLuminosityBlock(DQMStore::IBooker& ibooker,
                                       DQMStore::IGetter& igetter,
                                       edm::LuminosityBlock const&,
                                       edm::EventSetup const& iSetup)
{
  if ( !isBooked_ ) myBooker(ibooker);
  meDAQFraction_->Fill(-1);
  const int nFEDs = maxFEDId_-minFEDId_+1;
  if ( nFEDs < 1 ) return;

  if ( auto runInfoRec = iSetup.tryToGet<RunInfoRcd>() ) {
    //get fed summary information
    edm::ESHandle<RunInfo> sumFED;
    runInfoRec->get(sumFED);

    int nValidFEDs = 0;
    //loop on all active feds
    for ( const int fedId : sumFED->m_fed_in ) {
      if ( fedId >= minFEDId_ && fedId <= maxFEDId_ ) ++nValidFEDs;
    }

    //Fill active fed fraction ME
    meDAQFraction_->Fill(double(nValidFEDs)/nFEDs);
  }
}

void RPCDaqInfo::myBooker(DQMStore::IBooker& ibooker)
{
  //fraction of alive FEDs
  ibooker.setCurrentFolder("RPC/EventInfo/DAQContents");

  //daq summary for RPCs
  ibooker.setCurrentFolder("RPC/EventInfo");
  meDAQFraction_ = ibooker.bookFloat("DAQSummary");
  MonitorElement* meDAQMap = ibooker.book2D("DAQSummaryMap", "RPC DAQ Summary Map", 15, -7.5, 7.5, 12, 0, 12);

  //customize the 2d histo
  std::stringstream binLabel;
  for (int i=1; i<=12; ++i) {
    binLabel.str("");
    binLabel << "Sec" << i;
    meDAQMap->setBinLabel(i, binLabel.str(), 2);
  }

  for (int i = -2; i <= 2; ++i) {
    binLabel.str("");
    binLabel << "Wheel" << i;
    meDAQMap->setBinLabel((i + 8), binLabel.str(), 1);
  }

  for (int i = 1; i <= nDisks_; ++i) {
    binLabel.str("");
    binLabel << "Disk" << i;
    meDAQMap->setBinLabel((i + 11), binLabel.str(), 1);
    binLabel.str("");
    binLabel << "Disk" << -i;
    meDAQMap->setBinLabel((-i + 5), binLabel.str(), 1);
  }

  isBooked_ = true;
}
