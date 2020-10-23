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

class RPCTTUMonitor : public DQMEDAnalyzer
{
public:
  explicit RPCTTUMonitor(const edm::ParameterSet &);
  ~RPCTTUMonitor() override = default;

  int discriminateGMT(const edm::Event &iEvent);
  void fillDiscriminateDecision(bool dataDec, bool emulDec, int indx);

protected:
  void analyze(const edm::Event &, const edm::EventSetup &) override;
  void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;

private:
  const std::string ttuFolder_;
  const std::vector<unsigned> m_ttBits_;

  MonitorElement *m_ttBitsDecisionData;
  MonitorElement *m_ttBitsDecisionEmulator;
  MonitorElement *m_bxDistDiffPac[8];
  MonitorElement *m_bxDistDiffDt[8];
  MonitorElement *m_dataVsemulator[8];

  int m_maxttBits;

  bool m_dtTrigger;
  bool m_rpcTrigger;

  std::vector<int> m_GMTcandidatesBx;
  std::vector<int> m_DTcandidatesBx;
  std::vector<int> m_RPCcandidatesBx;

  edm::EDGetTokenT<L1GlobalTriggerReadoutRecord> m_gtReadoutLabel;
  edm::EDGetTokenT<L1MuGMTReadoutCollection> m_gmtReadoutLabel;
  edm::EDGetTokenT<L1GtTechnicalTriggerRecord> m_rpcTechTrigEmu;
};
