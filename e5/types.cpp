/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
#include "parser.tab.h"
#include "yylvallib.h"
#include "tree.h"
#include "types.h"
#include "errors.h"

using namespace std;

list<symbols_table> scopes;
list<int> scope_deslocs;
symbols_table undefined_type_entries;
vector<argument> arguments_collector;
vector<node*> parameters_collector;

extern void *arvore;

int expected_ret_type;

void local_init_types_validate_and_add_to_scope(int type) {
    for(auto it = undefined_type_entries.begin(); it!=undefined_type_entries.end(); ++it) {
        if(it->second->type > UND_T) {
            it->second->type = get_inferred_type_symtable_cont(type, it->second->type, *(it->second));
        } else {
            it->second->type = type;
        }
        it->second->size *= bytes_of(it->second->type);
    }
    if(scope_deslocs.empty()) {
        scope_deslocs.push_front(0);
    }
    symbols_table deepcopy_table = deepcopy_symbols_table();
    if(scopes.empty()) {
        scopes.push_front(deepcopy_table);
    } else {
        scopes.front().insert(deepcopy_table.begin(), deepcopy_table.end());
    }
    undefined_type_entries.clear();
}
void global_init_types_and_add_to_scope(int type) 
{
    for(auto it = undefined_type_entries.begin(); it!=undefined_type_entries.end(); ++it) 
    {
        it->second->type = type;
        if(type == STRING_T) 
        {
            it->second->size *= 0;
            validate_not_string_vector(type, it->second->nature, *(it->second), it->first);
        } 
        else 
        {
            it->second->size *= bytes_of(type);
        }
    }

    
    if(scope_deslocs.empty()) {
        scope_deslocs.push_front(0);
    }
    symbols_table deepcopy_table = deepcopy_symbols_table();
    if(scopes.empty())
    {
        scopes.push_front(deepcopy_table);
    }
    else
    {
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
        case STRING_T: 
            return CHAR_SIZE_BYTES;
        
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
        copy->offset = scope_deslocs.front();
        copy->arguments = iter->second->arguments;
        copy->token_value_data = iter->second->token_value_data;

        int scope_offset = scope_deslocs.front();
        scope_deslocs.pop_front();
        scope_offset += iter->second->size;
        scope_deslocs.push_front(scope_offset);

        table.insert(entry(strdup(iter->first), copy));
    }
    free_undefs();
    return table;
}

void create_func_entry_with_args(struct lex_value_t *identifier, int type, int nat) {
    int tableDesloc = 16;

    validate_err_function_string(type, identifier->line, get_col_number());

    struct symtable_content *new_c = new symtable_content();
    new_c->lin = identifier->line;
    new_c->col = get_col_number();
    new_c->type = type;
    new_c->nature = nat;
    new_c->size = type != STRING_T ? bytes_of(type) : 0;
    new_c->token_value_data = identifier->token;

    expected_ret_type = type;

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
        scope_deslocs.push_front(0);
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
        arg_new_c->size = arg->type != STRING_T ? bytes_of(arg->type) : 0;
        arg_new_c->token_value_data.str = strdup(arg->id);

        validate_err_declared_symbol(*arg_new_c, arg_new_c->token_value_data.str, nat);

        arg_new_c->offset = tableDesloc;
        tableDesloc += INT_SIZE_BYTES;

        scopes.front().insert(entry(strdup(arg_new_c->token_value_data.str), arg_new_c));
    }

    if(strcmp(identifier->token.str, "main") != 0){
        int new_scope_offset = scope_deslocs.front(); 
        scope_deslocs.pop_front(); 
        scope_deslocs.push_front(new_scope_offset);
    }
}

void collect_arg(struct lex_value_t *identifier, int type) {
    validate_err_function_string(type, get_line_number(), get_col_number());

    if(identifier != NULL) {
        arguments_collector.push_back((struct argument){
            .id = strdup(identifier->token.str), 
            .type = type, 
            .col = get_col_number(), 
            .lin = get_line_number()
        });
    }
}

void collect_param(struct node *lit) {
    if(lit != NULL) {
        parameters_collector.push_back(lit);
    }
}

