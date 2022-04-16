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
}

void ifCode(struct node* parent) {
    int label_true  = newLabel();
    int label_false = newLabel();

    patch(parent->children[0]->patchTrue, label_true);
    patch(parent->children[0]->patchFalse, label_false);

    struct instr* label_true_code = codeLabel(label_true);
    struct instr* label_false_code = codeLabel(label_false);

    parent->children[0]->codeEnd->next = label_true_code;
    if(parent->children[1] != NULL) {
        label_true_code->next = parent->children[1]->code;
        parent->children[1]->codeEnd->next = label_false_code;
    } else {
        label_true_code->next = label_false_code;
    }
    parent->codeEnd = label_false_code; 

    if(parent->children[2] != NULL) { 
        int label_next = newLabel();
        struct instr* label_next_code = codeLabel(label_next); // Codigo do label para a inst depois do if else
        struct instr* end_then = newInstr(JUMPI, label_next, NU, NU);
        struct instr* end_else = newInstr(JUMPI, label_next, NU, NU);

        if(parent->children[1] != NULL){
            parent->children[1]->codeEnd->next = end_then;
        }else{
            label_true_code->next = end_then;
        }
        end_then->next = label_false_code;
        label_false_code->next = parent->children[2]->code;
        parent->children[2]->codeEnd->next = end_else;
        end_else->next = label_next_code;
        parent->codeEnd = label_next_code;  
    }

    parent->code = parent->children[0]->code;
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

    struct instr* code_jump_for = newInstr(JUMPI, for_start, NU, NU);

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
    if(mainFunct == TRUE) {
        struct instr* halt = newInstr(HALT, NU, NU, NU);
        node1->children[0]->codeEnd->next = halt;

        node1->code = node1->children[0]->code;
        node1->codeEnd = halt;
    } else {        
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

void codeOr(struct node* parent) {
    int label = newLabel();
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
    int label = newLabel();
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

void codeUnaryOp(struct node* parent, struct node* exp){

    switch (parent->value->token.character)
    {
        case '-': {
            int reg = newRegister();
            struct instr* neg = newInstr(RSUBI, exp->temp, 0, reg);
            exp->codeEnd->next = neg;
            parent->code = exp->code;
            parent->codeEnd = neg;
            parent->temp = reg;
            break;
        }
        case '!': {
            parent->patchFalse = exp->patchTrue;
            parent->patchTrue = exp->patchFalse;
            parent->code = exp->code;
            parent->codeEnd = exp->codeEnd;
            parent->temp = exp->temp;
            break;
        }
    }
}

void codeTernaryOp(struct node* parent) {
    int label_true  = newLabel();
    int label_false = newLabel();
    int label_next = newLabel();

    patch(parent->children[0]->patchTrue, label_true);
    patch(parent->children[0]->patchFalse, label_false);

    struct instr* label_true_code = codeLabel(label_true);
    struct instr* label_false_code = codeLabel(label_false);
    struct instr* next_label_code = codeLabel(label_next);

    int temp = newRegister();
    struct instr* transfer_if_true  = newInstr(I2I, parent->children[1]->temp, temp, NU);
    struct instr* transfer_if_false = newInstr(I2I, parent->children[2]->temp, temp, NU);

    struct instr* end_then = newInstr(JUMPI, label_next, NU, NU);
    struct instr* end_else = newInstr(JUMPI, label_next, NU, NU);

    parent->children[0]->codeEnd->next = label_true_code;
    label_true_code->next = parent->children[1]->code;

    parent->children[1]->codeEnd->next = transfer_if_true;
    transfer_if_true->next = end_then;
    end_then->next = label_false_code;
    label_false_code->next = parent->children[2]->code;

    parent->children[2]->codeEnd->next = transfer_if_false;
    transfer_if_false->next = end_else;
    end_else->next = next_label_code;

    parent->code = parent->children[0]->code;
    parent->codeEnd = next_label_code;
    parent->temp = temp;
}

void patch(list<int*> patchList, int label) {
    for(auto aux = patchList.begin(); aux != patchList.end(); aux++) {
        **aux = label;
    }
    patchList.clear();
}

list<int*> concatPatches(list<int*> patchList1, list<int*> patchList2) {
    patchList1.insert(patchList1.end(), patchList2.begin(), patchList2.end());
    return patchList1;
}

void initCode(void *tree) {
    struct node *root = (struct node*)tree;

    struct instr* init_rfp = newInstr(LOADI, 1024, RFP, NU);
    struct instr* init_rsp = newInstr(LOADI, 1024, RSP, NU);
    struct instr* init_rbss = newInstr(LOADI, 1000, RBSS, NU);

    int label = getFuncLabel("main");
    struct instr* jump = newInstr(JUMPI, label, NU, NU);

    init_rfp->next = init_rsp;
    init_rsp->next = init_rbss;
    init_rbss->next = jump;
    jump->next = root->code;

    root->code = init_rfp;
}

void printCode(void *tree) {
    struct node *root = (struct node*)tree;
    struct instr* code = root->code;

    while(code != NULL) {
        switch (code->op) {
            case LOADAI:  printRegConstRegInstr("loadAI", code); break;
            case LOADI:   printOneReg("loadI", codigo); break;
            case ADD:     printThreeReg("add", codigo, '='); break;
            case SUB:     printThreeReg("sub", codigo, '='); break;
            case DIV:     printThreeReg("div", codigo, '='); break;
            case MULT:    printThreeReg("mult", codigo, '='); break;
            case RSUBI:   printRegConstRegInstr("rsubI", codigo); break;
            case CMP_LT:  printThreeReg("cmp_LT", codigo, '-'); break;
            case CMP_LE:  printThreeReg("cmp_LE", codigo, '-'); break;
            case CMP_EQ:  printThreeReg("cmp_EQ", codigo, '-'); break;
            case CMP_NE:  printThreeReg("cmp_NE", codigo, '-'); break;
            case CMP_GT:  printThreeReg("cmp_GT", codigo, '-'); break;
            case CMP_GE:  printThreeReg("cmp_GE", codigo, '-'); break;
            case CBR:     printf("cbr\tr%d  ->  L%d, L%d\n", codigo->arg1, codigo->arg2, codigo->arg3); break;
            case NOP:     printf("L%d: nop\n", codigo->label); break;
            case STOREAI: printStoreAI(codigo); break;
            case JUMPI:   printf("jumpI\t->  L%d\n", codigo->arg1); break;
            case JUMP:    printf("jump\t->  r%d\n", codigo->arg1); break;
            case ADDI:    printRedConstRegInstr("addI", codigo); break;
            case HALT:    printf("halt\n"); break;
            case I2I:     printTwoReg("i2i", codigo); break;
        }
        code = code->next;
    }
}

void printReg(int id) {
    switch(id){
        case RFP:  printf("rfp"); break;
        case RSP:  printf("rsp"); break;
        case RBSS: printf("rbss"); break;
        case RPC:  printf("rpc"); break;
        default:   printf("r%d", id);
    }
}

void printRegConstRegInstr(char* op, struct instr* inst) {
    printf("%s\t", op);
    printNomeReg(inst->arg1);
    printf(", %d  =>  ", inst->arg2);
    printNomeReg(inst->arg3);
    printf("\n");
}

void printStoreAI(struct instr* instr1) {
    printf("storeAI\t");
    printNomeReg(inst->arg1);
    printf("  =>  ");
    printNomeReg(inst->arg2);
    printf(", %d\n", inst->arg3);
}

void printOneReg(char* op, struct instr* instr1) {
    printf("%s\t%d  =>  ", op, inst->arg1);
    printNomeReg(inst->arg2);
    printf("\n");
}

void printTwoReg(char* op, struct instr* instr1) {
    printf("%s\t", op);
    printNomeReg(inst->arg1);
    printf("  =>  ");
    printNomeReg(inst->arg2);
    printf("\n");
}

void printThreeReg(char* op, struct instr* instr1, char arrow) {
    printf("%s\t", op);
    printNomeReg(inst->arg1);
    printf(", ");
    printNomeReg(inst->arg2);
    printf("  %c>  ", arrow);
    printNomeReg(inst->arg3);
    printf("\n");
}