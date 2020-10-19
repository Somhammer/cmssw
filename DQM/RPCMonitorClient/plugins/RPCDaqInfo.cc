#include "DQM/RPCMonitorClient/interface/RPCDaqInfo.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "CondFormats/RunInfo/interface/RunInfo.h"
#include "CondFormats/RunInfo/interface/RunSummary.h"
#include "CondFormats/DataRecord/interface/RunSummaryRcd.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

RPCDaqInfo::RPCDaqInfo(const edm::ParameterSet& ps):
  minFEDId_(ps.getUntrackedParameter<unsigned int>("MinimumRPCFEDId", 790)),
  maxFEDId_(ps.getUntrackedParameter<unsigned int>("MaximumRPCFEDId", 792))
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

  isBooked_ = true;
}
