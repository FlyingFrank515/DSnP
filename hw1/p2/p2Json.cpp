/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"
#include <fstream>
#include <iomanip>

using namespace std;
// Implement member functions of class Row and Table here
void JsonElem::printElem_test(){
      cout << "_key=" << _key << endl;
      cout << "_value=" << _value << endl;
   }
void JsonElem::printElem_as_format(){
      cout << _key << " : " << _value;
   }
int JsonElem::getvalue(){
   return _value;
} 
bool string_to_int(string input){
   if(input[0] == 45 || (input[0] >= 48 && input[0] <= 57)){
      for(int i = 1;i < int(input.length());i++){
         if(int(input[i]) >= 58 || int(input[i]) <= 47){
            return false;
         }
      }
      return true;
   }
   else return false;
}
bool Json::read(const string& jsonFile){
   //TODO
   //open the json file
   ifstream inputfile(jsonFile);
   if(! inputfile){
      return false;
   }
   //format it
   vector<string> buffer;
   string temp;
   while(inputfile >> temp){
      buffer.push_back(temp);
      string temp;
   }
   int j = 1;
   while(j < int(buffer.size())){
      string key = buffer[j];
      if(key == "}") break;
      string value_str = buffer[j+2];
      if(value_str.find(",") != value_str.npos){
         value_str.erase(int(value_str.find(",")));
         j += 3;
      }
      else j += 4;
      int value = stoi(value_str);
      JsonElem temp(key,value);
      _obj.push_back(temp);
   }
   // for(int i = 0 ; i < int(_obj.size()) ; i++){
   //    _obj[i].printElem_test();
   // }
   return true;
}
//*******************************************************************
// complicated solution
// bool Json::read(const string& jsonFile)
// {
//    //TODO
//    //open the json file
//    // ifstream inputfile;
//    // inputfile.open(jsonFile,ios::binary);
//    ifstream inputfile(jsonFile);
//    if(! inputfile){
//       return false;
//    }
//    //turn it into string array
//    string ori_input[4096];
//    int length;
//    for(int i = 0 ; i < 4096 ; i++){
//       inputfile >> ori_input[i];
//       //delete meaningless input
//       if(ori_input[i].find(" ") != ori_input[i].npos || 
//       ori_input[i] == "{"  || ori_input[i] == ":" || ori_input[i] == ","){
//          i = i-1;
//       }
//       //delete ","
//       else if(ori_input[i].find(",") != ori_input[i].npos){
//          int pos = ori_input[i].find(",");
//          ori_input[i].erase(pos);
//       }
//       //end
//       else if(ori_input[i] == "}"){
//          ori_input[i] = " ";
//          length = i+1;
//          break;
//       }
//    }
//    //test
//    // for(int i = 0 ; i < length ; i++){
//    //    cout << ori_input[i] << endl;
//    // }

