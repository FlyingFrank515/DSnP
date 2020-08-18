/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <algorithm>

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
void jump_connect(CirGate*, CirGate*, CirGate*, bool);
void ban_fanout(CirGate* , CirGate*);
void ban_fanin(CirGate* , CirGate*);
/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
  GateList new_aig;
  GateList new_und;
  map<unsigned, CirGate*> new_dict;
  vector<unsigned> del;
  for(size_t i = 0; i < _aiglist.size(); i++){
    if(!_aiglist[i]->reachable()){
      CirGate* garbage = _aiglist[i];
      del.push_back(garbage->get_ID());
      cout << "Sweeping: AIG(" << garbage->get_ID() << ") removed..." << endl;
      delete garbage;
    }
    else {
      new_aig.push_back(_aiglist[i]);
    }
  }

  for(size_t i = 0; i < _undlist.size(); i++){
    if(!_undlist[i]->reachable()){
      CirGate* garbage = _undlist[i];
      del.push_back(garbage->get_ID());
      cout << "Sweeping: UNDEF(" << garbage->get_ID() << ") removed..." << endl;
      delete garbage;
    }
    else {
      new_und.push_back(_undlist[i]);
    }
  }
  map<unsigned, CirGate*>::iterator iter;
  for(iter = _dict.begin(); iter !=_dict.end(); iter++){
      if(find(del.begin(), del.end(), iter->first) == del.end()){
        unsigned key = iter->first;
        CirGate* value = iter->second;
        new_dict[key] = value;
      }
  }
  _aiglist = new_aig;
  _undlist = new_und;
  _dict = new_dict;
	
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
	for(size_t i = 0; i < _dfslist.size(); i++){
	   if(_dfslist[i]->getTypeStr() != "AIG") continue;
	
	//case1:Fanin has constant 1
      if(_dfslist[i]->_fanin[0]->getTypeStr() == "CONST" && _dfslist[i]->get_invphase(0)){
			for(size_t j = 0; j < _dfslist[i]->_fanout.size(); j++)
				jump_connect(_dfslist[i], _dfslist[i]->_fanin[1],  _dfslist[i]->_fanout[j], _dfslist[i]->get_invphase(1));
			ban_fanout(_dfslist[i]->_fanin[0], _dfslist[i]);
			_dfslist[i]->delete_it();
			//DEBUG--------------------------------------------------------
			string output = "Simplifying: "+ to_string(_dfslist[i]->_fanin[1]->get_ID()) +" merging ";
			if(_dfslist[i]->get_invphase(1)) output += "!";
			output += (to_string(_dfslist[i]->get_ID()) + "...");
			cout << output << endl;
			//-------------------------------------------------------------
			continue;
      }
		if(_dfslist[i]->_fanin[1]->getTypeStr() == "CONST" && _dfslist[i]->get_invphase(1)){
			for(size_t j = 0; j < _dfslist[i]->_fanout.size(); j++)
				jump_connect(_dfslist[i], _dfslist[i]->_fanin[0],  _dfslist[i]->_fanout[j], _dfslist[i]->get_invphase(0));
			ban_fanout(_dfslist[i]->_fanin[1], _dfslist[i]);
			_dfslist[i]->delete_it();
			//DEBUG--------------------------------------------------------
			string output = "Simplifying: "+ to_string(_dfslist[i]->_fanin[0]->get_ID()) +" merging ";
			if(_dfslist[i]->get_invphase(0)) output += "!";
			output += (to_string(_dfslist[i]->get_ID()) + "...");
			cout << output << endl;
			//-------------------------------------------------------------
			continue;
      }
   
	//case2:Fanin has constant 0
		if(_dfslist[i]->_fanin[0]->getTypeStr() == "CONST" && !_dfslist[i]->get_invphase(0)){
			for(size_t j = 0; j < _dfslist[i]->_fanout.size(); j++){
				jump_connect(_dfslist[i], _dict[0], _dfslist[i]->_fanout[j], _dfslist[i]->get_invphase(1));	
			}
			ban_fanout(_dfslist[i]->_fanin[1], _dfslist[i]);
			_dfslist[i]->delete_it();
			//DEBUG--------------------------------------------------------
			string output = "Simplifying: 0 merging ";
			output += (to_string(_dfslist[i]->get_ID()) + "...");
			cout << output << endl;
			//-------------------------------------------------------------
			continue;
		}
		if(_dfslist[i]->_fanin[1]->getTypeStr() == "CONST" && !_dfslist[i]->get_invphase(1)){
			for(size_t j = 0; j < _dfslist[i]->_fanout.size(); j++){
				jump_connect(_dfslist[i], _dict[0], _dfslist[i]->_fanout[j], _dfslist[i]->get_invphase(0));	
			}
			ban_fanout(_dfslist[i]->_fanin[0], _dfslist[i]);
			_dfslist[i]->delete_it();
			//DEBUG--------------------------------------------------------
			string output = "Simplifying: 0 merging ";
			output += (to_string(_dfslist[i]->get_ID()) + "...");
			cout << output << endl;
			//-------------------------------------------------------------
			continue;
		}
   
	//case3:Identical fanins
		if(_dfslist[i]->_fanin[1]->get_ID() == _dfslist[i]->_fanin[0]->get_ID() && _dfslist[i]->get_invphase(0) == _dfslist[i]->get_invphase(1)){
			for(size_t j = 0; j < _dfslist[i]->_fanout.size(); j++){
				jump_connect(_dfslist[i], _dfslist[i]->_fanin[0], _dfslist[i]->_fanout[j], _dfslist[i]->get_invphase(0));
			}
			_dfslist[i]->delete_it();
			//DEBUG--------------------------------------------------------
			string output = "Simplifying: "+ to_string(_dfslist[i]->_fanin[1]->get_ID()) +" merging ";
			if(_dfslist[i]->get_invphase(0)) output += "!";
			output += (to_string(_dfslist[i]->get_ID()) + "...");
			cout << output << endl;
			//-------------------------------------------------------------
			continue;
		}
   
	//case4:inverted fanins
		if(_dfslist[i]->_fanin[1]->get_ID() == _dfslist[i]->_fanin[0]->get_ID() && _dfslist[i]->get_invphase(0) != _dfslist[i]->get_invphase(1)){
			for(size_t j = 0; j < _dfslist[i]->_fanout.size(); j++){
				jump_connect(_dfslist[i], _dict[0], _dfslist[i]->_fanout[j], false);
			}
			ban_fanout(_dfslist[i]->_fanin[0], _dfslist[i]);
			_dfslist[i]->delete_it();
			//DEBUG--------------------------------------------------------
			string output = "Simplifying: 0 merging ";
			output += (to_string(_dfslist[i]->get_ID()) + "...");
			cout << output << endl;
			//-------------------------------------------------------------
			continue;
		}
   }
	// cout << "update begin" << endl;
	this->update();
	// cout << "update success" << endl;
	_dfslist.clear();
	CirMgr::DFStraversal(_polist);
	strash_reset();
	// cout << "traverse success" << endl;
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
void jump_connect(CirGate* now, CirGate* last, CirGate* next, bool inv){
	for(size_t i = 0; i < now->_fanout.size(); i++){
		last->_fanout.push_back(now->_fanout[i]);
	}
	ban_fanout(last, now);
	for(size_t i = 0; i < next->_fanin.size(); i++){
		if(next->_fanin[i] == now){
			next->_fanin[i] = last;
			if(inv == next->get_invphase(i)) next->set_invphase(i,false);
			else if(inv != next->get_invphase(i)) next->set_invphase(i,true);
		}
	}
}
void ban_fanout(CirGate* target, CirGate* banned){
	GateList new_fanout;
	for(size_t j = 0; j < target->_fanout.size(); j++){
		if(target->_fanout[j] != banned){
			new_fanout.push_back(target->_fanout[j]);
		}
	}
	target->_fanout = new_fanout;
}
void ban_fanin(CirGate* target, CirGate* banned){
	GateList new_fanin;
	for(size_t j = 0; j < target->_fanin.size(); j++){
		if(target->_fanin[j] != banned){
			new_fanin.push_back(target->_fanin[j]);
		}
	}
	target->_fanout = new_fanin;
}
void CirMgr::update(){
	GateList new_und;
	for(size_t i = 0; i < _undlist.size(); i++){
		_undlist[i]->DFSreset();
		if(!_undlist[i]->not_used()){
			new_und.push_back(_undlist[i]);
		}
		else{
			for(size_t j = 0; j < _undlist[i]->_fanout.size(); j++){
				ban_fanin(_undlist[i]->_fanout[j], _undlist[i]);
			}
			delete _undlist[i];
		}
	}
	_undlist = new_und;
	GateList new_aig;
	GateList del_list;
	for(size_t i = 0; i < _aiglist.size(); i++){
		if(!_aiglist[i]->delete_or_not()){
			new_aig.push_back(_aiglist[i]);
		}
		else{
			// cout << "delete" << _aiglist[i]->get_ID() << endl; 
			del_list.push_back(_aiglist[i]);
		}
	}
	_aiglist = new_aig;
	map<unsigned, CirGate*>::iterator iter;
	map<unsigned, CirGate*> new_dict;
	for(iter = _dict.begin(); iter != _dict.end(); iter++){
		iter->second->DFSreset();
		if(!iter->second->delete_or_not())
			new_dict[iter->first] = iter->second;
	}
	_dict = new_dict;
	for(size_t i = 0; i < del_list.size(); i++) delete del_list[i];
}
