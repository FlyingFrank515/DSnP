/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr
//========modify the value to show debug message============
#define DEBUGGER  false
bool compare(map<size_t , CirGateV*>, map<size_t , CirGateV*>);
//==========================================================

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{ 
   if(DEBUGGER) cerr << "start reading" << endl;
   ifstream inputfile(fileName);
   if(!inputfile) return false;
   vector<string> buffer;
   string temp;
   while(getline(inputfile, temp)){
      buffer.push_back(temp);
   }
   
   // add CONST0 gate to the map of all gates
   _dict[0] = CONST0;
   
   // read Header
   if(buffer.size() == 0) return false;
   if(!readHeader(0, buffer[0])) return false;
   if(DEBUGGER) {
      cerr << "readHeader successfully" << endl;
      cerr << "M:" << M << endl;
      cerr << "I:" << I << endl;
      cerr << "O:" << O << endl;
      cerr << "A:" << A << endl;
   }
   // read input for I times;
   for(unsigned i = 1; i < (I+1); i++){
      if(buffer.size() <= i) return false;
      if(!readInput(i+1, buffer[i])){
         return false;
      }
   }
   if(DEBUGGER) cerr << "readInput successfully" << endl;
   // [pass]read latches for I times

   // read output for O times(with GATEID begin from M+1)
   int j = 1;
   for(unsigned i = I+1; i < I+O+1; i++,j++){
      if(buffer.size() <= i) return false;
      if(!readOutput(i+1, buffer[i], j)) return false; 
   }
   if(DEBUGGER) cerr << "readOutput successfully" << endl;
   // read AND gate for A times
   for(unsigned i = I+O+1; i < I+O+A+1; i++){
      if(buffer.size() <= i) return false;
      if(!readAIG(i+1, buffer[i])) return false;
   }
   if(DEBUGGER) cerr << "readAIG successfully" << endl;
   // read symbols until "c" appear
   unsigned i = I+O+A+1;
   while(i < buffer.size() && buffer[i] != "c"){
      if(buffer.size() < i) return false;
      if(buffer.size() == i) break;
      if(!readSymbol(buffer[i])) return false;
      i++;
   }
   if(DEBUGGER) cerr << "readSymbol successfully" << endl;
   // update the symbol to each gates' data member
   map<unsigned, string>::iterator iter;
   for(iter = _pi_symbol.begin(); iter != _pi_symbol.end(); iter++){
      _pilist[iter->first]->set_symbol(iter->second);
   }
   for(iter = _po_symbol.begin(); iter != _po_symbol.end(); iter++){
      _polist[iter->first]->set_symbol(iter->second);
   }
   i++;
   
   // read comments
   for(;i < buffer.size(); i++){
      if(buffer.size() < i) return false;
      if(buffer.size() == i) break;
      _comment.push_back(buffer[i]);
   }
   if(DEBUGGER) cerr << "readcomments successfully" << endl;
   // connect 
   if(DEBUGGER) cerr << "start connecting" << endl;
   connect(); 
   if(DEBUGGER) cerr << "connect successfully" << endl;
   // traverse and create te dfslist
   CirMgr::DFStraversal(_polist);
   if(DEBUGGER) cerr << "traverse successfully" << endl;
   if(DEBUGGER) cerr << "read AIGER file succesfully" << endl;
   return true;
}
bool CirMgr::readHeader(size_t linenum, const string& line){
   string token;
   for(int i = 0; i < 3 && line.size() > 4; i++) token += line[i];
   if(token != "aag") return false;
   if(line[3] != ' ') return false;
   if(line[4] == ' ') return false;
   unsigned temp[5] = {0,0,0,0,0}; // M,I,O,A
   token = "";
   int num, i = 0;
   size_t begin = 4;
   if(line[3] != ' ') return false;
   while(1){
      begin = myStrGetTok(line, token, begin);
      if(begin == string::npos) return false;
      if(DEBUGGER) cerr << "token:" << token << endl;
      if(!myStr2Int(token, num)) return false;
      if(num < 0) return false;
      if(DEBUGGER) cerr << "num:" << num << endl;
      temp[i] = num;
      begin++;
      i++;
      if(line[begin] == ' ') return false;
      if(i == 4) break;
   }
   token = line.substr(begin, line.size() - begin);
   if(DEBUGGER) cerr << "token:" << token << endl;
   if(!myStr2Int(token, num)) return false;
   if(num < 0) return false;
   if(DEBUGGER) cerr << "num:" << num << endl;
   temp[4] = num;
   if(DEBUGGER){
      for(int i = 0; i < 5; i++){
         cout << temp[i] << endl;
      }
   }  
   this->M = temp[0];
   this->I = temp[1];
   this->L = temp[2];
   this->O = temp[3];
   this->A = temp[4];
   return true;
}
bool CirMgr::readInput(size_t linenum, const string& line){
   string token;
   int num;
   size_t pos = 0;
   if(line.size() == 0) return false;
   while(pos != line.size()){
      if(line[pos] == ' ') return false;
      token += line[pos];
      pos++;
   }
   if(DEBUGGER) cerr << "token:" << token << endl;
   if(!myStr2Int(token, num)) return false;
   if(num <= 0) return false;
   if(num%2 != 0) return false;
   num = num / 2; // get gate_ID
   if(num > int(M)) return false;
   if(DEBUGGER) cerr << "num:" << num << endl;
   // to check whether gate_ID has been appeared
   
   // map<unsigned, CirGate*>::iterator iter = _dict.find(num); 
   // if(iter != _dict.end()) return false;
   
   //create a gate and add it into the map
   CirGate* pi = new CirPiGate(linenum, num);
   _pilist.push_back(pi);
   _dict[num] = pi;
   return true;
}
bool CirMgr::readOutput(size_t linenum, const string& line, int j){
   string token;
   int num;
   size_t pos = 0;
   if(line.size() == 0) return false;
   while(pos != line.size()){
      if(line[pos] == ' ') return false;
      token += line[pos];
      pos++;
   }

   if(!myStr2Int(token, num)) return false;
   if(num < 0) return false;
   bool inv = (num % 2 == 1);
   num = num / 2;
   if(num > int(M)) return false; 

   // create a gate and add it into the map
   CirGate* po = new CirPoGate(linenum, (M + j), inv, num); 
   //add it into _polist for future DFStraversal
   _polist.push_back(po);
   _dict[M+j] = po;
   return true;
}
bool CirMgr::readAIG(size_t linenum, const string& line){
   int temp[3]; // L,R1,R2
   string token[3];
   int appear = 0;
   size_t pos = 0;
   if(line.size() == 0) return false;
   while(pos != line.size()){
      if(line[pos] == ' '){
         if(!myStr2Int(token[appear], temp[appear])) 
            return false;
         if(temp[appear] < 0) return false;
         appear++;
      }
      else token[appear] += line[pos];
      if(appear > 2) return false;
      if(pos == line.size()-1){
         if(!myStr2Int(token[appear], temp[appear])) 
            return false;
         if(temp[appear] < 0) return false;
         break;
      }
      if(appear > 2) return false;
      pos++;
   }
   
   if(temp[0] % 2 != 0) return false;
   temp[0] = temp[0] / 2;
   bool inv1 = (temp[1] % 2 == 1);
   temp[1] = temp[1] / 2;
   bool inv2 = (temp[2] % 2 == 1);
   temp[2] = temp[2] / 2;
   if(temp[0] > int(M)) return false; 
   if(temp[1] > int(M)) return false; 
   if(temp[2] > int(M)) return false;
   
   // to check whether gate_ID has been appeared
   
   // map<unsigned, CirGate*>::iterator iter = _dict.find(temp[0]); 
   // if(iter != _dict.end()) return false;
   
   // create a gate and add it into the map
   CirGate* AIG = new CirAIGGate(linenum, temp[0], inv1, inv2, temp[1], temp[2]);
   _dict[temp[0]] = AIG;
   _aiglist.push_back(AIG);
   return true;
}
bool CirMgr::readSymbol(const string& line){
   _symbol.push_back(line);
   if(line[0] =='i' || line[0] == 'o'){
      char c = 'i';
      if(line[0] == 'o') c = 'o';
      string token = "";
      int num;
      size_t i = 1;
      while(i != line.size()){
         if(line[i] == ' ') break;
         else token += line[i];
         i++;
      }
      
      if(!myStr2Int(token, num)) return false;
      if(c == 'i') 
         if((unsigned)num >= I || num < 0) return false;
      if(c == 'o') 
         if((unsigned)num >= O || num < 0) return false;
      
      if(i == line.size()) return false;
      i++;
      token = "";
      while(i != line.size()){
         token += line[i];
         i++;
      }
      if(c == 'i') _pi_symbol[num] = token;
      if(c == 'o') _po_symbol[num] = token;
   }
   else return false;
   return true;
}
void CirMgr::connect(){
   map<unsigned, CirGate*>::iterator iter;
   for(iter = _dict.begin(); iter != _dict.end(); iter++){
      // cout << "processing:" << iter->first << "->";
      iter->second->dis_init_fec();
      int type = iter->second->getTypeInt();
      switch(type){
         case 2:{
            unsigned conn = iter->second->get_in(0);
            map<unsigned, CirGate*>::iterator itr_f = _dict.find(conn);
            //didnt find
            if(itr_f == _dict.end()){
               map<unsigned, CirGate*>::iterator itr_UNDEF = _UNDEFlist.find(conn);
               if(itr_UNDEF == _UNDEFlist.end()){
                  CirGate* UNDEF = new CirUNDEFGate(conn);
                  UNDEF->dis_init_fec();
                  _UNDEFlist[conn] = UNDEF;
                  _undlist.push_back(UNDEF);
                  iter->second->push_fanin(UNDEF);
                  UNDEF->push_fanout(iter->second);
               }
               else{
                  CirGate* UNDEF = itr_UNDEF->second;
                  iter->second->push_fanin(UNDEF);
                  UNDEF->push_fanout(iter->second);
               }
               // cout << "push UNDEF" << endl;
            }
            //find
            else{
               CirGate* to = itr_f->second;
               iter->second->push_fanin(to);
               //let the fanin's fanout(LOL) point to this gate
               to->push_fanout(iter->second);
               // cout << "push gate" << endl;
            }
            break;
         }
         case 3:{
            for(int i = 0; i < 2; i++){
               unsigned conn = iter->second->get_in(i);
               map<unsigned, CirGate*>::iterator itr_f = _dict.find(conn);
               //didnt find
               if(itr_f == _dict.end()){
                  map<unsigned, CirGate*>::iterator itr_UNDEF = _UNDEFlist.find(conn);
                  CirGate* UNDEF;
                  if(itr_UNDEF == _UNDEFlist.end()){
                     CirGate* UNDEF = new CirUNDEFGate(conn);
                     UNDEF->dis_init_fec();
                     _UNDEFlist[conn] = UNDEF;
                     _undlist.push_back(UNDEF);
                     iter->second->push_fanin(UNDEF);
                     UNDEF->push_fanout(iter->second);
                  }
                  else{
                     CirGate* UNDEF = itr_UNDEF->second;
                     iter->second->push_fanin(UNDEF);
                     UNDEF->push_fanout(iter->second);
                  }
                  // cout << "push UNDEF" << endl;
               }
               //find
               else{
                  CirGate* to = itr_f->second;
                  iter->second->push_fanin(to);
                  //let the fanin's fanout(LOL) point to this gate
                  to->push_fanout(iter->second);
                  // cout << "push gate" << endl;
               }
            }
            break;
         }
         default:
            break;
      }
   }
}




