/*  \author Anna Cimmino*/
//#include <cmath>
#include <sstream>
#include <DQM/RPCMonitorClient/interface/RPCOccupancyTest.h>
#include "DQM/RPCMonitorDigi/interface/utils.h"

// Framework
#include "FWCore/MessageLogger/interface/MessageLogger.h"
//Geometry
#include "Geometry/RPCGeometry/interface/RPCGeomServ.h"

RPCOccupancyTest::RPCOccupancyTest(const edm::ParameterSet& ps) {
  edm::LogVerbatim("rpceventsummary") << "[RPCOccupancyTest]: Constructor";

  prescaleFactor_ = ps.getUntrackedParameter<int>("DiagnosticPrescale", 1);
  numberOfDisks_ = ps.getUntrackedParameter<int>("NumberOfEndcapDisks", 4);
  numberOfRings_ = ps.getUntrackedParameter<int>("NumberOfEndcapRings", 2);
  useNormalization_ = ps.getUntrackedParameter<bool>("testMode", true);
  useRollInfo_ = ps.getUntrackedParameter<bool>("useRollInfo_", true);

  std::string subsystemFolder = ps.getUntrackedParameter<std::string>("RPCFolder", "RPC");
  std::string recHitTypeFolder = ps.getUntrackedParameter<std::string>("RecHitTypeFolder", "AllHits");

  prefixDir_ = subsystemFolder + "/" + recHitTypeFolder;
}

RPCOccupancyTest::~RPCOccupancyTest() {}

void RPCOccupancyTest::beginJob(std::string& workingFolder) {
  edm::LogVerbatim("rpceventsummary") << "[RPCOccupancyTest]: Begin job ";
  globalFolder_ = workingFolder;

  totalStrips_ = 0.;
  totalActive_ = 0.;
}

void RPCOccupancyTest::getMonitorElements(std::vector<MonitorElement*>& meVector, std::vector<RPCDetId>& detIdVector, std::string& clientHistoName) {
  //Get NumberOfDigi ME for each roll
  for (unsigned int i = 0; i < meVector.size(); i++) {
    std::string meName = meVector[i]->getName();
    std::cout << "Roll Name: " << meName << std::endl;

    if (meName.find(clientHistoName) != std::string::npos) {
      myOccupancyMe_.push_back(meVector[i]);
      myDetIds_.push_back(detIdVector[i]);
    }
  }
}

void RPCOccupancyTest::clientOperation() {
  edm::LogVerbatim("rpceventsummary") << "[RPCOccupancyTest]: Client Operation";

  //Loop on MEs
  for (unsigned int i = 0; i < myOccupancyMe_.size(); i++) {
    this->fillGlobalME(myDetIds_[i], myOccupancyMe_[i]);
  }  //End loop on MEs

  //Active Channels
  if (Active_Fraction && totalStrips_ != 0.) {
    Active_Fraction->setBinContent(1, (totalActive_ / totalStrips_));
  }
  if (Active_Dead) {
    Active_Dead->setBinContent(1, totalActive_);
    Active_Dead->setBinContent(2, (totalStrips_ - totalActive_));
  }
}

