#include "types.h"
#include<iostream>
#include<sstream>
#include<cstdlib>
std::vector<SymbolTable *> tables;
std::stack<SymbolTable *> scope;

static int blocks = 1;

std::string nextBlock()
{
    std::ostringstream name;
    name << "BLOCK " << blocks++;
    return name.str();
}

void nameExists(std::string name)
{
    // TODO: implement this
    // nameExists() should check the entire current symbol table stack for the given name
    // if it exists, return a true value and if not, return a false value

    // Take this function and call it everytime you see a "this->entries.push_back(...)" to make sure
    // that we only add a name to the symbol table if it is not already defined
    
    for (SymbolTable * table : tables)
    {
    	if (table->name == name)
       	{
            std::cout << "DECLARATION ERROR " << name << "\n";
            exit(0);
	}
    }
}

std::string StringEntry::show()
{
    std::ostringstream out;
    out << "name " << this->name << " STRING value \"" << this->value << "\"";
    return out.str();
}

std::string NumericEntry::show()
{
    std::ostringstream out;
    out << "name " << this->name << (this->isInteger ? " INT" : " FLOAT");
    return out.str();
}

void SymbolTable::addStringEntry(std::string name, std::string value)
{
	nameExists(name);
    	StringEntry * se = new StringEntry(name, value);
    	this->entries.push_back(se);
}

void SymbolTable::addNumericEntry(std::string name, bool isInt)
{
    nameExists(name);
    NumericEntry * ne = new NumericEntry(name, isInt);
    this->entries.push_back(ne);
}

void SymbolTable::addEntry(std::vector<NumericEntry *> e)
{
    for (NumericEntry * ne : e)
    {
	nameExists(name);
	this->entries.push_back(ne);
    }
}

std::vector<NumericEntry *> makeEntries(idList * names, bool isInt)
{
	std::vector<NumericEntry *> entries;

	for (; names != NULL; names = names->next) {
		nameExists(names->name);
		entries.push_back(new NumericEntry(names->name, isInt));
	}
	return entries;
}

void showIDs(idList * names)
{
    std::cout << "ID " << names->name << " ";
    if (names->next) showIDs(names->next);
    else std::cout << "END\n";
}

void startBlock(std::string name)
{
    // TODO: implement this
    // startBlock() should push a new, empty symbol table to the stack
    // and try to figure out its name (if possible)
    scope.push(new SymbolTable(name));
    tables.push_back(scope.top());
}

void endBlock()
{
    // TODO: implement this
    // endBlock() should name the symbol table on the top of the stack,
    // add it to the vector of symbol tables, and then pop it from the stack
    scope.pop();
}

void showAllTables()
{
    for (SymbolTable * table : tables)
    {
        std::cout << "Symbol table " << table->name << "\n";
        for (Entry * e : table->entries)
            std::cout << e->show() << "\n";
        std::cout << "\n";
    }
}
