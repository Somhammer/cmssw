#include "DQM/RPCMonitorDigi/interface/RPCHistoHelper.h"
#include <iostream>

std::string RPCHistoHelper::suggestPath(const RPCDetId* detId)
{
  const static int n = 100;
  char buffer[n] = "Error/Folder/Creation";

  if ( detId->region() == 0 ) {
    snprintf(buffer, n, "Barrel/Wheel_%2d/Sector_%2d/Station_%2d",
                         detId->ring(), detId->sector(), detId->station());
  }
  else if ( detId->region() == +1 ) {
    snprintf(buffer, n, "Endcap+/Disk_+%2d/Ring_%2d/Sector_%2d",
                         detId->station(), detId->ring(), detId->sector());
  }
  else if ( detId->region() == -1 ) {
    snprintf(buffer, n, "Endcap-/Disk_-%2d/Ring_%2d/Sector_%2d",
                         detId->station(), detId->ring(), detId->sector());
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

  const int nLabels = 22;
  const static std::array<const std::string, nLabels> labels = {{
    "RB1in_B", "RB1in_F", "RB1out_B", "RB1out_F",
    "RB2in_B", "RB2in_F", "RB2in_M", "RB2out_M", "RB2out_B", "RB2out_F",
    "RB3-_B", "RB3-F", "RB3+_B", "RB3+_F",
    "RB4,-_B", "RB4,-_F", "RB4+_B", "RB4+_F", // comma in RB4,- makes DQM axis little bit nicd
    "RB4--_B", "RB4--_F", "RB4++_B", "RB4++_F"
  }};

  // Fill RB1in,out,RB2in_B/F
  for ( int i=0; i<6; ++i ) h->GetYaxis()->SetBinLabel(i+1, labels[i].c_str());
  // RB2in_M for wheel -1,0,+1 & RB2out_M for wheel -2,+2
  if ( std::abs(wheel) < 2 ) h->GetYaxis()->SetBinLabel(7, labels[6].c_str());
  else h->GetYaxis()->SetBinLabel(7, labels[7].c_str());
  // Fill RB2out_B,F, RB3 and RB4
  for ( int i=8; i<nLabels; ++i ) h->GetYaxis()->SetBinLabel(i, labels[i].c_str());
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

std::pair<int, int> RPCHistoHelper::findBinRoll(const RPCDetId* detId)
{
  const int sector = detId->sector();
  const int subsector = detId->subsector();
  const int roll = detId->roll();
  if ( detId->region() == 0 ) {
    const int wheel = detId->ring();
    const int station = detId->station();
    const int layer = detId->layer();

    int ybin = 0;
    if ( station == 1 ) {
      // layers: (RB1in, RB1out), (Backward, Forward)
      ybin = 2*(layer-1) + (roll-1)/2 + 1;
    }
    else if ( station == 2 ) {
      if ( (std::abs(wheel) == 3 and layer == 1) or 
           (std::abs(wheel) != 3 and layer == 2) ) {
        // RB2, cases with B/F
        ybin = 5 + 2*(layer-1) + (roll-1)/2;
      }
      else {
        // RB2in or RB2out, cases with B/M/F
        // Note: detId->roll() gives [1,2,3] => [B,M,F]
        // but in the histogram, we have been putting [M,B,F] order
        ybin = 7 + 2*(layer-1) + (roll == 2 ? 0 : (roll-1)/2);
      }
    }
    else if ( station == 3 ) {
      ybin = 10 + 2*(subsector-1) + (roll-1)/2;
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
      ybin = 14 + ss + (roll-1)/2;
    }
    
    return std::make_pair(sector, ybin);
  }
  else {
    const int ring = detId->ring();
    const int nsub = (ring == 1 and detId->station() > 1) ? 3 : 6;
    const int segment = detId->subsector() + (detId->sector()-1)*nsub;

    // Note: roll [1,2,3]->[A,B,C] change the order [C,B,A] bottom to above of the histogram
    const int ybin = 1+ 3*(ring-2) + (3-detId->roll());

    return std::make_pair(segment, ybin);
  }

  return std::make_pair(0,0);
}

