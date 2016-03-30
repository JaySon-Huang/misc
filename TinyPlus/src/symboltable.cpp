#include "tinyp_structs.h"
#include "symboltable.h"
#include <string>
using std::string;
#include <set>
using std::set;

void
SymbolTable::update(
    const token_pair_t *token,
    enum ObjectType obj_type,
    enum ValueType val_type)
{
    struct symbol_t symbol;
    symbol.token = *token;
    symbol.obj_type = obj_type;
    symbol.val_type = val_type;

    set<struct symbol_t>::iterator found = m_table.find(symbol);
    if (found != m_table.end()){
        m_table.erase(found);
    }
    m_table.insert(symbol);
}

const struct symbol_t* 
SymbolTable::find(const string *name)
{
    struct symbol_t symbol;
    symbol.token.value = *name;
    const set<struct symbol_t>::iterator found = m_table.find(symbol);
    if (found == m_table.end()){
        return NULL;
    }else{
        return &(*found);
    }
}

struct symbol_t* 
SymbolTable::del(const string *name)
{
    struct symbol_t symbol;
    symbol.token.value = *name;
    const set<struct symbol_t>::iterator found = m_table.find(symbol);
    if (found == m_table.end()){
        return NULL;
    }else{
        m_table.erase(found);
        return NULL;
    }
}
