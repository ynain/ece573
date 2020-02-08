#include<string>
#include<vector>
#include<stack>

class Entry {
public:
    std::string name; // id name of the variable
    Entry(std::string name) : name(name) {}
    virtual std::string show() = 0;
};

class StringEntry : public Entry {
public:
    std::string value;
    StringEntry(std::string name, std::string value) : Entry(name), value(value) {}
    std::string show();
};

class NumericEntry : public Entry {
public:
    bool isInteger;
    NumericEntry(std::string name, bool isInteger) : Entry(name), isInteger(isInteger) {}
    std::string show();
};



class SymbolTable {
public:
    std::string name; // BLOCK x or GLOBAL or FUNCTION blah
    std::vector<Entry *> entries; // variable entries

    SymbolTable(std::string name) : name(name) {}
    SymbolTable() : name("") {}

    void addStringEntry(std::string, std::string);
    void addNumericEntry(std::string, bool);
    void addEntry(std::vector<NumericEntry *>);

};

struct idList {
    std::string name;
    idList * next;
    idList() : next(NULL) {}
};

void showIDs(idList *);
std::vector<NumericEntry *> makeEntries(idList *, bool);

extern std::vector<SymbolTable *> tables; // create a new table on this every time you see a block end
extern std::stack<SymbolTable *> scope;
void startBlock(std::string);
std::string nextBlock();
void endBlock();
void showAllTables();
