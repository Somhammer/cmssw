#ifndef RPCRollNameHelper_H
#define RPCRollNameHelper_H

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include <string>

struct RPCRollNameHelper
{
  static std::string rollName(const RPCDetId* detId);
  static std::string chamberName(const RPCDetId* detId);

};
#endif

