/*
 Student Name:
 Date:

=======================
ECE 2035 Project 2-1:
=======================
This file provides definition for the structs and functions declared in the
header file. It also contains helper functions that are not accessible from
outside of the file.

FOR FULL CREDIT, BE SURE TO TRY MULTIPLE TEST CASES and DOCUMENT YOUR CODE.

===================================
Naming conventions in this file:
===================================
1. All struct names use camel case where the first letter is capitalized.
  e.g. "HashTable", or "HashTableEntry"

2. Variable names with a preceding underscore "_" will not be called directly.
  e.g. "_HashTable", "_HashTableEntry"

  Recall that in C, we have to type "struct" together with the name of the struct
  in order to initialize a new variable. To avoid this, in hash_table.h
  we use typedef to provide new "nicknames" for "struct _HashTable" and
  "struct _HashTableEntry". As a result, we can create new struct variables
  by just using:
    - "HashTable myNewTable;"
     or
    - "HashTableEntry myNewHashTableEntry;"

  The preceding underscore "_" simply provides a distinction between the names
  of the actual struct defition and the "nicknames" that we use to initialize
  new structs.
  [See Hidden Definitions section for more information.]

3. Functions, their local variables and arguments are named with camel case, where
  the first letter is lower-case.
  e.g. "createHashTable" is a function. One of its arguments is "numBuckets".
       It also has a local variable called "newTable".

4. The name of a struct member is divided by using underscores "_". This serves
  as a distinction between function local variables and struct members.
  e.g. "num_buckets" is a member of "HashTable".

*/

/****************************************************************************
* Include the Public Interface
*
* By including the public interface at the top of the file, the compiler can
* enforce that the function declarations in the the header are not in
* conflict with the definitions in the file. This is not a guarantee of
* correctness, but it is better than nothing!
***************************************************************************/
#include "hash_table.h"


/****************************************************************************
* Include other private dependencies
*
* These other modules are used in the implementation of the hash table module,
* but are not required by users of the hash table.
***************************************************************************/
#include <stdlib.h>   // For malloc and free
#include <stdio.h>    // For printf

/**
 * This structure represents an a hash table.
 * Use "HashTable" instead when you are creating a new variable. [See top comments]
 */
struct _HashTable {
  /** The array of pointers to the head of a singly linked list, whose nodes
      are HashTableEntry objects */
  HashTableEntry** buckets;

  /** The hash function pointer */
  HashFunction hash;

  /** The number of buckets in the hash table */
  unsigned int num_buckets;
};

/**
 * This structure represents a hash table entry.
 * Use "HashTableEntry" instead when you are creating a new variable. [See top comments]
 */
struct _HashTableEntry {
  /** The key for the hash table entry */
  unsigned int key;

  /** The value associated with this hash table entry */
  void* value;

  /**
  * A pointer pointing to the next hash table entry
  * NULL means there is no next entry (i.e. this is the tail)
  */
  HashTableEntry* next;
};


/****************************************************************************
* Private Functions
*
* These functions are not available outside of this file, since they are not
* declared in hash_table.h.
***************************************************************************/
/**
* createHashTableEntry
*
* Helper function that creates a hash table entry by allocating memory for it on
* the heap. It initializes the entry with key and value, initialize pointer to
* the next entry as NULL, and return the pointer to this hash table entry.
*
* @param key The key corresponds to the hash table entry
* @param value The value stored in the hash table entry
* @return The pointer to the hash table entry
*/
static HashTableEntry* createHashTableEntry(unsigned int key, void* value) {
  HashTableEntry* myHTEntry = (HashTableEntry*)malloc(sizeof(HashTableEntry));
  myHTEntry->key =key; // store key
  myHTEntry->value = value; // store value 
  myHTEntry->next = NULL; // initialize next pointer.
  return myHTEntry; // return HTentry pointer
}

/**
* findItem
*
* Helper function that checks whether there exists the hash table entry that
* contains a specific key.
*
* @param hashTable The pointer to the hash table.
* @param key The key corresponds to the hash table entry
* @return The pointer to the hash table entry, or NULL if key does not exist
*/
static HashTableEntry* findItem(HashTable* hashTable, unsigned int key) {
  unsigned int bk_index = hashTable->hash(key); // store HTfunction value for buckets array's index
  HashTableEntry* this_node = hashTable->buckets[bk_index]; // first entry address in buckets
  int count=0; // for counting entry
  for(int i=0; i<hashTable->num_buckets; i++)
  if(hashTable->buckets[i] == NULL ) count++; // HTEntry is null
  if(count == hashTable->num_buckets) return NULL; // return NULL when hashTable is empty
  while(this_node){ // if this_node has some entry, keep looping
    if(this_node->key == key ){ // compare key
      return this_node; // if key is found, return HTEntry
    }
    this_node = this_node->next; // move to next entry on this linkedlist in the buckets array[bk_index]
  }
  return NULL; // return NULL if not found
}

