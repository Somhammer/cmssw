#include "DQM/RPCMonitorDigi/interface/RPCTTUMonitor.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DQM/RPCMonitorDigi/interface/RPCHistoHelper.h"

RPCTTUMonitor::RPCTTUMonitor(const edm::ParameterSet& iConfig):
  ttuFolder_(iConfig.getUntrackedParameter<std::string>("TTUFolder", "RPC/TTU")),
  m_ttBits_(iConfig.getParameter<std::vector<unsigned> >("BitNumbers"))
{
  m_gtReadoutLabel = consumes<L1GlobalTriggerReadoutRecord>(iConfig.getParameter<edm::InputTag>("GTReadoutRcd"));
  m_gmtReadoutLabel = consumes<L1MuGMTReadoutCollection>(iConfig.getParameter<edm::InputTag>("GMTReadoutRcd"));
  m_rpcTechTrigEmu = consumes<L1GtTechnicalTriggerRecord>(iConfig.getParameter<edm::InputTag>("L1TTEmuBitsLabel"));

  m_maxttBits = m_ttBits_.size();
}

// ------------ method called to for each event  ------------
void RPCTTUMonitor::analyze(const edm::Event& iEvent, const edm::EventSetup&)
{
  // Data .
  edm::Handle<L1GlobalTriggerReadoutRecord> gtRecord;
  iEvent.getByToken(m_gtReadoutLabel, gtRecord);
  if (!gtRecord.isValid()) {
    edm::LogError("RPCTTUMonitor") << "can nout find L1GlobalTriggerRecord \n";
    return;
  }

  // Emulator .
  edm::Handle<L1GtTechnicalTriggerRecord> emuTTRecord;
  iEvent.getByToken(m_rpcTechTrigEmu, emuTTRecord);
  if (!emuTTRecord.isValid()) {
    edm::LogError("RPCTTUMonitor") << "can not find L1GtTechnicalTriggerRecord (emulator) \n";
    return;
  }

  //Timing difference between RPC-PAT and DT
  const int dGMT = discriminateGMT(iEvent);
  if (dGMT < 0) return;

  const int bxX = iEvent.bunchCrossing();  // ... 1 to 3564

  for (int k = 0; k < m_maxttBits; ++k) {
    std::map<int, bool> ttuDec;
    for (int iebx = 0; iebx <= 2; iebx++) {
      const TechnicalTriggerWord gtTTWord = gtRecord->technicalTriggerWord(iebx - 1);
      ttuDec[iebx - 1] = gtTTWord[24 + k];
    }

    //. RPC
    if (m_rpcTrigger) {
      const int bx1 = (bxX - m_GMTcandidatesBx[0]);
      for ( auto dec : ttuDec ) {
        if (!dec.second) continue;
        const int bx2 = dec.first;
        const double bxdiffPacTT = (bx1 - bx2);
        m_bxDistDiffPac[k]->Fill(bxdiffPacTT);
      }
    }

    //.. DT
    if (m_dtTrigger) {
      const int bx1 = (bxX - m_DTcandidatesBx[0]);
      for (auto dec : ttuDec ) {
        if (!dec.second) continue;
        const int bx2 = dec.first;
        const double bxdiffDtTT = (bx1 - bx2);
        m_bxDistDiffDt[k]->Fill(bxdiffDtTT);
      }
    }
  }

  m_GMTcandidatesBx.clear();
  m_DTcandidatesBx.clear();

  //... For Data Emulator comparison
  const TechnicalTriggerWord gtTTWord = gtRecord->technicalTriggerWord();
  const std::vector<L1GtTechnicalTrigger> ttVec = emuTTRecord->gtTechnicalTrigger();
  if (ttVec.empty()) return;

  int k = 0;
  for (auto bits : m_ttBits_ ) {
    const bool hasDataTrigger = gtTTWord.at(bits);
    m_ttBitsDecisionData->Fill(bits, (int)hasDataTrigger);

    const bool hasEmulatorTrigger = ttVec[k].gtTechnicalTriggerResult();
    m_ttBitsDecisionEmulator->Fill(ttVec[k].gtTechnicalTriggerBitNumber(), (int)hasEmulatorTrigger);

    fillDiscriminateDecision(hasDataTrigger, hasEmulatorTrigger, k);

    ++k;
  }
}

