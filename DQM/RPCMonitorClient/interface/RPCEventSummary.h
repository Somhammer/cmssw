#ifndef RPCEventSummary_H
#define RPCEventSummary_H

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/DQMEDHarvester.h"

#include <string>

class RPCEventSummary : public DQMEDHarvester
{
public:
  RPCEventSummary(const edm::ParameterSet &ps);
  ~RPCEventSummary() override = default;

protected:
  void dqmEndLuminosityBlock(DQMStore::IBooker &,
                             DQMStore::IGetter &,
                             edm::LuminosityBlock const &,
                             edm::EventSetup const &) override;       //performed in the endLumi
  void dqmEndJob(DQMStore::IBooker &, DQMStore::IGetter &) override;  //performed in the endJob

private:
  void clientOperation(DQMStore::IGetter &igetter);
  const int minFED_, maxFED_;
  const int nDisks_;

  std::string eventInfoPath_;

  //  bool tier0_;
  int prescaleFactor_, minimumEvents_;

  bool init_, isIn_;
  bool offlineDQM_;
  int lumiCounter_;
  std::string globalFolder_, prefixFolder_;

  bool doEndcapCertification_;

  enum RPCQualityFlags { DEAD = 6, PARTIALLY_DEAD = 5 };
};

#endif
