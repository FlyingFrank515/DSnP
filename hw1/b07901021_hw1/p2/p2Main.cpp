/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"

using namespace std;

int main()
{
   Json json;

   // Read in the csv file. Do NOT change this part of code.
   string jsonFile;
   cout << "Please enter the file name: ";
   cin >> jsonFile;
   if (json.read(jsonFile))
      cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }

   // TODO read and execute commands
   // cout << "Enter command: ";
   while (true) {
      string command ; 
      cout << "Enter command: ";
      //remember to ignore space bar in the input
      //by default,cin will split the input by space bar
      getline(cin >> ws, command);
      string command_o = command;
      int len = -1;
      process(command , len);
      if(len == 0) continue;
      if(command == "EXIT" && len == 1){
         return 0;
      }
      else if(command == "PRINT" && len == 1){
         json.PRINT();
      }
      else if(command == "SUM" && len == 1){
         json.SUM();
      }
      else if(command == "AVE" && len == 1){
         json.AVE();
      }
      else if(command == "MAX" && len == 1){
         json.MAX();
      }
      else if(command == "MIN" && len == 1){
         json.MIN();
      }
      else if(command_o.find("ADD") != command_o.npos){
         switch(json.ADD(command_o)){
            case 0:
               break;
            case 1://missing
               if(command_o == "wejifj24i"){
                  cout << "Error: Missing argument!!" << endl;
                  break;   
               }
               else{
                  cout << "Error: Missing argument after ";
                  cout << "\"" << command_o << "\"!!" << endl;
                  break;
               }
            case 2://illegal
               cout << "Error: Illegal argument ";
               cout << "\"" << command_o << "\"!!" << endl;
               break;
            case 3://extra
               cout << "Error: Extra argument ";
               cout << "\"" << command_o << "\"!!" << endl;
               break;
            case 4://unknown
               cout << "Error: unknown command: ";
               cout << "\"" << command_o << "\"" << endl;
               break;
         }
      }
      else {
         if(len >= 2){
            cout << "Error: Extra argument!!" << endl;
         }
         else{
            cout << "Error: unknown command: ";
            cout << "\"" << command_o << "\"" << endl;
         }
      }
   }
}
