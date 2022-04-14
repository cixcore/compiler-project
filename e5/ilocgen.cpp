#include "parser.tab.h"
#include "yylvallib.h"
#include "tree.h"
#include "types.h"
#include "ilocgen.h"

using namespace std;

int reg_counter = 0;
int label_counter = 0;
int mainFunct;
label_stack func_labels;

int newRegister() {
    return reg_counter++;
}

int newLabel() {
    return label_counter++;
}

void generate_iloc(void *root){
    struct node* tree = (struct node*) root;
}

void funcDecCode(struct node* node1, struct lex_value_t* func) {

    int label = getFuncLabel(func->token.str);
    struct instr* code_label = codeLabel(label);

    extern struct list<symbols_table> scopes;
    extern list<int> scope_deslocs;


    struct instr* updated_rsp = newInstr(ADDI, RSP, scope_deslocs.front(),RSP);

    code_label->next = updated_rsp;
    if(node1->children[0] != NULL) {
        updated_rsp->next = node1->children[0]->code;
    }

    if(strcmp(node1->value->token.str, "main") != 0) {
        struct instr* update_rfp = newInstr(I2I, RSP, RFP, NU);
        code_label->next = update_rfp;
        update_rfp->next = updated_rsp;
    }

    node1->code = code_label;
    if(node1->children[0] != NULL) {
        node1->codeEnd = node1->children[0]->codeEnd;
    } else {
        node1->codeEnd = updated_rsp;
    }

    func_labels.push_front(make_pair(strdup(node1->value->token.str), label));
}

int getFuncLabel(char* func) {
    for(auto fun = func_labels.begin(); fun != func_labels.end(); fun++) {
        if(strcmp(func, fun->first) == 0) {
            return fun->second;
        }
    }
    return -1;
}

struct instr* codeLabel(int label) {
    struct instr* aux = new instr();
    aux->label = label;
    aux->op = NOP;
    aux->arg1 = NU;
    aux->arg2 = NU;
    aux->arg3 = NU;

    return aux;
}

struct instr* newInstr(int op, int arg1, int arg2, int arg3){
    struct instr* new_instr = new instr();
    new_instr->op = op;
    new_instr->arg1 = arg1;
    new_instr->arg2 = arg2;
    new_instr->arg3 = arg3;
    return new_instr;
}

void setMain(struct lex_value_t* funcName) {
    if(strcmp(funcName->token.str, "main") == 0) {
        mainFunct = TRUE;
    } else {
        mainFunct = FALSE;
    }
}

void createLabelFunc(struct lex_value_t* func){
    int label = newLabel();
    func_labels.push_front(make_pair(strdup(func->token.str), label));
}

void initCode(struct node* node1) {
    struct node* aux = node1;

    storeVar(aux, aux->children[0]->value, aux->children[1]);
    if(aux != node1)
    {  
        node1->codeEnd->next = aux->code;
        node1->codeEnd = aux->codeEnd;
    }
    aux = aux->next;
}

void storeVar(struct node* parent, struct lex_value_t* var, struct node* exp) {
    int scope;
    int offset = getDesloc(var->token.str, &scope);

    int base = scope == GLOBAL ? RBSS : RFP;

    struct instr* store = newInstr(STOREAI, exp->temp, base, offset);
    exp->codeEnd->next = store;
    parent->code = exp->code; 
    parent->codeEnd = store; 
}