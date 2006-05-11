#ifndef L1GCTEMCAND_H_
#define L1GCTEMCAND_H_

#include <bitset>

using std::bitset;

/*! \file L1GctEmCand.h
 * \Header file for the Gct electron 
 *  candidate
 * 
 * \author: Jim Brooke
 *
 * Set methods added by Maria Hansen
 * \date: 15/03/2006
 */

typedef unsigned long int ULong;

class L1GctEmCand
{
public:
  L1GctEmCand(ULong data=0);
  L1GctEmCand(ULong rank, ULong eta, ULong phi);
  ~L1GctEmCand();
	
  ///
  /// set rank bits
  void setRank(unsigned long rank) { myRank = rank; }
  ///
  /// set eta bits
  void setEta(unsigned long eta) { myEta = eta; }
  ///
  /// set phi bits
  void setPhi(unsigned long phi) { myPhi = phi; }
  
  ///
  /// get rank bits
  inline unsigned long getRank() { return myRank.to_ulong(); }
  ///
  /// get eta bits
  inline unsigned long getEta() { return myEta.to_ulong(); }
  ///
  /// get phi bits
  inline unsigned long getPhi() { return myPhi.to_ulong(); }



private:

    static const int RANK_BITWIDTH = 6;
    static const int ETA_BITWIDTH = 5;
    static const int PHI_BITWIDTH = 5;

    bitset<RANK_BITWIDTH> myRank;
    bitset<ETA_BITWIDTH> myEta;
    bitset<PHI_BITWIDTH> myPhi;
      
};

#endif /*L1GCTEMCAND_H_*/
