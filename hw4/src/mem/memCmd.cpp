/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   int obj_num;
   if(!lexOptions(option, options)) 
      return CMD_EXEC_ERROR;
   if(options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   
   if(options.size() == 1){
      if(myStr2Int(options[0], obj_num)){
         if(obj_num >= 0){
            try { mtest.newObjs((size_t)obj_num); }
            catch(bad_alloc) { return CMD_EXEC_ERROR; }
            return CMD_EXEC_DONE;
         }
         else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
      }
      else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   }
   
   else if(options.size() == 2)
      return CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
   
   else if(options.size() == 3){
      bool a = false;
      int arraysize; 
      int a_pos;
      for(int i = 0 ; i < (int)options.size() ; i++){
         if((a == false) && (myStrNCmp("-Array", options[i], 2) == 0)){
            a = true;
            a_pos = i;
         }
      }
      string arr,num;
      if(a_pos == 0){
         arr = options[1];
         num = options[2];
      }
      if(a_pos == 1){
         arr = options[2];
         num = options[0];
      }

      if(myStr2Int(num, obj_num)){
         if(obj_num <= 0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, num);
      }
      else return CmdExec::errorOption(CMD_OPT_ILLEGAL, num);
      
      if(myStr2Int(arr, arraysize)){
         if(arraysize <= 0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, arr);
      }
      else return CmdExec::errorOption(CMD_OPT_ILLEGAL, arr);
      
      if(!a) return CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
      if(a){
         try { mtest.newArrs((size_t)obj_num, (size_t)arraysize);}
         catch(bad_alloc) { 
            return CMD_EXEC_ERROR; }
         return CMD_EXEC_DONE;
      }

   }
   else if(options.size() >3)
      return  CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
   // Use try-catch to catch the bad_alloc exception
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   int mode = 0;
   // mode = 1 : -Index
   // mode = 2 : -Random
   if(!lexOptions(option, options)) 
      return CMD_EXEC_ERROR;
   if(options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   
   bool a = false;
   vector<string> processed;
   for(int i = 0 ; i < (int)options.size() ; i++){
      if((a == false) && (myStrNCmp("-Array", options[i], 2) == 0)){
         a = true;
      }
      else{
         processed.push_back(options[i]);
      }
   }
   if(a && options.size() < 3 ) return CmdExec::errorOption(CMD_OPT_MISSING, "");
   if(a && options.size() > 3 ) return CmdExec::errorOption(CMD_OPT_EXTRA, processed[2]);
   if(!a && options.size() < 2 ) return CmdExec::errorOption(CMD_OPT_MISSING, "");
   if(!a && options.size() > 2 ) return CmdExec::errorOption(CMD_OPT_EXTRA, options[2]);
   
   if(!(myStrNCmp("-Index", processed[0], 2)) || !(myStrNCmp("-Random", processed[0], 2))){
      if(myStrNCmp("-Index", processed[0], 2) == 0) mode = 1;
      else if(myStrNCmp("-Random", processed[0], 2) == 0) mode = 2;
   }
   else{
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, processed[0]);
   }
   int num;
   if(myStr2Int(processed[1], num)){
      if(num < 0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, processed[0]);
   }
   else return CmdExec::errorOption(CMD_OPT_ILLEGAL, processed[0]);
   assert(mode == 1 || mode == 2);

   //index mode
   if(mode == 1){
      //array mode
      if(a){
         if((size_t)num >= mtest.getArrListSize()){
            cerr << "Size of array list (" << mtest.getArrListSize() <<  "is <= " << num << "!!";
            cerr << endl;
            return CMD_EXEC_ERROR;
         }
         else{
            mtest.deleteArr(num);
            return CMD_EXEC_DONE;
         } 
      }
      //nonarray mode
      else{
         if((size_t)num >= mtest.getObjListSize()){
            cerr << "Size of object list (" << mtest.getObjListSize() <<  "is <= " << num << "!!";
            cerr << endl;
            return CMD_EXEC_ERROR;
         }
         else{
            mtest.deleteObj(num);
            return CMD_EXEC_DONE;
         } 
      }
   }
   //random mode
   if(mode == 2){
      //arraymode
      if(a){
         if(mtest.getArrListSize() == 0){
            cerr << "Size of array list is 0!!" << endl;
            return CMD_EXEC_ERROR;
         }
         else{
            for(int i = 0 ; i < num ; i++){ 
               size_t rnd = (size_t)rnGen(int(mtest.getArrListSize()));
               mtest.deleteArr(rnd);
            }
            return CMD_EXEC_DONE;
         }
      }
      //nonarray mode
      else{
         if(mtest.getObjListSize() == 0){
            cerr << "Size of object list is 0!!" << endl;
            return CMD_EXEC_ERROR;
         }
         else{
            for(int i = 0 ; i < num ; i++){
               size_t rnd = (size_t)rnGen(int(mtest.getObjListSize()));
               mtest.deleteObj(rnd);
            }
            return CMD_EXEC_DONE;
         }
      }
   }
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