void declare_id_entry_missing_type(struct lex_value_t *identifier)
{
    declare_entry_missing_type(identifier, VAR_N, 1);
}
void declare_vector_entry_missing_type(struct lex_value_t *identifier, struct lex_value_t *vec_size)
{
    declare_entry_missing_type(identifier, VEC_N, vec_size->token.integer);
}
void declare_entry_missing_type(struct lex_value_t *identifier, int nat, int mult) {
    struct symtable_content *new_c = new symtable_content();
    new_c->lin = identifier->line;
    new_c->col = get_col_number();
    new_c->nature = nat;
    new_c->type = UND_T;
    new_c->size = mult;
    new_c->token_value_data = identifier->token;

    validate_err_declared_symbol(*new_c, identifier->token.str, nat);

    undefined_type_entries.insert(entry(strdup(identifier->token.str), new_c));
}

void declare_id_entry_missing_type_init_id(struct lex_value_t *id_to_add, struct lex_value_t *id_init) {

    int type = get_type_or_err_undeclared_symbol(*id_init, VAR_N);
    int init_size = 0;
    if(type == STRING_T) init_size = size_of(*id_init);

    struct symtable_content *new_c = new symtable_content();
    new_c->lin = id_to_add->line;
    new_c->col = get_col_number();
    new_c->type = type;
    new_c->nature = VAR_N;
    new_c->size = type == STRING_T && init_size > 0 ? init_size : 0;
    new_c->token_value_data = id_to_add->token;

    validate_err_declared_symbol(*new_c, id_to_add->token.str, VAR_N);

    undefined_type_entries.insert(entry(strdup(id_to_add->token.str), new_c));

}
void declare_id_entry_missing_type_init_lit(struct lex_value_t *id_to_add, struct node *lit_init) {
    struct symtable_content *new_c = new symtable_content();
    new_c->lin = id_to_add->line;
    new_c->col = get_col_number();
    new_c->type = lit_init->type;
    new_c->nature = VAR_N;
    new_c->size = lit_init->type == STRING_T ? strlen(lit_init->value->token.str) : 1;
    new_c->token_value_data = id_to_add->token;

    validate_err_declared_symbol(*new_c, id_to_add->token.str, VAR_N);

    undefined_type_entries.insert(entry(strdup(id_to_add->token.str), new_c));
}

void add_symtable_lit(struct lex_value_t *lit) {
    int type = type_from_lex_val(lit->type);
    struct symtable_content *new_c = new symtable_content();
    new_c->lin = lit->line;
    new_c->col = get_col_number();
    new_c->type = type;
    new_c->nature = LIT_N;
    new_c->size = type != STRING_T ? bytes_of(type) : strlen(lit->token.str) ;
    new_c->token_value_data = lit->token;

    if(scopes.empty()) {
        symbols_table new_table;
        scopes.push_front(new_table);
        scope_deslocs.push_front(0);
    }

    char* key;
    switch(lit->type){
        case LIT_BOOL:
            lit->token.boolean == TRUE ? key = strdup("true") : key = strdup("false");
            break;
        case LIT_CHAR:
            key = (char*)malloc(4*sizeof(char));
            key[0] = '\'';
            key[1] = lit->token.character;
            key[2] = '\'';
            key[3] = '\0';
            break;
        case LIT_FLOAT:
            key = strdup(to_string(lit->token.flt).c_str());
            break;
        case LIT_INT:
            key = strdup(to_string(lit->token.integer).c_str());
            break;
        case LIT_STR:
            key = strcat(strcat(strdup("\""), strdup(lit->token.str)), "\"");
            break;
    }

    // adiciona literais no escopo global
    scopes.back().insert(entry(strdup(key), new_c));
    free(key);
}

int type_from_lex_val(int lex_val_type) {
    switch (lex_val_type) {
        case LIT_INT:
            return INT_T;
        case LIT_FLOAT:
            return FLOAT_T;
        case LIT_CHAR:
            return CHAR_T;
        case LIT_BOOL:
            return BOOL_T;
        case LIT_STR:
            return STRING_T;
        
        default:
            return UND_T;
    }
}

