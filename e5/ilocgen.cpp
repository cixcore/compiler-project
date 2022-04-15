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

void loadVar(struct node* no, char* id){
    int scope;
    int offset = getOffset(id, &scope);

    int base = scope == GLOBAL ? RBSS : RFP;

    int reg = newRegister();
    no->code = newInstr(LOADAI, base, offset, reg);
    no->codeEnd = no->code;
    no->temp = reg;
}

void loadLit(struct node* no){
    int reg = newRegister();
    no->code = newInstr(LOADI, no->value->token.integer, reg, NU);
    no->codeEnd = no->code;
    no->temp = reg;
}

void storeVar(struct node* parent, struct lex_value_t* var, struct node* exp) {
    int scope;
    int offset = getOffset(var->token.str, &scope);

    int base = scope == GLOBAL ? RBSS : RFP;

    struct instr* store = newInstr(STOREAI, exp->temp, base, offset);
    exp->codeEnd->next = store;
    parent->code = exp->code; 
    parent->codeEnd = store; 
}

void funcCallCode(struct node* node1){
    struct instr* store_rsp = newInstr(STOREAI, RSP, RSP, 4);     // Salva o RSP
    struct instr* store_rfp = newInstr(STOREAI, RFP, RSP, 8);     // Salva o RFP
    store_rsp->next = store_rfp;

    int offset = 16; 
    struct node* param = node1->children[0];
    struct instr* end = store_rfp;

    while(param != NULL){
        struct instr* param_store = newInstr(STOREAI, param->temp, RSP, offset);

        end->next = param->code;
        param->codeEnd->next = param_store;

        offset += INT_SIZE_BYTES; // só int na e5
        end = param_store;
        param = param->next;
    }

    int return_addr = newRegister();
    struct instr* l_return_addr = newInstr(ADDI, RPC, 3, return_addr);
    struct instr* s_return_addr = newInstr(STOREAI, return_addr, RSP, 0); 
    end->next = l_return_addr;
    l_return_addr->next = s_return_addr;

    int label = getFuncLabel(node1->value->token.str);
    struct instr* jump = newInstr(JUMPI, label, NU, NU);
    s_return_addr->next = jump;

    int return_value = newRegister();
    struct instr* l_ret = newInstr(LOADAI, RSP, 12, return_value);
    jump->next = l_ret;

    node1->code = store_rsp;
    node1->codeEnd = l_ret;
    node1->temp = return_value;
}

void returnCode(struct node* node1)
{
    if(mainFunct == TRUE)
    {
        struct instr* halt = newInstr(HALT, NU, NU, NU);
        node1->children[0]->codeEnd->next = halt;

        node1->code = node1->children[0]->code;
        node1->codeEnd = halt;
    } else
    {        
        int temp = newRegister();

        struct instr* store_ret = newInstr(STOREAI, node1->children[0]->temp, RFP, 12); //Store do valor a ser retornado no campo especifico
        struct instr* l_end_ret = newInstr(LOADAI, RFP, 0, temp); // Carrega end de retorno em reg temporario
        struct instr* rsp_update = newInstr(LOADAI, RFP, 4, RSP);
        struct instr* rfp_update = newInstr(LOADAI, RFP, 8, RFP);
        struct instr* jump = newInstr(JUMP, temp, NU, NU); // Salta para o endere�o de retorno

        node1->children[0]->codeEnd->next = store_ret;
        store_ret->next = l_end_ret;
        l_end_ret->next = rsp_update;
        rsp_update->next = rfp_update;
        rfp_update->next = jump;

        node1->code = node1->children[0]->code;
        node1->codeEnd = jump;
    }
}