/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = false;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {_node = _node->_next ; return *this; }
      iterator operator ++ (int) {iterator temp = *this ; _node = _node->_next ; return temp; }
      iterator& operator -- () {_node = _node->_prev ; return *this; }
      iterator operator -- (int) {iterator temp = *this ; _node = _node->_prev ; return temp; }

      iterator& operator = (const iterator& i) { this->_node = i._node; return *this;}

      bool operator != (const iterator& i) const { 
         if(i._node == this->_node) return false;
         else return true;
      }
      bool operator == (const iterator& i) const {
         if(i._node == this->_node) return true;
         else return false;
      }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const {
      if(empty()) return end();
      iterator temp(_head->_next);
      return temp;
   }
   iterator end() const {
      // important:forms a “ring”
      if(empty()){ 
         iterator temp(_head);
         return temp;
      }
      iterator temp(_head->_prev);
      return temp;
   }
   bool empty() const {
      if(_head->_next == _head && _head->_prev == _head) return true;
      else return false;
   }
   size_t size() const {
      if(empty()) return 0;
      size_t len = 0;
      for(iterator iter = begin(); iter != end(); iter++){
         len += 1;
      }
      return len;
   }

   void push_back(const T& x) {
      _isSorted = false;
      if(!empty()){
         //nonempty-> insert between last & end
         DListNode<T>* list_end = _head->_prev;
         DListNode<T>* list_last = list_end->_prev;       
         DListNode<T>* new_mem = new DListNode<T>(x, list_last, list_end);
         list_last->_next = new_mem;
         list_end->_prev = new_mem;
      }
      else{
         //empty-> new a member also dummylist
         DListNode<T>* new_mem = new DListNode<T>(x, _head, 0);
         DListNode<T>* dummy = new DListNode<T>(x, new_mem, _head);
         new_mem->_next = dummy;
         _head->_next = new_mem;
         _head->_prev = dummy;
      }
   }
   void pop_front() {
      // _head go forward,then delete _head
      if (empty()) return;
      DListNode<T>* garbage = _head;
      _head = _head->_next;
      _head->_prev = garbage->_prev;
      delete garbage;
      _isSorted = false;
   }
   void pop_back() { 
      if(empty()) return;
      _isSorted = false;
      if(_head->_next->_next->_next == _head){
         DListNode<T>* garbage1 = _head->_next;
         DListNode<T>* garbage2 = _head->_prev;
         _head->_prev = _head->_next = _head;
         delete garbage1;
         delete garbage2;
      }
      // dummy become _head,last list become dummy,then delete _head
      DListNode<T>* list_end = _head->_prev;
      DListNode<T>* garbage = _head;
      _head = list_end;
      _head->_next = garbage->_next;
      delete garbage;
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      if(empty()) return false;
      _isSorted = false;
      DListNode<T>* list_prev = pos._node->_prev;
      DListNode<T>* list_next = pos._node->_next;
      DListNode<T>* garbage = pos._node;
      list_prev->_next = list_next;
      list_next->_prev = list_prev;
      delete garbage;
      return true;
   }
   bool erase(const T& x) {
      if(empty()) return false;
      _isSorted = false;
      for(iterator iter = begin(); iter != end(); iter++){
         if(*iter == x){
            erase(iter);
            return true;
         }
      }
      return false;
   }

   iterator find(const T& x) { 
      for(iterator iter = begin(); iter != end(); iter++){
         if(*iter == x) return iter;
      }
      return end();
   }
   // delete all nodes except for the dummy node
   void clear() {
      for(iterator iter = begin(); iter != end(); iter++){
         DListNode<T>* garbage = iter._node;
         delete garbage;
      }
      _head->_next = _head;
      _head->_prev = _head;
   } 
   //bubble sort->too slow(??)
   void sort() const {
      if(_isSorted) return;
      iterator iter_last = end();
      iter_last--;
      iterator iter_begin = begin();
      for(iterator iter_i = iter_last; iter_i != iter_begin; iter_i--){
         for(iterator iter_j = begin(); iter_j != iter_i; iter_j++){
            if(*iter_j > *iter_i) swap(iter_j, iter_i);
         }
      }
      _isSorted = true;
      return;   
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
   void swap(iterator &a, iterator &b) const{
      T temp = *a;
      *a = *b;
      *b = temp;
   }
};

#endif // DLIST_H
