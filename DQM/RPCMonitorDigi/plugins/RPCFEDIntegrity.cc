/*  \author Anna Cimmino*/
#include "DQM/RPCMonitorDigi/interface/RPCFEDIntegrity.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"

RPCFEDIntegrity::RPCFEDIntegrity(const edm::ParameterSet& ps):
  minFEDNum_(ps.getUntrackedParameter<int>("minimumFEDID", 790)),
  maxFEDNum_(ps.getUntrackedParameter<int>("maximumFEDID", 792)),
  numOfFED_(maxFEDNum_-minFEDNum_+1)
{
  rawCountsLabel_ = consumes<RPCRawDataCounts>(ps.getUntrackedParameter<edm::InputTag>("rawCountsInputTag"));
  prefixDir_ = ps.getUntrackedParameter<std::string>("prefixDir", "RPC/FEDIntegrity");

}

void RPCFEDIntegrity::bookHistograms(DQMStore::IBooker& ibooker, edm::Run const&, edm::EventSetup const&)
{
  ibooker.cd();
  ibooker.setCurrentFolder(prefixDir_);

  hEntries_ = ibooker.book1D("FEDEntries", "FED Entries", numOfFED_, minFEDNum_, maxFEDNum_ + 1);
  hFatal_ = ibooker.book1D("FEDFatal", "FED Fatal Errors", numOfFED_, minFEDNum_, maxFEDNum_ + 1);
  hNonFatal_ = ibooker.book1D("FEDNonFatal", "FED NON Fatal Errors", numOfFED_, minFEDNum_, maxFEDNum_ + 1);

  char buffer[5];
  for ( int fed=minFEDNum_; fed<=maxFEDNum_; ++fed ) {
    snprintf(buffer, 5, "%3d", fed);

    hEntries_ ->setBinLabel(fed-minFEDNum_+1, buffer, 1);
    hFatal_   ->setBinLabel(fed-minFEDNum_+1, buffer, 1);
    hNonFatal_->setBinLabel(fed-minFEDNum_+1, buffer, 1);
  }

}

void RPCFEDIntegrity::analyze(const edm::Event& iEvent, const edm::EventSetup&)
{
  //get hold of raw data counts
  edm::Handle<RPCRawDataCounts> rawCounts;
  iEvent.getByToken(rawCountsLabel_, rawCounts);
  if ( !rawCounts.isValid() ) return;

  for ( int fed=minFEDNum_; fed<=maxFEDNum_; ++fed ) {
    if ( rawCounts->fedBxRecords(fed)    ) hEntries_->Fill(fed);
    if ( rawCounts->fedFormatErrors(fed) ) hFatal_->Fill(fed);
    if ( rawCounts->fedErrorRecords(fed) ) hNonFatal_->Fill(fed);
  }
}

