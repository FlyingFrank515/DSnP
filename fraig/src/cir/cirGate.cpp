/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;
// TODO: Implement memeber functions for class(es) in cirGate.h
unsigned CirGate::_GlobalRef = 0;
/**************************************/
/*   class CirGate member functions   */
/**************************************/
CirGate::CirGate(unsigned no, unsigned ID):_lineno(no), _gateID(ID){
   _ref = 0;
   _symbol = "";
}
void CirGate::reset(){
   _fanin.clear();
   _fanout.clear();
   _fec = 0;
   _lineno = 0;
   _gateID = 0;
   _simvalue = 0;
}

// used to get HashKey
// method:Cantor pairing function
// key = (a + b) * (a + b + 1) / 2 + a; where a, b >= 0
// need to sort a b at first
size_t
CirGate::get_Hash_key () const 
{
   size_t key, a, b;
   assert(_fanin.size() > 1);
   a = 2*_fanin[0]->get_ID();
   if(get_invphase(0)) a += 1;
   b = 2*_fanin[1]->get_ID();
   if(get_invphase(1)) b += 1;
   // check a >= b
   if(a < b){
      size_t tmp = a;
      a = b;
      b = tmp;
   }
   key = (a + b) * (a + b + 1) / 2 + a;
   return key;
}

void CirGate::simulate(){
   if(getTypeStr() == "AIG" || getTypeStr() == "PO"){
      size_t new_simvalue0 = _fanin[0]->get_simvalue();
      if(get_invphase(0)) new_simvalue0 = ~new_simvalue0;
      size_t new_simvalue1 = ~0;
      if(_fanin.size() > 1){
         new_simvalue1 = _fanin[1]->get_simvalue();
         if(get_invphase(1)) new_simvalue1 = ~new_simvalue1;
      }
      size_t new_simvalue = new_simvalue1 & new_simvalue0;
      set_simvalue(new_simvalue);
      return;
   }
   return;
}  
   // //Recursively simulate each fanin.
   // //If (no fanin has value change) return false;
   // //Simulate this gate;
   // //if (value changed) return true;
   // //return false;
   // if(getTypeStr() == "CONST") return false;
   // if(getTypeStr() == "UNDEF") return false;
   // if(getTypeStr() == "PI") return false;
   // if(getTypeStr() == "PO"){
   //    assert(_fanin.size() == 1);
      
   //    //initialize
   //    if(get_simvalue() == 0){
   //       size_t new_simvalue = _fanin[0]->get_simvalue();
   //       if(get_invphase(0)) new_simvalue = ~new_simvalue;
   //       set_simvalue(new_simvalue);
   //    }
      
   //    if(!_fanin[0]->simulate()) return false;
      
   //    size_t old_simvalue = get_simvalue();
   //    size_t new_simvalue = _fanin[0]->get_simvalue();
   //    if(get_invphase(0)) new_simvalue = ~new_simvalue;
   //    set_simvalue(new_simvalue);
      
   //    cout << "set " << get_ID() << " to " << new_simvalue << endl;

   //    if(old_simvalue != new_simvalue) return true;
   //    return false;
   // }
   // if(getTypeStr() == "AIG"){
   //    assert(_fanin.size() == 2);
      
   //    //initialize
   //    if(get_simvalue() == 0){
   //       size_t new_simvalue0 = _fanin[0]->get_simvalue();
   //       if(get_invphase(0)) new_simvalue0 = ~new_simvalue0;
   //       size_t new_simvalue1 = _fanin[1]->get_simvalue();
   //       if(get_invphase(1)) new_simvalue1 = ~new_simvalue1;
   //       size_t new_simvalue = new_simvalue1 & new_simvalue0;
   //       set_simvalue(new_simvalue);
   //    }

   //    if(!(_fanin[0]->simulate()) && !(_fanin[1]->simulate())) return false;
      
   //    size_t old_simvalue = get_simvalue();
   //    size_t new_simvalue0 = _fanin[0]->get_simvalue();
   //    if(get_invphase(0)) new_simvalue0 = ~new_simvalue0;
   //    size_t new_simvalue1 = _fanin[1]->get_simvalue();
   //    if(get_invphase(1)) new_simvalue1 = ~new_simvalue1;
   //    size_t new_simvalue = new_simvalue1 & new_simvalue0;
   //    set_simvalue(new_simvalue);

   //    cout << "set " << get_ID() << " to " << new_simvalue << endl;
      
   //    if(old_simvalue != new_simvalue) return true;
   //    return false;
   // }
   // return false;