symtable_content* validate_attr_id(char* symtable_key, int line, int col)
{
    for(auto scope = scopes.begin(); scope!= scopes.end(); scope++) {
        auto table_entry = scope->find(symtable_key);
        if(table_entry != scope->end())
        {
            return table_entry->second;
        }
    }
    cout << "Undeclared variable at ln "<<line<<", col "<<col<<": identifier '"<<symtable_key<<"'" << endl;    
    printf("program exit code (%d).", ERR_UNDECLARED);
    libera(arvore);
    exit(ERR_UNDECLARED); 
}
void validate_attr_expr(lex_value_t* id, int nature, struct node* expr)
{
    symtable_content* id_content = validate_attr_id(id->token.str, id->line, get_col_number());
    validate_nature(id_content->nature, nature, get_line_number());

    if(expr->value->type == ID) {
        symtable_content* expr_content = validate_attr_id(expr->value->token.str, expr->value->line, get_col_number());
        validate_nature(expr_content->nature, VAR_N, get_line_number());
        validate_size(*id_content, *expr_content);
    }
    if(id_content->type != expr->type && (id_content->type == CHAR_T || id_content->type == STRING_T || expr->type == CHAR_T || expr->type == STRING_T))
    {
        cout << "Attempt to attribute variable of type <";print_type_str(id_content->type);cout<<"> to type <";print_type_str(expr->type);
        cout <<"> at ln "<<id_content->lin<<", col "<<id_content->col<<".\n";
        printf("program exit code (%d).", ERR_WRONG_TYPE);
        libera(arvore);
        exit(ERR_WRONG_TYPE);
    }
    validate_attr_vec_acess(expr);
}
void validate_attr_vec_acess(struct node* expr) {
    if(expr->value->type == OC && strcmp(expr->value->token.str, "[]") == 0) {
        // significa que é um acesso a  vetor, o primeiro filho é o identificador utilizado
        symtable_content* expr_content = validate_attr_id(expr->children[0]->value->token.str, expr->value->line, get_col_number());
        validate_nature(expr_content->nature, VEC_N, expr_content->lin);
        // não valida tamanho pois não existe vetor de string, e os outros tipos podem ser convertidos e já foram validados
    }
}

