#include "DQM/RPCMonitorDigi/interface/RPCHistoHelper.h"
#include <iostream>

std::string RPCHistoHelper::joinStrInt(const std::string prefix, const int i, const std::string suffix)
{
  std::stringstream ss;
  ss << prefix << i << suffix;
  return ss.str();
}

std::string RPCHistoHelper::suggestPath(const RPCDetId& detId)
{
  const static int n = 100;
  char buffer[n] = "Error/Folder/Creation";

  if ( detId.region() == 0 ) {
    snprintf(buffer, n, "Barrel/Wheel_%d/Sector_%d/Station_%d",
                         detId.ring(), detId.sector(), detId.station());
  }
  else if ( detId.region() == +1 ) {
    snprintf(buffer, n, "Endcap+/Disk_+%d/Ring_%d/Sector_%d",
                         detId.station(), detId.ring(), detId.sector());
  }
  else if ( detId.region() == -1 ) {
    snprintf(buffer, n, "Endcap-/Disk_-%d/Ring_%d/Sector_%d",
                         detId.station(), detId.ring(), detId.sector());
  }

  return buffer;
}

void RPCHistoHelper::decorateAxisBarrelRoll(TH1* h, const std::string prefix, const int wheel)
{
  const static int bs = 10;
  char buffer[bs];

  snprintf(buffer, bs, "Wheel_%2d", wheel);
  h->SetTitle((prefix+"_Roll_vs_Sector_"+buffer).c_str());

  h->GetXaxis()->SetNoAlphanumeric(true);
  h->GetYaxis()->SetNoAlphanumeric(true);
  h->GetXaxis()->SetTitle("");
  h->GetYaxis()->SetTitle("");

  for ( short i=1, n=h->GetNbinsX(); i<=n; ++i ) {
    snprintf(buffer, bs, "Sec%2d", i);
    h->GetXaxis()->SetBinLabel(i, buffer);
  }

  const int nLabels = 24;
  const static std::array<const std::string, nLabels> labels = {{
    "RB1in_B", "RB1in_F", "RB1out_B", "RB1out_F",
    "RB2in_B", "RB2in_F", "RB2in_M", "RB2out_M", "RB2out_B", "RB2out_F",
    "RB3-_B", "RB3-F", "RB3+_B", "RB3+_F",
    "RB4_B", "RB4_F",
    "RB4,-_B", "RB4,-_F", "RB4+_B", "RB4+_F", // comma in RB4,- makes DQM axis little bit nicd
    "RB4--_B", "RB4--_F", "RB4++_B", "RB4++_F"
  }};

  // Fill RB1, RB2in_B
  for ( int i=0; i<5; ++i ) h->GetYaxis()->SetBinLabel(i+1, labels[i].c_str());
  // Fill RB2
  // RB2in_M for wheel -1,0,+1 & RB2out_M for wheel -2,+2
  if ( std::abs(wheel) < 2 ) {
    h->GetYaxis()->SetBinLabel(6, labels[6].c_str());
    h->GetYaxis()->SetBinLabel(7, labels[5].c_str());
    h->GetYaxis()->SetBinLabel(8, labels[8].c_str());
  }
  else {
    h->GetYaxis()->SetBinLabel(6, labels[5].c_str());
    h->GetYaxis()->SetBinLabel(7, labels[8].c_str());
    h->GetYaxis()->SetBinLabel(8, labels[7].c_str());
  }
  // Fill RB2out_B
  h->GetYaxis()->SetBinLabel(9, labels[9].c_str());
  // Fill RB3
  for ( int i=10; i<14; ++i ) h->GetYaxis()->SetBinLabel(i, labels[i].c_str());
  // Fill RB4
  const int nYbin = h->GetNbinsY();
  for ( int i=14; i<nLabels; ++i ) {
    int j = i;
    if ( i > nYbin ) break;
    if ( nYbin != 15 ) j = i + 2;
    h->GetYaxis()->SetBinLabel(i, labels[j].c_str());
  }
  /*
  if ( h->GetYaxis()->GetNbinsY() == 15 ) {
    for ( int i=14; i<16; ++i ) h->GetYaxis()->SetBinLabel(i, labels[i].c_str());
  }
  else if( h->GetYaxis()->GetNbinsY() == 17 ) {
    for ( int i=16; i<20; ++i ) h->GetYaxis()->SetBinLabel(i-2, labels[i].c_str());
  }
  else {
    for ( int i=16; i<24; ++i ) h->GetYaxis()->SetBinLabel(i-2, labels[i].c_str());
  */
}

