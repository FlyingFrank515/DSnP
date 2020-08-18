/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   CirGate():_gateID(0),_lineno(0){}
   CirGate(unsigned, unsigned);
   virtual ~CirGate(){}

   // Basic access methods
   virtual string getTypeStr() const = 0;
   virtual int getTypeInt() const = 0;
   unsigned getLineNo() const { return _lineno; }
   unsigned get_in(int i) const {return _in[i];}
   unsigned get_ID() { return _gateID;}
   bool get_invphase(int i) const {return _invphase[i];}
   bool get_fanin_UNDEF(int i) const{ return (_fanin[i]->getTypeInt() == 0);}
   string get_symbol() {return _symbol;}
   void push_fanin(CirGate*);
   void push_fanout(CirGate*);
   void set_symbol(string s){_symbol = s;}
   void clear(){}
   bool find_vec(vector<unsigned> vec) const{
    for(size_t i = 0; i < vec.size(); i++){
        if(vec[i] == _gateID){
          return true;
        }
    }
    return false;
   }
   // DFS funtions
   bool isGlobalRef(){ return (_ref == _GlobalRef); }
   void setToGlobalRef(){ _ref = _GlobalRef; }
   static void setGlobalRef() { _GlobalRef++; }
   void DFStraversal(GateList& dfslist);
   // Printing functions
   virtual void printGate() const = 0;
   bool with_FloatGates();
   bool not_used(){ return (_fanout.size() == 0); }
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanin_rcv(int, int, bool, vector<unsigned>&) const;
   void reportFanout(int level) const;
   void reportFanout_rcv(int, int, size_t, vector<unsigned>&) const;

private:
  GateType _type = UNDEF_GATE;
protected:
  // basic members
  unsigned _lineno;
  unsigned _gateID;
  GateList _fanin;
  GateList _fanout;
  vector<unsigned> _in;
  vector<bool> _invphase;
  string _symbol;
  // DFS members
  mutable unsigned _ref;
  static unsigned _GlobalRef ;
};

class CirPiGate : public CirGate
{
public:
  CirPiGate(unsigned , unsigned);
  ~CirPiGate(){}
  void printGate() const{ cout << _gateID; }
  string getTypeStr() const{ return "PI"; }
  int getTypeInt() const{ return int(_type); }
private:
  GateType _type = PI_GATE;
};

class CirPoGate : public CirGate
{
public:
  CirPoGate(unsigned, unsigned, bool, unsigned);
  ~CirPoGate(){}
  void printGate() const{ cout << _gateID; }
  string getTypeStr() const{ return "PO"; }
  int getTypeInt() const{ return int(_type); }
private:
  GateType _type = PO_GATE;
};

class CirAIGGate : public CirGate
{
public:
  CirAIGGate(unsigned, unsigned, bool, bool, unsigned, unsigned);
  ~CirAIGGate(){}
  void printGate() const{ cout << _gateID; }
  string getTypeStr() const{ return "AIG"; }
  int getTypeInt() const{ return int(_type); }
private:
  GateType _type = AIG_GATE;
};

class CirUNDEFGate : public CirGate
{
public:
  CirUNDEFGate(unsigned ID):CirGate(){ _gateID = ID;_lineno = 0;}
  ~CirUNDEFGate(){}
  void printGate() const{return;}
  string getTypeStr() const{ return "UNDEF"; }
  int getTypeInt() const{ return int(_type); }
private:
  GateType _type = UNDEF_GATE;
};

class CirCONST0Gate : public CirGate
{
public:
  CirCONST0Gate():CirGate(){}
  ~CirCONST0Gate(){}
  void printGate() const{return;}
  string getTypeStr() const{ return "CONST"; }
  int getTypeInt() const{ return int(_type); }
private:
  GateType _type = TOT_GATE;
};



#endif // CIR_GATE_H
