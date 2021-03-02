#include "DQM/RPCMonitorClient/interface/RPCSummaryMap.h"
#include <sstream>

typedef dqm::harvesting::DQMStore DQMStore;
typedef dqm::harvesting::MonitorElement MonitorElement;

MonitorElement* RPCSummaryMap::bookSummaryMap(DQMStore::IBooker& ibooker, const std::string name, const std::string title) const
{
  MonitorElement* me = ibooker.book2D(name, title, 15, -7.5, 7.5, 12, 0, 12);

  //customize the 2d histo
  std::stringstream binLabel;
  for (int i=1; i<=12; ++i) {
    binLabel.str("");
    binLabel << "Sec" << i;
    me->setBinLabel(i, binLabel.str(), 2);
  }

  for (int i = -2; i <= 2; ++i) {
    binLabel.str("");
    binLabel << "Wheel" << i;
    me->setBinLabel(i + 8, binLabel.str(), 1);
  }

  for (int i = 1; i <= nDisks_; ++i) {
    binLabel.str("");
    binLabel << "Disk" << i;
    me->setBinLabel((i + 11), binLabel.str(), 1);
    binLabel.str("");
    binLabel << "Disk" << -i;
    me->setBinLabel((-i + 5), binLabel.str(), 1);
  }

  //fill histogram bins with -1 to indicate invalid area
  for ( int i=1; i<=15; ++i ) {
    for (int j=1; j<=12; ++j) {
      me->setBinContent(i, j, -1);
    }
  }

  return me;
}