void CirGate::push_fanin(CirGate* to){
  _fanin.push_back(to);
}

void CirGate::push_fanout(CirGate* to){
  _fanout.push_back(to);
}

void CirGate::DFStraversal(GateList& dfslist){
   for(size_t i = 0; i < _fanin.size(); i++){
      CirGate* next = _fanin[i];
      //UNDEF ->pass
      if(next->getTypeInt() == 0){
         next->_dfs = true;
         continue;
      }
      if(!(next->isGlobalRef())){
         next->setToGlobalRef();
         next->DFStraversal(dfslist);
      }
   }
   dfslist.push_back(this);
   this->_dfs = true;
}
bool CirGate::with_FloatGates(){
   for(size_t i = 0; i < _fanin.size(); i++){
      if(_fanin[i]->getTypeInt() == 0)
         return true;
   }
   return false;
}


/**************************************/
/*   class CirPiGate member functions */
/**************************************/
CirPiGate::CirPiGate(unsigned no, unsigned ID):CirGate(no, ID)
{
}
/**************************************/
/*   class CirPoGate member functions */
/**************************************/
CirPoGate::CirPoGate(unsigned no, unsigned ID, bool inv, unsigned fanin):CirGate(no, ID)
{
   _in.push_back(fanin);
   _invphase.push_back(inv);
}

/**************************************/
/*   class CirAIGGate member functions */
/**************************************/
CirAIGGate::CirAIGGate(unsigned no, unsigned ID, bool inv1, bool inv2, unsigned fanin1, unsigned fanin2):CirGate(no, ID)
{
   _in.push_back(fanin1);
   _invphase.push_back(inv1);
   _in.push_back(fanin2);
   _invphase.push_back(inv2);
}
/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{  
   cout << "================================================================================" << endl;
   string output = "= "+ getTypeStr()+ "("+ to_string(_gateID) + ")";
   if(_symbol != "") output += ("\""+_symbol+ "\"");
   output += (", line " + to_string(_lineno));
   cout << output << endl; 
   
   cout << "= FECs:";
   if(_fec_init){
      assert(_fec->find(this->_gateID) != _fec->end());
      bool my =  _fec->find(this->_gateID)->second->get_inv();
      for(map<size_t, CirGateV*>::iterator j = _fec->begin(); j !=  _fec->end(); j++){
         if(j->first == this->_gateID) continue;
         cout << " ";
         if(j->second->get_inv() != my) cout << "!";
         cout << j->first;
      }
   }
   cout << endl;
   
   bitset<64> sim(_simvalue);
   cout << "= Value: ";
   
   if(_fec_init){
      for(size_t i = 64; i > 0; i--){
         cout << sim[i-1];
         if(i%8 == 1 && i != 1) cout << "_";
      }
   }
   else{
      for(size_t i = 64; i > 0; i--){
         cout << '0';
         if(i%8 == 1 && i != 1) cout << "_";
      }
   }
   
   cout << endl;
   
   cout << "================================================================================" << endl;
}

