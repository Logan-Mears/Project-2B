#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <type_traits>
using namespace std;


// PERSON STRUCT ===============================================================================================================================================================
// All rows from CSV files will be processed as a PERSON struct, which will be stored in a vector of PERSON structs. 
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

    // Default constructor for PERSON struct
    PERSON() {
        index = 0;
        userId = 0;
        first = "";
        last = "";
        sex = "";
        email = "";
        phone = "";
        dob = "";
        occupation = "";
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

    int hashFunc(const string& key, int cap) const { // modded to be constant to get right capacity for rehashing
        int hashValue = 0;
        for (char c : key) {
            hashValue = (hashValue * 31 + c) % cap;
        }
        return hashValue;
    }

    void resize() { 
        int newCapacity = capacity * 2; // Double the capacity when resizing for now
        vector<HashEntry*> newTable(newCapacity, nullptr); // Create a new table with the new capacity

        // Rehash all existing entries into the new table: block recommended by VSCode suggestion, slightly modified
        for (int i = 0; i < capacity; i++) {
            HashEntry* entry = table[i];
            while (entry != nullptr) {
                int newHashIndex = hashFunc(entry->key, newCapacity);
                HashEntry* nextEntry = entry->next; // Store pointer to the next entry BEFORE rehashing

                // Insert the entry into the new table at the new hash index (handling collisions by chaining as described in reference 5 video of the report)
                entry->next = newTable[newHashIndex]; // Point to the current head of the list at the new index
                newTable[newHashIndex] = entry; // Update the head of the list at the new index to be this entry

                entry = nextEntry; // Move to the next entry in the old table
            }
        }

        // Update the hash table to be the new table
        table = newTable;
    }

    // Constructor for HashTable
    HashTable(int cap) {
        capacity = cap;
        numEntries = 0;
        table.resize(capacity, nullptr);
    }
    // Default Constructor w/ default capacity of 100,000 (as per project requirements)
    HashTable() { 
        capacity = 100000; 
        numEntries = 0; 
        table.resize(capacity, nullptr); // Initialize the hash table with null pointers
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

    void addEntry(const string& key, PERSON* pers) {
        // Hash the key to get table index
        int hashIndex = hashFunc(key);   
        HashEntry* currentEntry = table[hashIndex]; 

        // Insert new entry at the beginning of the list for the newly hashed index
        HashEntry* newEntry = new HashEntry(key, pers);
        newEntry->next = table[hashIndex]; 
        table[hashIndex] = newEntry; 
        numEntries++;

        // Check load factor and resize if necessary
        if ((double)numEntries/capacity > 0.75) { // Load factor threshold of 0.75 as rec'd by GfG as cited, cast as double to avoid integer division
            resize();
        }
    }

    

    // SEARCH IMPLEMENTATION ---------------------------------------------------------------------------------------------------------------------------------------------------
    PERSON* search(const string& key) {
        // Hash the key to get table index
        int hashIndex = hashFunc(key);
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
        numEnTries = 0;
    }

    // Destructor for Trie
    ~Trie() {
        delete root; // Deleting the root will recursively delete all child nodes due to the destructor in TrieNode (hopefully)
    }

    // Helper function for addEnTrie; fixes out-of-bounds due to using the wrong indexes (forgot about ASCIII numbering)
    int getIndex(char c) {
        if (c >= '0' && c <= '9') {
            return c - '0'; // Map '0'-'9' to 0-9
        }
        if (c >= 'a' && c <= 'z') {
            return c - 'a' + 10; // Map 'a'-'z' to 10-35
        }
        if (c >= 'A' && c <= 'Z') {
            return 36 + c - 'A'; // Map 'A'-'Z' to 36-61
        }
        return -1; // Invalid character for  trie 
    }

    void addEnTrie(const string& key, PERSON* pers) {
        TrieNode* currentNode = root;

        // Traverse through trie using key characters to find the corresponding nodes
        for (char c : key) {
            int index = getIndex(c);
            if (index == -1) {
                continue; // Skip invalid characters that are not letters or digits (if any?)
            }
            if (currentNode->children[index] == nullptr) {
                currentNode->children[index] = new TrieNode(); // Create new node if it doesn't exist
            }
            currentNode = currentNode->children[index]; // Move to the next node
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
    for (char c : key) {
        int index = getIndex(c);
        if (index == -1) continue; // Skip invalid chars
        
        if (index < 0 || index >= 62 || currentNode->children[index] == nullptr) {
            return nullptr;
        }
        currentNode = currentNode->children[index];
    }
    
    if (currentNode->isEndID) {
        return currentNode->pers;
    }
    return nullptr;
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
        // Use stringstream to parse the line into individual fields based on comma delimiters
        stringstream ss(line);
        string column;
        vector<string> columns;
        // Add each column to a vector of strings for easier access
        while (getline(ss, column, ',')) {
            columns.push_back(column); 
        }
        if (columns.size() >= 9) {
            // Create a new PERSON object using the parsed columns and add a ptr to it to the vector of people
            PERSON* newPerson = new PERSON(stoi(columns[0]), stoi(columns[1]), columns[2], columns[3], columns[4], columns[5], columns[6], columns[7], columns[8]);
            people.push_back(newPerson);
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
    bool automatic_print = false;
  public:
    void reset() noexcept {
      last = Clock::now();
    }   

    Stopwatch(bool auto_print = false) noexcept : automatic_print(auto_print) { // Edited to stop destructor issues
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





// DATA STRUCTURES MEASURED ==================================================================================================================================================
// Separate functions to measure time taken for trie operations; somewhat awkward but necessary for how I set up the stopwatch struct I think
// Format of returned vec: [0] = person's details (if found), [1] = time taken for operations (add + search) in microseconds as a string (for easy printing and comparison with trie results)

vector<string> hash_measured(HashTable& hashTable, string userID, vector<PERSON*>& people) {
    vector<string> hash_results; 
    // Initiate stopwatch to measure time taken for rest of function to execute; hopefully is ended by the destructor when the function ends
    Stopwatch sw; 
    // Add all entries to the hash table
    for (PERSON* person : people) {
        hashTable.addEntry(to_string(person->userId), person);
    }
    // Search for a specific user ID in the hash table
    PERSON* foundPers = hashTable.search(userID);
    hash_results.push_back(foundPers->first + " " + foundPers->last); // Just for testing to make sure the search is working; will likely want to print more details in the final version
    hash_results.push_back(to_string(sw())); // Add the time taken for the operations to the results vector; will be used for comparison with trie results
    return hash_results;
}

vector<string> trie_measured(Trie& trie, string userID, vector<PERSON*>& people) {
    vector<string> trie_results;
    Stopwatch sw; 
    // Measure time taken to add all entries to the trie
    for (PERSON* person : people) {
        trie.addEnTrie(to_string(person->userId), person);
    }
    // Measure time taken to search for a specific user ID in the trie
    PERSON* foundPers = trie.search(userID);
    trie_results.push_back(foundPers->first + " " + foundPers->last); // Ditto
    trie_results.push_back(to_string(sw())); // Add the time taken for the operations to the results vector; will be used for comparison with hash table results
    return trie_results;
}





// MAIN FUNCTION / ACTUAL TESTING ==================================================================================================================================================
int main() {
    // Formatting to make it look pretty (I really don't have time to create a separate GUI so will likely use console output for testing and demo purposes)
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
    cout << "----- ID Lookup Using Hash Table -----" << endl;
    HashTable newHashTable;
    vector<string> timeHashVector = hash_measured(newHashTable, userId, people);
    // Display results of the search & time taken (if found, print the person's details; if not found, indicate that the user ID was not found)
    cout << "Hash Table Results:" << endl;
    cout << "Person found: " << timeHashVector[0] << endl;
    double timeInSeconds = stod(timeHashVector[1]) / 1000000.0;
    cout << "Time taken (seconds): " << timeInSeconds << endl;




    // Create and Test ID Lookup Using Trie --------------------------------------------------------------------------------------------------------------------------------------
    cout << "----- ID Lookup Using Trie -----" << endl;
    Trie newTrie;
    vector<string> timeTrieVector = trie_measured(newTrie, userId, people);
    // Display results of the search & time taken (if found, print the person's details; if not found, indicate that the user ID was not found)
    cout << "Trie Results:" << endl;
    cout << "Person found: " << timeTrieVector[0] << endl;
    double timeInSecondsTrie = stod(timeTrieVector[1]) / 1000000.0;
    cout << "Time taken (seconds): " << timeInSecondsTrie << endl << endl; // Reports in microseconds, so *10^6



    // Comparison of Hash Table and Trie Performance -----------------------------------------------------------------------------------------------------------------------------
    cout << "----- Performance Comparison -----" << endl;
    // Reiterate the time taken for both the hash table and trie for adding entries and searching for a specific user ID & compare
    if (stoi(timeHashVector[1]) < stoi(timeTrieVector[1])) {
        cout << "The hash table was faster than the trie for adding entries and searching for the user ID." << endl << "Comparison:" << stoi(timeHashVector[1]) << " vs " << stoi(timeTrieVector[1]) << endl;
    } else if (stoi(timeHashVector[1]) > stoi(timeTrieVector[1])) {
        cout << "The trie was faster than the hash table for adding entries and searching for the user ID." << endl << "Comparison:" << stoi(timeTrieVector[1]) << " vs " << stoi(timeHashVector[1]) << endl;
    } else {
        cout << "The hash table and trie had the same performance for adding entries and searching for the user ID." << "Comparison:" << stoi(timeHashVector[1]) << " vs " << stoi(timeTrieVector[1]) << endl;
    }



    for (PERSON* person : people) {
        delete person; // Clean up 
    }
    return 0;
}