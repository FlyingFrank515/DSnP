/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   vector<string> buffer;
   string temp;
   while(is >> temp){
      buffer.push_back(temp);
      string temp;
   }
   int k = 1;
   while(k < int(buffer.size())){
      string key = buffer[k];
      if(key == "}") break;
      string value_str = buffer[k+2];
      if(value_str.find(",") != value_str.npos){
         value_str.erase(int(value_str.find(",")));
         k += 3;
      }
      else k += 4;
      int value = stoi(value_str);
      string key_del = key.substr(1, key.size()-2); 
      DBJsonElem temp(key_del, value);
      j.add(temp);
   }
   if(!j) j.change();
   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   os << "{" << endl;
   if(int(j._obj.size()) != 0){
      for(int i = 0 ; i < int(j._obj.size()) ; i++){
         os << "  ";
         DBJsonElem pr = j._obj[i];
         pr.printElem_as_format(os);
         if(i != int(j._obj.size())-1){
            os << ","<< endl;
         }
         else os << endl;
      }
   }
   os << "}" << endl;
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
   _obj.clear();
   if(_read) _read = false;  
   return;
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   for(int i = 0 ; i < (int)_obj.size() ; i++){
      if(_obj[i].key() == elm.key()){
         return false;
      }
   }
   _obj.push_back(elm);
   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   if(_obj.empty()) {return NAN;}
   float sum = 0;
   for(int i = 0 ; i < (int)_obj.size() ; i++){
      sum += (float)_obj[i].value();
   }
   float avg = sum / (float)_obj.size();
   return avg;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   if(_obj.empty()) {
      idx = _obj.size() - 1;
   }
   else{
      for(unsigned int i = 0 ; i < _obj.size() ; i++){
         if(_obj[i].value()> maxN){
            maxN = _obj[i].value();
            idx = i;
         } 
      }
   }
   return maxN;
}

// If DBJson is empty, set idx to size() and return INT_MAX
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   if(_obj.empty()) {
      idx = _obj.size();
   }
   else{
      for(unsigned int i = 0 ; i < _obj.size() ; i++){
         if(_obj[i].value() < minN) {
            minN = _obj[i].value();
            idx = i;
         }
      }
   }
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   int s = 0;
   for(int i = 0 ; i < (int)_obj.size() ; i++){
      s += _obj[i].value(); 
   }
   return s;
}
