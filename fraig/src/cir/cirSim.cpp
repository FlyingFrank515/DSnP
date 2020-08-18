/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <math.h>

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
bool readPattern(vector<size_t>& , string , size_t);
/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
	size_t FAIL_TIMES = pow(log(_aiglist.size()),2);
	vector<size_t> rand_pattern;
	RandomNumGen seed;
	RandomNumGen machine(seed(INT_MAX));
	for(size_t i = 0; i <_pilist.size(); i++){
		size_t n = machine(INT_MAX) + ((size_t)machine(INT_MAX) << 32);
		rand_pattern.push_back(n);
	}
	size_t FEC = 0;
	size_t times = 0;
	size_t count = 0;
	do{
		assert(rand_pattern.size() == _pilist.size());
		simulate(rand_pattern);
		count += 64;
		if(_simLog != 0) writelogs(64);
		if(_FECgroups->size() == FEC) times += 1;
		else{
			FEC = _FECgroups->size();
			times = 0;
		}
		for(size_t i = 0; i <_pilist.size(); i++){
			size_t n = machine(INT_MAX) + ((size_t)machine(INT_MAX) << 32);
			rand_pattern[i] = n;
		}
	} while(times < FAIL_TIMES );
	cout << count <<" patterns simulated." << endl;	
	// if(!_FECgroups) cout << _FECgroups->size() << " FECgroups found." << endl;	
	sort_FEC();
	set_FEC(); 
}

void
CirMgr::fileSim(ifstream& patternFile)
{
	// do not use buffer->let memory boom
	string buffer;
	size_t level = 0;
	size_t count = 0;
	vector<size_t> pattern;
	vector<bitset<64>> pattern_bit;
	for(size_t i = 0; i < _pilist.size();i++){
		pattern.push_back(0);
	}
   while(patternFile >> buffer){
		assert(pattern.size() == _pilist.size());
		if(!readPattern(pattern, buffer, level)){
			cout << "0 patterns simulated." << endl;
			return;
		}
		level++;
		count += 1;
		
		// if(count > 22848) cout << buffer[0] << endl;
		if(level > 63){
			level = 0;
			this->simulate(pattern);
			if(_simLog != 0) writelogs(64);
			pattern.clear();
			for(size_t i = 0; i < _pilist.size();i++){
				pattern.push_back(0);
			}
		}
	}
	if(level != 0) this->simulate(pattern);
	if(_simLog != 0) writelogs(level);
	// cout << "last set " << _pilist[0]->get_ID() << " to " << _pilist[0]->get_simvalue() << endl;
	cout << count <<" patterns simulated." << endl;	
	// if(!_FECgroups) cout << _FECgroups->size() << " FECgroups found." << endl;	
	sort_FEC();
	set_FEC(); 
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
bool readPattern(vector<size_t>& pattern, string buffer, size_t level){
	assert(level < 64);
	if(buffer.size() != pattern.size()){
		cerr << "Error: Pattern("<<buffer<<") length("<<buffer.size()<<
		") does not match the number of inputs("<< pattern.size()<<") in a circuit!!"<< endl;
		return false;
	}
	assert(pattern.size() == buffer.size());
	for(size_t j = 0; j < pattern.size(); j++){
		assert(j < buffer.size());
		if(buffer[j] != '0' && buffer[j] != '1'){
			cerr << "Error: Pattern("<<buffer<<"contains a non-0/1 character('"<<buffer[j]<<"')."<<endl;
			return false;
		}
		assert(buffer[j] == '0' || buffer[j] == '1');
		if(buffer[j] == '1') pattern[j] += ((size_t)1 << level);
	}
	return true;
}
void CirMgr::simulate(vector<size_t> pattern){
	assert(pattern.size() == _pilist.size());
	for(size_t i = 0; i < _pilist.size(); i++){
		_pilist[i]->set_simvalue(pattern[i]);
		// cout << "set " << _pilist[i]->get_ID() << " to " << pattern[i] << endl;
	}
	// cout <<"start simulation" << endl;
	
	//use all-gate simulation(correct????)
	for(size_t i = 0; i < _dfslist.size(); i++){
		_dfslist[i]->simulate();
	}
	update_FEC();
}
void CirMgr::update_FEC(){	
	//================================================================
	//=  Warning::maybe need to delete CirGateV in futuretimes ?!?!  =
	//================================================================
	map<size_t, CirGateV*> blank;
	if(!_init){
		_FECgroups = new vector<map<size_t, CirGateV*>>;
		_FECgroups->clear();
		assert(_FECgroups->empty());
		_FECgroups->push_back(blank);
		assert(!_FECgroups->empty());
		CirGateV* temp = new CirGateV(CONST0, false);
		_FECgroups->at(0)[0] = temp;
		CONST0->init_fec();
		for(size_t i = 0; i < _dfslist.size(); i++){
			if(_dfslist[i]->getTypeStr() == "AIG"){
				temp = new CirGateV(_dfslist[i], false);
				_FECgroups->at(0)[temp->get_ID()] = temp;
				_dfslist[i]->init_fec();
			}
		}
		_init = true;
	}
	vector<map<size_t, CirGateV*>>* new_FECgroups = new vector<map<size_t, CirGateV*>>;
	// cout << "no need to initialize" << endl;
	assert(!_FECgroups->empty());
	size_t key;
	for(size_t i = 0; i < _FECgroups->size(); i++){
		
		unordered_map<size_t, map<size_t, CirGateV*>> sim_map;
		vector<size_t> key_list;
		map<size_t, CirGateV*>::iterator iter;
		
		for(iter = _FECgroups->at(i).begin(); iter != _FECgroups->at(i).end(); iter++){
			key = iter->second->get_simvalue();
			if(iter->second->get_inv()) key = ~key;
			unordered_map<size_t, map<size_t, CirGateV*>>::iterator ita = sim_map.find(key);
			unordered_map<size_t, map<size_t, CirGateV*>>::iterator itb = sim_map.find(~key);
			if(ita != sim_map.end()){
				sim_map[key][iter->first] = iter->second;
			}
			else if(itb != sim_map.end()){
				// inv()->creating from new
				CirGateV* temp = new CirGateV(iter->second->get_gate(), 1);
				sim_map[~key][iter->first] = temp;
			}
			else{
				sim_map[key] = blank;
				sim_map[key][iter->first] = iter->second;
				key_list.push_back(key);
			}
		}
		
		for(size_t j = 0; j < key_list.size(); j++){
			if(sim_map[key_list[j]].size() < 2){
				delete sim_map[key_list[j]].begin()->second;
				sim_map[key_list[j]].begin()->second = 0;
				continue;
			}
			new_FECgroups->push_back(sim_map[key_list[j]]);
		}
	}
	vector<map<size_t, CirGateV*>>* garbage = _FECgroups;
	_FECgroups = new_FECgroups;
	delete garbage;
	garbage = 0;
}
void CirMgr::set_FEC(){
	for(size_t i = 0; i < _FECgroups->size(); i++){
		map<size_t, CirGateV*>::iterator iter;
		map<size_t, CirGateV*>* address = &_FECgroups->at(i);
		for(iter = _FECgroups->at(i).begin(); iter != _FECgroups->at(i).end(); iter++){
			iter->second->set_fec(address);
		}
	}
}