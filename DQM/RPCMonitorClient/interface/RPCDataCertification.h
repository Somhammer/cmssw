#ifndef RPCMonitorClient_RPCDataCertification_H
#define RPCMonitorClient_RPCDataCertification_H

#include "DQMServices/Core/interface/DQMEDHarvester.h"
#include "DQMServices/Core/interface/DQMStore.h"

class RPCDataCertification : public DQMEDHarvester
{
public:
  RPCDataCertification(const edm::ParameterSet& pset);
  ~RPCDataCertification() override = default;

protected:
  void dqmEndLuminosityBlock(DQMStore::IBooker&,
                             DQMStore::IGetter&,
                             edm::LuminosityBlock const&,
                             edm::EventSetup const&) override;      //performed in the endLumi
  void dqmEndJob(DQMStore::IBooker&, DQMStore::IGetter&) override;  //performed in the endJob

  void myBooker(DQMStore::IBooker&);
  void checkFED(edm::EventSetup const&);

  const int minFEDId_, maxFEDId_;
  const int nDisks_;
  const bool isOfflineDQM_;

  bool isFEDChecked_;
  double fracCert_;
};

#endif
