/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include <stack>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
  
   //constructor??
   BSTreeNode(const T& d, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0):
   _data(d), _left(l), _right(r) {}
   
   T                 _data;
   BSTreeNode<T>*    _left;
   BSTreeNode<T>*    _right;

};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   BSTree(){ 
      _root = new BSTreeNode<T>(T());
      _size = 0;}
   ~BSTree(){ clear();}

   class iterator {
      friend class BSTree;  
   public:
      // iterator(BSTreeNode<T>* n, BSTreeNode<T>* r){
      //    //use it to record the trace from root to the node calling this funcion
      //    BSTreeNode<T>* move_node = r;
      //    while(move_node != n && move_node != NULL){
      //       //bigger->go right
      //       if(n->_data > move_node->_data){
      //          trace.push_back(move_node);
      //          move_node = move_node->_right;
      //       }
      //       //smaller->go left
      //       else if(n->_data <= move_node->_data){
      //          trace.push_back(move_node);
      //          move_node = move_node->_left;
      //       }
      //    }
      //    _node = move_node;
      // }
      iterator(BSTreeNode<T>* node, stack<BSTreeNode<T>*> trace):_node(node),_trace(trace){}
      ~iterator() {}
      void operator()(){
         cout << "data=" << _node->_data << endl;
         stack<BSTreeNode<T>*> pr = _trace;
         while(!pr.empty()){
            BSTreeNode<T>* n = pr.top();
            pr.pop();
            cout << n->_data << endl;
         }
         
      }
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {
         //only root
         if(_trace.empty() && _node->_right == NULL){
            _node = _node->_right;
            return *this;
         }
         
         if(_node->_right != NULL){
            _trace.push(_node);
            _node = _node->_right;
            while(_node != NULL){
               _trace.push(_node);
               _node = _node->_left;
            }
            _node = _trace.top();
            _trace.pop();
         }
         else{
            BSTreeNode<T>* last_node  = _trace.top();
            bool end_flag = false;
            while(last_node->_right == _node){
               _node = last_node;
               _trace.pop();
               if(_trace.empty()){
                  end_flag = true;
                  break;
               }
               last_node = _trace.top();
            }
            if(end_flag) _node = 0;
            else{
               _node = last_node;
               _trace.pop();
            }
         }
         return *this;
      }
      iterator operator ++ (int) {iterator temp = *this; ++(*this); return temp;}
      iterator& operator -- () { 
         //especially for case of end()--
         if(_node == NULL){
            BSTreeNode<T>* last_node  = _trace.top();
            _node = last_node;
            _trace.pop();
            return *this;
         }

         if(_node->_left != NULL){
            _trace.push(_node);
            _node = _node->_left;
            while(_node != NULL){
               _trace.push(_node);
               _node = _node->_right;
            }
            _node = _trace.top();
            _trace.pop();
         }
         else{
            BSTreeNode<T>* last_node  = _trace.top();
            bool end_flag = false;
            while(last_node->_left == _node){
               _node = last_node;
               _trace.pop();
               if(_trace.empty()){
                  end_flag = true;
                  break;
               }
               last_node = _trace.top();
            }
            if(end_flag) _node = 0;
            else{
               _node = last_node;
               _trace.pop();
            }
         }
         return *this;
      }
      iterator operator -- (int) {iterator temp = *this ; --*this; return temp; }

      iterator& operator = (const iterator& i) { this->_node = i._node; return *this;}

      bool operator != (const iterator& i) const { 
         if(i._node == this->_node) return false;
         else return true;
      }
      bool operator == (const iterator& i) const {
         if(i._node == this->_node) return true;
         else return false;
      }
      BSTreeNode<T>* getlast_node(){
         BSTreeNode<T>* last = _trace.top();
         return last;
      }
      BSTreeNode<T>* get_node(){
         BSTreeNode<T>* now = _node;
         return now;
      }
   private:
      BSTreeNode<T>*               _node;
      stack< BSTreeNode<T>* >     _trace;
   };
   
   // TODO: BSTree functions
   iterator begin() const {
      if(empty()) return end();
      iterator temp = findmin_it(_root);
      return temp;
   }
   iterator end() const {
      if(empty()){
         stack<BSTreeNode<T>*> blank;
         iterator temp(_root, blank);
         return temp;
      }
      iterator temp = findmax_next_it(_root);
      return temp;
   }
   bool empty() const {
      if(_size == 0) return true;
      else return false;
   }
   size_t size() const { return _size; }

   void insert(const T& x) {
      if(empty()){
         if(_root == NULL) _root = new BSTreeNode<T>(T());
         _root->_data = x;
         _size++;
         return;
      }
      BSTreeNode<T>* cur_node = _root;
      BSTreeNode<T>* record = 0;
      int dir = 0;
      while(cur_node != NULL){
         if(x > cur_node->_data){
            record = cur_node;
            cur_node = cur_node->_right;
            dir = 1;
         }   
         else{
            record = cur_node;
            cur_node = cur_node->_left;
            dir = 2;
         } 
      }
      cur_node = new BSTreeNode<T>(x);
      if(dir == 1) record->_right = cur_node;
      if(dir == 2) record->_left = cur_node;
      _size++;
      return;
   }
   
   void pop_front() { erase(begin()); }
   void pop_back()  { 
      iterator temp = end();
      temp--;
      erase(temp); }

   // return false if nothing to erase
   bool erase(iterator pos) {
      // succ ->return null or minimum in the right subtree
      // without successor
      if(empty()) return false;
      
      // //for test==============================================
      // cout << "need to delete:" << pos.get_node()->_data << endl;
      // //======================================================
      
      iterator successor = findsucc(pos.get_node());
      BSTreeNode<T>* suc_node = successor.get_node();
      
      // //for test==============================================
      // cout << "successor's address:" << suc_node << endl;
      // //======================================================
      
      if( suc_node == NULL){
         BSTreeNode<T>* pos_node = pos.get_node();
         if(pos_node == _root){
            _root = _root->_left;
            
            // //for test==============================================
            // cout << "delete:" << pos_node->_data << endl;
            // cout << endl;
            // //======================================================
            
            delete pos_node;
            _size--;
            return true;
         }
         BSTreeNode<T>* pos_parent = pos.getlast_node();
         if(pos_parent->_right == pos_node)
            pos_parent->_right = pos_node->_left;
         if(pos_parent->_left == pos_node) 
            pos_parent->_left = pos_node->_left;
         
         // //for test==============================================
         // cout << "delete:" << pos_node->_data << endl;
         // cout << endl;
         // //======================================================
         
         delete pos_node;
         _size--;
         return true;
      }
      // with successor
      BSTreeNode<T>* suc_parent = successor.getlast_node();
      BSTreeNode<T>* pos_node = pos.get_node();
      bool del_root = false;
      if(pos_node == _root) del_root = true;

      if(suc_parent->_left == suc_node) suc_parent->_left = suc_parent->_left->_right;
      if(suc_parent->_right == suc_node) suc_parent->_right = suc_parent->_right->_right;
      suc_node->_left = pos_node->_left;
      suc_node->_right = pos_node->_right;
      
      if(del_root){
         _root = suc_node;
      }
      else{
         BSTreeNode<T>* pos_parent = pos.getlast_node();
         if(pos_parent->_right == pos_node) pos_parent->_right = suc_node;
         if(pos_parent->_left == pos_node) pos_parent->_left = suc_node;
      }
      _size--;
      
      // //for test==============================================
      // cout << "delete:" << pos_node->_data << endl;
      // cout << endl;
      // //======================================================
      
      delete pos_node;
      return true;
   }
   bool erase(const T& x) {
      iterator pos = find(x);
      if(pos == end()) return false;
      return erase(pos);
   }

   iterator find(const T& x) { 
      BSTreeNode<T>* cur_node = _root;
      stack<BSTreeNode<T>*> trace;
      while(cur_node != NULL && cur_node->_data != x){
         if(x > cur_node->_data){
            trace.push(cur_node);
            cur_node = cur_node->_right;
         }
         else{
            trace.push(cur_node);
            cur_node = cur_node->_left;
         }
      }
      if(cur_node == NULL) return end();
      else{
         // while(cur_node->_left->_data == x){
         //    trace.push(cur_node);
         //    cur_node = cur_node->_left;
         // }
         iterator temp(cur_node, trace);
         return temp;
      }
   }

   void clear() { 
      while(_size != 0 && _root->_left != NULL){
         pop_front();
      }
      while(_size != 0 && _root->_right != NULL){
         pop_back();
      }
      _size = 0;

   } 
   void sort() const { return; }
   void print() const{ print(_root);}
   void print(BSTreeNode<T>* node) const{
      static int level = 0;
      if(node == NULL){
         for(int i = 0; i < level; i++)
            cout << "  ";
         cout << "[0]" << endl;
      }
      else{
         for(int i = 0; i < level; i++){
            cout << "  ";
         }
         cout << node->_data << endl;
         level += 1;
         print(node->_left);
         print(node->_right);
         level -= 1;
      }
      
   }

