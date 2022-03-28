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
    symbols_table deepcopy_table = deepcopy_symbols_table();
    if(scopes.empty()) {
        scopes.push_front(deepcopy_table);
    } else {
        scopes.front().insert(deepcopy_table.begin(), deepcopy_table.end());
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
symbols_table deepcopy_symbols_table() {
    symbols_table table;
    for(auto iter = undefined_type_entries.begin(); iter != undefined_type_entries.end(); iter++) {
        struct symtable_content *copy = new symtable_content();
        copy->lin = iter->second->lin;
        copy->col = iter->second->col;
        copy->type = iter->second->type;
        copy->nature = iter->second->nature;
        copy->size = iter->second->size;
        copy->arguments = iter->second->arguments;
        copy->token_value_data = iter->second->token_value_data;

        table.insert(entry(strdup(iter->first), copy));
    }
    free_undefs();
    return table;
}
void create_entry_missing_type(struct lex_value_t *identifier, int nat, int mult) {
    struct symtable_content *new_c = new symtable_content();
    new_c->lin = identifier->line;
    new_c->col = get_col_number();
    new_c->nature = nat;
    new_c->size = mult;
    new_c->token_value_data = identifier->token;

    undefined_type_entries.insert(entry(strdup(identifier->token.str), new_c));
}
void id_entry_missing_type(struct lex_value_t *identifier) {
    create_entry_missing_type(identifier, VAR_N, 1);
}
void vector_entry_missing_type(struct lex_value_t *identifier, struct lex_value_t *vec_size) {
    create_entry_missing_type(identifier, VEC_N, vec_size->token.integer);
}

char* string_to_char_array(const char* str) {
    char * charr;
    strcpy(charr, str);
    return strdup(charr);
}

int get_inferred_type(int type1, int type2) {
    if(type1 == type2) return type1;
    if((type1 == INT_T && type2 == FLOAT_T) || (type2 == INT_T && type1 == FLOAT_T)) return FLOAT_T;
    if((type1 == INT_T && type2 == BOOL_T) || (type2 == INT_T && type1 == BOOL_T)) return INT_T;
    if((type1 == FLOAT_T && type2 == BOOL_T) || (type2 == FLOAT_T && type1 == BOOL_T)) return FLOAT_T;

    return -1;
}

void print_type(int type) {
    switch (type) {
        case INT_T:
            printf("int");
            break;
        case BOOL_T:
            printf("bool");
            break;
        case FLOAT_T:
            printf("float");
            break;
        case CHAR_T:
            printf("char");
            break;
        case STRING_T:
            printf("string");
            break;
        
        default:
            break;
    }
}

void clearTypeStructures() {
    for (auto itl = scopes.begin(); itl != scopes.end(); ++itl) {
        for (auto itm = undefined_type_entries.begin(); itm != undefined_type_entries.end(); ++itm) {
            free(itm->first);
            free(itm->second->token_value_data.str);
            free(itm->second);
        }
    }
    free_undefs();
}

void free_undefs() {
    for (auto iter = undefined_type_entries.begin(); iter != undefined_type_entries.end(); ++iter) {
        free(iter->first);
        free(iter->second);
    }
}

void print_scopes() {
    cout << endl << "print scopes (size: " << scopes.size() << ")\n";
    for (auto itl = scopes.begin(); itl != scopes.end(); ++itl) {
        cout << "   > print table (size: " << itl->size() << ")\n";
        for (auto itm = itl->begin(); itm != itl->end(); ++itm) {
            cout << "       " << itm->first << "(type: ";
            print_type(itm->second->type);
            cout << " | size: " << itm->second->size << ")\n";
        }
    }
}

void print_undef() {
    cout << "print print_undef (size: " << undefined_type_entries.size() << ")\n";
    for (auto itm = undefined_type_entries.begin(); itm != undefined_type_entries.end(); ++itm) {
        cout << "       " << itm->first << "(type: ";
        print_type(itm->second->type);
        cout << " | size: " << itm->second->size << ")\n";
    }
}