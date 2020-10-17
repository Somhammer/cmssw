#include "DQM/RPCMonitorDigi/interface/RPCRollNameHelper.h"

std::string RPCRollNameHelper::rollName(const RPCDetId* detId)
{
  if ( !detId ) return "";
  std::string chName(chamberName(detId));

  const int rollNumber = detId->roll();
  if ( detId->region() == 0 ) {
    if      ( rollNumber == 1 ) chName += "_B";
    else if ( rollNumber == 2 ) chName += "_M";
    else if ( rollNumber == 3 ) chName += "_F";
  }
  else {
    if      ( rollNumber == 1 ) chName += "_A";
    else if ( rollNumber == 2 ) chName += "_B";
    else if ( rollNumber == 3 ) chName += "_C";
    else if ( rollNumber == 4 ) chName += "_D";
    else if ( rollNumber == 5 ) chName += "_E";
  }

  return chName;
}

std::string RPCRollNameHelper::chamberName(const RPCDetId* detId)
{
  if ( !detId ) return "";

  const int n = 100;
  char buffer[n] = "";
  const int sector = detId->sector();
  if ( detId->region() == 0 ) {
    const int wheel = detId->ring();
    const int station = detId->station();
    const char* substr = [](const int station, const int layer, const int sector, const int subsector) {
      if      ( station <= 2 ) return layer == 1 ? "in" : "out";
      else if ( station == 3 ) return subsector == 1 ? "-" : "+";

      if (sector == 4) {
        switch ( subsector ) {
          case 1: return "--";
          case 2: return "-" ;
          case 3: return "+" ;
          case 4: return "++";
        }
      }
      else if (sector != 9 && sector != 11) {
        return subsector == 1 ? "-" : "+";
      }

      return "";
    }(station, detId->layer(), detId->sector(), detId->subsector());

    snprintf(buffer, n, "W%+2d_RB%d%s_S%02d", wheel, station, substr, sector);
  }
  else {
    const int disk = detId->region()*detId->station();
    const int ring = detId->ring();
    const int nsub = (ring == 1 and detId->station() > 1) ? 3 : 6;
    const int segment = detId->subsector() + (detId->sector()-1)*nsub;

    snprintf(buffer, n, "RE%+2d_R%d_CH%02d", disk, ring, segment);
  }

  return buffer;
}

