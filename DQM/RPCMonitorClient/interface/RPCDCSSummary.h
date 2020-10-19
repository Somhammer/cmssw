#ifndef RPCMonitorClient_RPCDCSSummary_H
#define RPCMonitorClient_RPCDCSSummary_H

#include "DQMServices/Core/interface/DQMEDHarvester.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class RPCDCSSummary : public DQMEDHarvester
{
public:
  RPCDCSSummary(const edm::ParameterSet &);
  ~RPCDCSSummary() override = default;

protected:
  void dqmEndLuminosityBlock(DQMStore::IBooker &,
                             DQMStore::IGetter &,
                             edm::LuminosityBlock const &,
                             edm::EventSetup const &) override;       //performed in the endLumi
  void dqmEndJob(DQMStore::IBooker &, DQMStore::IGetter &) override;  //performed in the endJob

private:
  // Note: separate booker is needed to call this function at the dqmEndOfLumi(online) or dqmEndJob(offline)
  void myBooker(DQMStore::IBooker &);
  void checkDCSbit(edm::EventSetup const &);

  const int minFEDId_, maxFEDId_;
  const int nDisks_;
  const int isOfflineDQM_;

  bool isFilled_;
  double fracDCS_;
};

#endif
