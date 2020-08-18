/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr() {}
   ~CirMgr() {}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { 
     
     map<unsigned, CirGate*>::const_iterator iter;
     for(iter = _UNDEFlist.begin(); iter != _UNDEFlist.end(); iter++){
       if(iter->first == gid){
         return iter->second;
       }
     }
     for(iter = _dict.begin(); iter != _dict.end(); iter++){
       if(iter->first == gid){
         return iter->second;
       }
     }
     return 0;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);
   bool readHeader(size_t, const string&);
   bool readInput(size_t, const string&);
   bool readOutput(size_t, const string&, int);
   bool readAIG(size_t, const string&);
   bool readSymbol(const string&);
   void connect();
   // Member functions about circuit reporting
   void DFStraversal(GateList);
   unsigned countPIs() const;
   unsigned countPOs() const;
   unsigned countAIGs() const;
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;

private:
  // represent:
  // M:maximal variable index
  // I:number of inputs
  // L:number of latches(useless in HW6)
  // O:number of outputs
  // A:number of AND gates
  unsigned int M;
  unsigned int I;
  unsigned int L;
  unsigned int O;
  unsigned int A;
  
  // typedef vector<unsigned>           IdList;
  // in cirDef.h
  
  // typedef vector<CirGate*>           GateList;
  // in cirDef.h
  GateList _pilist;
  GateList _polist;
  GateList _dfslist;
  CirGate* CONST0;
  // this is the gate_list->used to find gates with their ID
  map<unsigned, CirGate*>  _dict;
  map<unsigned, CirGate*>  _UNDEFlist;
  map<unsigned, string> _pi_symbol;
  map<unsigned, string> _po_symbol;
  vector<string> _symbol;
  vector<string> _comment;
};

#endif // CIR_MGR_H
