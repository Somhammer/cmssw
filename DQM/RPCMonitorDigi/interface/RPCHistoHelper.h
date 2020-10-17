#ifndef RPCHistoHelper_H
#define RPCHistoHelper_H

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "TH1.h"
#include <utility>

struct RPCHistoHelper
{
  static std::string suggestPath(const RPCDetId* detId);
  static void decorateAxisBarrelRoll(TH1* h, const std::string prefix, const int wheel);
  static void decorateAxisEndcapRoll(TH1* h, const std::string prefix, const int disk);
  static std::pair<int, int> findBinRoll(const RPCDetId* detId);
};

#endif

