#ifndef __TINYP_SYMBOLTABLE__
#define __TINYP_SYMBOLTABLE__

#include "tinyp_structs.h"
#include <string>
using std::string;
#include <set>
using std::set;

class SymbolLess {
   public:
    bool operator()(const struct symbol_t &s1,
                    const struct symbol_t &s2) const {
        return s1.token.value < s2.token.value;
    }
};

class SymbolTable {
   public:
    set<struct symbol_t, SymbolLess> m_table;

   public:
    /*
     * 实现插入新符号到符号表的操作。
     * @token
     *    符号对应的token
     * @obj_type
     *    符号对应的类型
     * @return
     *    插入成功后返回该符号的Sym指针，失败则返回null。
     */
    void update(const token_pair_t *token, enum ObjectType obj_type,
                enum ValueType val_type);

    /*
     * 实现查找功能。在符号表中查找名字为name的符号。
     * @name
     *    查找符号的名字
     * @return
     *    返回该符号的Sym指针，失败则返回null。
     */
    const struct symbol_t *find(const string *name);

    /*
     * 实现删除功能。
     * @name
     *    在符号表中删除的符号的名字。
     */
    struct symbol_t *del(const string *name);
};

#endif
