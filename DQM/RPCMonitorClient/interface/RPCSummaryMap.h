#ifndef RPCSummaryMap_H
#define RPCSummaryMap_H

#include "DQMServices/Core/interface/DQMStore.h"
#include <string>

class RPCSummaryMap
{
public:
  RPCSummaryMap(const int nDisks): nDisks_(nDisks) {};

protected:
  typedef dqm::harvesting::DQMStore DQMStore;
  typedef dqm::harvesting::MonitorElement MonitorElement;
  MonitorElement* bookSummaryMap(DQMStore::IBooker& ibooker, const std::string name, const std::string title) const;

  const int nDisks_;
};

#endif