private:
   BSTreeNode<T>*       _root;
   size_t               _size = 0;
      
   
   // below:functions could be used by public functions 
   // BSTreeNode<T>* findmin_node(BSTreeNode<T>* node){
   //    if(node->_left == NULL) return node;
   //    else return findmin_node(node->_left);
   // }
   iterator findsucc(BSTreeNode<T>* node){
      stack<BSTreeNode<T>*> trace;
      if(node->_right == NULL){
         iterator temp(0,trace);
         return temp;
      }
      if(node->_left == NULL){
         
      }
      trace.push(node);
      node = node->_right;
      while(node != NULL){
         trace.push(node);
         node = node->_left;
      }
      node = trace.top();
      trace.pop();
      iterator temp(node,trace);
      return temp;
   }
   iterator findmin_it(BSTreeNode<T>* _start) const {
      BSTreeNode<T>* temp = _start;
      stack<BSTreeNode<T>*> trace;
      while(temp != NULL){
         trace.push(temp);
         temp = temp->_left;
      }
      temp = trace.top();
      trace.pop();
      iterator i(temp, trace);
      return i;
   } 
   iterator findmax_next_it(BSTreeNode<T>* _start) const {
      BSTreeNode<T>* temp = _start;
      stack<BSTreeNode<T>*> trace; 
      while(temp != NULL){
         trace.push(temp);
         temp = temp->_right;
      }
      iterator i(0, trace);
      return i;
   } 
};

#endif // BST_H
