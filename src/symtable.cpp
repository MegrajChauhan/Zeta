#include "../includes/symtable.hpp"

void zeta::symtable::SymTable::add_entry(std::string key, SymTableEntry val)
{
    symtable[key] = val;
}

void zeta::symtable::SymTable::add_entry(std::string key, SymEntryType type, std::string val, nodes::DataType dtype)
{
    symtable[key] = SymTableEntry(type, val, dtype);
}

std::unordered_map<std::string, zeta::symtable::SymTableEntry>::iterator zeta::symtable::SymTable::find_entry(std::string key)
{
    return symtable.find(key);
}

bool zeta::symtable::SymTable::is_valid(std::unordered_map<std::string, SymTableEntry>::iterator iter)
{
    return !(iter == symtable.end());
}