#include "DQM/RPCMonitorDigi/interface/RPCMonitorRecHits.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
//#include "Geometry/RPCGeometry/interface/RPCGeomServ.h"
#include "DQM/RPCMonitorDigi/interface/RPCRollNameHelper.h"
#include "DQM/RPCMonitorDigi/interface/RPCHistoHelper.h"

RPCMonitorRecHits::RPCMonitorRecHits(const edm::ParameterSet& pset):
  rpcRecHitsToken_(consumes<RPCRecHitCollection>(pset.getParameter<edm::InputTag>("rpcRecHits"))),
  subFolderName_(pset.getUntrackedParameter<std::string>("subFolder", "AllHits")),
  doFillRollCenter_(pset.getUntrackedParameter<bool>("fillRollCenter", true)),
  doSetAxisLabel_(pset.getUntrackedParameter<bool>("setAxisLabel", true))
{
  typedef std::vector<double> vdouble;
  typedef std::vector<std::string> vstring;

  if ( doFillRollCenter_ ) {
    const edm::ParameterSet binEdgesPhi = pset.getParameter<edm::ParameterSet>("binEdgesPhi");
    binsPhi_RB_[11] = to_vfloat(binEdgesPhi.getUntrackedParameter<vdouble>("RB1in" ));
    binsPhi_RB_[12] = to_vfloat(binEdgesPhi.getUntrackedParameter<vdouble>("RB1out"));
    binsPhi_RB_[21] = to_vfloat(binEdgesPhi.getUntrackedParameter<vdouble>("RB2in" ));
    binsPhi_RB_[22] = to_vfloat(binEdgesPhi.getUntrackedParameter<vdouble>("RB2out"));
    binsPhi_RB_[31] = to_vfloat(binEdgesPhi.getUntrackedParameter<vdouble>("RB3"   ));
    binsPhi_RB_[41] = to_vfloat(binEdgesPhi.getUntrackedParameter<vdouble>("RB4"   ));
    binsPhi_RE_     = to_vfloat(binEdgesPhi.getUntrackedParameter<vdouble>("RE"    ));
  }
  else {
    const int nbinsPhi = pset.getUntrackedParameter<double>("nbinsPhi", 100);
    std::vector<float> binsPhi;
    binsPhi.reserve(nbinsPhi+1);
    const double dphi = 2*3.141592/nbinsPhi;
    for ( int i=0; i<=nbinsPhi; ++i ) binsPhi.push_back(binEdge0PhiBarrel_ + dphi*i);
    binsPhi_RB_[11] = binsPhi;
    binsPhi_RB_[12] = binsPhi;
    binsPhi_RB_[21] = binsPhi;
    binsPhi_RB_[12] = binsPhi;
    binsPhi_RB_[31] = binsPhi;
    binsPhi_RB_[41] = binsPhi;

    binsPhi.clear();
    for ( int i=0; i<=nbinsPhi; ++i ) binsPhi.push_back(binEdge0PhiEndcap_ + dphi*i);
    binsPhi_RE_ = binsPhi;
  }

  const edm::ParameterSet binEdgesZ = pset.getParameter<edm::ParameterSet>("binEdgesZ");
  binsZ_RB_[11] = to_vfloat(binEdgesZ.getUntrackedParameter<vdouble>("RB1in" ));
  binsZ_RB_[12] = to_vfloat(binEdgesZ.getUntrackedParameter<vdouble>("RB1out"));
  binsZ_RB_[21] = to_vfloat(binEdgesZ.getUntrackedParameter<vdouble>("RB2in" ));
  binsZ_RB_[22] = to_vfloat(binEdgesZ.getUntrackedParameter<vdouble>("RB2out"));
  binsZ_RB_[31] = to_vfloat(binEdgesZ.getUntrackedParameter<vdouble>("RB3"   ));
  binsZ_RB_[41] = to_vfloat(binEdgesZ.getUntrackedParameter<vdouble>("RB4"   ));

  const edm::ParameterSet binEdgesR = pset.getParameter<edm::ParameterSet>("binEdgesR");
  binsR_RE_[1] = to_vfloat(binEdgesR.getUntrackedParameter<vdouble>("RE1"));
  binsR_RE_[2] = to_vfloat(binEdgesR.getUntrackedParameter<vdouble>("RE2"));
  binsR_RE_[3] = to_vfloat(binEdgesR.getUntrackedParameter<vdouble>("RE3"));
  binsR_RE_[4] = to_vfloat(binEdgesR.getUntrackedParameter<vdouble>("RE4"));

  if ( doSetAxisLabel_ ) {
    const edm::ParameterSet binLabelsZ = pset.getParameter<edm::ParameterSet>("binLabelsZ");
    labelsZ_RB_[11] = binLabelsZ.getUntrackedParameter<vstring>("RB");
    labelsZ_RB_[12] = binLabelsZ.getUntrackedParameter<vstring>("RB");
    labelsZ_RB_[21] = binLabelsZ.getUntrackedParameter<vstring>("RB2in");
    labelsZ_RB_[22] = binLabelsZ.getUntrackedParameter<vstring>("RB2out");
    labelsZ_RB_[31] = binLabelsZ.getUntrackedParameter<vstring>("RB");
    labelsZ_RB_[41] = binLabelsZ.getUntrackedParameter<vstring>("RB");

    const edm::ParameterSet binLabelsR = pset.getParameter<edm::ParameterSet>("binLabelsR");
    labelsR_RE_[1] = binLabelsR.getUntrackedParameter<vstring>("RE12");
    labelsR_RE_[2] = binLabelsR.getUntrackedParameter<vstring>("RE12");
    labelsR_RE_[3] = binLabelsR.getUntrackedParameter<vstring>("RE34");
    labelsR_RE_[4] = binLabelsR.getUntrackedParameter<vstring>("RE34");

    if ( doFillRollCenter_ ) {
      const edm::ParameterSet binLabelsPhi = pset.getParameter<edm::ParameterSet>("binLabelsPhi");
      labelsPhi_RB_[11] = binLabelsPhi.getUntrackedParameter<vstring>("RB");
      labelsPhi_RB_[12] = binLabelsPhi.getUntrackedParameter<vstring>("RB");
      labelsPhi_RB_[21] = binLabelsPhi.getUntrackedParameter<vstring>("RB");
      labelsPhi_RB_[22] = binLabelsPhi.getUntrackedParameter<vstring>("RB");
      labelsPhi_RB_[31] = binLabelsPhi.getUntrackedParameter<vstring>("RB3");
      labelsPhi_RB_[41] = binLabelsPhi.getUntrackedParameter<vstring>("RB4");
    }
  }
}

