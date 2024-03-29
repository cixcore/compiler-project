#include "parser.tab.h"
#include "yylvallib.h"
#include "tree.h"
#include "types.h"
#include "ilocgen.h"
#include "assembly.h"

int cmp;

char* getGlobalVar(int offset) {
    symbols_table global = global_scope();
    for(auto it = global.begin(); it != global.end(); it++) {
        if(it->second->offset == offset && it->second->nature == VAR_N) {
            return it->first;
        }
    }

    return NULL;
}

char* getRegisterName(int id) {
    switch(id) {
        case RFP:  return "%rbp";
        case RSP:  return "%rsp";
        case RBSS: return "%rip";
    }
}

void loadaiToAssembly(struct instr* instr1) {
    if(instr1->notation == RET_LOAD ) {
        printf("subq\t$4, %%rsp\n");
        printf("movl\t%%eax, (%%rsp)\n");
    }
    else {
        printf("subq\t$4, %%rsp\n");
        if(instr1->arg1 == RBSS){
            printf("movl\t%s(%%rip), %%edx\n", getGlobalVar(instr1->arg2));
        }else{
            int offset = instr1->arg2;
            if(instr1->notation == MAIN_VAR){
                offset -= 4;
            }
            else{
                offset -= 12;
            }
            printf("movl\t-%d(%%rbp), %%edx\n", offset);
        }
        printf("movl\t%%edx, (%%rsp)\n");
    }
}

void loadiToAssembly(struct instr* instr1) {
    printf("subq\t$4, %%rsp\n");
    printf("movl\t$%d, (%%rsp)\n", instr1->arg1);
}

void cmpToAssembly(struct instr* instr1) {
    cmp = instr1->op;
    printf("movl\t(%%rsp), %%ebx\n");
    printf("addq\t$4, %%rsp\n");

    printf("movl\t(%%rsp), %%eax\n");
    printf("addq\t$4, %%rsp\n");

    printf("cmpl\t%%ebx, %%eax\n");
}

void cbrToAssembly(struct instr* instr1) {
    switch(cmp) {
        case CMP_LT: printf("jl\t.L%d\n", instr1->arg2); break;
        case CMP_LE: printf("jle\t.L%d\n", instr1->arg2); break;
        case CMP_EQ: printf("je\t.L%d\n", instr1->arg2); break;
        case CMP_NE: printf("jne\t.L%d\n", instr1->arg2); break;
        case CMP_GT: printf("jg\t.L%d\n", instr1->arg2); break;
        case CMP_GE: printf("jge\t.L%d\n", instr1->arg2); break;
    }

    printf("jmp\t.L%d\n", instr1->arg3);
}

void binaryOpToAssembly(struct instr* instr1, const char* op) {
    printf("movl\t(%%rsp), %%eax\n");
    printf("addq\t$4, %%rsp\n");

    printf("movl\t(%%rsp), %%ebx\n"); 
    printf("addq\t$4, %%rsp\n");

    printf("%sl\t%%eax, %%ebx\n", op);

    printf("subq\t$4, %%rsp\n");
    printf("movl\t%%ebx, (%%rsp)\n");
}

void divToAssembly(){
    printf("movl\t(%%rsp), %%ebx\n"); 
    printf("addq\t$4, %%rsp\n");

    printf("movl\t(%%rsp), %%eax\n"); 
    printf("addq\t$4, %%rsp\n");

    printf("cdq\n");        
    printf("idiv\t%%ebx\n"); 

    printf("subq\t$4, %%rsp\n");
    printf("movl\t%%eax, (%%rsp)\n");
}

struct instr* addiToAssembly(struct instr* instr1){
    if(instr1->notation == CALL){
        int label = instr1->next->next->arg1;
        printf("call\t%s\n", getFuncName(label)); 
        return instr1->next->next;
    }
    if(instr1->notation == UPDATE_RSP){
        printf("subq\t$%d, %%rsp\n", instr1->arg2);
    }
    return instr1;
}

