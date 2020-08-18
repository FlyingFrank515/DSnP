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
#include <unordered_map>
#include <set>
// #include "myHashSet.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
#include "cirGate.h"
extern CirMgr *cirMgr;
// class fec_group;
class CirMgr
{
friend class CirGate;
public:
	 CirMgr() {
		 CONST0 = new CirCONST0Gate();
		 _FECgroups = 0;
		 _init = false;
	 }
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
	 static CirGate* Const0;

	 // Member functions about circuit construction
	 void reset();
	 
	 // Member functions about circuit optimization
	 void sweep();
	 void optimize();
	 void update();

	 // Member functions about simulation
	 void randomSim();
	 void fileSim(ifstream&);
	 void setSimLog(ofstream *logFile) { _simLog = logFile; }
	 void simulate(vector<size_t>);
	 void update_FEC();
	 void set_FEC();
	 void sort_FEC();

	 // Member functions about fraig
	 void strash();
	 void strash_reset(){_Hashset.clear(); _Hash = false;}
	 void printFEC() const;
	 void fraig();
	 void genProofModel(SatSolver& s);
	 void get_SAT_result(pair<CirGateV*, CirGateV*>& , SatSolver& , bool& );
	 void reportResult(const SatSolver& , bool);

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
	 void printFECPairs() const;
	 void writeAag(ostream&) const;
	 void writeGate(ostream&, CirGate*) const;
	 void writelogs(size_t);

private:
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
	GateList _aiglist;
	GateList _undlist;
	CirGate* CONST0;
	// this is the gate_list->used to find gates with their ID
	map<unsigned, CirGate*>  _dict;
	map<unsigned, CirGate*>  _UNDEFlist;
	map<unsigned, string> _pi_symbol;
	map<unsigned, string> _po_symbol;
	vector<string> _symbol;
	vector<string> _comment;
	ofstream           *_simLog;

	//used for strash
	unordered_map <size_t , CirGate*> _Hashset;
	bool _Hash = false;
	
	
	// used for simulation
	vector<map<size_t, CirGateV*>>* _FECgroups;
	bool _init;

	// vector<fec_group*> _allFECgroups;
};
// used for simulation
// FEC'groups members are stored in the first gate
// class fec_group
// {
// public:
// 	fec_group(vector<CirGate*> n) {
// 		_member = n;
// 		for(size_t i = 0; i < _member.size();i++){
// 			_ID.insert(_member[i]->get_ID());
// 		}
// 	}
// 	fec_group() {}
// 	~fec_group() {reset();}
// 	void add(CirGate* n){_member.push_back(n); _ID.insert(n->get_ID());}
// 	bool check(CirGate* n){ return _ID.count(n->get_ID()) != 0;}
// 	size_t size(){return _member.size();}
// 	void reset(){ _member.clear(); _ID.clear();}
// 	bool operator == (const fec_group& n) const { return _member == n._member;}
// 	CirGate* operator [] (const size_t n) const { assert(n < _member.size()); return _member[n];}
// 	size_t get_hash_key(size_t j) const { return _member[j]->get_simvalue();}
// private:
// 	set<size_t>      _ID;
// 	vector<CirGate*> _member;
// };

#endif // CIR_MGR_H