void RPCMonitorRecHits::bookHistograms(DQMStore::IBooker& ibooker, edm::Run const& r, edm::EventSetup const& setup)
{
  setup.get<MuonGeometryRecord>().get(rpcGeomHandle_);
  const RPCGeometry* rpcGeom = rpcGeomHandle_.product();

  ibooker.cd();
  ibooker.setCurrentFolder("RPC/"+subFolderName_);
  hEvents_ = ibooker.book1D("RPCEvents", "RPCEvents", 2, 0, 2);
  hEvents_->setBinLabel(1, "AllEvents", 1);
  hEvents_->setBinLabel(2, "RPCEvents", 1);

  for ( auto chamber : rpcGeom->chambers() ) {
    const RPCDetId chId(chamber->id());
    const std::string chamberName = RPCRollNameHelper::chamberName(chId);

    int nstrip = 1;
    std::vector<std::string> rollNames;
    for ( auto roll : chamber->rolls() ) {
      nstrip = std::max(nstrip, roll->nstrips());
      rollNames.push_back(RPCRollNameHelper::rollName(roll->id()));
    }
    if ( rollNames.empty() ) continue;

    const std::string subdir = RPCHistoHelper::suggestPath(chId);
    ibooker.setCurrentFolder("RPC/"+subFolderName_+"/"+subdir);

    hBx_byDetId_[chId] = ibooker.book1D("BunchCrossing_"+chamberName, "Bunch crossing "+chamberName, 11, -5.5, 5.5);
    hCls_byDetId_[chId] = ibooker.book1D("ClusterSize_"+chamberName, "Cluster size "+chamberName, 14, 1, 15);
    hClsBx0_byDetId_[chId] = ibooker.book1D("ClusterSizeAtBx0_"+chamberName, "Cluster size at Bx0"+chamberName, 14, 1, 15);

    const int nRolls = rollNames.size();
    hOccup_byDetId_[chId] = ibooker.book2D("Occupancy_"+chamberName, "Occupancy "+chamberName, nstrip, 1, nstrip+1, nRolls, 1, nRolls+1);
    for ( int i=0; i<nRolls; ++i ) {
      const std::string label = rollNames[i];//.substr(chamberName.size());
      hOccup_byDetId_[chId]->setBinLabel(i+1, label, 2);
    }
  }

  ibooker.setCurrentFolder("RPC/"+subFolderName_);

  // Book XY-views for barrel layers
  const std::map<int, const std::string> layerNames = {{
    {11,"RB1in"}, {12,"RB1out"}, {21,"RB2in"}, {22,"RB2out"}, {31,"RB3"}, {41,"RB4"}
  }};
  for ( auto key : layerNames ) {
    const int s = key.first;
    const std::string name = "Occupancy_"+key.second;
    const std::string title = "Occupancy "+key.second+";z(cm);#phi";
    hOccupXY_byLayer_[s] = ibooker.book2D(name, title, binsZ_RB_[s].size()-1, &(binsZ_RB_[s][0]),
                                                       binsPhi_RB_[s].size()-1, &(binsPhi_RB_[s][0]));

    if ( doSetAxisLabel_ ) {
      const auto& labelsZ = labelsZ_RB_[s];
      for ( size_t i=0; i<labelsZ.size(); ++i ) {
        hOccupXY_byLayer_[s]->setBinLabel(i+1, labelsZ[i], 1);
      }
      if ( doFillRollCenter_ ) {
        const auto& labelsPhi = labelsPhi_RB_[s];
        for ( size_t i=0; i<labelsPhi.size(); ++i ) {
          hOccupXY_byLayer_[s]->setBinLabel(i+1, labelsPhi[i], 2);
        }
      }
    }
  }

  // Book XY-views for endcap layers
  const std::map<int, const std::string> stationNames = {{
    {-4,"RE-4"}, {-3,"RE-3"}, {-2,"RE-2"}, {-1,"RE-1"},
    {+4,"RE+4"}, {+3,"RE+3"}, {+2,"RE+2"}, {+1,"RE+1"}
  }};
  for ( auto key : stationNames ) {
    const int disk = key.first;
    const int d = abs(disk);
    const std::string name = "Occupancy_"+key.second;
    const std::string title = "Occupancy "+key.second+";#phi;R (cm)";
    hOccupXY_byDisk_[disk] = ibooker.book2D(name, title, binsPhi_RE_.size()-1, &(binsPhi_RE_[0]),
                                                         binsR_RE_[d].size()-1, &(binsR_RE_[d][0]));
    if ( doSetAxisLabel_ ) {
      const auto& labelsR = labelsR_RE_[d];
      for ( size_t i=0; i<labelsR.size(); ++i ) {
        hOccupXY_byDisk_[disk]->setBinLabel(i+1, labelsR[i], 2);
      }
    }
  }
}

