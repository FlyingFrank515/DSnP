/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"


using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
void merge(CirGate*, CirGate*, bool);

// use some function defined in "cirOpt.cpp"
extern void ban_fanout(CirGate* , CirGate*);
extern void ban_fanin(CirGate* , CirGate*);

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
	if(_Hash){
		cerr << "Error: circuit has been strashed!!" << endl;
		return;
	}
	unordered_map <size_t, CirGate*>:: iterator iter;
	for(size_t i = 0; i < _dfslist.size(); i++){
		if(_dfslist[i]->getTypeStr() != "AIG") continue;
		iter = _Hashset.find(_dfslist[i]->get_Hash_key());
		if(iter == _Hashset.end()){
			//no find->add it into set
			_Hashset[_dfslist[i]->get_Hash_key()] = _dfslist[i];
			// cout << "adding-> key: " << setw(4) <<  _dfslist[i]->get_Hash_key();
			// cout << "/value: " << _dfslist[i]->get_ID() << endl;
		}
		else{
			merge(iter->second, _dfslist[i], false);
			cout << "Strashing: " << iter->second->get_ID() << " merging " << _dfslist[i]->get_ID() <<"..." << endl;
			_dfslist[i]->delete_it();
		}
	}
	_Hash = true;
	this->update();
	_dfslist.clear();
	CirMgr::DFStraversal(_polist);
}
void
CirMgr::fraig()
{
	if(!_init) return;
	// vector<size_t> level;
	// bool o;
	// vector<size_t> pattern;
	// vector<vector<size_t>> pattern_Log;
	// for(size_t i = 0; i <_pilist.size(); i++){
	// 	pattern.push_back(0);
	// 	level.push_back(0);
	// }
	SatSolver solver;
	solver.initialize();
	genProofModel(solver);
	for(size_t i = 0; i <_FECgroups->size(); i++){
		map<size_t, CirGateV*>::iterator itra;
		for(itra = _FECgroups->at(i).begin(); itra != _FECgroups->at(i).end(); itra++){
			map<size_t, CirGateV*>::iterator itrb = itra;
			itrb++;
			for(; itrb != _FECgroups->at(i).end(); itrb++){
				if(itra->second->pass_or_not() || itrb->second->pass_or_not()) 
					continue;
				pair<CirGateV*, CirGateV*> target = make_pair(itra->second, itrb->second);
				
				bool result = false;
				get_SAT_result(target, solver, result);
				if(result)
				{
					itrb->second->pass();
					cout << "Fraig: ";
					bool inva = target.first->get_inv();
					bool invb = target.second->get_inv();
					if(inva) cout << "!";
					cout << target.first->get_VarID() << " ";
					cout << "merging ";
					if(invb) cout << "!";
					cout << target.second->get_VarID() << " ";
					cout << "..." << endl;
					
					// target.second->get_gate()->set_ID(target.first->get_gate()->get_ID());
					// target.second->get_gate()->_fanin = target.first->get_gate()->_fanin;
					// target.second->get_gate()->_invphase = target.first->get_gate()->_invphase;
					// if(inva != invb)
					// for(size_t i = 0; i < target.second->get_gate()->_invphase.size(); i++)
					// 	target.second->get_gate()->_invphase[i] != target.second->get_gate()->_invphase[i];
					
					
					// solver.addAigCNF(target.second->get_gate()->get_VarID(), target.second->get_gate()->get_fanin(0)->get_VarID(), target.second->get_gate()->get_invphase(0)
					// 									, target.second->get_gate()->get_fanin(1)->get_VarID(), target.second->get_gate()->get_invphase(1));
				}
			}
		}
	}
	vector<map<size_t, CirGateV*>>* new_FECgroups = new vector<map<size_t, CirGateV*>>;
	for(size_t i = 0; i < _FECgroups->size(); i++){
		map<size_t, CirGateV*>::iterator itra;
		map<size_t, CirGateV*> new_map;
		for(itra = _FECgroups->at(i).begin(); itra != _FECgroups->at(i).end(); itra++){
			if(itra->second->pass_or_not()){
				delete itra->second;
				itra->second = 0;
			}
			else{
				new_map[itra->first] = itra->second;
			}
		}
		if(new_map.size() < 2){
			delete new_map.begin()->second;
			new_map.begin()->second = 0;
			continue;
		}
		new_FECgroups->push_back(new_map);
	}
	vector<map<size_t, CirGateV*>>* garbage = _FECgroups;
	_FECgroups = new_FECgroups;
	delete garbage;
	garbage = 0;
	set_FEC();
	// for(size_t i = 0; i < pattern_Log.size(); i++){
	// 	simulate(pattern_Log[i]);
	// }
}
void
CirMgr::genProofModel(SatSolver& s)
{
   // Allocate and record variables; No Var ID for POs
   for(size_t i = 0; i< _dfslist.size(); i++) {
      if(_dfslist[i]->getTypeStr() != "PO"){
			Var v = s.newVar();
			_dfslist[i]->set_varID(v);
		}
   }

   // Hard code the model construction here...
   for(size_t i = 0; i< _aiglist.size(); i++){
		s.addAigCNF(_aiglist[i]->get_VarID(), _aiglist[i]->get_fanin(0)->get_VarID(), _aiglist[i]->get_invphase(0)
														, _aiglist[i]->get_fanin(1)->get_VarID(), _aiglist[i]->get_invphase(1));
	}
}
void CirMgr::get_SAT_result(pair<CirGateV*, CirGateV*>& target, SatSolver& solver, bool& result){
	Var newV = solver.newVar();
	solver.addXorCNF(newV, target.first->get_VarID() , target.first->get_inv()
								, target.second->get_VarID(), target.second->get_inv());
	solver.assumeRelease();  // Clear assumptions
	solver.assumeProperty(newV, true);  // k = 1
	result = solver.assumpSolve();
		// reportResult(solver, iter->second);
}
void CirMgr::reportResult(const SatSolver& solver, bool result)
{
   solver.printStats();
   cout << (result? "SAT" : "UNSAT") << endl;
   if (result) {
      for(size_t i = 0; i< _aiglist.size(); i++)
         cout << solver.getValue(_aiglist[i]->get_VarID()) << endl;
   }
}
/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void merge(CirGate* target, CirGate* banned, bool inv = false){
	for(size_t i = 0; i < banned->_fanout.size(); i++){
		target->_fanout.push_back(banned->_fanout[i]);
		for(size_t j = 0; j < banned->_fanout[i]->_fanin.size();j++){
			if(banned->_fanout[i]->_fanin[j] == banned){
				banned->_fanout[i]->_fanin[j] = target;
				if(inv) {
					banned->_fanout[i]->set_invphase(0, !target->get_invphase(0));
					banned->_fanout[i]->set_invphase(1, !target->get_invphase(1));
				}
			}
		}
	}
	for(size_t i = 0; i < banned->_fanin.size(); i++){
		ban_fanout(banned->_fanin[i], banned);
	}
}