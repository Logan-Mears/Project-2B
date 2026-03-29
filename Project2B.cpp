#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <type_traits>
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
        string key;
        PERSON* pers;
        HashEntry* next; // Pointer to the next entry in case of table collisions (hopefully)
        // Constructor for HashEntry
        HashEntry(const string& k, PERSON* v) {
            key = k;
            pers = v;
            next = nullptr;
        }
    };

    // Setup variables for the hash table itself
    vector<HashEntry*> table; // Vector of pointers to HashEntry objects
    int numEntries;           // Number of entries currently in the hash table
    int capacity;             // Total capacity of the hash table

    // Constructor for HashTable
    HashTable(int cap) {
        capacity = cap;
        numEntries = 0;
        //table.resize(capacity, nullptr);
    }

    // Destructor for HashTable
    ~HashTable() {
        for (int i = 0; i < capacity; i++) {
            HashEntry* entry = table[i];
            while (entry != nullptr) {
                HashEntry* temp = entry;
                entry = entry->next;
                delete temp;
            }
        }
    }

    void addEntry(const string& key) {
        // Hash the key to get table index
        // int hashIndex = hashFunc(key);   
        HashEntry* currentEntry = table[hashIndex]; 

        // Insert new entry at the beginning of the list for the newly hashed index
        HashEntry* newEntry = new HashEntry(key);
        newEntry->next = table[hashIndex]; // nts: as above
        table[hashIndex] = newEntry; // nts: as above
        numEntries++;

        // Check load factor and resize if necessary
        
    }

    

    // SEARCH IMPLEMENTATION ---------------------------------------------------------------------------------------------------------------------------------------------------
    PERSON* search(const string& key) {
        // Hash the key to get table index
        // int hashIndex = hashFunc(key);
        HashEntry* currentEntry = table[hashIndex]; 

        // Traverse the linked list at the hashed index to find the entry with the matching key
        while (currentEntry != nullptr) {
            if (currentEntry->key == key) {
                return currentEntry->pers; // Return pointer to PERSON object if found
            }
            currentEntry = currentEntry->next; // Move to next entry
        }
        return nullptr; // Return nullptr if key not found(?)
    }

};





// TRIE STRUCTURE ==============================================================================================================================================================
struct Trie {

    // Set up substructure for each node in the trie
    struct TrieNode {
        TrieNode* children[26+26+10]; // Assuming uppercase and lowercase letters a-z + 10 digits 0-9
        bool isEndID;                 // Flag to indicate end of a valid id (will be key)
        PERSON* pers;                // Pointer to PERSON object associated with the key (if this node is the end of a key)

        // Constructor for TrieNode
        TrieNode() {
            for (int i = 0; i < 62; i++) {
                children[i] = nullptr;
            }
            isEndID = false;
            pers = nullptr;
        }

        // Destructor for TrieNode (Unknown if needed)
        ~TrieNode() {
            for (int i = 0; i < 62; i++) {
                if (children[i] != nullptr) {
                    delete children[i];
                }
            }
        }  
    };

    // Setup variables for the trie itself
    TrieNode* root;
    int numEnTries; // Wordplay useful to distinguish from numEntries in hash table but mostly for my own amusement

    // Constructor for Trie
    Trie() {
        root = new TrieNode(); 
    }

    // Destructor for Trie
    ~Trie() {
        // Destructor (Unknown if needed again)
    }

    void addEnTrie(const string& key, PERSON* pers) {
        TrieNode* currentNode = root;

        // Traverse through trie using key characters to find the corresponding nodes
        for (char c : key) {
            char charIndex = c;
            if (currentNode->children[charIndex] == nullptr) {
                currentNode->children[charIndex] = new TrieNode(); // Create new node if it doesn't exist
            }
            currentNode = currentNode->children[charIndex]; // Move to the next node
        }

        // After traversing the key, mark the end of the ID/key and store the associated PERSON object
        currentNode->isEndID = true;
        currentNode->pers = pers; // Store pointer to PERSON object at the end node
        numEnTries++;
    }

    void populateTrie(TrieNode* node, vector<PERSON*>& people) {
        if (node->isEndID == true && node->pers != nullptr) {
            people.push_back(node->pers); // Add PERSON pointer to vector if this node is the end of a valid ID/key
        }
        for (int i = 0; i < 62; i++) {
            if (node->children[i] != nullptr) {
                populateTrie(node->children[i], people); // Recursively traverse child nodes to pop entire trie
            }
        }
    }

    // SEARCH IMPLEMENTATION ---------------------------------------------------------------------------------------------------------------------------------------------------
    PERSON* search(const string& key) {
        TrieNode* currentNode = root;

        // Traverse through trie using key characters to find the corresponding nodes
        for (char c : key) {
            char charIndex = c;
            if (currentNode->children[charIndex] == nullptr) {
                return nullptr; // Key not found in trie
            }
            currentNode = currentNode->children[charIndex]; // If found, move to the next node
        }

        // After traversing the key, check if at a valid end of an ID/key to return the associated PERSON object
        if (currentNode->isEndID) {
            return currentNode->pers; // Return pointer to PERSON object if found
        }
        return nullptr; // Key not found as a valid ID in trie(?)
    }
};





