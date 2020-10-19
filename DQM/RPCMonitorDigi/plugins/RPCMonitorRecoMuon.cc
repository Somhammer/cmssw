#include "DQM/RPCMonitorDigi/interface/RPCMonitorRecoMuon.h"
#include "DQM/RPCMonitorDigi/interface/RPCHistoHelper.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "RecoMuon/TransientTrackingRecHit/interface/MuonTransientTrackingRecHit.h"
#include <TMath.h>

RPCMonitorRecoMuon::RPCMonitorRecoMuon(const edm::ParameterSet& pset):
  muPtCut_(pset.getUntrackedParameter<double>("MuonPtCut", 3.0)),
  muEtaCut_(pset.getUntrackedParameter<double>("MuonEtaCut", 2.1))
{
  subsystemFolder_ = pset.getUntrackedParameter<std::string>("RPCFolder", "RPC");
  muonFolder_ = pset.getUntrackedParameter<std::string>("MuonFolder", "RecoMuon");

  muonLabel_ = consumes<reco::CandidateView>(pset.getParameter<edm::InputTag>("MuonLabel"));
  scalersRawToDigiLabel_ = consumes<DcsStatusCollection>(pset.getParameter<edm::InputTag>("ScalersRawToDigiLabel"));
}

void RPCMonitorRecoMuon::bookHistograms(DQMStore::IBooker& ibooker,
                                          edm::Run const& r,
                                          edm::EventSetup const& iSetup)
{
  ibooker.setCurrentFolder(subsystemFolder_+"/"+muonFolder_);

  hNMuon_ = ibooker.book1D("NMuon", "Number of muons;Number of muons;Events", 10, 0, 10);
  hNMuon_noHit_ = ibooker.book1D("NMuonNoHit", "Number of muons without RPCRecHits;Number of muons;Events", 10, 0, 10);
  hMuonEta_ = ibooker.book1D("MuonEta", "Muon #eta;Pseudorapidity #eta;Entries", 100, -2.5, 2.5);

  hMuonPt_B_  = ibooker.book1D("MuonPt_B" , "Muon p_{T} in Barrel;p_{T} (GeV);Entries" , 100, int(muPtCut_), 100);
  hMuonPt_EP_ = ibooker.book1D("MuonPt_EP", "Muon p_{T} in Endcap+;p_{T} (GeV);Entries", 100, int(muPtCut_), 100);
  hMuonPt_EM_ = ibooker.book1D("MuonPt_EM", "Muon p_{T} in Endcap-;p_{T} (GeV);Entries", 100, int(muPtCut_), 100);

  const double pi = TMath::Pi();
  hMuonPhi_B_  = ibooker.book1D("MuonPhi_B" , "Muon #phi in Barrel;Azimuthal angle #phi;Entries" , 100, -pi, pi);
  hMuonPhi_EP_ = ibooker.book1D("MuonPhi_EP", "Muon #phi in Endcap+;Azimuthal angle #phi;Entries", 100, -pi, pi);
  hMuonPhi_EM_ = ibooker.book1D("MuonPhi_EM", "Muon #phi in Endcap-;Azimuthal angle #phi;Entries", 100, -pi, pi);

  hNRPCRecHitMuon_ = ibooker.book1D("NRPCRecHitInMuon", "Number of RPCRecHits per Muon;Number of RPCRecHits;Events", 20, 0, 20);
  hNRPCRecHitMuon_MuonEta_ = ibooker.book2D("RPCRecHitInMuon_MuonEta", "Number Of RPCRecHits per Muon vs Eta;#eta;nRPCRecHits", 100, -2.5, 2.5, 7, 0.5, 7.5);

  for (int i = 0; i < 6; i++) {
    hMuonPt_nHit_[i] =  ibooker.book1D(RPCHistoHelper::joinStrInt("MuonPt_minNHit", i+1),
                                      RPCHistoHelper::joinStrInt("Muon p_{T} at least ", i+1, " RPCRecHits;Muon p_{T} (GeV);Entries"),
                                      100, 0, 100);
    hMuonEta_nHit_[i] =  ibooker.book1D(RPCHistoHelper::joinStrInt("MuonEta_minNHit", i+1, ""),
                                       RPCHistoHelper::joinStrInt("Muon #eta at least ", i+1, " RPCRecHits;Muon pseudorapidity #eta;Entries"),
                                       100, -2.5, 2.5);
    hMuonPhi_nHit_[i] =  ibooker.book1D(RPCHistoHelper::joinStrInt("MuonPhi_minNHit", i+1, ""),
                                       RPCHistoHelper::joinStrInt("Muon #phi at least ", i+1, " RPCRecHits;Muon azimuthal angle #phi;Entries"),
                                       100, -pi, pi);
  }
  hMuonPt_noHit_  = ibooker.book1D("MuonPt_noHit" , "Muon p_{T} without RPCRecHit;Muon p_{T} (GeV);Entries", 100, int(muPtCut_), 100);
  hMuonEta_noHit_ = ibooker.book1D("MuonEta_noHit", "Muon #eta without RPCRecHit;Muon pseudorapidity #eta;Entries", 100, -2.5, 2.5);
  hMuonPhi_noHit_ = ibooker.book1D("MuonPhi_noHit", "Muon #phi without RPCRecHit;Muon azimuthal angle #phi;Entries", 100, -pi, pi);

}

