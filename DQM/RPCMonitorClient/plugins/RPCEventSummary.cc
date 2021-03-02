#include "DQM/RPCMonitorClient/interface/RPCEventSummary.h"

/*  \author Anna Cimmino*/

#include "CondFormats/RunInfo/interface/RunInfo.h"
#include "CondFormats/DataRecord/interface/RunSummaryRcd.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DQM/RPCMonitorDigi/interface/RPCHistoHelper.h"

RPCEventSummary::RPCEventSummary(const edm::ParameterSet& ps):
  minFED_(ps.getUntrackedParameter<unsigned int>("MinimumRPCFEDId", 790)),
  maxFED_(ps.getUntrackedParameter<unsigned int>("MaximumRPCFEDId", 792)),
  nDisks_(ps.getUntrackedParameter<int>("NumberOfEndcapDisks", 4))
{
  init_ = false;

  prescaleFactor_ = ps.getUntrackedParameter<int>("PrescaleFactor", 1);
  eventInfoPath_ = ps.getUntrackedParameter<std::string>("EventInfoPath", "RPC/EventInfo");

  std::string subsystemFolder = ps.getUntrackedParameter<std::string>("RPCFolder", "RPC");
  std::string recHitTypeFolder = ps.getUntrackedParameter<std::string>("RecHitTypeFolder", "AllHits");
  std::string summaryFolder = ps.getUntrackedParameter<std::string>("SummaryFolder", "SummaryHistograms");

  globalFolder_ = subsystemFolder + "/" + recHitTypeFolder + "/" + summaryFolder;
  prefixFolder_ = subsystemFolder + "/" + recHitTypeFolder;

  minimumEvents_ = ps.getUntrackedParameter<int>("MinimumRPCEvents", 10000);
  doEndcapCertification_ = ps.getUntrackedParameter<bool>("EnableEndcapSummary", false);

  offlineDQM_ = ps.getUntrackedParameter<bool>("OfflineDQM", true);
}

void RPCEventSummary::dqmEndLuminosityBlock(DQMStore::IBooker& ibooker,
                                            DQMStore::IGetter& igetter,
                                            edm::LuminosityBlock const& lb,
                                            edm::EventSetup const& setup)
{
  if (!init_) {
    lumiCounter_ = prescaleFactor_;

    edm::eventsetup::EventSetupRecordKey recordKey(
        edm::eventsetup::EventSetupRecordKey::TypeTag::findType("RunInfoRcd"));

    int defaultValue = 1;
    isIn_ = true;

    if (auto runInfoRec = setup.tryToGet<RunInfoRcd>()) {
      defaultValue = -1;
      //get fed summary information
      edm::ESHandle<RunInfo> sumFED;
      runInfoRec->get(sumFED);
      std::vector<int> FedsInIds = sumFED->m_fed_in;
      unsigned int f = 0;
      isIn_ = false;
      while (!isIn_ && f < FedsInIds.size()) {
        const int fedID = FedsInIds[f];
        //make sure fed id is in allowed range
        if (fedID >= minFED_ && fedID <= maxFED_) {
          defaultValue = 1;
          isIn_ = true;
        }
        f++;
      }
    }

    ibooker.setCurrentFolder(eventInfoPath_);

    //a global summary float [0,1] providing a global summary of the status
    //and showing the goodness of the data taken by the the sub-system
    MonitorElement* me = ibooker.bookFloat("reportSummary");
    me->Fill(defaultValue);

    //TH2F ME providing a mapof values[0-1] to show if problems are localized or distributed
    me = ibooker.book2D("reportSummaryMap", "RPC Report Summary Map", 15, -7.5, 7.5, 12, 0.5, 12.5);

    //customize the 2d histo
    for (int i = 1; i <= 15; i++) {
      if (i < 13) me->setBinLabel(i, RPCHistoHelper::joinStrInt("Sec", i), 2);

      if      (i <  5) me->setBinLabel(i, RPCHistoHelper::joinStrInt("Disk", (i- 5)), 1);
      else if (i > 11) me->setBinLabel(i, RPCHistoHelper::joinStrInt("Disk", (i-11)), 1);
      else if (i == 11 || i == 5) me->setBinLabel(i, "", 1);
      else me->setBinLabel(i, RPCHistoHelper::joinStrInt("Wheel", (i-8)), 1);
    }

    //fill the histo with "1" --- just for the moment
    for (int i = 1; i <= 15; i++) {
      for (int j = 1; j <= 12; j++) {
        if (i == 5 || i == 11 || (j > 6 && (i < 6 || i > 10)))
          me->setBinContent(i, j, -1);  //bins that not correspond to subdetector parts
        else
          me->setBinContent(i, j, defaultValue);
      }
    }

    if (nDisks_ < 4) {
      for (int j = 1; j <= 12; j++) {
        me->setBinContent(1, j, -1);  //bins that not correspond to subdetector parts
        me->setBinContent(15, j, -1);
      }
    }

    //the reportSummaryContents folder containins a collection of ME floats [0-1] (order of 5-10)
    // which describe the behavior of the respective subsystem sub-components.
    ibooker.setCurrentFolder(eventInfoPath_ + "/reportSummaryContents");

    std::vector<std::string> segmentNames;
    for (int i = -2; i <= 2; i++) {
      segmentNames.push_back(RPCHistoHelper::joinStrInt("RPC_Wheel", i));
    }

    for (int i = -nDisks_; i <= nDisks_; i++) {
      if (i == 0) continue;
      segmentNames.push_back(RPCHistoHelper::joinStrInt("RPC_Disk", i));
    }

    for (unsigned int i = 0; i < segmentNames.size(); i++) {
      MonitorElement* me = ibooker.bookFloat(segmentNames[i]);
      me->Fill(defaultValue);
    }

    lumiCounter_ = prescaleFactor_;
    init_ = true;
  }

  if (isIn_ && !offlineDQM_ && lumiCounter_ % prescaleFactor_ == 0) {
    this->clientOperation(igetter);
  }

  ++lumiCounter_;
}

