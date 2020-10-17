#ifndef RPCFEDIntegrity_H
#define RPCFEDIntegrity_H

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "EventFilter/RPCRawToDigi/interface/RPCRawDataCounts.h"
#include <string>

class RPCFEDIntegrity : public DQMEDAnalyzer
{
public:
  RPCFEDIntegrity(const edm::ParameterSet &ps);
  ~RPCFEDIntegrity() override = default;

  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;

private:
  edm::EDGetTokenT<RPCRawDataCounts> rawCountsLabel_;
  std::string prefixDir_;
  const int minFEDNum_, maxFEDNum_, numOfFED_;

  MonitorElement* hEntries_;
  MonitorElement* hFatal_;
  MonitorElement* hNonFatal_;

};

#endif

