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
#include <algorithm>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

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


void CirGate::push_fanin(CirGate* to){
  _fanin.push_back(to);
}

void CirGate::push_fanout(CirGate* to){
  _fanout.push_back(to);
}

void CirGate::DFStraversal(GateList& dfslist){
   for(size_t i = 0; i < _fanin.size(); i++){
      CirGate* next = _fanin[i];
      //UNDEF or CONST0->pass
      if(next->getTypeInt() == 0) continue;
      // if(next->getTypeInt() == 5) continue;
      if(!(next->isGlobalRef())){
         next->setToGlobalRef();
         next->DFStraversal(dfslist);
      }
   }
   dfslist.push_back(this);
}
bool CirGate::with_FloatGates(){
   for(size_t i = 0; i < _fanin.size(); i++){
      if(_fanin[i]->getTypeInt() == 0)
         return true;
   }
   return false;
}
void
CirGate::reportGate() const
{  
   cout << "==================================================" << endl;
   string output = "= "+ getTypeStr()+ "("+ to_string(_gateID) + ")";
   if(_symbol != "") output += ("\""+_symbol+ "\"");
   output += (", line " + to_string(_lineno));
   cout << setw(49) << left << output << "=" << endl;
   cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   vector<unsigned> _ID;
   string output = (getTypeStr() + " " + to_string(_gateID));
   _ID.push_back(_gateID);
   cout << output << endl;
   for(size_t i = 0; i < _fanin.size(); i++){
      bool _inv;
      if(_invphase[i]) _inv = true;
      else _inv = false;
      _ID.push_back(_gateID);
      _fanin[i]->reportFanin_rcv(level, level-1, _inv, _ID);
   }
}
// void CirGate::reportFanin_rcv(int level, int now, bool inv, vector<unsigned>& _ID) const{
//    for(int j = 0; j< (level-now); j++) cout << "  ";
//    if(inv) cout << "!";
//    string output = (getTypeStr() + " " + to_string(_gateID));
//    _ID.push_back(_gateID);
//    cout << output << endl;
//    for(size_t i = 0; i < _fanin.size(); i++){
//       bool _inv;
//       if(_invphase[i]) _inv = true;
//       else _inv = false;
//       if(now - 1 >= 0){
//          if(!_fanin[i]->find_vec(_ID))
//             _fanin[i]->reportFanin_rcv(level, now-1, _inv, _ID);
//          else{
//             for(int j = 0; j< (level-now+1); j++) cout << "  ";
//             if(_invphase[i]) cout << "!";
//             string output = (_fanin[i]->getTypeStr() + " " + to_string(_fanin[i]->get_ID()));
//             if(_fanin[i]->getTypeInt() != 1 && _fanin[i]->getTypeInt() != 0 && _fanin[i]->getTypeInt() != 5)
//                output += " (*)";
//             cout << output << endl;
//          }
//       }
         
//    }
// }
void CirGate::reportFanin_rcv(int level, int now, bool inv, vector<unsigned>& _ID) const{
   for(int j = 0; j< (level-now); j++) cout << "  ";
   if(inv) cout << "!";
   string output = (getTypeStr() + " " + to_string(_gateID));
   if(find_vec(_ID)){
      if(getTypeInt() != 1 && getTypeInt() != 0 && getTypeInt() != 5)
         output += " (*)";
      cout << output << endl;
      return;
   }
   cout << output << endl;
   for(size_t i = 0; i < _fanin.size(); i++){
      bool _inv;
      if(_invphase[i]) _inv = true;
      else _inv = false;
      if(now - 1 >= 0){
         _ID.push_back(_gateID);
         _fanin[i]->reportFanin_rcv(level, now-1, _inv, _ID);
      }   
   }
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   vector<unsigned> _ID;
   string output = (getTypeStr() + " " + to_string(_gateID));
   _ID.push_back(_gateID);
   cout << output << endl;
   size_t address = (size_t)this; 
   for(size_t i = 0; i < _fanout.size(); i++){
      _ID.push_back(_gateID);
      _fanout[i]->reportFanout_rcv(level, level-1, address, _ID);
   }
}
// void CirGate::reportFanout_rcv(int level, int now, size_t last, vector<unsigned>& _ID) const{
//    for(int j = 0; j< (level-now); j++) cout << "  ";
//    for(size_t j = 0; j < _fanin.size(); j++){
//       if(_fanin[j] == (CirGate*)last && _invphase[j] == true)
//          cout << "!";
//    }
//    string output = (getTypeStr() + " " + to_string(_gateID));
//    if(find_vec(_ID)) output += " (*)";
//    else _ID.push_back(_gateID);
//    cout << output << endl;
//    for(size_t i = 0; i < _fanout.size(); i++){
//       bool _inv = false;
//       if(_invphase[i]) _inv = true;
//       else _inv = false;
//       if(now - 1 >= 0){
//          size_t address = (size_t)this; 
//          if(!_fanout[i]->find_vec(_ID)){
//             _fanout[i]->reportFanout_rcv(level, now-1, address, _ID);
//          }
//       }
//    }
// }
void CirGate::reportFanout_rcv(int level, int now, size_t last, vector<unsigned>& _ID) const{
   for(int j = 0; j< (level-now); j++) cout << "  ";
   for(size_t j = 0; j < _fanin.size(); j++){
      if(_fanin[j] == (CirGate*)last && _invphase[j] == true){
         cout << "!";
         break;
      }
   }
   string output = (getTypeStr() + " " + to_string(_gateID));
   if(find_vec(_ID)){
      if(getTypeInt() != 1 && getTypeInt() != 0 && getTypeInt() != 5)
         output += " (*)";
      cout << output << endl;
      return;
   }
   cout << output << endl;
   for(size_t i = 0; i < _fanout.size(); i++){
      bool _inv = false;
      if(_invphase[i]) _inv = true;
      else _inv = false;
      if(now - 1 >= 0){
         size_t address = (size_t)this; 
         _ID.push_back(_gateID);
         _fanout[i]->reportFanout_rcv(level, now-1, address, _ID);

      }
   }
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