void RPCOccupancyTest::myBooker(DQMStore::IBooker & ibooker){

  ibooker.setCurrentFolder( globalFolder_);
  
  std::stringstream histoName;
  rpcdqm::utils rpcUtils;

  histoName.str("");
  histoName << "RPC_Active_Channel_Fractions";
  Active_Fraction = ibooker.book1D(histoName.str().c_str(), histoName.str().c_str(), 1, 0.5, 1.5);
  Active_Fraction->setBinLabel(1, "Active Fraction", 1);

  histoName.str("");
  histoName << "RPC_Active_Inactive_Strips";
  Active_Dead = ibooker.book1D(histoName.str().c_str(), histoName.str().c_str(), 2, 0.5, 2.5);
  Active_Dead->setBinLabel(1, "Active Strips", 1);
  Active_Dead->setBinLabel(2, "Inactive Strips", 1);

  histoName.str("");
  histoName<<"EndCap_OccupancyByWheel";
  
  for (int w = -2; w<=2; w++ ){//loop on wheels
 
    histoName.str("");
    histoName << "AsymmetryLeftRight_Roll_vs_Sector_Wheel" << w;

    AsyMeWheel[w + 2] = ibooker.book2D(histoName.str().c_str(), histoName.str().c_str(), 12, 0.5, 12.5, 21, 0.5, 21.5);

    rpcUtils.labelXAxisSector(AsyMeWheel[w + 2]);
    rpcUtils.labelYAxisRoll(AsyMeWheel[w + 2], 0, w, useRollInfo_);

  }  //end Barrel

  for (int d = -numberOfDisks_; d <= numberOfDisks_; d++) {
    if (d == 0)
      continue;

    int offset = numberOfDisks_;
    if (d > 0)
      offset--;  //used to skip case equale to zero

    histoName.str("");
    histoName<<"AsymmetryLeftRight_Ring_vs_Segment_Disk"<<d;
    AsyMeDisk[d+offset] = ibooker.book2D(histoName.str().c_str(), histoName.str().c_str(), 36, 0.5, 36.5, 3*numberOfRings_, 0.5,3*numberOfRings_+ 0.5);
    
    rpcUtils.labelXAxisSegment(AsyMeDisk[d+offset]);
    rpcUtils.labelYAxisRing(AsyMeDisk[d+offset], numberOfRings_,  useRollInfo_);
    
  }//End loop on Endcap

  histoName.str("");
  histoName << "Occupancy_Roll_by_Roll_RB1in";
  Barrel_OccByRoll[0] = ibooker.book2D(histoName.str().c_str(), histoName.str().c_str(), 10, -2.5, 2.5, 12, 0.5, 12.5);
  rpcUtils.labelXAxisWheel(Barrel_OccByRoll[0]);
  rpcUtils.labelYAxisSector(Barrel_OccByRoll[0], 1);
  
  histoName.str("");
  histoName << "Occupancy_Roll_by_Roll_RB1out";
  Barrel_OccByRoll[1] = ibooker.book2D(histoName.str().c_str(), histoName.str().c_str(), 10, -2.5, 2.5, 12, 0.5, 12.5);
  rpcUtils.labelXAxisWheel(Barrel_OccByRoll[1]);
  rpcUtils.labelYAxisSector(Barrel_OccByRoll[1], 1);

  histoName.str("");
  histoName << "Occupancy_Roll_by_Roll_RB2in";
  const double RB2in_bin[14] = {-2.5, -2.0, -1.5, (-1.5+1.0/3.0), (-1.5+2.0/3.0), -0.5, (-0.5+1.0/3.0), (-0.5+2.0/3.0), 0.5, (0.5+1.0/3.0), (0.5+2.0/3.0), 1.5, 2.0, 2.5};
  TH2F *tmp2D1 = new TH2F(histoName.str().c_str(), histoName.str().c_str(), 13, RB2in_bin, 12, 0.5, 12.5);
  Barrel_OccByRoll[2] = ibooker.book2D(histoName.str().c_str(), tmp2D1);
  rpcUtils.labelXAxisWheel(Barrel_OccByRoll[2]);
  rpcUtils.labelYAxisSector(Barrel_OccByRoll[2], 2);
  
  histoName.str("");
  histoName << "Occupancy_Roll_by_Roll_RB2out";
  const double RB2out_bin[13] = {-2.5, (-2.5+1.0/3.0), (-2.5+2.0/3.0), -1.5, -1.0, -0.5, 0.0, 0.5, 1.0, 1.5, (1.5+1.0/3.0), (1.5+2.0/3.0), 2.5};
  TH2F *tmp2D2 = new TH2F(histoName.str().c_str(), histoName.str().c_str(), 12, RB2out_bin, 12, 0.5, 12.5);
  Barrel_OccByRoll[3] = ibooker.book2D(histoName.str().c_str(), tmp2D2);
  rpcUtils.labelXAxisWheel(Barrel_OccByRoll[3]);
  rpcUtils.labelYAxisSector(Barrel_OccByRoll[3], 2);

  histoName.str("");
  histoName << "Occupancy_Roll_by_Roll_RB3";
  Barrel_OccByRoll[4] = ibooker.book2D(histoName.str().c_str(), histoName.str().c_str(), 10, -2.5, 2.5, 24, 0.5, 24.5);
  rpcUtils.labelXAxisWheel(Barrel_OccByRoll[4]);
  rpcUtils.labelYAxisSector(Barrel_OccByRoll[4], 3);

  histoName.str("");
  histoName << "Occupancy_Roll_by_Roll_RB4";
  Barrel_OccByRoll[5] = ibooker.book2D(histoName.str().c_str(), histoName.str().c_str(), 10, -2.5, 2.5, 24, 0.5, 24.5);
  rpcUtils.labelXAxisWheel(Barrel_OccByRoll[5]);
  rpcUtils.labelYAxisSector(Barrel_OccByRoll[5], 4);

  for(int iStat = 1; iStat <= 4; iStat++){
    histoName.str("");
    histoName << "Occupancy_Roll_by_Roll_RE+" << iStat;
    EndCap_OccByRollplus[iStat-1] = ibooker.book2D(histoName.str().c_str(), histoName.str().c_str(), 6, 0.5, 6.5, 36, 0.5, 36.5);

    rpcUtils.labelXAxisRing(EndCap_OccByRollplus[iStat-1]);
    rpcUtils.labelYAxisChamber(EndCap_OccByRollplus[iStat-1]);
    
    histoName.str("");
    histoName << "Occupancy_Roll_by_Roll_RE-" << iStat;
    EndCap_OccByRollminus[iStat-1] = ibooker.book2D(histoName.str().c_str(), histoName.str().c_str(), 6, 0.5, 6.5, 36, 0.5, 36.5);

    rpcUtils.labelXAxisRing(EndCap_OccByRollminus[iStat-1]);
    rpcUtils.labelYAxisChamber(EndCap_OccByRollminus[iStat-1]);
  }
 }