void RPCMonitorRecHits::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
  if ( !rpcGeomHandle_.isValid() ) setup.get<MuonGeometryRecord>().get(rpcGeomHandle_);
  const RPCGeometry* rpcGeom = rpcGeomHandle_.product();
  hEvents_->Fill(0);

  edm::Handle<RPCRecHitCollection> rpcRecHitsHandle;
  event.getByToken(rpcRecHitsToken_, rpcRecHitsHandle);
  if ( rpcRecHitsHandle.isValid() ) {
    if ( rpcRecHitsHandle->size() == 0 ) hEvents_->Fill(1);

    const LocalPoint lp0(0,0,0);
    for ( auto hitItr = rpcRecHitsHandle->begin(); hitItr != rpcRecHitsHandle->end(); ++hitItr ) {
      const int bx = hitItr->BunchX();
      const int cls = hitItr->clusterSize();
      const int strip1 = hitItr->firstClusterStrip();
      const int strip2 = strip1+cls-1;

      const auto rpcId = hitItr->rpcId();
      const auto roll = rpcGeom->roll(rpcId);
      const long long chId = rpcId.chamberId();

      const int binRoll = RPCHistoHelper::findRollIndex(rpcId);

      hBx_byDetId_[chId]->Fill(bx);
      hCls_byDetId_[chId]->Fill(cls);
      for ( int strip = strip1; strip <= strip2; ++strip ) {
        hOccup_byDetId_[chId]->Fill(strip, binRoll);
      }

      if ( bx == 0 ) hClsBx0_byDetId_[chId]->Fill(cls);

      if ( rpcId.region() == 0 ) {
        const int stla = rpcId.station()*10 + rpcId.layer();
        const auto gPos = roll->toGlobal(doFillRollCenter_ ? lp0 : hitItr->localPosition());
        const double phi = gPos.barePhi();
        hOccupXY_byLayer_[stla]->Fill(gPos.z(), (phi >= binEdge0PhiBarrel_ ? phi : phi+2*3.141592));
      }
      else {
        const int disk = rpcId.region()*rpcId.station();
        const auto gPos = roll->toGlobal(doFillRollCenter_ ? lp0 : hitItr->localPosition());
        //hOccupXY_byDisk_[disk]->Fill(gPos.x(), gPos.y());
        const double phi = gPos.barePhi();
        hOccupXY_byDisk_[disk]->Fill((phi >= binEdge0PhiEndcap_ ? phi : phi+2*3.141592), gPos.perp());
      }
    }
  }
}

std::vector<float> RPCMonitorRecHits::to_vfloat(const std::vector<double>& from) const
{
  std::vector<float> ret(from.begin(), from.end());
  return ret;
}