void validate_input_id(lex_value_t* id) {
    symtable_content* id_content = validate_attr_id(id->token.str, id->line, get_col_number());
    validate_nature(id_content->nature, VAR_N, get_line_number());
    if(id_content->type != INT_T && id_content->type != FLOAT_T) 
    {
        cout << "Command 'input' at ln " << get_line_number() << ", col " << get_col_number() << " must be used with identifier of type int or float. Received <";
        print_type_str(id_content->type); cout<<">.\n";
        printf("program exit code (%d).", ERR_WRONG_PAR_INPUT);
        libera(arvore);
        exit(ERR_WRONG_PAR_INPUT);
    }
}
void validate_output_id(lex_value_t* id) {
    symtable_content* id_content = validate_attr_id(id->token.str, id->line, get_col_number());
    validate_nature(id_content->nature, VAR_N, get_line_number());
    if(id_content->type != INT_T && id_content->type != FLOAT_T)
    {
        cout << "Command 'output' at ln " << get_line_number() << ", col " << get_col_number() << " must be used with identifier or literal of type int or float. Received <";
        print_type_str(id_content->type); cout<<">.\n";
        printf("program exit code (%d).", ERR_WRONG_PAR_OUTPUT);
        libera(arvore);
        exit(ERR_WRONG_PAR_OUTPUT); 
    }
}
void validate_output_lit(int type){
    if(type != INT_T && type != FLOAT_T)
    {
        cout << "Command 'output' at ln " << get_line_number() << ", col " << get_col_number() << " must be used with identifier or literal of type int or float. Received <";
        print_type_str(type); cout<<">.\n";
        printf("program exit code (%d).", ERR_WRONG_PAR_OUTPUT);
        libera(arvore);
        exit(ERR_WRONG_PAR_OUTPUT);
    }
}
void validate_return(int type){
    if(type != expected_ret_type && (type == STRING_T || expected_ret_type == STRING_T))
    {
        cout << "<string> is not supported as return type; found at line " << get_line_number() << ".\n";
        printf("program exit code (%d).", ERR_FUNCTION_STRING);
        libera(arvore);
        exit(ERR_FUNCTION_STRING);
    }
    if(type != expected_ret_type && (type == CHAR_T || expected_ret_type == CHAR_T))
    {
        cout << "Wrong return type <";print_type_str(type);cout<<"> at line " << get_line_number() << ". Function expected <";
        print_type_str(expected_ret_type); cout<<">.\n";
        printf("program exit code (%d).", ERR_WRONG_PAR_RETURN);
        libera(arvore);
        exit(ERR_WRONG_PAR_RETURN);
    }
}
void validate_func_cal_parameters(struct lex_value_t *func)
{
    symtable_content* func_content = validate_attr_id(func->token.str, func->line, get_col_number());

    if(func_content->arguments.size() > parameters_collector.size())
    {
        cout << "Function '"<<func_content->token_value_data.str<<"' expected "<<func_content->arguments.size()<<" arguments, ";
        cout << "but found "<<parameters_collector.size()<<" at ln "<<func->line<<", col "<<get_col_number()<<" .\n";
        printf("program exit code (%d).", ERR_MISSING_ARGS);
        libera(arvore);
        exit(ERR_MISSING_ARGS);
    }
    if(func_content->arguments.size() < parameters_collector.size())
    {
        cout << "Function '"<<func_content->token_value_data.str<<"' expected "<<func_content->arguments.size()<<" arguments, ";
        cout << "but found "<<parameters_collector.size()<<" at ln "<<func->line<<", col "<<get_col_number()<<" .\n";
        printf("program exit code (%d).", ERR_EXCESS_ARGS);
        libera(arvore);
        exit(ERR_EXCESS_ARGS);  
    }
    int i = 0;
    reverse(parameters_collector.begin(), parameters_collector.end());
    for(auto arg = func_content->arguments.begin(); arg != func_content->arguments.end(); arg++)
    {
        if(arg->type != parameters_collector[i]->type && (arg->type == STRING_T || parameters_collector[i]->type == STRING_T))
        {
            cout << "Parameter of type <string> passed to function at line " << get_line_number() << " is not supported.\n";
            printf("program exit code (%d).", ERR_FUNCTION_STRING);
            libera(arvore);
            exit(ERR_FUNCTION_STRING);
        }
        if(arg->type != parameters_collector[i]->type && (arg->type == CHAR_T || parameters_collector[i]->type == CHAR_T))
        {
            cout << "Wrong parameter type <";print_type_str(parameters_collector[i]->type);cout<<"> at line " << get_line_number() << ". Function expected <";
            print_type_str(arg->type); cout<<">.\n";
            printf("program exit code (%d).", ERR_WRONG_TYPE_ARGS);
            libera(arvore);
            exit(ERR_WRONG_TYPE_ARGS);
        }
        i++;
    }

    parameters_collector.clear();
}
void validate_shift(struct lex_value_t *lit) {
    if(lit->token.integer > 16) {
        cout << "Command 'shift' at ln " << lit->line << ", col " << get_col_number() << " cannot be used with integet value greater that 16 (received "<<lit->token.integer<<").\n";
        printf("program exit code (%d).", ERR_WRONG_PAR_SHIFT);
        libera(arvore);
        exit(ERR_WRONG_PAR_SHIFT);
    }
}

