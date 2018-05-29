#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <stdexcept>
#include <tuple>
#include <complex>
#include <utility>
#include <initializer_list>

const int MAXLEVEL = 32;
using namespace std;
namespace cs540{
template <typename Key_T, typename Mapped_T> class Map{

public:
	typedef pair<const Key_T, Mapped_T> ValueType;
	
	//SkipList
	int numElements = 0;
	
	//Forward Declarations
	struct Skip_Array_Node;
	class Iterator;
	class ConstIterator;
	class ReverseIterator;
	
	//Skip List Node
	struct Skip_Node{
		Skip_Node(){};
		ValueType * value;
		Skip_Node * next;
		Skip_Node * prev;
		Skip_Array_Node * addr; //Store the Address of the Skip_Array_Node
	};
	
	//Skip_Array_Node
	struct Skip_Array_Node{
		Skip_Node ** skipArray;
		Skip_Array_Node * next;
		Skip_Array_Node * prev;
		int arraySize;	//Size of the skip Array
	};
	
	Skip_Array_Node * head;
	Skip_Array_Node * tail;

	//Adds a new node to the skip list
	void addNode(ValueType value){
		int currentLevel = MAXLEVEL -1;
		Skip_Node * currentNodePos;
		//cout<<"AddNode called"<<endl;
		while(head->skipArray[currentLevel]->next == tail->skipArray[currentLevel] && currentLevel > 0){
			currentLevel--;
		}
		//CHANGE ALL THEM KEY ONLY HAS < SO WE HAVE TO FLIP THE > THEN OPERATIONS
		//FIRST NODE ADDED
		if(currentLevel == 0 && head->skipArray[currentLevel] -> next == tail->skipArray[currentLevel]){
			Skip_Array_Node * arrayNode;
			arrayNode = new Skip_Array_Node();
			//Fixing my Skip_List_Array pointers
			updateSkipListNode(arrayNode, head, tail);
			//Setting up my new arrays Nodes
			createSkipArray(arrayNode,value);
			//Updating the pointers of the Skip_Nodes in our array
			updateSkipNode(arrayNode);
			//cout<<"First Node added"<<endl;
		}
		else{
			currentNodePos = head->skipArray[currentLevel] -> next;
			bool foundSpot = false;
			//cout<<"Head " <<head << head->arraySize << endl;
			while(foundSpot == false){
				//Case where need to go right and next value is the tail
				
				//cout<< endl<< value.first <<" vs " << (*currentNodePos->value).first<<endl;
				if((get<0>(*(currentNodePos->value)) < get<0>(value))&&(currentNodePos->next == tail->skipArray[currentLevel])){ 
					//cout<<"Case one called"<<endl;
					//Go down a level
					if(currentLevel != 0){
						currentLevel --; //currentNodePos goes down a level here
					}
					else{   //Case where curr is less and the next value is tail
						//Node gets written between these two values
						//cout<<"Case one called"<<endl;
						Skip_Array_Node * arrayNode;
						arrayNode = new Skip_Array_Node();
						//Fixing my Skip_List_Array pointers
						updateSkipListNode(arrayNode, currentNodePos->addr, tail);
						//Setting up my new arrays Nodes
						createSkipArray(arrayNode, value);
						//Updating the pointers of the Skip_Nodes in our array
						updateSkipNode(arrayNode);
						foundSpot = true;	
					}
				}
				//Case where we need to go right and next value isnt tail
				else if((get<0>(*(currentNodePos->value)) < get<0>(value))&&(currentNodePos->next != tail->skipArray[currentLevel])){
					//cout<<"Case two called"<<endl;
					//Next value is greater but not at bottem
					if((get<0>(value) < get<0>(*(currentNodePos->next->value))) && currentLevel!= 0){
						currentLevel--;
					}
					//Next value is greater and at bottem
					else if((get<0>(value) < get<0>(*(currentNodePos->next->value))) && currentLevel== 0){
						//cout<<"Case two called"<<endl;
						//Case where currentNodePos is less but next is greater
						Skip_Array_Node * arrayNode;
						arrayNode = new Skip_Array_Node();
						//Fixing my Skip_List_Array pointers
						updateSkipListNode(arrayNode, currentNodePos->addr, currentNodePos->next->addr);
						//Setting up my new arrays Nodes
						createSkipArray(arrayNode, value);
						//Updating the pointers of the Skip_Nodes in our array
						updateSkipNode(arrayNode);
						foundSpot = true;
					}
					//If next value is also less then key we move over
					else if((get<0>(*(currentNodePos->next->value))) < get<0>(value)){
						currentNodePos = currentNodePos -> next;
					}
			
				}
				//Case where need to go left and next value is the start
				else if((get<0>(value) < get<0>(*(currentNodePos->value)))&&(currentNodePos->prev == head->skipArray[currentLevel])){
				//Go down a level
					//cout<<"Case three called"<<endl;
					if(currentLevel != 0){
						currentLevel --;//currentNodePos goes down a level here
					}
					else{	//Case where currentNodePos is less and the prev node is the head
						//cout<<"Case three called"<<endl;
						Skip_Array_Node * arrayNode;
						arrayNode = new Skip_Array_Node();
						//Fixing my Skip_List_Array pointers
						updateSkipListNode(arrayNode, head, currentNodePos->addr);
						//Setting up my new arrays Nodes
						createSkipArray(arrayNode, value);
						
						//Updating the pointers of the Skip_Nodes in our array
						updateSkipNode(arrayNode);
						foundSpot = true;
					}
				}
				//Case where we need to go left and next value isnt start
				else if((get<0>(value) < get<0>(*(currentNodePos->value)))&&(currentNodePos->prev != head->skipArray[currentLevel])){
					//Next value is less but not at bottem
					//cout<<"Case four called"<<endl;
					if(((get<0>(*(currentNodePos->prev->value))) < get<0>(value)) && currentLevel!= 0){
						currentLevel--;
					}
					//Next value is less and at bottem
					else if((get<0>(*(currentNodePos->prev->value))) < get<0>(value) && currentLevel== 0){
						//cout<<"Case four called"<<endl;
						//Case where currentNodePos is greater and prev one is less then key
						Skip_Array_Node * arrayNode;
						arrayNode = new Skip_Array_Node();
						//Fixing my Skip_List_Array pointers
						updateSkipListNode(arrayNode, currentNodePos->prev->addr, currentNodePos->addr);
						//Setting up my new arrays Nodes
						createSkipArray(arrayNode, value);
						//Updating the pointers of the Skip_Nodes in our array
						updateSkipNode(arrayNode);
						foundSpot = true;
					}
					//If next value is also greater then key we move over
					else if(get<0>(value) < (get<0>(*(currentNodePos->prev->value)))){
						currentNodePos = currentNodePos -> prev;
					}
				}
				currentNodePos = currentNodePos->addr->skipArray[currentLevel];
			}
		}
		numElements++;
	}
	//Finds a key if it exists and returns pointer to that node (will return iterator later)
//If value isnt found return the end iterator
	
	Iterator find(const Key_T & key){
		//cout<<"Hi"<<endl;
		//cout<<findSkip(key)<<endl;
		//cout<<"sasad"<<endl;
		return Iterator(findSkip(key));
	}

	ConstIterator find(const Key_T & key) const {
		//cout<<"Const find called"<<endl;
		return ConstIterator(findSkip(key));
	}
	
	
	Skip_Array_Node * findSkip(const Key_T key){
		int currentLevel = MAXLEVEL -1;
		//cout<<"Called"<<endl;
		Skip_Node * currentNodePos;
		//Skip_Array_Node * returnValue;
		while(head->skipArray[currentLevel]->next == tail->skipArray[currentLevel] && currentLevel > 0){
			currentLevel--;
		}
		//cout<<"Called"<<endl;
		if(currentLevel == 0 && head->skipArray[currentLevel] -> next == tail->skipArray[currentLevel]){
			//returnValue = tail;
			return tail;
		}
		else{
			//cout<<"currentLevel" <<currentLevel<<endl;
			currentNodePos = head->skipArray[currentLevel] -> next;
			bool findCompleted = false;
			while(findCompleted == false){
				//Case where need to go right and next value is the tail
				//cout <<"Head "<< head << " Tail " << tail <<endl;
				//cout <<key <<get<0> (currentNodePos->next->value) <<endl;
				//cout<< "Prev addr " << currentNodePos->prev->addr<< " level " << currentLevel << " Size :"<< currentNodePos->prev->addr->arraySize <<  endl;
				//cout<< "Next addr " << currentNodePos->next->addr << " level " << currentLevel << " Size :"<< currentNodePos->next->addr->arraySize <<  endl;
				//cout<<"Seg fault"<<endl;
				if((get<0>(*(currentNodePos->value)) < key)&&(currentNodePos->next == tail->skipArray[currentLevel])){
					//cout<< "Case 1" <<endl;
					//Go down a level
					if(currentLevel != 0){
						currentLevel --; //currentNodePos goes down a level here
						
					}
					//Value wasnt found so return the end
					else{   
						//returnValue = tail;
						findCompleted = true;
						return tail;
					}
				}
				//Case where we need to go right and next value isnt tail
				else if((get<0>(*(currentNodePos->value)) < key)&&(currentNodePos->next != tail->skipArray[currentLevel])){
					//cout<<"Second case"<< endl;
					//Next value is greater but not at bottem
					if((key < get<0>(*(currentNodePos->next->value))) && currentLevel!= 0){
						currentLevel--;
					}
					//If next value is also less then key we move over
					else if((get<0>(*(currentNodePos->next->value))) < key){
						currentNodePos = currentNodePos ->next;
					}
					//Next value is greater and at bottem therefore key doesnt exist
					else if((key < get<0>(*(currentNodePos->next->value))) && currentLevel== 0){
						//returnValue = tail;
						findCompleted = true;
						return tail;
					}
					//Next value was equal to the key
					else if((get<0>(*(currentNodePos->next->value)) == key)){
						//returnValue = currentNodePos->next->addr;
						findCompleted = true;
						return currentNodePos->next->addr;
					}
			
				}
				//Case where need to go left and next value is the start
				else if((key < get<0>(*(currentNodePos->value)))&&(currentNodePos->prev == head->skipArray[currentLevel])){
					//cout<<"Third case"<<endl;
					//Go down a level
					if(currentLevel != 0){
						currentLevel --;//currentNodePos goes down a level here
					}
					else{//Case where currentNodePos is less and the prev node is the head
						//Key therefore wasnt in the skiplist so return end
						//returnValue = tail;
						findCompleted = true;
						return tail;
					}
				}
				//Case where we need to go left and next value isnt start
				else if((key < get<0>(*(currentNodePos->value)))&&(currentNodePos->prev != head->skipArray[currentLevel])){
					//cout<<"Fourth case"<<endl;
					//Next value is less but not at bottem
					if((get<0>(*(currentNodePos->prev->value)) < key) && currentLevel!= 0){
						currentLevel--;
					}
					//Next value is less and at bottem
					else if((get<0>(*(currentNodePos->prev->value))) < key && currentLevel== 0){
						//Case where currentNodePos is greater and prev one is less then key
						//Key therefore wasnt in the skiplist so return end
						//returnValue = tail;
						findCompleted = true;
						return tail;
					}
					//If next value is also greater then key we move over
					else if(key < (get<0>(*(currentNodePos->prev->value)))){
						currentNodePos = currentNodePos -> prev;
					}
					//Prev value was equal to the key
					else if((get<0>(*(currentNodePos->prev->value)) == key)){
						//returnValue = currentNodePos->prev->addr;
						findCompleted = true;
						return currentNodePos->prev->addr;
					}
				}
				//Case where first value is equal to the key
				else if(get<0>(*(currentNodePos->value)) == key){
					//cout<<"First key is equal case"<<endl;
					//returnValue = currentNodePos->addr;
					findCompleted = true;
					return currentNodePos->addr;
				}
				currentNodePos = currentNodePos->addr->skipArray[currentLevel];
			}
		}
		//cout<<"Return value : "<<returnValue << " Tail: " << tail<<endl;
		//return returnValue;
		return 0;
	}
	Skip_Array_Node * findSkip(const Key_T key)const{
		int currentLevel = MAXLEVEL -1;
		//cout<<"Called"<<endl;
		Skip_Node * currentNodePos;
		//Skip_Array_Node * returnValue;
		while(head->skipArray[currentLevel]->next == tail->skipArray[currentLevel] && currentLevel > 0){
			currentLevel--;
		}
		//cout<<"Called"<<endl;
		if(currentLevel == 0 && head->skipArray[currentLevel] -> next == tail->skipArray[currentLevel]){
			//returnValue = tail;
			return tail;
		}
		else{
			//cout<<"currentLevel" <<currentLevel<<endl;
			currentNodePos = head->skipArray[currentLevel] -> next;
			bool findCompleted = false;
			while(findCompleted == false){
				//Case where need to go right and next value is the tail
				//cout <<"Head "<< head << " Tail " << tail <<endl;
				//cout <<key <<get<0> (currentNodePos->next->value) <<endl;
				//cout<< "Prev addr " << currentNodePos->prev->addr<< " level " << currentLevel << " Size :"<< currentNodePos->prev->addr->arraySize <<  endl;
				//cout<< "Next addr " << currentNodePos->next->addr << " level " << currentLevel << " Size :"<< currentNodePos->next->addr->arraySize <<  endl;
				//cout<<"Seg fault"<<endl;
				if((get<0>(*(currentNodePos->value)) < key)&&(currentNodePos->next == tail->skipArray[currentLevel])){
					//cout<< "Case 1" <<endl;
					//Go down a level
					if(currentLevel != 0){
						currentLevel --; //currentNodePos goes down a level here
						
					}
					//Value wasnt found so return the end
					else{   
						//returnValue = tail;
						findCompleted = true;
						return tail;
					}
				}
				//Case where we need to go right and next value isnt tail
				else if((get<0>(*(currentNodePos->value)) < key)&&(currentNodePos->next != tail->skipArray[currentLevel])){
					//cout<<"Second case"<< endl;
					//Next value is greater but not at bottem
					if((key < get<0>(*(currentNodePos->next->value))) && currentLevel!= 0){
						currentLevel--;
					}
					//If next value is also less then key we move over
					else if((get<0>(*(currentNodePos->next->value))) < key){
						currentNodePos = currentNodePos ->next;
					}
					//Next value is greater and at bottem therefore key doesnt exist
					else if((key < get<0>(*(currentNodePos->next->value))) && currentLevel== 0){
						//returnValue = tail;
						findCompleted = true;
						return tail;
					}
					//Next value was equal to the key
					else if((get<0>(*(currentNodePos->next->value)) == key)){
						//returnValue = currentNodePos->next->addr;
						findCompleted = true;
						return currentNodePos->next->addr;
					}
			
				}
				//Case where need to go left and next value is the start
				else if((key < get<0>(*(currentNodePos->value)))&&(currentNodePos->prev == head->skipArray[currentLevel])){
					//cout<<"Third case"<<endl;
					//Go down a level
					if(currentLevel != 0){
						currentLevel --;//currentNodePos goes down a level here
					}
					else{//Case where currentNodePos is less and the prev node is the head
						//Key therefore wasnt in the skiplist so return end
						//returnValue = tail;
						findCompleted = true;
						return tail;
					}
				}
				//Case where we need to go left and next value isnt start
				else if((key < get<0>(*(currentNodePos->value)))&&(currentNodePos->prev != head->skipArray[currentLevel])){
					//cout<<"Fourth case"<<endl;
					//Next value is less but not at bottem
					if((get<0>(*(currentNodePos->prev->value)) < key) && currentLevel!= 0){
						currentLevel--;
					}
					//Next value is less and at bottem
					else if((get<0>(*(currentNodePos->prev->value))) < key && currentLevel== 0){
						//Case where currentNodePos is greater and prev one is less then key
						//Key therefore wasnt in the skiplist so return end
						//returnValue = tail;
						findCompleted = true;
						return tail;
					}
					//If next value is also greater then key we move over
					else if(key < (get<0>(*(currentNodePos->prev->value)))){
						currentNodePos = currentNodePos -> prev;
					}
					//Prev value was equal to the key
					else if((get<0>(*(currentNodePos->prev->value)) == key)){
						//returnValue = currentNodePos->prev->addr;
						findCompleted = true;
						return currentNodePos->prev->addr;
					}
				}
				//Case where first value is equal to the key
				else if(get<0>(*(currentNodePos->value)) == key){
					//cout<<"First key is equal case"<<endl;
					//returnValue = currentNodePos->addr;
					findCompleted = true;
					return currentNodePos->addr;
				}
				currentNodePos = currentNodePos->addr->skipArray[currentLevel];
			}
		}
		//cout<<"Return value : "<<returnValue << " Tail: " << tail<<endl;
		//return returnValue;
		return 0;
	}
	void remove(const Key_T key){
		Skip_Array_Node * removeVal = findSkip(key);	//Later will return iterator
		//If find returns the tail, then that value isnt in our skipList
		if(removeVal == tail){
			throw out_of_range("Value is not currently in the skiplist so can't remove it.");
		}
		else{
			Skip_Array_Node * removePosArray = removeVal;
			//Fixes the Node pointers
			for(int i = 0; i < (removePosArray->arraySize); i++){
				Skip_Node * prevNode = removePosArray->skipArray[i]->prev;
				prevNode->next =  removePosArray->skipArray[i]->next;
				Skip_Node * nextNode = removePosArray->skipArray[i]->next;
				nextNode->prev =  removePosArray->skipArray[i]->prev;
			}
			//Fixes the Skip Node Array Pointers
			Skip_Array_Node * prevSkipArray = removePosArray->prev;
			prevSkipArray->next = removePosArray->next;
			Skip_Array_Node * nextSkipArray = removePosArray->next;
			nextSkipArray->prev = removePosArray->prev;
			
			//Free the removed Skip_Node
			for(int i = 0; i < (removePosArray->arraySize);i++){
				delete(removePosArray->skipArray[i]->value);
				delete(removePosArray->skipArray[i]);
			}
			//Free Skip_Array_Node
			delete [](removePosArray->skipArray);
			delete(removePosArray);
		}
	}
			
	
	//Determines the amount of levels a skip list node has
	int coinFlip(){
		int coin = rand() % 2;
		int promotion = 1;
		while(coin == 0 && promotion < 31){
			promotion++;
			coin = rand() % 2;
		}
		return promotion;
	}

	//Sets up a skipList array
	void createSkipArray(Skip_Array_Node * arrayNode, ValueType value){
		int promotions = coinFlip(); //ADD NODE AND RANDOMIZE NUMBER OF LEVELS
		arrayNode->arraySize = promotions;
		//cout<<"Num of promotions " << promotions << endl;
		arrayNode->skipArray = new Skip_Node * [promotions]();
		//cout<<"Array size is " << promotions << endl;
		for(int i = 0; i < promotions; i++){
			//cout<<i << " " << promotions << endl;
			Skip_Node * node = new Skip_Node();
			node->value = new ValueType(value);
			node->addr = arrayNode;
			//cout<<"Addr at "<< i <<" : "<< node->addr << endl;
			arrayNode->skipArray[i] = node;
		}
	}
	//Updates the array node pointers
	void updateSkipListNode(Skip_Array_Node * curr, Skip_Array_Node* front, Skip_Array_Node* back){
		//cout<<"curr " << curr << " Front "<<front << " Back " << back <<endl;
		curr->prev = front;
		front->next = curr;
		curr->next = back;
		back->prev = curr;
	}
	//Updates the prev and next pointers in the new node we added
	void updateSkipNode(Skip_Array_Node * curr){
		int currSize = curr->arraySize;
		int count = 0;
		//Left Path
		Skip_Array_Node * prevArray = curr->prev;
		while(currSize != count){
			if(count >= prevArray->arraySize){
				prevArray = prevArray->prev;
			}
			else{
				while(count < (prevArray->arraySize) && currSize != count){
					curr->skipArray[count]->prev = prevArray->skipArray[count];
					prevArray->skipArray[count]->next = curr->skipArray[count];
					count++;
				}
			}
		}
		//Right Path
		count = 0;
		Skip_Array_Node * nextArray = curr->next;
		while(currSize != count){
			if(count >= (nextArray->arraySize)){
				nextArray = nextArray->next;
			}
			else{
				while(count < (nextArray->arraySize) && currSize != count){
					curr->skipArray[count]->next = nextArray->skipArray[count];
					nextArray->skipArray[count]->prev = curr->skipArray[count];
					count++;
				}
			}
		}
	}
	//This constructor creates an empty map.
	Map(){
		head = new Skip_Array_Node();
		tail = new Skip_Array_Node();
		head->next = tail;
		tail->prev = head;
		head->skipArray = new  Skip_Node * [MAXLEVEL]();
		head->arraySize = MAXLEVEL;
		tail->skipArray = new  Skip_Node * [MAXLEVEL]();
		tail->arraySize = MAXLEVEL;
		for(int i = 0; i < MAXLEVEL; i++){
			Skip_Node * headNode = new Skip_Node();
			Skip_Node * tailNode = new Skip_Node();
			headNode->addr = head;
			tailNode->addr = tail;
			head->skipArray[i] = headNode;
			tail->skipArray[i] = tailNode;
			head->skipArray[i]-> next = tail->skipArray[i];
			tail->skipArray[i]-> prev = head->skipArray[i];		
		}
	}	

	//Initializer skip list. Support for creation of Map with initial values, such as Map<string,int> m{{"key1", 1}, {"key2", 2}};.
	Map(std::initializer_list<std::pair<const Key_T, Mapped_T>> valueList){
		head = new Skip_Array_Node();
		tail = new Skip_Array_Node();
		head->next = tail;
		tail->prev = head;
		head->skipArray = new  Skip_Node * [MAXLEVEL]();
		head->arraySize = MAXLEVEL;
		tail->skipArray = new  Skip_Node * [MAXLEVEL]();
		tail->arraySize = MAXLEVEL;
		for(int i = 0; i < MAXLEVEL; i++){
			Skip_Node * headNode = new Skip_Node();
			Skip_Node * tailNode = new Skip_Node();
			headNode->addr = head;
			tailNode->addr = tail;
			head->skipArray[i] = headNode;
			tail->skipArray[i] = tailNode;
			head->skipArray[i]-> next = tail->skipArray[i];
			tail->skipArray[i]-> prev = head->skipArray[i];		
		}
	
		
		auto it = valueList.begin();
		while(it != valueList.end()){
			addNode(*(it));
			it++;
			
		}
	}

	//Destructor, release any acquired resources.
	~Map(){
		Skip_Array_Node * freePosArray = tail;
		//Frees tail Skip_Nodes
		for(int i = 0; i < (freePosArray->arraySize);i++){
			//delete(freePosArray->skipArray[i]->value);
			delete(freePosArray->skipArray[i]);
			
		}
		delete [](freePosArray->skipArray);
		freePosArray = freePosArray->prev;
		
		while(freePosArray != head){
			//Free the removed Skip_Node
			for(int i = 0; i < (freePosArray->arraySize);i++){
				delete(freePosArray->skipArray[i]->value);
				delete(freePosArray->skipArray[i]);
			}
			delete [](freePosArray->skipArray);
			freePosArray = freePosArray->prev;
		}
		
		//Free the heads Skip_Nodes
		for(int i = 0; i < (freePosArray->arraySize);i++){
			//delete(freePosArray->skipArray[i]->value);
			delete(freePosArray->skipArray[i]);
		}
		delete [](freePosArray->skipArray);
		
		freePosArray = tail;
		Skip_Array_Node * prevFreePos;
		//Free the skip_array_nodes
		while(freePosArray != head){
			prevFreePos = freePosArray->prev;
			delete(freePosArray);
			freePosArray = prevFreePos;
		}
		//Free head skip array node
		delete(freePosArray);
	}
		
	//Copy constructor
	Map(const Map & oldMap){
		//cout<<"Copy thingy called"<<endl;
		ConstIterator iter = oldMap.begin();
		head = new Skip_Array_Node();
		tail = new Skip_Array_Node();
		head->next = tail;
		tail->prev = head;
		head->skipArray = new  Skip_Node * [MAXLEVEL]();
		head->arraySize = MAXLEVEL;
		tail->skipArray = new  Skip_Node * [MAXLEVEL]();
		tail->arraySize = MAXLEVEL;
		for(int i = 0; i < MAXLEVEL; i++){
			Skip_Node * headNode = new Skip_Node();
			Skip_Node * tailNode = new Skip_Node();
			headNode->addr = head;
			tailNode->addr = tail;
			head->skipArray[i] = headNode;
			tail->skipArray[i] = tailNode;
			head->skipArray[i]-> next = tail->skipArray[i];
			tail->skipArray[i]-> prev = head->skipArray[i];		
		}
		Skip_Array_Node * curr = head;
		while(iter != oldMap.end()){
			//Case where currentNodePos is greater and prev one is less then key
			Skip_Array_Node * arrayNode;
			arrayNode = new Skip_Array_Node();
			//Fixing my Skip_List_Array pointers
			updateSkipListNode(arrayNode, curr, curr->next);
			//Setting up my new arrays Nodes
			createSkipArray(arrayNode, (*iter.it->skipArray[0]->value));
			//Updating the pointers of the Skip_Nodes in our array
			updateSkipNode(arrayNode);
			iter++;
			curr= curr->next;
		}
		setNumElements(oldMap.size());
	}

	//NEED TO WRITE THIS
	Map &operator=(const Map & oldMap){
		ConstIterator iter = oldMap.begin();
		head = new Skip_Array_Node();
		tail = new Skip_Array_Node();
		head->next = tail;
		tail->prev = head;
		head->skipArray = new  Skip_Node * [MAXLEVEL]();
		head->arraySize = MAXLEVEL;
		tail->skipArray = new  Skip_Node * [MAXLEVEL]();
		tail->arraySize = MAXLEVEL;
		for(int i = 0; i < MAXLEVEL; i++){
			Skip_Node * headNode = new Skip_Node();
			Skip_Node * tailNode = new Skip_Node();
			headNode->addr = head;
			tailNode->addr = tail;
			head->skipArray[i] = headNode;
			tail->skipArray[i] = tailNode;
			head->skipArray[i]-> next = tail->skipArray[i];
			tail->skipArray[i]-> prev = head->skipArray[i];		
		}
		Skip_Array_Node * curr = head;
		while(iter != oldMap.end()){
			//Case where currentNodePos is greater and prev one is less then key
			Skip_Array_Node * arrayNode;
			arrayNode = new Skip_Array_Node();
			//Fixing my Skip_List_Array pointers
			updateSkipListNode(arrayNode, curr, curr->next);
			//Setting up my new arrays Nodes
			createSkipArray(arrayNode, (*iter.it->skipArray[0]->value));
			//Updating the pointers of the Skip_Nodes in our array
			updateSkipNode(arrayNode);
			
			iter++;
			
			curr= curr->next;

		}
		setNumElements(oldMap.size());
		return *this;
	//	
	}



	//Returns a reference to the mapped object at the specified key. If the key is not in the Map, throws std::out_of_range.
	Mapped_T &at(const Key_T & key){//Calls find if find returns the end then throw exception
		Iterator iter = (find(key));
		if(iter.it != tail){
			return get<1>(*iter.it->skipArray[0]->value);
		}
		else{
			throw out_of_range("Value is not currently in the skiplist so can't access it.");
		}
	}
	
	
	//Returns a const reference to the mapped object at the specified key. If the key is not in the map, throws std::out_of_range.
	const Mapped_T &at(const Key_T & key) const{
		//cout<<"Hi"<<endl;
		ConstIterator iter = find(key);
		if(iter.it != tail){
			return get<1>(*iter.it->skipArray[0]->value);
		}
		else{
			throw out_of_range("Value is not currently in the skiplist so can't access it.");
		}
	}
	

	//If key is in the map, return a reference to the corresponding mapped object. If it is not, value initialize a mapped object for that key and returns a reference to it. This operator may not be used for a Mapped_T class type that does not support default construction.
	Mapped_T &operator[](const Key_T & key){
		Iterator iter;
		iter.it = findSkip(key);
		Mapped_T returnVal;
		if(iter.it != tail){
			ValueType val = (*iter.it->skipArray[0]->value);
			return get<1>(val);
		}
		else{
			ValueType newValue (key,returnVal);
			addNode(newValue);
			iter.it = findSkip(key);
			return get<1>(*(iter.it->skipArray[0]->value));	
		}
	}

	//See modifers section for these
	std::pair<Iterator, bool> insert(const ValueType & value){
		bool newElement = false;
		Iterator iter;
		//cout<<"Value added here"<<endl;
		iter.it = findSkip(get<0>(value));
		//Element doesn't exist
		if(iter.it == tail){
			addNode(value);
			iter.it = findSkip(get<0>(value));
			newElement = true;
		}
		pair<Iterator,bool> returnValue(iter, newElement);
		return returnValue;
	}

	//ASK SOMEONE ABOUT A LITTLE CONFUSED
	template <typename IT_T>
	void insert(IT_T range_beg, IT_T range_end){
		for(IT_T it = range_beg; it < range_end; it++){
			addNode(*(it));
		}
	}
	//Returns the number of elements in the map. Just keep a parameter thats edited in add and remove
	size_t size() const{
		return numElements;
	}

	bool empty() const{
		bool returnVal = false;
		if(numElements == 0){
			returnVal = true;
		}
		return returnVal;
	}
	
	//Removes the given object from the map. The object may be indicated by iterator, or by key. If given by key, throws std::out_of_range if the key is not in the Map
	void erase(Iterator pos){
		remove(get<0> (*(pos.it->skipArray[0]->value)));
		numElements--;
	}
	
	
	void erase(const Key_T & key){
		remove(key);
		numElements--;
	}

	//Removes all elements from the map.
	void clear(){
		Skip_Array_Node * clear = head->next;
		while(clear != tail){
			remove(get<0> (*(clear->skipArray[0]->value)));
			clear = clear->next;
		}
		numElements = 0;
	}
	Skip_Array_Node * getHead() const{
		return this->head;
	}
	void setNumElements(int num){
		numElements = num;
	}	
	int getNumElements(){
		return numElements;
	}
	Skip_Array_Node * getTail()const{
		return this->tail;
	}
	
	//Iterators
	class Iterator{
		public:
		Skip_Array_Node * it;
		//Might need to define these not sure
		Iterator(){
			it = NULL;
		}
		Iterator(Skip_Array_Node * pos){
			//cout<<pos<<endl;
			it = pos;
		}
		//Iterator(const Iterator & iter);
		//~Iterator();	
		Iterator& operator=(const Iterator & iter){
			this->it = iter.it;
			return *this;
		}

		Iterator &operator++(){
			this->it = this->it->next;
			return *this;
		}	

		Iterator operator++(int){
			this->it = this->it->next;
			return *this;
		}	

		Iterator &operator--(){
			this->it = this->it->prev;
			return *this;
		}		

		Iterator operator--(int){			
			this->it = this->it->prev;
			return *this;
		}

		ValueType &operator*() const {
			return *this->it->skipArray[0]->value;
		}
		
		//MIGHT BE WRONG
		ValueType *operator->() const{
			return (this->it->skipArray[0]->value);
		}
		
		
	};
	class ConstIterator{
		public:
		Skip_Array_Node * it;
		ConstIterator(){
			it = NULL;
		}
		ConstIterator(Skip_Array_Node * pos){
			it = pos;
		}
		//ConstIterator(const ConstIterator & );

		ConstIterator(const Iterator & iter){
			this->it = iter.it;
		}	
		//~ConstIterator();
		ConstIterator& operator=(const ConstIterator & constIter){
			ConstIterator iter;
			this->it = constIter.it;
			return *this;
		}
		ConstIterator &operator++(){
			this->it = this->it->next;
			return *this;
		}

		ConstIterator operator++(int){
			this->it = this->it->next;
			return *this;
		}	

		ConstIterator &operator--(){			
			this->it = this->it->prev;
			return *this;
		}
	
		ConstIterator &operator--(int){			
			this->it = this->it->prev;
			return *this;
		}
		
		const ValueType &operator*() const{			
			return *this->it->skipArray[0]->value;
		}
	
		const ValueType *operator->() const{			
			return (this->it->skipArray[0]->value);	
		}
		
	};
	class ReverseIterator{
		public:
		Skip_Array_Node * it;
		ReverseIterator(){
			it = NULL;
		}
		//ReverseIterator(const ReverseIterator & );
		//~ReverseIterator();
		ReverseIterator& operator=(const ReverseIterator & reverseIter){
			this->it = reverseIter.it;
			return *this;
		}	

		ReverseIterator &operator++(){
			this->it = this->it->prev;
			return *this;
		}

		ReverseIterator operator++(int){
			this->it = this->it->prev;
			return *this;
		}

		ReverseIterator &operator--(){			
			this->it = this->it->next;
			return *this;
		}

		ReverseIterator operator--(int){			
			this->it = this->it->next;
			return *this;
		}
	
		ValueType &operator*() const{			
			return (*this->it->skipArray[0]->value);
		}
		
		ValueType *operator->() const{			
			return (this->it->skipArray[0]->value);
		}
		
	};

	//Returns an Iterator pointing to the first element, in order. 
	Iterator begin(){
		Iterator iter;
		iter.it = head->next;
		return iter;
	}

	//Returns an Iterator pointing one past the last element, in order.
	Iterator end(){
		Iterator iter;
		iter.it = tail;
		return iter;
	}
	
	//Returns a ConstIterator pointing to the first element, in order.
	ConstIterator begin() const{
		ConstIterator iter;
		iter.it = head->next;
		return iter;
	}

	//Returns a ConstIterator pointing one past the last element, in order.
	ConstIterator end() const{
		ConstIterator iter;
		iter.it = tail;
		return iter;
	}

	//Returns an ReverseIterator to the first element in reverse order, which is the last element in normal order.
	ReverseIterator rbegin(){
		ReverseIterator iter;
		iter.it = tail->prev;
		return iter;
	}

	//Returns an ReverseIterator pointing to one past the last element in reverse order, which is one before the first element in normal order.
	ReverseIterator rend(){
		ReverseIterator iter;
		iter.it = head;
		return iter;
	}

	
	friend bool operator==(const Iterator & iter1, const Iterator & iter2){
		bool equal = false;
		if(iter1.it == iter2.it){
			equal = true;
		}
		return equal;
	}
	friend bool operator==(const ConstIterator & iter1, const ConstIterator & iter2){
		bool equal = false;
		if(iter1.it == iter2.it){
			equal = true;
		}
		return equal;
	}
	friend bool operator==(const Iterator & iter1, const ConstIterator & iter2){
		bool equal = false;
		if(iter1.it == iter2->it){
			equal = true;
		}
		return equal;
	}
	friend bool operator==(const ConstIterator & iter1, const Iterator & iter2){
		bool equal = false;
		if(iter1.it == iter2.it){
			equal = true;
		}
		return equal;
	}
	friend bool operator!=(const Iterator & iter1, const Iterator & iter2){
		bool notEqual = true;
		if(iter1.it == iter2.it){
			notEqual = false;
		}
		return notEqual;
	}
	friend bool operator!=(const ConstIterator & iter1, const ConstIterator & iter2){
		bool notEqual = true;
		if(iter1.it == iter2.it){
			notEqual = false;
		}
		return notEqual;
	}
	friend bool operator!=(const Iterator & iter1, const ConstIterator & iter2){
		bool notEqual = true;
		if(iter1.it == iter2.it){
			equal = false;
		}
		return equal;
	}
	friend bool operator!=(const ConstIterator & iter1, const Iterator & iter2){
		bool notEqual = true;
		if(iter1.it == iter2.it){
			equal = false;
		}
		return equal;
	}
	friend bool operator==(const ReverseIterator & iter1, const ReverseIterator & iter2){
		bool equal = false;
		if(iter1.it == iter2.it){
			equal = true;
		}
		return equal;
	}
	friend bool operator!=(const ReverseIterator & iter1, const ReverseIterator & iter2){
		bool notEqual = true;
		if(iter1.it == iter2.it){
			notEqual = false;
		}
		return notEqual;
	}

	//Check comparsion section for these
	
	friend bool operator==(const Map & map1, const Map & map2){
		bool equal = true;
		ConstIterator iter1 = map1.begin();
		ConstIterator iter2 = map2.begin();
		if(map1.size() == map2.size()){
			while(iter1 != map1.end() && iter2 != map2.end()){
				if((*iter1.it->skipArray[0]->value) != (*iter2.it->skipArray[0]->value)){
					equal = false;
				}
				
				iter1++;
				iter2++;
			}
			
		}
		else{
			equal = false;
		}
		return equal; 
	}
	
friend bool operator!=(const Map & map1, const Map & map2){
		bool equal = false;
		ConstIterator iter1 = map1.begin();
		ConstIterator iter2 = map2.begin();
		if(map1.size() == map2.size()){
			while(iter1 != map1.end()){
				if(iter1.it->skipArray[0]->value != iter2.it->skipArray[0]->value){
					equal = true;
				}
				iter1.it++;
				iter2.it++;
			}
			
		}
		else{
			equal = true;
		}
		return equal; 
	}

	friend bool operator<(const Map & map1, const Map & map2){
		bool shorter = false;
		ConstIterator iter1 = map1.begin();
		ConstIterator iter2 = map2.begin();
		if(map1.size() == map2.size()){
			while(iter1 != map1.end()){
				if(get<0> (*iter1.it->skipArray[0]->value) < get<0> (*iter2.it->skipArray[0]->value)){
					shorter = true;
				}
				if(get<1> (*iter1.it->skipArray[0]->value) < get<1> (*iter2.it->skipArray[0]->value)){
					shorter = true;
				}
				iter1.it = iter1.it->next;
				iter2.it = iter2.it->next;
			}
			
		}
		if(map1.getNumElements() < map2.getNumElements() && shorter == false){
			shorter = true;
		}
		return shorter;
	}
	
};
}
#endif
	
