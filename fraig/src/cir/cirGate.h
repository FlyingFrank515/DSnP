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
#include "sat.h"
#include <bitset>
#include <map>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.
class CirGate;
class CirGateV;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
friend class CirGateV;
friend class CirMgr;
public:
   CirGate():_gateID(0),_lineno(0), _simvalue(0){}
   CirGate(unsigned, unsigned);
   ~CirGate(){ }
   
   //for convenience let this two member be public
   GateList _fanin;
   GateList _fanout;
   
   // Basic access methods
   void reset();
	 virtual string getTypeStr() const = 0;
   virtual int getTypeInt() const = 0;
   virtual bool isAig() const = 0;
   unsigned getLineNo() const { return _lineno; }
   unsigned get_in(int i) const {return _in[i];}
   void set_ID(unsigned n) {_gateID = n;}
   unsigned get_ID() { return _gateID;}
   CirGate* get_fanin(int i ){ return _fanin[i]; }
   bool get_invphase(int i) const {return _invphase[i];}
   void set_invphase(int i, bool inv) { _invphase[i] = inv;}
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
   void delete_it(){ _delete = true;}
   bool delete_or_not(){ return _delete;}
   
   // DFS funtions
   bool isGlobalRef(){ return (_ref == _GlobalRef); }
   void setToGlobalRef(){ _ref = _GlobalRef; }
   static void setGlobalRef() { _GlobalRef++; }
   void DFSreset() {_dfs = false;}
   void DFStraversal(GateList& dfslist);
   bool reachable(){ return _dfs;}

   // simulate functions
   size_t get_simvalue() {
		if(getTypeStr() == "UNDEF") return 0;
		if(getTypeStr() == "CONST") return 0;
		return _simvalue;
	 }
   void set_simvalue(size_t sim) {_simvalue = sim;}
   void simulate();
   void set_fec(map<size_t, CirGateV*>* n ){_fec = n;}
   void init_fec(){_fec_init = true;}
   void dis_init_fec(){_fec_init = false;}

   // friag functions
   void set_varID(const Var& n){_varID = n;}
   Var get_VarID(){return _varID;}

   // Printing functions
   virtual void printGate() const = 0;
   bool with_FloatGates();
   bool not_used(){ return (_fanout.size() == 0); }
   void reportGate() const;
  
  //  void reportFanin(int level) const;
  //  void reportFanin_rcv(int, int, bool, vector<unsigned>&) const;
  //  void reportFanout(int level) const;
  //  void reportFanout_rcv(int, int, size_t, vector<unsigned>&) const;
  //===========================================
  void reportFanin(int level);
  void reportFanout(int level);
  //===========================================

  // operator and function for strash
  bool operator == (const CirGate& n) const { return _gateID == n._gateID; }
  size_t get_Hash_key () const;


private:
  GateType _type = UNDEF_GATE;
  //===========================================
  void _dfsFanin(CirGate*, unsigned, bool, int);
  void _dfsFanout(CirGate*, unsigned, bool, int);
  //===========================================
protected:
  // basic members
  unsigned _lineno;
  unsigned _gateID;
  vector<unsigned> _in;
  vector<bool> _invphase;
  string _symbol;
  
  // DFS members
  mutable unsigned _ref;
  static unsigned _GlobalRef ;
  bool _dfs = false;
  bool _delete = false;
  
  // simulate members
  size_t _simvalue;
  map<size_t, CirGateV*>* _fec;
  bool _fec_init;

  // fraig members
  Var _varID;

};

class CirPiGate : public CirGate
{
public:
  CirPiGate(unsigned , unsigned);
  ~CirPiGate(){}
  void printGate() const{ cout << _gateID; }
  string getTypeStr() const{ return "PI"; }
  int getTypeInt() const{ return int(_type); }
  bool isAig() const {return false;}
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
  bool isAig() const {return false;}
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
  bool isAig() const {return true;}
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
  bool isAig() const {return false;}
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
  bool isAig() const {return false;}
private:
  GateType _type = TOT_GATE;
};

// used for simulation
class CirGateV
{
friend class CirGate;
public:
	CirGateV(CirGate* g, bool i = false):_gate(g),_inv(i),_pass(false){}
	~CirGateV(){ _gate = 0; _inv = false;}
	unsigned get_ID(){ return _gate->get_ID();}
	bool get_inv(){ return _inv;}
  CirGate* get_gate(){return _gate;}
  Var get_VarID(){return _gate->get_VarID();}
	size_t get_simvalue(){
		assert(_gate != 0);
		if(_gate->getTypeStr() == "UNDEF") return 0;
		if(_gate->getTypeStr() == "CONST") return 0;
		return _gate->get_simvalue();
	}
  void set_fec(map<size_t, CirGateV*>* n ){_gate->set_fec(n);}
	CirGateV* inv(){ 
		CirGateV* temp = new CirGateV(this->_gate, 1);
		return temp;
	}
  void pass(){ _pass = true;}
  bool pass_or_not(){ return _pass;}

private:
	CirGate* _gate;
	bool _inv;
  bool _pass;
};

#endif // CIR_GATE_H