/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
void CirMgr::DFStraversal(GateList sinklist){
   CirGate::setGlobalRef();
   for(size_t i = 0; i < sinklist.size(); i++){
      sinklist[i]->DFStraversal(_dfslist);
   }
}
unsigned CirMgr::countPIs() const {
   unsigned i = 0;
   map<unsigned, CirGate*>::const_iterator iter;
   for(iter = _dict.begin(); iter !=_dict.end(); iter++){
      if(iter->second->getTypeInt() == 1){
         i++;
      }
   }
   return i;
}
unsigned CirMgr::countPOs() const{
   unsigned i = 0;
   map<unsigned, CirGate*>::const_iterator iter;
   for(iter = _dict.begin(); iter !=_dict.end(); iter++){
      if(iter->second->getTypeInt() == 2){
         i++;
      }
   }
   return i;
}
unsigned CirMgr::countAIGs() const{
   unsigned i = 0;
   map<unsigned, CirGate*>::const_iterator iter;
   for(iter = _dict.begin(); iter !=_dict.end(); iter++){
      if(iter->second->getTypeInt() == 3){
         i++;
      }
   }
   return i;
}
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   unsigned i = _pilist.size()+_polist.size()+_aiglist.size();
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI" << setw(12) << right << _pilist.size() << endl;
   cout << "  PO" << setw(12) << right << _polist.size() << endl;
   cout << "  AIG" << setw(11) << right << _aiglist.size() << endl;
   cout << "------------------" << endl;
   cout << "  Total" << setw(9) << right << i << endl;
}

