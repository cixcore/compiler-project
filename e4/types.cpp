/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
#include "parser.tab.h"
#include "yylvallib.h"
#include "types.h"

using namespace std;

list<symbols_table> scopes;
symbols_table undefined_type_entries;


void init_types_and_add_to_scope(int type) {
    for(auto it = undefined_type_entries.begin(); it!=undefined_type_entries.end(); ++it) {
        it->second->type = type;
        it->second->size *= bytes_of(type);
    }
    if(scopes.empty()) {
        symbols_table deepcopy_table;
        deepcopy_table.insert(undefined_type_entries.begin(), undefined_type_entries.end());
        scopes.push_front(deepcopy_table);
    } else {
        scopes.front().insert(undefined_type_entries.begin(), undefined_type_entries.end());
    }
    undefined_type_entries.clear();
}
int bytes_of(int type) {
    switch (type) {
        case INT_T: 
            return INT_SIZE_BYTES;
        case FLOAT_T:     
            return FLOAT_SIZE_BYTES;
        case CHAR_T: 
            return CHAR_SIZE_BYTES;  
        case BOOL_T: 
            return BOOL_SIZE_BYTES;
        
        default: return -1;
    };
}
void create_entry_nat_size_missing_type(struct lex_value_t *identifier, int nat, int mult) {
    struct symtable_content *new_c = (struct symtable_content *)malloc(sizeof(struct symtable_content));
    new_c->lin = identifier->line;
    new_c->col = get_col_number();
    new_c->nature = nat;
    new_c->size = mult;
    new_c->arguments = vector<argument>();
    new_c->token_value_data = identifier->token;

    undefined_type_entries.insert(entry(string(identifier->token.str).c_str(), new_c));
}
void create_entry_missing_type(struct lex_value_t *identifier) {
    create_entry_nat_size_missing_type(identifier, VAR_N, 1);
}
void create_entry_vector_size_missing_type(struct lex_value_t *identifier, struct lex_value_t *vec_size) {
    create_entry_nat_size_missing_type(identifier, VEC_N, vec_size->token.integer);
}

int get_inferred_type(int type1, int type2) {
    if(type1 == type2) return type1;
    if((type1 == INT_T && type2 == FLOAT_T) || (type2 == INT_T && type1 == FLOAT_T)) return FLOAT_T;
    if((type1 == INT_T && type2 == BOOL_T) || (type2 == INT_T && type1 == BOOL_T)) return INT_T;
    if((type1 == FLOAT_T && type2 == BOOL_T) || (type2 == FLOAT_T && type1 == BOOL_T)) return FLOAT_T;

    return -1;
}

const char* int_to_symtable_key(int value) {
    return to_string(value).c_str();
}
const char* float_to_symtable_key(float value) {
    return to_string(value).c_str();
}


void clearTypeStructures() {
    for (auto itl = scopes.begin(); itl != scopes.end(); ++itl) {
        for (auto itm = undefined_type_entries.begin(); itm != undefined_type_entries.end(); ++itm) {
            delete itm->first;
            delete itm->second;
        }
    }
        
    for (auto iter = undefined_type_entries.begin(); iter != undefined_type_entries.end(); ++iter) {
        delete iter->first;
        delete iter->second;
    }
}