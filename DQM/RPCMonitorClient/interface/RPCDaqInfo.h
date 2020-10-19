#ifndef DQM_RPCMonitorClient_DQMDaqInfo_H
#define DQM_RPCMonitorClient_DQMDaqInfo_H

#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/DQMEDHarvester.h"

class RPCDaqInfo : public DQMEDHarvester
{
public:
  RPCDaqInfo(const edm::ParameterSet &);
  ~RPCDaqInfo() override = default;

protected:
  void dqmEndLuminosityBlock(DQMStore::IBooker &,
                             DQMStore::IGetter &,
                             edm::LuminosityBlock const &,
                             edm::EventSetup const &) override;       //performed in the endLumi
  void dqmEndJob(DQMStore::IBooker &, DQMStore::IGetter &) override {};  //performed in the endJob

  // Note: separate booker is needed to call this function at the dqmEndOfLumi(online) or dqmEndJob(offline)
  void myBooker(DQMStore::IBooker &);

  const int minFEDId_, maxFEDId_;
  const int nDisks_;
  bool isBooked_;

  MonitorElement *meDAQFraction_;
};

#endif
