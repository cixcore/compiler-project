#include "parser.tab.h"
#include "yylvallib.h"
#include "tree.h"
#include "types.h"
#include "ilocgen.h"
#include "tree.h"
#include "types.h"

using namespace std;

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
    int offset = getOffset(var->token.str, &scope);

    int base = scope == GLOBAL ? RBSS : RFP;
}

void codeFor(struct node* node1) {
    int for_start = newLabel();
    int label_true = newLabel();
    int label_false = newLabel();

    patch(node1->children[1]->patchTrue,label_true);
    patch(node1->children[1]->patchFalse,label_false);

    struct instr* code_for = codeLabel(for_start);
    struct instr* code_true = codeLabel(label_true);
    struct instr* code_false = codeLabel(label_false);

    struct instr* code_jump_for = newInstr(JUMPI, start_for, NU, NU);

    node1->children[0]->codeEnd->next = code_for;
    code_for->next = node1->children[1]->code;
    node1->children[1]->codeEnd->next = code_true;

    if(node1->children[3] != NULL) {
        code_true->next = node1->children[3]->code;
        node1->children[3]->codeEnd->next = node1->children[2]->code;
    }else {
        code_true->next = node1->children[2]->code;
    }

    node1->children[2]->codeEnd->next = code_jump_for;
    code_jump_for->next = code_false;

    node1->code = node1->children[0]->code;
    node1->codeEnd = code_false;
}

void codeWhile(struct node* node1) {
    int while_start = newLabel();
    int label_true = newLabel();
    int label_false = newLabel();

    patch(node1->children[0]->patchTrue, label_true);
    patch(node1->children[0]->patchFalse, label_false);

    struct instr* code_while = codeLabel(while_start);
    struct instr* code_true = codeLabel(label_true);
    struct instr* code_false = codeLabel(label_false);

    struct instr* code_jump_while = newInstr(JUMPI, while_start, NU, NU);

    code_while->next = node1->children[0]->code;
    node1->children[0]->codeEnd->next = code_true;
    if(node1->children[1] != NULL) {
        code_true->next = node1->children[1]->code;
        node1->children[1]->codeEnd->next = code_jump_while;
    } else {
        code_true->next = code_jump_while;
    }
    code_jump_while->next = code_false;

    node1->code = code_while;
    node1->codeEnd = code_false;
}

void codeOr(struct node* parent) {
    int label newLabel();
    patch(parent->children[0]->patchFalse, label);

    parent->patchFalse = parent->children[1]->patchFalse;
    parent->patchTrue = concatPatches(parent->children[0]->patchTrue, parent->children[1]->patchTrue);

    struct instr* nop = codeLabel(label);
    parent->children[0]->codeEnd->next = nop;
    nop->next = parent->children[1]->code;

    parent->code = parent->children[0]->code;
    parent->codeEnd = parent->children[1]->codeEnd;
}

void codeAnd(struct node* parent) {
    int label newLabel();
    patch(parent->children[0]->patchTrue, label);

    parent->patchTrue = parent->children[1]->patchTrue;
    parent->patchFalse = concatPatches(parent->children[0]->patchFalse, parent->children[1]->patchFalse);

    struct instr* nop = codeLabel(label);
    parent->children[0]->codeEnd->next = nop;
    nop->next = parent->children[1]->code;

    parent->code = parent->children[0]->code;
    parent->codeEnd = parent->children[1]->codeEnd;
}

void codeRelOp(struct node* parent, int op) {
    int reg = newRegister();
    struct instr* cmp = newInstr(op, parent->children[0]->temp, parent->children[1]->temp, reg);
    struct instr* cbr = newInstr(CBR, reg, NU, NU);

    parent->children[0]->codeEnd->next = parent->children[1]->code;
    parent->children[1]->codeEnd->next = cmp;
    cmp->next = cbr;

    parent->code = parent->children[0]->code;
    parent->codeEnd = cbr;
    
    parent->patchTrue.push_front(&(cbr->arg2));
    parent->patchFalse.push_front(&(cbr->arg3));
}

void codeBinaryOp(struct node* parent, int op) {
    int reg = newRegister();
    struct instr* opCode = newInstr(op, parent->children[0]->temp, parent->children[1]->temp, reg);

    parent->children[0]->codeEnd->next = parent->children[1]->code;
    parent->children[1]->codeEnd->next = opCode;

    parent->code = parent->children[0]->code;
    parent->codeEnd = opCode;
    parent->temp = reg;
}

void codeUnaryOp(struct node* parent, struct node* op, struct node* exp){
    if(op == NULL || (op->value->token.character != '-' && op->value->token.character != '!')) {
        return;
    }

    if(op->value->token.character == '-'){
        int reg newRegister();
        struct inst* neg = newInstr(RSUBI, exp->temp, 0, reg);
        exp->codeEnd->next = neg;
        parent->code = exp->code;
        parent->codeEnd = neg;
        parent->temp = reg;
    }

    if(op->value->token.character == '!') {
        parent->patchFalse = exp->patchTrue;
        parent->patchTrue = exp->patchFalse;
        parent->code = exp->code;
        parent->codeEnd = exp->codeEnd;
        parent->temp = exp->temp;
    }
}

void patch(list<int*> patchList, int label) {
    for(auto aux = patchList.begin() ; aux != patchList.end ; aux++) {
        *aux = label
    }
    patchList.clear();
}

list<int*> concatPatches(list<int*> patchList1, list<int*> patchList2) {
    if(patchList1 == NULL) {
        return patchList2;
    }
    patchList1.insert(patchList1.end(), patchList2.begin(), patchList2.end());
    return patchList1;
}