// GET CSV CONTENTS ============================================================================================================================================================
vector<PERSON*> getCSVContents(const string& filename) {
    vector<PERSON*> people;
    ifstream file(filename);
    // Make sure time isn't wasted if file isn't openable for some reason
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return people; // Return empty vector if file cannot be opened
    }
    string line;
    getline(file, line); // Skip header line with column names

    // Read each line of the CSV file and create PERSON objects using each row's data
    while (getline(file, line)) {
        PERSON newPerson;

        // Use stringstream to parse the line into individual fields based on comma delimiters
        stringstream ss(line);
        string column;
        vector<string> columns;
        string currentColumn;
        for (char c : line) {
            if (c == ',' && !currentColumn.empty()) {
                columns.push_back(currentColumn);
                currentColumn.clear();
            } else {
                currentColumn += c;
            }
        }

        // Assign column names to each column
        if (columns.size() >= 9) {
            newPerson.index = columns[0];      // Error here ************
            newPerson.userId = columns[1];     // Error here ************
            newPerson.first = columns[2];
            newPerson.last = columns[3];
            newPerson.sex = columns[4];
            newPerson.email = columns[5];
            newPerson.phone = columns[6];
            newPerson.dob = columns[7];
            newPerson.occupation = columns[8];
        }
    }

    file.close();
    cout << "Number of people processed: " << people.size() << endl; // Just for testing to make sure >= 100,000 people are processed as specified in the project requirements
    return people;
}





// PERFORMANCE EVALUATION FUNCTION(S) ==========================================================================================================================================
// Note: Most of this block of code is derived from https://stackoverflow.com/questions/22387586/measure-time-in-c-using-chrono-in-a-simple-way and modified, as cited in the report
template <typename Resolution = std::chrono::duration<double,std::micro>>
class Stopwatch {
  typedef std::chrono::steady_clock Clock;
  private:
    std::chrono::time_point<Clock> last;
    string name;
    bool automatic_print;
  public:
    void reset() noexcept {
      last = Clock::now();
    }   

    Stopwatch() noexcept {
      reset();
    }   

    auto time_elapsed() const noexcept {
      return Resolution(Clock::now() - last).count(); // Find the difference between the start and end times 
    }

    auto operator()() const noexcept {// returns time in Resolution
      return time_elapsed();
    }

    void print() const noexcept {
      std::cout << time_elapsed() << "\n";
    }

    ~Stopwatch() {
      if (automatic_print) {
        print();
      }
    }
};





// MAIN FUNCTION / ACTUAL TESTING ==================================================================================================================================================
int main() {
    // Formatting to make it look pretty (I really don't want to create a separate GUI so will likely use console output for testing and demo purposes)
    cout << "----- Personal Detail Lookup System with Data Structure Comparison: Project 2 COP3530 -----" << endl;
    cout << "Hash Tables vs. Tries" << endl << endl << endl;
    cout << "Instructions:"<< endl << "Move a .csv file with the format described in the README.md file into the working directory." << endl 
        << "Then, enter the filename, including '.csv': " << endl;
    string filename;
    cin >> filename;
    cout << "Processing file: " << filename << "..." << endl << "This may take a moment..." << endl;
    vector<PERSON*> people = getCSVContents(filename);
    // As per project requirements, there must be 100,000 people in the csv file, so this check is unnecessary besides this specification
    if (people.size() < 100000) {
        cout << "Heads Up: Expected at least 100,000 people in the CSV file. Found only " << people.size() << "." << endl;
        // Allow program to continue since this might be intentional for testing
    }
    // Prompt for user ID, this will be key for lookup in both DSs
    cout << "Enter a user ID to look up: " << endl;
    string userId;
    cin >> userId;



    // Create and Test ID Lookup Using Hash Table --------------------------------------------------------------------------------------------------------------------------------
    vector<string> hash_measured(HashTable& hashTable) {
        Stopwatch sw; 
        // Measure time taken to add all entries to the hash table
        // Measure time taken to search for a specific user ID in the hash table
    }

    cout << "----- ID Lookup Using Hash Table -----" << endl;
    HashTable newHashTable;
    vector<string> timeHashVector = hash_measured(newHashTable);
    // Display results of the search & time taken (if found, print the person's details; if not found, indicate that the user ID was not found)



    // Create and Test ID Lookup Using Trie --------------------------------------------------------------------------------------------------------------------------------------
    // Separate function to measure time taken for trie operations; somewhat awkward but hopefully suffices
    vector<string> trie_measured(Trie& trie) {
        Stopwatch sw; 
        // Measure time taken to add all entries to the trie
        // Measure time taken to search for a specific user ID in the trie
    }
    
    cout << "----- ID Lookup Using Trie -----" << endl;
    Trie newTrie;
    vector<string> timeTrieVector = trie_measured(newTrie);
    // Display results of the search & time taken (if found, print the person's details; if not found, indicate that the user ID was not found)




    // Comparison of Hash Table and Trie Performance -----------------------------------------------------------------------------------------------------------------------------
    cout << "----- Performance Comparison -----" << endl;
    // Reiterate the time taken for both the hash table and trie for adding entries and searching for a specific user ID
    // Discuss any observed differences in performance between the two data structures based on the results obtained from the tests above (h or t was faster)
    
}