void RPCEventSummary::dqmEndJob(DQMStore::IBooker& ibooker, DQMStore::IGetter& igetter)
{
  if (isIn_) this->clientOperation(igetter);
}

void RPCEventSummary::clientOperation(DQMStore::IGetter& igetter)
{
  MonitorElement* hRPCEvents = igetter.get(prefixFolder_ + "/RPCEvents");
  const float rpcevents = hRPCEvents ? hRPCEvents->getBinContent(1) : minimumEvents_;
  if (rpcevents < minimumEvents_) return;

  MonitorElement* reportMe = igetter.get(eventInfoPath_ + "/reportSummaryMap");

  //BARREL
  float barrelFactor = 0;

  for (int w = -2; w < 3; w++) {
    MonitorElement* me = igetter.get(RPCHistoHelper::joinStrInt(globalFolder_+"/RPCChamberQuality_Roll_vs_Sector_Wheel", w));
    if ( !me ) continue;

    float wheelFactor = 0;
    for (int s = 1; s <= me->getNbinsX(); s++) {
      float sectorFactor = 0;
      int rollInSector = 0;

      for (int r = 1; r <= me->getNbinsY(); r++) {
        if ((s != 4 && r > 17) || ((s == 9 || s == 10) && r > 15)) continue;
        ++rollInSector;

        if      (me->getBinContent(s, r) == PARTIALLY_DEAD) sectorFactor += 0.8;
        else if (me->getBinContent(s, r) == DEAD)           sectorFactor += 0;
        else sectorFactor += 1;
      }
      if (rollInSector != 0) sectorFactor = sectorFactor / rollInSector;

      if (reportMe) reportMe->setBinContent(w + 8, s, sectorFactor);
      wheelFactor += sectorFactor;
    }  //end loop on sectors
    wheelFactor = wheelFactor / me->getNbinsX();

    MonitorElement* globalMe = igetter.get(RPCHistoHelper::joinStrInt(eventInfoPath_+"/reportSummaryContents/RPC_Wheel", w));
    if (globalMe) globalMe->Fill(wheelFactor);

    barrelFactor += wheelFactor;
  }    //end loop on wheel
  barrelFactor = barrelFactor / 5;

  float endcapFactor = 0;
  if (doEndcapCertification_) {
    //Endcap
    for (int d = -nDisks_; d <= nDisks_; d++) {
      if (d == 0) continue;

      MonitorElement* myMe = igetter.get(RPCHistoHelper::joinStrInt(globalFolder_+"/RPCChamberQuality_Ring_vs_Segment_Disk", d));
      if (!myMe) continue;

      float diskFactor = 0;

      float sectorFactor[6] = {0, 0, 0, 0, 0, 0};

      for (int i = 0; i < 6; i++) {
        int firstSeg = (i * 6) + 1;
        int lastSeg = firstSeg + 6;
        int rollInSector = 0;
        for (int seg = firstSeg; seg < lastSeg; seg++) {
          for (int y = 1; y <= myMe->getNbinsY(); y++) {
            ++rollInSector;
            if (myMe->getBinContent(seg, y) == PARTIALLY_DEAD) sectorFactor[i] += 0.8;
            else if (myMe->getBinContent(seg, y) == DEAD) sectorFactor[i] += 0;
            else sectorFactor[i] += 1;
          }
        }
        sectorFactor[i] = sectorFactor[i] / rollInSector;
      }  //end loop on Sectors

      for (int sec = 0; sec < 6; sec++) {
        diskFactor += sectorFactor[sec];
        if (reportMe) {
          if (d < 0) reportMe->setBinContent(d + 5, sec + 1, sectorFactor[sec]);
          else reportMe->setBinContent(d + 11, sec + 1, sectorFactor[sec]);
        }
      }

      diskFactor = diskFactor / 6;

      MonitorElement* globalMe = igetter.get(RPCHistoHelper::joinStrInt(eventInfoPath_+"/reportSummaryContents/RPC_Disk", d));
      if (globalMe) globalMe->Fill(diskFactor);

      endcapFactor += diskFactor;
    }  //end loop on disks
    endcapFactor = endcapFactor / (nDisks_ * 2);
  }

  //Fill repor summary
  MonitorElement* globalMe = igetter.get(eventInfoPath_ + "/reportSummary");
  const float rpcFactor = doEndcapCertification_ ? (barrelFactor + endcapFactor) / 2 : barrelFactor;
  if (globalMe) globalMe->Fill(rpcFactor);
}