void
CirMgr::printNetlist() const
{
   cout << endl;
   for(size_t i = 0; i < _dfslist.size(); i++){
      cout << "[" << i << "] ";
      switch(_dfslist[i]->getTypeInt()){
         case 5:{
            cout << "CONST0" << endl;
            break;
         }
         case 1:{
            cout << "PI  " << _dfslist[i]->get_ID() ;
            // remember to print symbol!!
            if(_dfslist[i]->get_symbol() != ""){
               cout << " (" << _dfslist[i]->get_symbol() << ")"; 
            }
            cout << endl;
            break;
         }
         case 2:{
            cout << "PO  " << _dfslist[i]->get_ID() << " ";
            if(_dfslist[i]->get_fanin_UNDEF(0)) cout << "*";
            if(_dfslist[i]->get_invphase(0)) cout << "!";
            cout << _dfslist[i]->_fanin[0]->get_ID();
            // remember to print symbol!!
            if(_dfslist[i]->get_symbol() != ""){
               cout << " (" << _dfslist[i]->get_symbol() << ")"; 
            }
            cout << endl;
            break;
         }
         case 3:{
            cout << "AIG " << _dfslist[i]->get_ID() << " ";
            if(_dfslist[i]->get_fanin_UNDEF(0)) cout << "*";
            if(_dfslist[i]->get_invphase(0)) cout << "!";
            cout << _dfslist[i]->_fanin[0]->get_ID() << " ";
            if(_dfslist[i]->get_fanin_UNDEF(1)) cout << "*";
            if(_dfslist[i]->get_invphase(1)) cout << "!";
            cout << _dfslist[i]->_fanin[1]->get_ID() ;
            cout << endl;
            break;
         }
      }
   }
}

