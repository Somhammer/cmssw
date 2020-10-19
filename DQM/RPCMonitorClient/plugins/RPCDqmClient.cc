// Package:    RPCDqmClient
// Original Author:  Anna Cimmino

#include "DQM/RPCMonitorClient/interface/RPCDqmClient.h"
#include "DQM/RPCMonitorDigi/interface/RPCBookFolderStructure.h"
#include "DQM/RPCMonitorDigi/interface/utils.h"
//include client headers
#include "DQM/RPCMonitorClient/interface/RPCDeadChannelTest.h"
#include "DQM/RPCMonitorClient/interface/RPCMultiplicityTest.h"
#include "DQM/RPCMonitorClient/interface/RPCClusterSizeTest.h"
#include "DQM/RPCMonitorClient/interface/RPCOccupancyTest.h"
#include "DQM/RPCMonitorClient/interface/RPCNoisyStripTest.h"
//Geometry
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeomServ.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
//Framework
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <FWCore/Framework/interface/ESHandle.h>

RPCDqmClient::RPCDqmClient(const edm::ParameterSet& pset)
{
  edm::LogVerbatim("rpcdqmclient") << "[RPCDqmClient]: Constructor";

  offlineDQM_ = pset.getUntrackedParameter<bool>("OfflineDQM", true);
  useRollInfo_ = pset.getUntrackedParameter<bool>("UseRollInfo", false);
  //check enabling
  enableDQMClients_ = pset.getUntrackedParameter<bool>("EnableRPCDqmClient", true);
  minimumEvents_ = pset.getUntrackedParameter<int>("MinimumRPCEvents", 10000);

  std::string subsystemFolder = pset.getUntrackedParameter<std::string>("RPCFolder", "RPC");
  std::string recHitTypeFolder = pset.getUntrackedParameter<std::string>("RecHitTypeFolder", "AllHits");
  std::string summaryFolder = pset.getUntrackedParameter<std::string>("SummaryFolder", "SummaryHistograms");

  prefixDir_ = subsystemFolder + "/" + recHitTypeFolder;
  globalFolder_ = subsystemFolder + "/" + recHitTypeFolder + "/" + summaryFolder;

  //get prescale factor
  prescaleGlobalFactor_ = pset.getUntrackedParameter<int>("DiagnosticGlobalPrescale", 5);

  //make default client list
  clientList_.push_back("RPCMultiplicityTest");
  clientList_.push_back("RPCDeadChannelTest");
  clientList_.push_back("RPCClusterSizeTest");
  clientList_ = pset.getUntrackedParameter<std::vector<std::string> >("RPCDqmClientList", clientList_);

  //get all the possible RPC DQM clients
  this->makeClientMap(pset);

  //clear counters
  lumiCounter_ = 0;
}

void RPCDqmClient::beginJob()
{
  if (!enableDQMClients_) return;

  //Do whatever the begin jobs of all client modules do
  for ( auto& module : clientModules_ ) {
    module->beginJob(globalFolder_);
  }
}

void RPCDqmClient::dqmEndLuminosityBlock(DQMStore::IBooker& ibooker,
                                         DQMStore::IGetter& igetter,
                                         edm::LuminosityBlock const& lumiSeg,
                                         edm::EventSetup const& c) {
  if (!enableDQMClients_) return;
  edm::LogVerbatim("rpcdqmclient") << "[RPCDqmClient]: End DQM LB";

  if (myDetIds_.empty()) {
    //Get RPCdetId...
    getRPCdetId(c);
    //...book summary histograms
    for ( auto& m : clientModules_ ) m->myBooker(ibooker);
  }

  if (!offlineDQM_) {  //Do this only for the online

    if (lumiCounter_ == 0) {  //only for the first lumi section do this...
      // ...get chamber based histograms and pass them to the client modules
      this->getMonitorElements(igetter);
    }

    //Do not perform client oparations every lumi block
    lumiCounter_++;
    if (lumiCounter_ % prescaleGlobalFactor_ != 0) {
      return;
    }

    //Check if there's enough statistics
    float rpcevents = minimumEvents_;
    if (RPCEvents_) {
      rpcevents = RPCEvents_->getBinContent(1);
    }
    if (rpcevents < minimumEvents_) {
      return;
    }

    edm::LogVerbatim("rpcdqmclient") << "[RPCDqmClient]: Client operations";
    for ( auto& m : clientModules_ ) m->clientOperation();
  }  //end of online operations
}