void RPCOccupancyTest::fillGlobalME(RPCDetId & detId, MonitorElement * myMe){
 
  rpcdqm::utils rpcUtils;
  
  if (!myMe) return;
    
  MonitorElement * AsyMe=nullptr;      //Left Right Asymetry 
  MonitorElement * OccupRoll=nullptr;
       
  if(detId.region() ==0){
    AsyMe= AsyMeWheel[detId.ring()+2];
    int nRollForArr = rpcUtils.detId2ChamberNr(detId) - 1;
    if( nRollForArr == 4 || nRollForArr == 5 ) nRollForArr = 4;
    if( nRollForArr > 5  && nRollForArr <= 9 ) nRollForArr = 5;
    OccupRoll = Barrel_OccByRoll[nRollForArr];
  }
  else{
    if( -detId.station() +  numberOfDisks_ >= 0 ){
      if(detId.region()<0){
	AsyMe= AsyMeDisk[-detId.station()  + numberOfDisks_];
      }
      else{
	AsyMe= AsyMeDisk[detId.station() + numberOfDisks_-1];
      }
    }
    if( detId.region() < 0 ) OccupRoll = EndCap_OccByRollminus[detId.station()-1];
    else                     OccupRoll = EndCap_OccByRollplus[detId.station()-1];
  }

  int xBin,yBin;
  if(detId.region()==0){//Barrel
    xBin= detId.sector();
    yBin = rpcUtils.detId2RollNr(detId);
  }
  else{//Endcap
    //get segment number
    RPCGeomServ RPCServ(detId);
    xBin = RPCServ.segment();
    (numberOfRings_ == 3 ? yBin= detId.ring()*3-detId.roll()+1 : yBin= (detId.ring()-1)*3-detId.roll()+1);
  }
      
  int stripInRoll=myMe->getNbinsX();
  totalStrips_ += (float)stripInRoll;
  float FOccupancy=0;
  float BOccupancy=0;
  
  float  totEnt =  myMe->getEntries();
  for(int strip = 1 ; strip<=stripInRoll; strip++){
    float stripEntries = myMe->getBinContent(strip);
    if(stripEntries > 0) {totalActive_++;}
    if(strip<=stripInRoll/2) {FOccupancy+=myMe->getBinContent(strip);}
    else  {BOccupancy+=myMe->getBinContent(strip);}
  }
  float asym = 0;
  if(totEnt != 0 ) asym =  fabs((FOccupancy - BOccupancy )/totEnt);
 
  float Occup = FOccupancy + BOccupancy;

  if(AsyMe)  AsyMe->setBinContent(xBin,yBin,asym);

  
  xBin = rpcUtils.detId2RollBin(detId);
  yBin = rpcUtils.detId2SectorBin(detId);

  if(detId.region()==0) {
    if(OccupRoll) OccupRoll->setBinContent(xBin, yBin, Occup);
  }
  else if(detId.region()==1) {
    if(OccupRoll) OccupRoll->setBinContent(xBin, yBin, Occup);
  }
  else {
    if(OccupRoll) OccupRoll->setBinContent(xBin, yBin, Occup);
  }
}

