#include<string>
#include<vector>
#include<stack>

enum var_type {
    TYPE_FLOAT,
    TYPE_INT,
    TYPE_STRING,
    ERROR
};

class Entry {
public:
    std::string name; // id name of the variable
    std::string alt; // alternative name (stack accessor)
    var_type type;
    Entry(std::string name, var_type t) : name(name), alt(name), type(t) {}
    virtual std::string show() = 0;
};

class StringEntry : public Entry {
public:
    std::string value;
    StringEntry(std::string name, std::string value) : Entry(name, TYPE_STRING), value(value) {}
    std::string show();
};

class NumericEntry : public Entry {
public:
    NumericEntry(std::string name, bool isInteger) : Entry(name, isInteger ? TYPE_INT : TYPE_FLOAT) {}
    std::string show();
};

class IdentifierAST;

class SymbolTable {
public:
    std::string name; // BLOCK x or GLOBAL or FUNCTION blah
    std::vector<Entry *> entries; // variable entries
    int id;

    SymbolTable(std::string name) : name(name) {}
    SymbolTable() : name("") {}

    void addStringEntry(std::string, std::string);
    void addNumericEntry(std::string, bool);
    void addEntry(std::vector<NumericEntry *>);

};

struct idList {
    std::string name;
    var_type type;
    idList * next;
    idList() : next(NULL) {}
};

var_type verify(std::string&);
var_type verify(std::string, SymbolTable *);
IdentifierAST * verify(IdentifierAST *);
idList * verify(idList *);

void showIDs(idList *);
std::vector<NumericEntry *> makeEntries(idList *, bool);

extern std::vector<SymbolTable *> tables; // create a new table on this every time you see a block end
extern std::vector<SymbolTable *> scope;
extern SymbolTable * all_funcs;
void startBlock();
std::string nextBlock();
SymbolTable * endBlock();
void showAllTables();
