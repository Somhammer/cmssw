#include "DQM/RPCMonitorClient/interface/RPCDataCertification.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/RunInfo/interface/RunInfo.h"
#include "CondFormats/DataRecord/interface/RunSummaryRcd.h"

RPCDataCertification::RPCDataCertification(const edm::ParameterSet& ps):
  RPCSummaryMap(ps.getUntrackedParameter<int>("NumberOfEndcapDisks", 4)),
  minFEDId_(ps.getUntrackedParameter<unsigned int>("MinimumRPCFEDId", 790)),
  maxFEDId_(ps.getUntrackedParameter<unsigned int>("MaximumRPCFEDId", 792)),
  isOfflineDQM_(ps.getUntrackedParameter<bool>("OfflineDQM", true))
{
  isFEDChecked_ = false;
  fracCert_ = 1.;
}

void RPCDataCertification::dqmEndLuminosityBlock(DQMStore::IBooker& ibooker,
                                                 DQMStore::IGetter& igetter,
                                                 edm::LuminosityBlock const& LB,
                                                 edm::EventSetup const& setup)
{
  if ( isFEDChecked_ ) return;

  checkFED(setup);
  if (!isOfflineDQM_) myBooker(ibooker);
}

void RPCDataCertification::dqmEndJob(DQMStore::IBooker& ibooker, DQMStore::IGetter& igetter)
{
  if (isOfflineDQM_) myBooker(ibooker);
}

void RPCDataCertification::checkFED(edm::EventSetup const& setup)
{
  fracCert_ = 1.;

  if ( auto runInfoRec = setup.tryToGet<RunInfoRcd>() ) {
    fracCert_ = -1;
    //get fed summary information
    edm::ESHandle<RunInfo> sumFED;
    runInfoRec->get(sumFED);

    for ( const int fedID : sumFED->m_fed_in ) {
      //make sure fed id is in allowed range
      if ( fedID >= minFEDId_ and fedID <= maxFEDId_ ) {
        fracCert_ = 1;
        break;
      }
    }
  }

  isFEDChecked_ = true;
}

void RPCDataCertification::myBooker(DQMStore::IBooker& ibooker)
{
  ibooker.setCurrentFolder("RPC/EventInfo");
  // global fraction
  MonitorElement* totalCertFraction = ibooker.bookFloat("CertificationSummary");
  totalCertFraction->Fill(fracCert_);

  MonitorElement* meCertMap = bookSummaryMap(ibooker, "CertificationSummaryMap", "RPC Certification Summary Map");

  //fill fractions for valid cells
  //First go through the wheel
  for (int i=-2; i<=2; ++i) {
    for (int j=1; j<=12; ++j) {
      meCertMap->setBinContent(i+8, j, fracCert_);
    }
  }

  for (int i=1; i<nDisks_; ++i ) {
    for (int j=1; j<6; ++j ) {
      meCertMap->setBinContent(11+i, j, fracCert_); // Disk+1 to Disk+4
      meCertMap->setBinContent( 5-i, j, fracCert_); // Disk-1 to Disk-4
    }
  }

  // book the ME
  ibooker.setCurrentFolder("RPC/EventInfo/CertificationContents");
  TH2F* h = meCertMap->getTH2F();
  for ( int i=1; i<h->GetNbinsX(); ++i ) {
    const std::string label = h->GetXaxis()->GetBinLabel(i);
    if ( label.empty() ) continue;

    MonitorElement* me = ibooker.bookFloat("RPC_"+label);
    me->Fill(fracCert_);
  }
}
