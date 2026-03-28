#include <iostream>
#include <string>
#include <vector>
using namespace std;


// PERSON STRUCT ===============================================================================================================================================================
//All rows from CSV files will be processed as a PERSON struct, which will be stored in a vector of PERSON structs. 
// Top line of CSV: Index,User Id,First Name,Last Name,Sex,Email,Phone,Date of birth,Job Title
struct PERSON {
    int index;
    int userId;
    string first;
    string last;
    string sex;
    string email;
    string phone;
    string dob;
    string occupation;

    // Constructor for PERSON struct
    PERSON(int idx, int uid, string f, string l, string s, string e, string p, string d, string o) {
        index = idx;
        userId = uid;
        first = f;
        last = l;
        sex = s;
        email = e;
        phone = p;
        dob = d;
        occupation = o;
    }

    // Display (if needed)
    void print() {
        cout << "Index: " << index << endl;
        cout << "User ID: " << userId << endl;
        cout << "First Name: " << first << endl;
        cout << "Last Name: " << last << endl;
        cout << "Sex: " << sex << endl;
        cout << "Email: " << email << endl; 
        cout << "Phone: " << phone << endl;
        cout << "Date of Birth: " << dob << endl;
        cout << "Occupation: " << occupation << endl;
    }
};



// HASH TABLE STRUCTURE =========================================================================================================================================================
struct HashTable {

    // Sub-structure for each entry within the table
    struct HashEntry {
        string key; // NOTE TO SELF: DO NOT USE INDEX, USE USER ID!
        PERSON* value;
        HashEntry* next; // Pointer to the next entry in case of table collisions
        // Constructor for HashEntry
        HashEntry(const string& k, PERSON* v) {
            key = k;
            value = v;
            next = nullptr;
        }
    };

    // Setup variables for the hash table itself
    vector<HashEntry*> table; // Vector of pointers to HashEntry objects
    int numEntries; // Number of entries currently in the hash table
    int capacity; // Total capacity of the hash table

    // Constructor for HashTable
    HashTable(int cap) {
        capacity = cap;
        numEntries = 0;
        // TODO: Implement resize function so that table can be initilized with null pointers
    }
};