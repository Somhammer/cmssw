#ifndef RPCMonitorRecHits_h
#define RPCMonitorRecHits_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"

#include <string>

class RPCMonitorRecHits : public DQMEDAnalyzer
{
public:
  RPCMonitorRecHits(const edm::ParameterSet &);
  ~RPCMonitorRecHits() override = default;

private:
  void analyze(const edm::Event &, const edm::EventSetup &) override;
  void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;

  std::vector<float> to_vfloat(const std::vector<double>& v) const;

private:
  edm::ESHandle<RPCGeometry> rpcGeomHandle_;

  edm::EDGetTokenT<RPCRecHitCollection> rpcRecHitsToken_;
  const std::string subFolderName_;
  const bool doFillRollCenter_, doSetAxisLabel_;
  std::map<int, std::vector<float>> binsPhi_RB_;
  std::vector<float> binsPhi_RE_;
  std::map<int, std::vector<float>> binsZ_RB_, binsR_RE_;
  std::map<int, std::vector<std::string>> labelsZ_RB_, labelsPhi_RB_, labelsR_RE_;
  std::vector<std::string> labelsPhi_RE_;

  MonitorElement* hEvents_;
  typedef std::map<long long, MonitorElement*> DetId2ME;
  DetId2ME hBx_byDetId_, hCls_byDetId_;
  DetId2ME hNHits_byDetId_;

  std::map<int, MonitorElement*> hNHitsXY_byLayer_, hNHitsXY_byDisk_;
};

#endif