void
CirMgr::printPIs() const
{
   // bool ini = false;
   // map<unsigned, CirGate*>::const_iterator iter;
   // for(iter = _dict.begin(); iter !=_dict.end(); iter++){  
   //    if(iter->second->getTypeInt() == 1){
   //       if(!ini){
   //          cout << "PIs of the circuit:";
   //          ini = true;
   //       }
   //       cout << " ";
   //       iter->second->printGate();
   //    }
   // }
   // cout << endl;
   cout << "PIs of the circuit:";
   for (size_t i = 0;i < _pilist.size(); ++i) 
      cout << " " << _pilist[i]->get_ID();
   cout << endl;
}

void
CirMgr::printPOs() const
{
   // bool ini = false;
   // map<unsigned, CirGate*>::const_iterator iter;
   // for(iter = _dict.begin(); iter !=_dict.end(); iter++){
   //    if(iter->second->getTypeInt() == 2){
   //       if(!ini){
   //          cout << "POs of the circuit:";
   //          ini = true;
   //       }
   //       cout << " ";
   //       iter->second->printGate();
   //    }
   // }
   // cout << endl;
   cout << "POs of the circuit:";
   for (size_t i = 0;i < _polist.size(); ++i) 
      cout << " " << _polist[i]->get_ID();
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   bool ini = false;
   map<unsigned, CirGate*>::const_iterator iter;
   for(iter = _dict.begin(); iter !=_dict.end(); iter++){
      if(iter == _dict.begin()) continue;
      if(iter->second->with_FloatGates()){
         if(!ini){
            cout << "Gates with floating fanin(s):";
            ini = true;
         }
         string output = " " + to_string(iter->second->get_ID());
         cout << output;
      }
   }
   if(ini) cout << endl;
   ini = false;
   for(iter = _dict.begin(); iter !=_dict.end(); iter++){
      if(iter == _dict.begin()) continue;
      if(iter->second->not_used() && iter->second->getTypeInt() != 2){
         if(!ini){
            cout << "Gates defined but not used  :";
            ini = true;
         }
         string output = " " + to_string(iter->second->get_ID());
         cout << output;
      }
   }
   if(ini) cout << endl;
}

