#ifndef RPCMonitorRecoMuon_h
#define RPCMonitorRecoMuon_h

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
//DQMServices
#include <DQMServices/Core/interface/DQMEDAnalyzer.h>
#include "DQMServices/Core/interface/DQMStore.h"
///Data Format
#include "DataFormats/Scalers/interface/DcsStatus.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include <string>

class RPCMonitorRecoMuon : public DQMEDAnalyzer
{
public:
  RPCMonitorRecoMuon(const edm::ParameterSet &);
  ~RPCMonitorRecoMuon() override = default;

protected:
  void analyze(const edm::Event &, const edm::EventSetup &) override;
  void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;

private:
  const double muPtCut_, muEtaCut_;

  std::string subsystemFolder_;
  std::string globalFolder_;
  std::string muonFolder_;

  edm::EDGetTokenT<reco::CandidateView> muonLabel_;
  edm::EDGetTokenT<DcsStatusCollection> scalersRawToDigiLabel_;

  MonitorElement* hNMuon_, *hNMuon_noHit_;
  MonitorElement* hMuonEta_;
  MonitorElement* hMuonPt_EP_, * hMuonPt_EM_, * hMuonPt_B_;
  MonitorElement* hMuonPhi_EP_, * hMuonPhi_EM_, * hMuonPhi_B_;

  MonitorElement* hNRPCRecHitMuon_;
  MonitorElement* hNRPCRecHitMuon_MuonEta_;
  MonitorElement* hMuonPt_nHit_[6];
  MonitorElement* hMuonEta_nHit_[6];
  MonitorElement* hMuonPhi_nHit_[6];
  MonitorElement* hMuonPt_noHit_;
  MonitorElement* hMuonEta_noHit_;
  MonitorElement* hMuonPhi_noHit_;

};

#endif
