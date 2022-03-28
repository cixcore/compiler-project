/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
#include "parser.tab.h"
#include "yylvallib.h"
#include "types.h"
#include "errors.h"

using namespace std;

list<symbols_table> scopes;
symbols_table undefined_type_entries;
vector<argument> arguments_collector;

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

void create_entry_with_args(struct lex_value_t *identifier, int type, int nat, int mult) {
    struct symtable_content *new_c = new symtable_content();
    new_c->lin = identifier->line;
    new_c->col = get_col_number();
    new_c->type = type;
    new_c->nature = nat;
    new_c->size = mult * bytes_of(type);
    new_c->token_value_data = identifier->token;

    validate_err_declared_symbol(*new_c, identifier->token.str, nat);

    for(auto iter = arguments_collector.begin(); iter != arguments_collector.end(); iter++) {
        new_c->arguments.push_back((struct argument)
            {.id = strdup(iter->id), 
            .type = iter->type, 
            .col = iter->col, 
            .lin = iter->lin});
        free(iter->id);
    }
    arguments_collector.clear();

    if(scopes.empty()) {
        symbols_table new_table;
        scopes.push_front(new_table);
    }
    scopes.front().insert(entry(strdup(identifier->token.str), new_c));

    // adiciona cada argumento como uma variável no escopo mais de cima da pilha
    push_scope();
    for(auto arg = new_c->arguments.begin(); arg != new_c->arguments.end(); arg++) {
        struct symtable_content *arg_new_c = new symtable_content();
        arg_new_c->lin = arg->lin;
        arg_new_c->col = arg->col;
        arg_new_c->type = arg->type;
        arg_new_c->nature = VAR_N;
        arg_new_c->size = mult * bytes_of(arg->type);
        arg_new_c->token_value_data.str = strdup(arg->id);

        validate_err_declared_symbol(*arg_new_c, arg_new_c->token_value_data.str, nat);

        scopes.front().insert(entry(strdup(arg_new_c->token_value_data.str), arg_new_c));
    }
}

void collect_arg(struct lex_value_t *identifier, int type) {
    arguments_collector.push_back((struct argument){
        .id = strdup(identifier->token.str), 
        .type = type, 
        .col = get_col_number(), 
        .lin = get_line_number()
    });
}

void create_entry(struct lex_value_t *identifier, int type, int nat, int mult) {
    struct symtable_content *new_c = new symtable_content();
    new_c->lin = identifier->line;
    new_c->col = get_col_number();
    new_c->type = type;
    new_c->nature = nat;
    new_c->size = mult;
    new_c->token_value_data = identifier->token;

    validate_err_declared_symbol(*new_c, identifier->token.str, nat);

    scopes.front().insert(entry(strdup(identifier->token.str), new_c));
}

void id_entry_missing_type(struct lex_value_t *identifier) {
    create_entry_missing_type(identifier, VAR_N, 1);
}
void vector_entry_missing_type(struct lex_value_t *identifier, struct lex_value_t *vec_size) {
    create_entry_missing_type(identifier, VEC_N, vec_size->token.integer);
}
void create_entry_missing_type(struct lex_value_t *identifier, int nat, int mult) {
    struct symtable_content *new_c = new symtable_content();
    new_c->lin = identifier->line;
    new_c->col = get_col_number();
    new_c->nature = nat;
    new_c->size = mult;
    new_c->token_value_data = identifier->token;

    validate_err_declared_symbol(*new_c, identifier->token.str, nat);

    undefined_type_entries.insert(entry(strdup(identifier->token.str), new_c));
}

void validate_err_declared_symbol(symtable_content content, char* symtable_key, int nature) {
    symbols_table scope = scopes.front();
    auto table_entry = scope.find(symtable_key);

    if(table_entry != scope.end()) {
        printf("Redeclaration at ln %d, col %d: identifier '%s' (%s) is already in use at ln %d, col %d (%s).\n", 
        content.lin, content.col, symtable_key, nature_str(content.nature), table_entry->second->lin, table_entry->second->col, nature_str(table_entry->second->nature));
        exit(ERR_DECLARED);
    }    
}

void pop_scope() {
    free_symbols_table(scopes.front());
    scopes.pop_front();
}
void push_scope() {
    symbols_table new_scope;
    scopes.push_front(new_scope);
}

int get_inferred_type(int type1, int type2) {
    if(type1 == type2) return type1;
    if((type1 == INT_T && type2 == FLOAT_T) || (type2 == INT_T && type1 == FLOAT_T)) return FLOAT_T;
    if((type1 == INT_T && type2 == BOOL_T) || (type2 == INT_T && type1 == BOOL_T)) return INT_T;
    if((type1 == FLOAT_T && type2 == BOOL_T) || (type2 == FLOAT_T && type1 == BOOL_T)) return FLOAT_T;

    return -1;
}

void clearTypeStructures() {
    for (auto itl = scopes.begin(); itl != scopes.end(); ++itl) {
        free_symbols_table(*itl);
    }
    free_undefs();
}
void free_symbols_table(symbols_table table) {
    for (auto itm = table.begin(); itm != table.end(); ++itm) {
        free(itm->first);
        free(itm->second);
    }
    table.clear();
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
            cout << "       " << itm->first << " (size: " << itm->second->size << ") = ";
            if(!(itm->second->arguments.empty())) {
                for(auto ita = itm->second->arguments.begin(); ita != itm->second->arguments.end(); ++ita) {
                    cout << "<";
                    print_type(ita->type);
                    cout << " " << ita->id;
                    cout << "> ";
                }
                 cout << " -> ";
            }
            print_type(itm->second->type);
            cout << endl;
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

char* nature_str(int nature) {
    switch (nature) {
        case VAR_N:
            return string_to_char_array("VAR");
        case VEC_N:
            return string_to_char_array("VECTOR");
        case FUNC_N:
            return string_to_char_array("FUNCTION");
        case LIT_N:
            return string_to_char_array("LITERAL");
        default:
            return string_to_char_array("");
    }
}
char* string_to_char_array(const char* str) {
    char * charr;
    strcpy(charr, str);
    return strdup(charr);
}