void
CirMgr::printFECPairs() const
{
   if(!_init) return;
   // for(size_t i = 0; i < _FECgroups->size(); i++){
   //    cout << "[" << i << "]" ;
   //    for(map<size_t, CirGateV*>::iterator j = _FECgroups->at(i).begin(); j !=  _FECgroups->at(i).end(); j++){
   //       if(j->second->get_inv()) cout << "X";
   //       else cout << "O";
   //    }
	// 	cout << endl;
	// }
   for(size_t i = 0; i < _FECgroups->size(); i++){
      cout << "[" << i << "]" ;
		map<size_t, CirGateV*>::iterator one =  _FECgroups->at(i).begin();
      for(map<size_t, CirGateV*>::iterator j = _FECgroups->at(i).begin(); j !=  _FECgroups->at(i).end(); j++){
         cout << " ";
         if(one->second->get_inv() != j->second->get_inv()) cout << "!";
         cout << j->first;
      }
		cout << endl;
	}
}

void CirMgr::sort_FEC(){
   sort(_FECgroups->begin(), _FECgroups->end(), compare);
}

void
CirMgr::writeAag(ostream& outfile) const
{
   unsigned A_new = 0;
   for(size_t i = 0; i < _dfslist.size(); i++)
      if(_dfslist[i]->getTypeInt() == 3)
         A_new++;
   outfile << "aag "<<M<<" " <<I<<" " <<L<<" "<<O<<" "<< A_new <<endl;
   for(size_t i = 0; i < _pilist.size(); i++){
      string output = to_string(2*_pilist[i]->get_ID());
      outfile << output << endl;
   }
   for(size_t i = 0; i < _polist.size(); i++){
      unsigned num = 2*_polist[i]->_fanin[0]->get_ID();
      if(_polist[i]->get_invphase(0)) num++;
      string output = to_string(num);
      outfile << output << endl;
   }
   for(size_t i = 0; i < _dfslist.size(); i++){
      if(_dfslist[i]->getTypeInt() == 3){
         unsigned num = 2*_dfslist[i]->get_ID();
         string output = to_string(num);
         output += " ";
         num = 2*_dfslist[i]->_fanin[0]->get_ID();
         if(_dfslist[i]->get_invphase(0)) num++;
         output += (to_string(num) + " ");
         num = 2*_dfslist[i]->_fanin[1]->get_ID();
         if(_dfslist[i]->get_invphase(1)) num++;
         output += to_string(num);
         outfile << output << endl;
      }
   }
   for(size_t i = 0; i < _symbol.size(); i++)
      outfile << _symbol[i] << endl;
   outfile << "c" << endl;
   outfile << "AAG output by HSUAN PAN, NTUEE" << endl;

}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{

}
void CirMgr::reset(){
   M = 0;
   I = 0;
   L = 0;
   O = 0;
   A = 0;
   _pilist.clear();
   _polist.clear();
   _dfslist.clear();
   _aiglist.clear();
   for (GateList::iterator iter = _undlist.begin(); iter != _undlist.end(); iter++) {
      delete *iter;
      (*iter) = 0;
   }
   _undlist.clear();
   for (map<unsigned, CirGate*>::iterator iter = _dict.begin(); iter != _dict.end(); iter++) {
      delete iter->second;
      iter->second = 0;
   }
	// cout << _FECgroups;
   // if(_FECgroups){
   //    for(size_t i = 0; i < _FECgroups->size(); i++){
   //       for(size_t j = 0; j < _FECgroups->at(i).size(); j++){
   //          delete _FECgroups->at(i)[j];
   //          _FECgroups->at(i)[j] = 0;
   //       }
   //       _FECgroups[i].clear();
   //    }
   //    _FECgroups->clear();
   //    delete _FECgroups;
   //    _FECgroups = 0;
   // }
   // delete _FECgroups;

   CONST0 = 0;
   _dict.clear();
   _UNDEFlist.clear();
   _pi_symbol.clear();
   _po_symbol.clear();
   _symbol.clear();
   _comment.clear();
   lineNo = 0;
   colNo = 0;
   _init = false;
}
bool compare(map<size_t, CirGateV*> a,map<size_t, CirGateV*> b){
   assert(a.size() > 0 && b.size() > 0);
   return (a.begin()->first <  b.begin()->first);
}
void CirMgr::writelogs(size_t much){
   for(size_t i = 0; i < much &&  i < 64 ; i++){
      for(size_t j = 0; j < _pilist.size(); j++){
         *_simLog << ((size_t)(_pilist[j]->get_simvalue() >> i) & ((size_t)1) != 0) ? "0" :"1" ;
      }
      *_simLog << " ";
      for(size_t j = 0; j < _polist.size(); j++){
         *_simLog << ((size_t)(_polist[j]->get_simvalue() >> i) & ((size_t)1) != 0) ? "0" :"1" ;
      }
      *_simLog << "\n";
   }
}

