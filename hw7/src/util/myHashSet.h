/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class  HashSet<Data>;
   public:
      iterator(){}
      iterator(Data _data, const HashSet<Data>* b, bool end = false):_table(b){
         if(end){
            _iter = _table->_buckets[_table->numBuckets() - 1].end();
            return;
         }

         for(size_t i = 0; i < _table->numBuckets(); i++){
            _iter = _table->_buckets[i].begin();
            for(size_t j = 0; j < _table->_buckets[i].size(); j++, _iter++){
               if((_table->_buckets[i])[j] == _data) break;
            }
            if(_iter != _table->_buckets[i].end()){
               _no = i;
               break;
            }
         }
      }
      ~iterator(){}
      const Data& operator * () const { return *_iter; }
      iterator& operator ++ () { 
         _iter++;
         while(_iter == _table->_buckets[_no].end()){
            if(_no == _table->numBuckets() - 1) break;
            _no++;
            _iter = _table->_buckets[_no].begin();
         }
         return (*this); 
      }
      iterator operator ++ (int) { iterator temp = *this; ++(*this); return temp;}
      iterator& operator -- () { 
         if(_iter != _table->_buckets[_no].begin()){
            _iter--;
         }
         else{
            while(_iter == _table->_buckets[_no].begin()){
               _no--;
               _iter = _table->_buckets[_no].end();
            }
            _iter--;
         }
         return (*this); 
      }
      iterator operator -- (int) {iterator temp = *this; --(*this); return temp;}
      iterator& operator = (const iterator& i) {
         this->_table = i->_table;
         this->_iter = i->_iter;
         this->_no = i->_no;
         return *this;
      }
      bool operator == (const iterator& i) const { return (this->_iter == i._iter);}
      bool operator != (const iterator& i) const { return !(*this == i); }
   private:
      const HashSet<Data>*                _table;
      typename vector<Data>::iterator     _iter;
      size_t                              _no;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      for(size_t i = 0; i < _numBuckets; i++){
         if(_buckets[i].size() != 0){
            return iterator(_buckets[i][0], this);
         }
      }
      return end();
   }
   // Pass the end
   iterator end() const {
      return iterator(Data(), this, true);
   }
   // return true if no valid data
   bool empty() const { return (size() == 0); }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      for(size_t i = 0; i < _numBuckets; i++){
         s += _buckets[i].size();
      }
      return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      size_t key = bucketNum(d);
      for(size_t i = 0; i < _buckets[key].size(); i++){
         if(_buckets[key][i] == d) return true;
      }
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      size_t key = bucketNum(d);
      for(size_t i = 0; i < _buckets[key].size(); i++){
         if(_buckets[key][i] == d){
            d = _buckets[key][i];
            return true;
         }
      }
      return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
      size_t key = bucketNum(d);
      for(size_t i = 0; i < _buckets[key].size(); i++){
         if(_buckets[key][i] == d){
            _buckets[key][i] = d;
            return true;
         }
      }
      _buckets[key].push_back(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      if(check(d)) return false;
      size_t key = bucketNum(d);
      _buckets[key].push_back(d);
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return false otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
      size_t key = bucketNum(d);
      for(size_t i = 0; i < _buckets[key].size(); i++){
         if(_buckets[key][i] == d){
            _buckets[key].erase(_buckets[key].begin() + i);
            return true;
         }
      }
      return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