void RPCHistoHelper::decorateAxisEndcapRoll(TH1* h, const std::string prefix, const int disk)
{
  const static int bs = 10;
  char buffer[bs];

  snprintf(buffer, bs, "Disk_%2d", disk);
  h->SetTitle((prefix+"_Roll_vs_Sector_"+buffer).c_str());

  h->GetXaxis()->SetNoAlphanumeric(true);
  h->GetYaxis()->SetNoAlphanumeric(true);
  h->GetXaxis()->SetTitle("Segments");
  h->GetYaxis()->SetTitle("");

  for ( short i=1, n=h->GetNbinsX(); i<=n; ++i ) {
    snprintf(buffer, bs, "%2d", i);
    h->GetXaxis()->SetBinLabel(i, buffer);
  }

  const int nLabels = 7;
  const static std::array<const std::string, nLabels> labels = {{
    "Ring1 A", // placeholder for upgrade scenario
    "C", "Ring2 B", "A",
    "C", "Ring3 B", "A"
  }};
  for ( int i=1, n=h->GetNbinsY(); i<=n; ++i ) {
    h->GetYaxis()->SetBinLabel(i, labels[i+nLabels-n-1].c_str());
  }
}

std::pair<int, int> RPCHistoHelper::findBinRoll(const RPCDetId& detId)
{
  const int sector = detId.sector();
  const int subsector = detId.subsector();
  int r = RPCHistoHelper::findRollIndex(detId, BFM);
  if ( detId.region() == 0 ) {
    const int station = detId.station();
    const int layer = detId.layer();

    int ybin = 0;
    if ( station == 1 ) {
      // layers: (RB1in, RB1out), (Backward, Forward)
      ybin = 2*(layer-1) + r;
    }
    else if ( station == 2 ) {
      // RB2 is special in Histogram binning,
      //  wheel +-2: [RB2in_B,RB2in_F,RB2out_M,RB2out_B,RB2out_F]
      //  wheel0+-1: [RB2in_B,RB2in_F,RB2in_M ,RB2out_B,RB2out_F]
      // action needed: change order to MBF for wheel+-2 RB2out
      const int wheel = detId.ring();
      if ( layer == 2 and std::abs(wheel) == 2 ) {
        r = RPCHistoHelper::findRollIndex(detId, MBF)-1;
      }
      ybin = 4 + 3*(layer-1) + r;
    }
    else if ( station == 3 ) {
      // [RB1:4 bins][RB2:5 bins] = 9
      ybin = 9 + 2*(subsector-1) + r;
    }
    else if ( station == 4 ) {
      int ss = 2*(subsector-1);
      if ( sector == 4 ) {
        switch ( subsector ) {
          case 1: ss += 4; break; // RB4--
          case 2: ss += 0; break; // RB4-
          case 3: ss += 2; break; // RB4+
          case 4: ss += 6; break; // RB4++
        }
      }
      // [RB1:4 bins][RB2:5 bins][RB3:4 bins] = 13
      ybin = 13 + ss + r;
    }
    
    return std::make_pair(sector, ybin);
  }
  else {
    const int ring = detId.ring();
    const int nsub = (ring == 1 and detId.station() > 1) ? 3 : 6;
    const int segment = detId.subsector() + (detId.sector()-1)*nsub;

    const int ybin = 3*(ring-2) + r;

    return std::make_pair(segment, ybin);
  }

  return std::make_pair(0,0);
}

int RPCHistoHelper::findRollIndex(const RPCDetId& detId, int ord)
{
  const int station = detId.station();
  const int roll = detId.roll();
  if ( detId.region() == 0 ) {
    const int awheel = std::abs(detId.ring());
    const int layer = detId.layer();
    if ( station == 2 and ( (layer == 1 and awheel != 2) or
                            (layer == 2 and awheel == 2) ) ) {
      // RB2in wheel 0,+-1 has B/M/F
      // RB2out wheel +-2 has B/M/F
      // Note: detId.roll() gives [1,2,3] => [B,M,F]
      //       but in the histogram, we have been putting [M,B,F] order
      switch ( ord ) {
        case BMF: return roll;
        case BFM: return (roll == 2 ? 3 : 1+(roll-1)/2);
        case MBF: return (roll == 2 ? 1 : 1+(roll-1)/2);
      }
    }

    return 1 + (roll-1)/2;
  }
  else {
    // Note: roll [1,2,3]->[A,B,C] change the order [C,B,A] bottom to above of the histogram
    return 1 + (3-roll);
  }
  return 0;
}