int get_type_or_err_undeclared_symbol(struct lex_value_t id_init, int nature) {
    for(auto scope = scopes.begin(); scope != scopes.end(); scope++) {

        auto table_entry = scope->find(id_init.token.str);
        if(table_entry != scope->end()) {
            validate_nature(table_entry->second->nature, nature, id_init.line);
            return table_entry->second->type;
        }  
    }

    auto table_entry = undefined_type_entries.find(id_init.token.str);
    if(table_entry != undefined_type_entries.end()) { 
        // significa que a variável utilizada ainda não foi declarada na tablela de simbolos, 
        // mas esta sendo declarada junto com a variavel que está sendo utilizada para inicializar
        return UND_T;
    }

    printf("Attempt to initialize var with unknown identifier '%s' at line %d.\n", id_init.token.str, id_init.line);
    printf("program exit code (%d).", ERR_UNDECLARED);
    libera(arvore);
    exit(ERR_UNDECLARED);  
}
void validate_err_declared_symbol(symtable_content content, char* symtable_key, int nature) {
    if(!scopes.empty()) {
        symbols_table scope = scopes.front();
        auto table_entry = scope.find(symtable_key);

        if(table_entry != scope.end()) {
            cout << "Redeclaration at ln "<<content.lin<<", col "<<content.col<<": identifier '"<<symtable_key<<"' (";
            print_nature_str(content.nature); cout<<") is already in use at ln "<<table_entry->second->lin<<", col "<<table_entry->second->col<<" (";
            print_nature_str(table_entry->second->nature); cout<<").\n";
            printf("program exit code (%d).", ERR_DECLARED);
            libera(arvore);
            exit(ERR_DECLARED);
        }  
    }  
}
int size_of(struct lex_value_t id_init) {
    for(auto scope = scopes.begin(); scope != scopes.end(); scope++)
    {
        auto table_entry = scope->find(id_init.token.str);
        if(table_entry != scope->end()) {
            return table_entry->second->size;
        }  
    }

    return 0; 
}
void validate_nature(int expected_n, int actual_n, int line) {
    if(expected_n == VAR_N && (actual_n == VEC_N || actual_n == FUNC_N)) {
        cout << "Attempt to use 'VARIABLE' as '";print_nature_str(actual_n);cout<<"' at ln " << line << endl;
        printf("program exit code (%d).", ERR_VARIABLE);
        libera(arvore);
        exit(ERR_VARIABLE);  
    } if(expected_n == VEC_N && (actual_n == VAR_N || actual_n == FUNC_N)) {
        cout << "Attempt to use 'VECTOR' as '";print_nature_str(actual_n);cout<<"' at ln " << line <<", col " << get_col_number() << ".\n";
        printf("program exit code (%d).", ERR_VECTOR);
        libera(arvore);
        exit(ERR_VECTOR); 
    } if(expected_n == FUNC_N && (actual_n == VAR_N || actual_n == VEC_N)) {
        cout << "Attempt to use 'FUNCTION' as '";print_nature_str(actual_n);cout<<"' at ln " << line << endl;
        printf("program exit code (%d).", ERR_FUNCTION);
        libera(arvore);
        exit(ERR_FUNCTION);  
    }
}
void validate_size(symtable_content content_max, symtable_content content_received) {
    if(content_max.size < content_received.size)
    {
        cout << "Attempt to initialize variable '"<<content_max.token_value_data.str<<"' with content exceedind its maximum bytes size ("<<content_max.size<<") ";
        cout<<"set at ln " << content_max.lin << ", col " << content_max.col << ".\n";
        printf("program exit code (%d).", ERR_STRING_MAX);
        libera(arvore);
        exit(ERR_STRING_MAX);  
    }
}
void validate_not_string_vector(int type, int nature, symtable_content content, char* symtable_key) {
    if(type == STRING_T && nature == VEC_N) {
        printf("Attempt to declare '%s' as a vector of type string at ln %d, col %d. String type does not support vectors.\n", symtable_key, content.lin, content.col);
        printf("program exit code (%d).", ERR_STRING_VECTOR);
        libera(arvore);
        exit(ERR_STRING_VECTOR);
    }    
}
void validate_err_function_string(int type, int line, int col)
{    
    if(type == STRING_T) {
        printf("Error ln %d, col %d: type of the return or parameters of a function cannot be <string>.\n", line, col);
        printf("program exit code (%d).", ERR_FUNCTION_STRING);
        libera(arvore);
        exit(ERR_FUNCTION_STRING); 
    }
}

void validate_node_type(struct node* n1, struct node* n2) {
    get_inferred_type_validate_char_string(n1->type, n2->type, get_line_number(), get_col_number());
}

