#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerRecord.h>
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetup.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include <DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h>

#include <DataFormats/L1GlobalTrigger/interface/L1GtTechnicalTrigger.h>
#include <DataFormats/L1GlobalTrigger/interface/L1GtTechnicalTriggerRecord.h>

#include <bitset>

class RPCTTUMonitor : public DQMEDAnalyzer
{
public:
  explicit RPCTTUMonitor(const edm::ParameterSet &);
  ~RPCTTUMonitor() override = default;

  std::bitset<2> discriminateGMT(const L1MuGMTReadoutCollection* gmtRC,
                                 std::vector<int>& gmtCandsBx, std::vector<int>& dtCandsBx) const;
  void fillDiscriminateDecision(bool dataDec, bool emulDec, int indx);

protected:
  void analyze(const edm::Event &, const edm::EventSetup &) override;
  void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;

private:
  const std::string ttuFolder_;
  const std::vector<unsigned> ttBits_;
  const int maxttBits_;

  MonitorElement *m_ttBitsDecisionData;
  MonitorElement *m_ttBitsDecisionEmulator;
  MonitorElement *m_bxDistDiffPac[8];
  MonitorElement *m_bxDistDiffDt[8];
  MonitorElement *m_dataVsemulator[8];

  std::vector<int> m_GMTcandidatesBx;
  std::vector<int> m_DTcandidatesBx;

  edm::EDGetTokenT<L1GlobalTriggerReadoutRecord> m_gtReadoutLabel;
  edm::EDGetTokenT<L1MuGMTReadoutCollection> m_gmtReadoutLabel;
  edm::EDGetTokenT<L1GtTechnicalTriggerRecord> m_rpcTechTrigEmu;
};