//    //turn it into JsonElem
//    for(int i = 0 ; i < length-1 ; i++){
//       if(!(i % 2)){
//          int v = stoi(ori_input[i+1]);
//          JsonElem temp(ori_input[i],v);
//          _obj.push_back(temp);
//          i = i+1;
//       }
//    }
//    //test
//    // for(int i = 0 ; i < int(_obj.size()) ; i++){
//    //    _obj[i].printElem_test();
//    // }
//    // cout <<  int(_obj.size()) << endl;
//    return true; 
// }
//*******************************************************************
void Json::PRINT(){
   cout<<"{"<<endl;
   if(int(_obj.size()) != 0){
      for(int i = 0 ; i < int(_obj.size()) ; i++){
         cout << "  ";
         _obj[i].printElem_as_format();
         if(i != int(_obj.size())-1){
            cout << ","<<endl;
         }
         else cout << endl;
      }
   }
   cout<<"}"<<endl;
}
void Json::SUM(){
   if(int(_obj.size()) != 0){
      int sum = 0 ;
      for(int i = 0 ; i < int(_obj.size()) ; i++){
         sum += _obj[i].getvalue();
      }
      cout << "The summation of the values is: " 
         << sum << "." << endl;
   }
   else cout << "Error: No element found!!" << endl;
}
void Json::AVE(){
   if(int(_obj.size()) != 0){
      float sum = 0 ;
      for(int i = 0 ; i < int(_obj.size()) ; i++){
         sum += float(_obj[i].getvalue());
      }
      float ave = sum / float(_obj.size());
      cout << "The average of the values is: " 
         << fixed << setprecision(1) << ave 
         << "." << endl;
   }
   else cout << "Error: No element found!!" << endl;
}
void Json::MAX(){
   if(int(_obj.size()) != 0){
      int pos = 0;
      int biggest = _obj[0].getvalue();
      for(int i = 0 ; i < int(_obj.size()) ; i++){
         if(_obj[i].getvalue() > biggest){
            biggest = _obj[i].getvalue();
            pos = i;
         }
      }
      cout << "The maximum element is: { ";
      _obj[pos].printElem_as_format();
      cout << " }." << endl;
   }
   else cout << "Error: No element found!!" << endl;
}
void Json::MIN(){
   if(int(_obj.size()) != 0){
      int pos = 0;
      int smallest = _obj[0].getvalue();
      for(int i = 0 ; i < int(_obj.size()) ; i++){
         if(_obj[i].getvalue() < smallest){
            smallest = _obj[i].getvalue();
            pos = i;
         }
      }
      cout << "The minimum element is: { ";
      _obj[pos].printElem_as_format();
      cout << " }." << endl;
   }
   else cout << "Error: No element found!!" << endl;
}


int Json::ADD(string &input){
   string origin = input;
   string blank = "ejgiejqnxi4mh3";
   string process[64];
   //initialize the string array
   for(int i = 0;i < 64;i++) process[i] = blank;
   int record = 0,j=0; 
   while(input.find(" ") != input.npos){
      if((int(input.find(" "))-record) != 0){
         process[j].assign(input , record , int(input.find(" "))-record);
         record = int(input.find(" "));
         input[record] = '$';
         record += 1;
         j+=1;
      }
      else{
         record = int(input.find(" "));
         input[record] = '$';
         record += 1;
      }
   }
   int len = input.length(); 
   if(len != record){
      process[j].assign(input , record , len);
   }
   for(int i = 0;i < 64;i++){
      if(process[i] != blank){
         len = i+1;
      }
   }
   
   if(process[0] != "ADD"){
      input = origin;
      return 4;
   }
   else if(len == 1){
      input = "wejifj24i";
      return 1;
   }
   else if(len == 2){
      input = process[1];
      return 1;
   }
   else if(len > 3){
      input = process[3];
      return 3;
   }
   else if(!(string_to_int(process[2]))){
      input = process[2];
      return 2;
   }
   else{
      int value = stoi(process[2]);
      process[1].insert(0,"\"");
      process[1]+="\"";
      JsonElem temp(process[1],value);
      _obj.push_back(temp);
      return 0;
   }
   return 0;  
}
ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}

void process(string &input , int &len){
   string blank = "ejgiejqnxi4mh3";
   string process[64];
   //initialize the string array
   for(int i = 0;i < 64;i++) process[i] = blank;
   int record = 0,j = 0; 
   while(input.find(" ") != input.npos){
      if((int(input.find(" "))-record) != 0){
         process[j].assign(input , record , int(input.find(" "))-record);
         record = int(input.find(" "));
         input[record] = '$';
         record += 1;
         j+=1;
      }
      else{
         record = int(input.find(" "));
         input[record] = '$';
         record += 1;
      }
   }
   len = input.length(); 
   if(len != record){
      process[j].assign(input , record , len);
   }
   for(int i = 0;i < 64;i++){
      if(process[i] != blank){
         len = i+1;
      }
   input = process[0];
   }   string origin = input;
}