int RPCTTUMonitor::discriminateGMT(const edm::Event& iEvent)
{
  edm::Handle<L1MuGMTReadoutCollection> pCollection;
  iEvent.getByToken(m_gmtReadoutLabel, pCollection);

  if (!pCollection.isValid()) {
    edm::LogError("discriminateGMT") << "can't find L1MuGMTReadoutCollection with label \n";
    return -1;
  }

  // get GMT readout collection
  const L1MuGMTReadoutCollection* gmtRC = pCollection.product();

  bool rpcBar_l1a = false;
  bool dtBar_l1a = false;
  for ( auto gmtRecord : gmtRC->getRecords() ) {
    const int bxInEvent = gmtRecord.getBxInEvent();
    const int bxInEventNew = gmtRecord.getBxNr();

    int nrpcB = 0;
    for ( auto l1Cands : gmtRecord.getBrlRPCCands() ) {
      if ( l1Cands.empty() ) continue;
      m_GMTcandidatesBx.push_back(bxInEventNew);
      ++nrpcB;
    }
    if (bxInEvent == 0 && nrpcB > 0) rpcBar_l1a = true;

    int ndtB = 0;
    for ( auto l1Cands : gmtRecord.getDTBXCands() ) {
      if ( l1Cands.empty() ) continue;
      m_DTcandidatesBx.push_back(bxInEventNew);
      ++ndtB;
    }
    if (bxInEvent == 0 && ndtB > 0) dtBar_l1a = true;
  }

  m_dtTrigger = false;
  m_rpcTrigger = false;

  int gmtDec = 0;
  if (rpcBar_l1a) {
    gmtDec = 1;
    m_rpcTrigger = true;
  }

  if (dtBar_l1a) {
    gmtDec = 2;
    m_dtTrigger = true;
  }

  return gmtDec;
}

void RPCTTUMonitor::fillDiscriminateDecision(bool dataDec, bool emulDec, int indx)
{
  if      (dataDec == 1 && emulDec == 1) m_dataVsemulator[indx]->Fill(1);
  else if (dataDec == 1 && emulDec == 0) m_dataVsemulator[indx]->Fill(3);
  else if (dataDec == 0 && emulDec == 1) m_dataVsemulator[indx]->Fill(5);
  else if (dataDec == 0 && emulDec == 0) m_dataVsemulator[indx]->Fill(7);
}

void RPCTTUMonitor::bookHistograms(DQMStore::IBooker& ibooker, edm::Run const& r, edm::EventSetup const&)
{
  ibooker.setCurrentFolder(ttuFolder_);

  m_ttBitsDecisionData = ibooker.book1D("TechTrigger.Bits.Data", "Technical Trigger bits : Summary", 10, 23, 33);
  m_ttBitsDecisionEmulator = ibooker.book1D("TechTrigger.Bits.Emulator", "Technical Trigger bits : Summary", 10, 23, 33);

  for (unsigned k=0; k<m_ttBits_.size(); ++k) {
    const int ttBit = m_ttBits_[k];

    m_bxDistDiffPac[k] = ibooker.book1D(RPCHistoHelper::joinStrInt("BX.diff.PAC-TTU.bit.", ttBit),
                                        "Timing difference between PAC and TTU", 7, -3, 3);
    m_bxDistDiffDt[k] = ibooker.book1D(RPCHistoHelper::joinStrInt("BX.diff.DT-TTU.bit.", ttBit),
                                       "Timing difference between DT and TTU", 7, -3, 3);
    m_dataVsemulator[k] = ibooker.book1D(RPCHistoHelper::joinStrInt("Emu.Ttu.Compare.bit.", ttBit),
                                         "Comparison between emulator and TT decisions", 10, 0, 10);
  }
}
