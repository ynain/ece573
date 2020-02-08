#include "asts.h"
#include<iostream>
#include<sstream>
#include<cstdlib>
std::vector<SymbolTable *> tables;
std::vector<SymbolTable *> scope;
SymbolTable * all_funcs = new SymbolTable();

static int blocks = 1;

var_type verify(std::string name, SymbolTable * table)
{
     for (int i = 0; i < table->entries.size(); i++)
    {
        if (table->entries[i]->name == name)
        {
            // name = table->entries[i]->alt;
            return table->entries[i]->type;
        }
    }
    return ERROR;
}

var_type verify(std::string& name)
{
    for (int tableNum = scope.size(); tableNum > 0; tableNum--)
    {
        SymbolTable * table = scope[tableNum - 1];
        for (int i = 0; i < table->entries.size(); i++)
        {
            if (table->entries[i]->name == name)
            {
                // name = table->entries[i]->alt;
                return table->entries[i]->type;
            }
        }
    }
    return ERROR;
}

IdentifierAST * verify(IdentifierAST * ast)
{
    var_type t = verify(ast->id);
    if (t == ERROR)
    {
        std::cerr << "Variable " << ast->id << " is not defined in this scope!\n";
        exit(-1);
    }

    ast->type = t;
    return ast;
}

idList * verify(idList * ids)
{
    var_type t = verify(ids->name);
    if (t == ERROR)
    {
        std::cerr << "Variable " << ids->name << " is not defined in this scope!\n";
        exit(-1);
    }
    ids->type = t;
    if (ids->next) ids->next = verify(ids->next);
    return ids;
}

std::string nextBlock()
{
    std::ostringstream name;
    name << "BLOCK " << blocks++;
    return name.str();
}

void nameExists(std::string name)
{

    SymbolTable * table = scope.back();
    for (int i = 0; i < table->entries.size(); i++)
    // for (Entry * e : table->entries)
    {
        if (table->entries[i]->name == name)
        {
            std::cout << "DECLARATION ERROR " << name << "\n";
            exit(0);
        }
    }
}

std::string StringEntry::show()
{
    std::ostringstream out;
    //out << "name " << this->name << " type STRING value \"" << this->value << "\"";
    out << "str " << this->name << " \"" << this->value << "\"";
    return out.str();
}

std::string NumericEntry::show()
{
    std::ostringstream out;
    //out << "name " << this->name << " type " << (this->type == TYPE_INT ? " INT" : " FLOAT");
    out << "var " << this->name;
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
    for (int i = 0; i < e.size(); i++)
    // for (NumericEntry * ne : e)
    {
        nameExists(e[i]->name);
        this->entries.push_back(e[i]);
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

void startBlock()
{
    // TODO: implement this
    // startBlock() should push a new, empty symbol table to the stack
    // and try to figure out its name (if possible)
    SymbolTable * top = new SymbolTable("random name");
    top->id = tables.size();
    scope.push_back(top);
    tables.push_back(scope.back());
}

SymbolTable * endBlock()
{
    // TODO: implement this
    // endBlock() should name the symbol table on the top of the stack,
    // add it to the vector of symbol tables, and then pop it from the stack
    SymbolTable * back = scope.back();
    scope.pop_back();
    return back;
}

void showAllTables()
{
    for (int i = 0; i < tables.size(); i++)
    // for (SymbolTable * table : tables)
    {
        SymbolTable * table = tables[i];
        //std::cout << "Symbol table " << table->name << "\n";
        for (int i = 0; i < table->entries.size(); i++)
        // for (Entry * e : table->entries)
            std::cout << table->entries[i]->show() << "\n";
        std::cout << "\n";
    }
}
