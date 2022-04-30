#include <bits/stdc++.h>

char* getGlobalVar(int offset);
void loadaiToAssembly(struct instr* instr1);
char* getRegisterName(int id);
void loadiToAssembly(struct instr* instr1);
void cmpToAssembly(struct instr* instr1);
void cbrToAssembly(struct instr* instr1);
void binaryOpToAssembly(struct instr* instr1, const char* op);
void divToAssembly();
struct instr* addiToAssembly(struct instr* instr1);
void rsubiToAssembly(struct instr* instr1);
void initFunc(struct instr* instr1);
void labelToAssembly(struct instr* instr1);
struct instr* storeaiToAssembly(struct instr* instr1);
void jumpiToAssembly(struct instr* instr1);
void i2iToAssembly(struct instr* instr1);
void mainEnd();
void printVarsGlobais();
void printAssembly(void* tree);