void rsubiToAssembly(struct instr* instr1) {
    printf("movl\t(%%rsp), %%eax\n");
    printf("negl\t%%eax\n");
    printf("movl\t%%eax, (%%rsp)\n");
}

void initFunc(struct instr* instr1) {
    if(strcmp("main", getFuncName(instr1->label)) == 0) {
        printf(".globl	main\n");
    }
    printf("%s:\n", getFuncName(instr1->label));
    printf("pushq\t%%rbp\n");

    if(strcmp("main", getFuncName(instr1->label)) == 0) {
        printf("movq\t%%rsp, %%rbp\n");
    }
}

void labelToAssembly(struct instr* instr1) {
    if(instr1->notation == FUNC_LABEL){
        initFunc(instr1);
    }
        printf(".L%d:\n", instr1->label);
}

struct instr* storeaiToAssembly(struct instr* instr1) {
    if(instr1->notation == FUN_CALL) {
        return instr1->next;
    } else if(instr1->notation == RET_FUNC) {
        printf("movl\t(%%rsp), %%eax\n");
        printf("addq\t$4, %%rsp\n");

        printf("leave\n");
        printf("ret\n");

        return instr1->next->next->next->next;
    }

    printf("movl\t(%%rsp), %%edx\n");
    printf("addq\t$4, %%rsp\n");
    if(instr1->arg2 == RBSS){
        printf("movl\t%%edx, %s(%%rip)\n", getGlobalVar(instr1->arg3));
    }else{
        int offset = instr1->arg3;
        if(instr1->arg2 == RFP) {
            if(instr1->notation == MAIN_VAR) {
                offset -=4;
            } else {
                offset -=12;
            }
        }
        printf("movl\t%%edx, -%d(%s)\n", offset, getRegisterName(instr1->arg2));
    }
    return instr1;
}

void jumpiToAssembly(struct instr* instr1){
    printf("jmp\t.L%d\n", instr1->arg1);
}

void i2iToAssembly(struct instr* instr1){
    if(instr1->arg1 < 0){
        printf("movq\t%s, %s\n", getRegisterName(instr1->arg1), getRegisterName(instr1->arg2));
    }
}

void mainEnd(){
    printf("movl\t(%%rsp), %%eax\n");
    printf("addq\t$4, %%rsp\n");
    printf("leave\n");
    printf("ret\n");
}

void printVarsGlobais(){
    symbols_table global = global_scope();
    for(auto it = global.begin(); it != global.end(); it++) {
        if(it->second->nature == VAR_N) {
            printf(".comm %s,4\n", it->first);
        }
    }
}

void generateAsm(void* tree){
    printVarsGlobais();

    struct node *root = (struct node*)tree;
    struct instr* code = root->code;

    code = code->next->next->next->next;

    while(code != NULL){
        //std::cout << code->op << '\n';
        switch(code->op){
            case LOADAI:  loadaiToAssembly(code); break;
            case LOADI:   loadiToAssembly(code); break;
            case ADD:     binaryOpToAssembly(code, "add"); break;
            case SUB:     binaryOpToAssembly(code, "sub"); break;
            case DIV:     divToAssembly(); break;
            case MULT:    binaryOpToAssembly(code, "imul"); break;
            case RSUBI:   rsubiToAssembly(code); break;
            case CMP_LT:  cmpToAssembly(code); break;
            case CMP_LE:  cmpToAssembly(code); break;
            case CMP_EQ:  cmpToAssembly(code); break;
            case CMP_NE:  cmpToAssembly(code); break;
            case CMP_GT:  cmpToAssembly(code); break;
            case CMP_GE:  cmpToAssembly(code); break;
            case CBR:     cbrToAssembly(code); break;
            case NOP:     labelToAssembly(code); break;
            case HALT:    mainEnd(); break;
            case STOREAI: code = storeaiToAssembly(code); break;
            case JUMPI:   jumpiToAssembly(code); break;
            case ADDI:    code = addiToAssembly(code); break;
            case I2I:     i2iToAssembly(code); break;
        }
        code = code->next;
    }
}