void RPCMonitorRecoMuon::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
  //Check RPC HV status
  edm::Handle<DcsStatusCollection> dcsStatusHandle;
  if ( event.getByToken(scalersRawToDigiLabel_, dcsStatusHandle) and  dcsStatusHandle.isValid() ) {
    for ( auto& dcs : *dcsStatusHandle ) {
      //if RPC not ON there's no need to continue
      if (!dcs.ready(DcsStatus::RPC)) return;
    }
  }

  //Muons
  edm::Handle<reco::CandidateView> muonHandle;
  event.getByToken(muonLabel_, muonHandle);
  if ( !muonHandle.isValid() ) return;

  int nMuon = 0, nMuonNoRPC = 0;
  for ( auto& cand : *muonHandle ) {
    const reco::Muon* muCand = dynamic_cast<const reco::Muon*>(&cand);
    if ( !muCand ) continue;

    if ( !muCand->isGlobalMuon() ) continue;
    const double eta = muCand->eta();
    const double pt = muCand->pt();
    if (pt < muPtCut_ or std::abs(eta) > muEtaCut_ ) continue;
    ++nMuon;
    const double phi = muCand->phi();

    hMuonEta_->Fill(eta);
    if ( eta > 0.8 ) {
      hMuonPt_EP_->Fill(pt);
      hMuonPhi_EP_->Fill(phi);
    }
    else if ( eta < -0.8 ) {
      hMuonPt_EM_->Fill(pt);
      hMuonPhi_EM_->Fill(phi);
    }
    else {
      hMuonPt_B_->Fill(pt);
      hMuonPhi_B_->Fill(phi);
    }

    int nRPCInTrack = 0;
    const reco::TrackRef muStaTrack = muCand->outerTrack();
    for ( auto hit = muStaTrack->recHitsBegin(); hit != muStaTrack->recHitsEnd(); ++hit ) {
      if ( !(*hit)->isValid() ) continue;
      if ( (*hit)->geographicalId().subdetId() == MuonSubdetId::RPC ) ++nRPCInTrack;
    }

    hNRPCRecHitMuon_->Fill(nRPCInTrack);
    hNRPCRecHitMuon_MuonEta_->Fill(eta, nRPCInTrack);
    for ( int nHit=1; nHit<=6; ++nHit ) {
      // fill pt,eta,phi of muon, at least N+1 RPCRecHits
      // e.g) hMuonPt_nHit[0] : fill muons pt at least 1 RPCRecHit.
      // e.g) hMuonPt_nHit[2] : fill muons pt at least 3 RPCRecHit.
      if ( nRPCInTrack < nHit ) break;
      hMuonPt_nHit_[nHit-1]->Fill(pt);
      hMuonEta_nHit_[nHit-1]->Fill(eta);
      hMuonPhi_nHit_[nHit-1]->Fill(phi);
    }
    if ( nRPCInTrack == 0 ) {
      ++nMuonNoRPC;
      hMuonPt_noHit_->Fill(pt);
      hMuonEta_noHit_->Fill(eta);
      hMuonPhi_noHit_->Fill(phi);
    }
  }

  hNMuon_->Fill(nMuon);
  hNMuon_noHit_->Fill(nMuonNoRPC);
}

