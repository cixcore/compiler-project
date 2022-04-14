#include "ilocgen.h"
#include "tree.h"
#include "types.h"

using namespace std;

int reg_counter = 0;
int label_counter = 0;
int mainFunct;
struct label_list_t* label_list = NULL;

int newRegister() {
    return reg_counter++;
}

int newLabel() {
    return label_counter++;
}

void generate_iloc(struct node* root){}

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

    struct label_list_t* new_label = new label_list_t;
    new_label->funcName = strdup(node1->value->token.str);
    new_label->label = label;
    new_label->next = label_list;
    label_list = new_label;
}

int getFuncLabel(char* func) {
    struct label_list_t* aux = label_list;
    while(aux != NULL) {
        if(strcmp(func, aux->funcName) == 0) {
            return aux->label;
        }
        aux = aux->next;
    }
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
    struct label_list_t* newLabel = new label_list_t;
    newLabel->funcName = strdup(func->token.str);
    newLabel->label = label;
    newLabel->next = label_list;
    label_list = newLabel;
}

void initCode(struct node* node1) {
    struct node* aux = node1;

    while(aux != NULL) {
        //storeVar

    }
}

void storeVar(struct node* parent, struct lex_value_t* var, struct node* exp) {
    int scope;
    int offset = getDesloc(var->token.str, &scope);

    int base;
    
}