void RPCDqmClient::dqmEndJob(DQMStore::IBooker& ibooker, DQMStore::IGetter& igetter) {
  if (!enableDQMClients_) return;

  edm::LogVerbatim("rpcdqmclient") << "[RPCDqmClient]: End DQM Job";

  // ...get chamber based histograms and pass them to the client modules
  if (offlineDQM_) getMonitorElements(igetter);

  float rpcevents = minimumEvents_;
  if (RPCEvents_) rpcevents = RPCEvents_->getBinContent(1);
  if (rpcevents < minimumEvents_) return;

  edm::LogVerbatim("rpcdqmclient") << "[RPCDqmClient]: Client operations";
  for ( auto& m: clientModules_ ) m->clientOperation();
}

void RPCDqmClient::getMonitorElements(DQMStore::IGetter& igetter)
{
  std::vector<MonitorElement*> myMeVect;
  std::vector<RPCDetId> myDetIds;

  //dbe_->setCurrentFolder(prefixDir_);
  std::unique_ptr<RPCBookFolderStructure> folderStr(new RPCBookFolderStructure());
  MonitorElement* myMe = nullptr;
  std::string rollName = "";

  //loop on all geometry and get all histos
  for (auto& detId : myDetIds_) {
    //Get name
    RPCGeomServ RPCname(detId);
    rollName = useRollInfo_ ? RPCname.name() : RPCname.chambername();

    //loop on clients
    for (unsigned int cl = 0, nCL = clientModules_.size(); cl < nCL; ++cl) {
      myMe = igetter.get(prefixDir_ + "/" + folderStr->folderStructure(detId) + "/" + clientHisto_[cl] + "_" + rollName);
      if (!myMe) continue;

      //	   dbe_->tag(myMe, clientTag_[cl]);
      myMeVect.push_back(myMe);
      myDetIds.push_back(detId);
    }  //end loop on clients
  }    //end loop on all geometry and get all histos

  RPCEvents_ = igetter.get(prefixDir_ + "/RPCEvents");
  for (unsigned int cl = 0; cl < clientModules_.size(); ++cl) {
    clientModules_[cl]->getMonitorElements(myMeVect, myDetIds, clientHisto_[cl]);
  }
}

void RPCDqmClient::getRPCdetId(const edm::EventSetup& eventSetup)
{
  myDetIds_.clear();

  edm::ESHandle<RPCGeometry> rpcGeo;
  eventSetup.get<MuonGeometryRecord>().get(rpcGeo);

  for (auto& det : rpcGeo->dets()) {
    const RPCChamber* ch = dynamic_cast<const RPCChamber*>(det);
    if (!ch) continue;

    //Loop on rolls in given chamber
    for (auto& r : ch->rolls()) {
      RPCDetId detId = r->id();
      myDetIds_.push_back(detId);
    }
  }
}

void RPCDqmClient::makeClientMap(const edm::ParameterSet& pset) 
{
  for (unsigned int i = 0; i < clientList_.size(); i++) {
    if (clientList_[i] == "RPCMultiplicityTest") {
      clientHisto_.push_back("Multiplicity");
      // clientTag_.push_back(rpcdqm::MULTIPLICITY);
      clientModules_.emplace_back(new RPCMultiplicityTest(pset));
    } else if (clientList_[i] == "RPCDeadChannelTest") {
      clientHisto_.push_back("Occupancy");
      clientModules_.emplace_back(new RPCDeadChannelTest(pset));
      // clientTag_.push_back(rpcdqm::OCCUPANCY);
    } else if (clientList_[i] == "RPCClusterSizeTest") {
      clientHisto_.push_back("ClusterSize");
      clientModules_.emplace_back(new RPCClusterSizeTest(pset));
      // clientTag_.push_back(rpcdqm::CLUSTERSIZE);
    } else if (clientList_[i] == "RPCOccupancyTest") {
      clientHisto_.push_back("Occupancy");
      clientModules_.emplace_back(new RPCOccupancyTest(pset));
      // clientTag_.push_back(rpcdqm::OCCUPANCY);
    } else if (clientList_[i] == "RPCNoisyStripTest") {
      clientHisto_.push_back("Occupancy");
      clientModules_.emplace_back(new RPCNoisyStripTest(pset));
      //clientTag_.push_back(rpcdqm::OCCUPANCY);
    }
  }

  return;
}