void smaller_mine_HW6()
{
   // void
   // CirGate::reportFanin(int level) const
   // {
   //    assert (level >= 0);
   //    vector<unsigned> _ID;
   //    string output = (getTypeStr() + " " + to_string(_gateID));
   //    _ID.push_back(_gateID);
   //    cout << output << endl;
   //    for(size_t i = 0; i < _fanin.size(); i++){
   //       bool _inv;
   //       if(_invphase[i]) _inv = true;
   //       else _inv = false;
   //       _ID.push_back(_gateID);
   //       _fanin[i]->reportFanin_rcv(level, level-1, _inv, _ID);
   //    }
   // }

   // void CirGate::reportFanin_rcv(int level, int now, bool inv, vector<unsigned>& _ID) const{
   //    for(int j = 0; j< (level-now); j++) cout << "  ";
   //    if(inv) cout << "!";
   //    string output = (getTypeStr() + " " + to_string(_gateID));
   //    if(find_vec(_ID)){
   //       if(getTypeInt() != 1 && getTypeInt() != 0 && getTypeInt() != 5)
   //          output += " (*)";
   //       cout << output << endl;
   //       return;
   //    }
   //    cout << output << endl;
   //    for(size_t i = 0; i < _fanin.size(); i++){
   //       bool _inv;
   //       if(_invphase[i]) _inv = true;
   //       else _inv = false;
   //       if(now - 1 >= 0){
   //          _ID.push_back(_gateID);
   //          _fanin[i]->reportFanin_rcv(level, now-1, _inv, _ID);
   //       }   
   //    }
   // }

   // void
   // CirGate::reportFanout(int level) const
   // {
   //    assert (level >= 0);
   //    vector<unsigned> _ID;
   //    string output = (getTypeStr() + " " + to_string(_gateID));
   //    _ID.push_back(_gateID);
   //    cout << output << endl;
   //    size_t address = (size_t)this; 
   //    for(size_t i = 0; i < _fanout.size(); i++){
   //       _ID.push_back(_gateID);
   //       _fanout[i]->reportFanout_rcv(level, level-1, address, _ID);
   //    }
   // }

   // void CirGate::reportFanout_rcv(int level, int now, size_t last, vector<unsigned>& _ID) const{
   //    for(int j = 0; j< (level-now); j++) cout << "  ";
   //    for(size_t j = 0; j < _fanin.size(); j++){
   //       if(_fanin[j] == (CirGate*)last && _invphase[j] == true){
   //          cout << "!";
   //          break;
   //       }
   //    }
   //    string output = (getTypeStr() + " " + to_string(_gateID));
   //    if(find_vec(_ID)){
   //       if(getTypeInt() != 1 && getTypeInt() != 0 && getTypeInt() != 5)
   //          output += " (*)";
   //       cout << output << endl;
   //       return;
   //    }
   //    cout << output << endl;
   //    for(size_t i = 0; i < _fanout.size(); i++){
   //       bool _inv = false;
   //       if(_invphase[i]) _inv = true;
   //       else _inv = false;
   //       if(now - 1 >= 0){
   //          size_t address = (size_t)this; 
   //          _ID.push_back(_gateID);
   //          _fanout[i]->reportFanout_rcv(level, now-1, address, _ID);

   //       }
   //    }
   // }
}
void
CirGate::reportFanin(int level)
{
   assert (level >= 0);
   setGlobalRef();
   _dfsFanin(this, 0, 0, level);
}

void
CirGate::_dfsFanin(CirGate* g, unsigned spaces, bool inv, int level) {
   for (size_t i = 0; i < spaces; ++i) cout << " ";
   if (inv) cout << "!";
   cout << g->getTypeStr() << " " << g->_gateID;
   // check if need to add (*)
   if (g->isGlobalRef() && level > 0 && !g->_fanin.empty()) {
      cout << " (*)" << endl; return;
   }
   else cout << endl;
   // dfs next level
   if (level == 0) return;
   g->setToGlobalRef();
   for (size_t i = 0; i < g->_fanin.size(); ++i) {
      _dfsFanin(g->_fanin[i], spaces + 2, g->_invphase[i], level - 1);
   }
}

void
CirGate::reportFanout(int level)
{
   assert (level >= 0);
   setGlobalRef();
   _dfsFanout(this, 0, 0, level);
}

void
CirGate::_dfsFanout(CirGate* g, unsigned spaces, bool inv, int level) {
   for (size_t i = 0; i < spaces; ++i) cout << " ";
   if (inv) cout << "!";
   cout << g->getTypeStr() << " " << g->_gateID;
   // check if need to add (*)
   if (g->isGlobalRef() && level > 0 && !g->_fanout.empty()) {
      cout << " (*)" << endl; return;
   }
   else cout << endl;
   // dfs next level
   if (level == 0) return;
   g->setToGlobalRef();
   for (size_t i = 0; i < g->_fanout.size(); ++i) {
      bool _inv = false;
      CirGate* next = g->_fanout[i]; 
      for(size_t j = 0; j < next->_fanin.size(); j++){
         if(next->_fanin[j] == g && next->_invphase[j] == true){
            _inv = true;
            break;
         }
      }
      _dfsFanout(g->_fanout[i], spaces + 2, _inv, level - 1);
   }
}