/****************************************************************************
* Public Interface Functions
*
* These functions implement the public interface as specified in the header
* file, and make use of the private functions and hidden definitions in the
* above sections.
****************************************************************************/
// The createHashTable is provided for you as a starting point.
HashTable* createHashTable(HashFunction hashFunction, unsigned int numBuckets) {
  // The hash table has to contain at least one bucket. Exit gracefully if
  // this condition is not met.
  if (numBuckets==0) {
    printf("Hash table has to contain at least 1 bucket...\n");
    exit(1);
  }

  // Allocate memory for the new HashTable struct on heap.
  HashTable* newTable = (HashTable*)malloc(sizeof(HashTable));

  // Initialize the components of the new HashTable struct.
  newTable->hash = hashFunction;
  newTable->num_buckets = numBuckets;
  newTable->buckets = (HashTableEntry**)malloc(numBuckets*sizeof(HashTableEntry*));

  // As the new buckets contain indeterminant values, init each bucket as NULL.
  unsigned int i;
  for (i=0; i<numBuckets; ++i) {
    newTable->buckets[i] = NULL;
  }

  // Return the new HashTable struct.
  return newTable;
}

void destroyHashTable(HashTable* hashTable) {
  HashTableEntry *temp, *this_node; // declared two entries for temperary store and moving to next entry
  for(int i=0; i<hashTable->num_buckets; i++){ // looping for each buckets
    this_node = hashTable->buckets[i]; // store each buckets' head (the first entry)
    while(this_node){ // While Statement to deallocate all of HashTableEntry in the ith bucket.
      temp = this_node->next; // store the next entry's address to another HashTableEntry temp.
      if(this_node->value) free(this_node->value); // if it has some value, Deallocate the value
      free(this_node); // Deallocating HashTableEntry
      this_node = temp; //moving to next entry
      }
  } // destoryed all HashTableEntry
  free(hashTable->buckets); // deallocating buckets' array
  free(hashTable); // Deallocating the hashTable
}

void* insertItem(HashTable* hashTable, unsigned int key, void* value) {
  HashTableEntry* this_node = createHashTableEntry(key, value); // creating the new HTentry (init)
  unsigned int bk_index = hashTable->hash(key); // store HTfunction value for buckets array's index
  HashTableEntry* found; // temparary entry
  HashTableEntry* head = hashTable->buckets[bk_index]; // the first entry in the buckets
  if( !head ) {// if the buckets are empty, push new Entry h1.
    hashTable->buckets[bk_index] = this_node; // connecting the first entry to the new HTentry
    return NULL; // out of function
  }else if( (found = findItem(hashTable, key)) ){ // found key, the found entry has the entry's address which matched the key.
    void *svalue = found->value; // store the found value's address to return
    found->value = this_node->value; // overwrite the value in the found entry.
    free(this_node); // free the new HTentry.
    return svalue; //return old value when overwritten
  }
  this_node->next = head; // when key is not in list, push the new entry to the linkedlist
  hashTable->buckets[bk_index] = this_node; // this_node should be head.
  return NULL;
}// pushing on head of list pattern

void* getItem(HashTable* hashTable, unsigned int key) {
  HashTableEntry* tmp = findItem(hashTable, key); // looking for item, then if found, store it to the tmp entry
  if(!tmp) return NULL; // if hashtable is empty, then return NULL
  return tmp->value; // return the found entry's value
}

void* removeItem(HashTable* hashTable, unsigned int key) {
  unsigned int bk_index = hashTable->hash(key); // store HTfunction value for buckets array's index
  HashTableEntry* head = hashTable->buckets[bk_index]; // the first entry in the buckets
  HashTableEntry* this_node = head; // this_node is on the first entry
  HashTableEntry* trailing; // for storing previous entry
  void* temp = NULL; // for return value
  if(!this_node) return temp; // return NULL, if this_node is empty
  if(this_node->key == key){ // found in head case
    hashTable->buckets[bk_index] = this_node->next; // the next entry will be head.
    temp = this_node->value; // store value for return
    free(this_node); // free this entry.
    return temp; // return only old value's address.
  }
  trailing = this_node; // store list address before moving to the next linkedlist
  this_node = this_node->next; // move to the next entry on linkedlist
  while(this_node){ // middle & tail case
    if(this_node->key == key){ // key found in the middle or tail
      trailing->next = this_node->next; // connect the previous entry's next node to the next entry's address
      temp = this_node->value; // store value to return
      free(this_node); // remove matched entry in linkedlist
      return temp; // return value's address
    }
    trailing = this_node; // not found yet (setting), move to next trailing entry
    this_node = this_node->next; // move to next entry to compare
  }
  return temp; // return NULL if anything not found.
  
}

void deleteItem(HashTable* hashTable, unsigned int key) { 
  void* valuetofree = removeItem(hashTable, key);  // remove the entry matching key, store the old entry's value
  if(valuetofree) free(valuetofree); // also free value's address
  return ;
}