void pop_scope(bool offset_backpropag) {
    
    if(offset_backpropag) { 
        // substitui o offset do segundo no topo pelo do topo que será removido
        int offset = scope_deslocs.front();
        scope_deslocs.pop_front();
        scope_deslocs.pop_front();
        scope_deslocs.push_front(offset);
    } else {
        scope_deslocs.pop_front();
    }
    
    free_symbols_table(scopes.front());
    scopes.pop_front();
}
void push_scope() {
    symbols_table new_scope;
    scopes.push_front(new_scope);
    int desloc = scope_deslocs.front();
    scope_deslocs.push_front(desloc);
}

int get_inferred_type_symtable_cont(int type1, int type2, symtable_content content) {
    return get_inferred_type_validate_char_string(type1, type2, content.lin, content.col);

}
int get_inferred_type_validate_char_string(int type1, int type2, int lin, int col) {
    if(type1 == type2) return type1;
    if((type1 == INT_T && type2 == FLOAT_T) || (type2 == INT_T && type1 == FLOAT_T)) return FLOAT_T;
    if((type1 == INT_T && type2 == BOOL_T) || (type2 == INT_T && type1 == BOOL_T)) return INT_T;
    if((type1 == FLOAT_T && type2 == BOOL_T) || (type2 == FLOAT_T && type1 == BOOL_T)) return FLOAT_T;

    if(type1 == CHAR_T) {
        cout << "Attempt to coerce variable of type <char> to type <";print_type_str(type2);cout <<"> at ln "<<lin<<", col "<<col<<".\n";
        printf("program exit code (%d).", ERR_CHAR_TO_X);
        libera(arvore);
        exit(ERR_CHAR_TO_X);
    }
    if(type2 == CHAR_T) {
        cout << "Attempt to coerce variable of type <char> to type <";print_type_str(type1);cout <<"> at ln "<<lin<<", col "<<col<<".\n";
        printf("program exit code (%d).", ERR_CHAR_TO_X);
        libera(arvore);
        exit(ERR_CHAR_TO_X);
    }
    if(type1 == STRING_T) {
        cout << "Attempt to coerce variable of type <string> to type <";print_type_str(type2);cout <<"> at ln "<<lin<<", col "<<col<<".\n";
        printf("program exit code (%d).", ERR_STRING_TO_X);
        libera(arvore);
        exit(ERR_STRING_TO_X);
    }
    if(type2 == STRING_T) {
        cout << "Attempt to coerce variable of type <string> to type <";print_type_str(type1);cout <<"> at ln "<<lin<<", col "<<col<<".\n";
        printf("program exit code (%d).", ERR_STRING_TO_X);
        libera(arvore);
        exit(ERR_STRING_TO_X);
    }

    return UND_T;
}
int get_inferred_type_node(struct node* n1, struct node* n2) {
    return get_inferred_type_validate_char_string(n1->type, n2->type, get_line_number(), get_col_number());
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
                    cout << "<";print_type_str(ita->type); cout << " " << ita->id << "> ";
                }
                 cout << " -> ";
            }
            print_type_str(itm->second->type); cout << endl;
        }
    }
}
void print_undef() {
    cout << "print print_undef (size: " << undefined_type_entries.size() << ")\n";
    for (auto itm = undefined_type_entries.begin(); itm != undefined_type_entries.end(); ++itm) {
        //cout << "       " << itm->first << "(type: type_str(itm->second->type)";
        cout << " | size: " << itm->second->size << ")\n";
    }
}

void print_type_str(int type) {
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
            printf("undefined");
    }
}
void print_nature_str(int nature) {
    switch (nature) {
        case VAR_N:
            printf("VARIABLE");
            break;
        case VEC_N:
            printf("VECTOR");
            break;
        case FUNC_N:
            printf("FUNCTION");
            break;
        case LIT_N:
            printf("LITERAL");
            break;
        default:
            printf(" ");
            break;
    }
}

int getOffset(char* id, int* scope) {
    *scope = LOCAL;
    for(auto auxScope = scopes.begin(); auxScope!= scopes.end(); auxScope++) {
        auto table_entry = auxScope->find(id);
        if(table_entry != auxScope->end()) {
            if(*auxScope == scopes.back()) {
                *scope = GLOBAL;
            }
            return table_entry->second->offset;
        }
    }
    return -1;
}