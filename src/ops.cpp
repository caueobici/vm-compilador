#include "ops.hpp"
#include "vm.hpp"
#include <iostream>
#include <stdexcept>

std::unordered_map<std::string, Operation> Operations::operations;

void Operations::initialize() {
    operations["LDC"] = reinterpret_cast<Operation>(opLDC);
    operations["LDV"] = reinterpret_cast<Operation>(opLDV);
    operations["ADD"] = reinterpret_cast<Operation>(opADD);
    operations["SUB"] = reinterpret_cast<Operation>(opSUB);
    operations["MULT"] = reinterpret_cast<Operation>(opMULT);
    operations["DIVI"] = reinterpret_cast<Operation>(opDIVI);
    operations["INV"] = reinterpret_cast<Operation>(opINV);
    operations["AND"] = reinterpret_cast<Operation>(opAND);
    operations["OR"] = reinterpret_cast<Operation>(opOR);
    operations["NEG"] = reinterpret_cast<Operation>(opNEG);
    operations["CME"] = reinterpret_cast<Operation>(opCME);
    operations["CMA"] = reinterpret_cast<Operation>(opCMA);
    operations["CEQ"] = reinterpret_cast<Operation>(opCEQ);
    operations["CDIF"] = reinterpret_cast<Operation>(opCDIF);
    operations["CMEQ"] = reinterpret_cast<Operation>(opCMEQ);
    operations["CMAQ"] = reinterpret_cast<Operation>(opCMAQ);
    operations["JMP"] = reinterpret_cast<Operation>(opJMP);
    operations["JMPF"] = reinterpret_cast<Operation>(opJMPF);
    operations["ALLOC"] = reinterpret_cast<Operation>(opALLOC);
    operations["DALLOC"] = reinterpret_cast<Operation>(opDALLOC);
    operations["RD"] = reinterpret_cast<Operation>(opRD);
    operations["PRN"] = reinterpret_cast<Operation>(opPRN);
    operations["STR"] = reinterpret_cast<Operation>(opSTR);
    operations["CALL"] = reinterpret_cast<Operation>(opCALL);
    operations["RETURN"] = reinterpret_cast<Operation>(opRETURN);
}

Operation Operations::getOperation(const std::string& name) {
    auto it = operations.find(name);
    if (it != operations.end()) {
        return it->second;
    }
    throw std::invalid_argument("Operação desconhecida: " + name);
}

void Operations::opLDC(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Carregar constante
    if (std::holds_alternative<int>(op1)) {
        int k = std::get<int>(op1);
        vm.sp++;
        vm.stack[vm.sp] = k;
    } else {
        throw std::runtime_error("LDC de uma constante inteira");
    }
}

void Operations::opLDV(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Carregar valor de um endereço
    if (std::holds_alternative<int>(op1)) {
        int n = std::get<int>(op1);
        vm.sp++;
        vm.stack[vm.sp] = vm.stack[n];
    } else {
        throw std::runtime_error("LDV precisa de um endereço inteiro");
    }
}

void Operations::opADD(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Somar os dois últimos valores da pilha
    if (vm.sp <= vm.start_sp+1) {
        throw std::runtime_error("Valores insuficientes na stack ADD");
    }
    vm.stack[vm.sp - 1] += vm.stack[vm.sp];
    vm.sp--;
}

void Operations::opSUB(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Subtrair os dois últimos valores da pilha
    if (vm.sp <= vm.start_sp+1) {
        throw std::runtime_error("Valores insuficientes na stack SUB");
    }
    vm.stack[vm.sp - 1] -= vm.stack[vm.sp];
    vm.sp--;
}

void Operations::opMULT(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Multiplicar os dois últimos valores da pilha
    if (vm.sp <= vm.start_sp+1) {
        throw std::runtime_error("Valores insuficientes na stack MULT");
    }
    vm.stack[vm.sp - 1] *= vm.stack[vm.sp];
    vm.sp--;
}

void Operations::opDIVI(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Divisão inteira dos dois últimos valores da pilha
    if (vm.sp <= vm.start_sp+1) {
        throw std::runtime_error("Valores insuficientes na stack DIVI");
    }
    if (vm.stack[vm.sp] == 0) {
        throw std::runtime_error("Divisao por 0");
    }
    vm.stack[vm.sp - 1] /= vm.stack[vm.sp];
    vm.sp--;
}

void Operations::opINV(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Inverter sinal do último valor da pilha
    vm.stack[vm.sp] = -vm.stack[vm.sp];
}

void Operations::opAND(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Conjunção lógica (considerando 0 como falso, qualquer outro valor como verdadeiro)
    if (vm.sp <= vm.start_sp+1) {
        throw std::runtime_error("Valores insuficientes na stack AND");
    }
    vm.stack[vm.sp - 1] = (vm.stack[vm.sp - 1] == 1 && vm.stack[vm.sp] == 1) ? 1 : 0;
    vm.sp--;
}

void Operations::opOR(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Disjunção lógica
    if (vm.sp <= vm.start_sp+1) {
        throw std::runtime_error("Valores insuficientes na stack OR");
    }
    vm.stack[vm.sp - 1] = (vm.stack[vm.sp - 1] == 1 || vm.stack[vm.sp] == 1) ? 1 : 0;
    vm.sp--;
}

void Operations::opNEG(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Negação bit
    vm.stack[vm.sp] = 1 - vm.stack[vm.sp];
}

void Operations::opCME(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Comparar menor
    if (vm.sp <= vm.start_sp+1) {
        throw std::runtime_error("Valores insuficientes na stack CME");
    }
    vm.stack[vm.sp - 1] = (vm.stack[vm.sp - 1] < vm.stack[vm.sp]) ? 1 : 0;
    vm.sp--;
}

void Operations::opCMA(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Comparar maior
    if (vm.sp <= vm.start_sp+1) {
        throw std::runtime_error("Valores insuficientes na stack CMA");
    }
    vm.stack[vm.sp - 1] = (vm.stack[vm.sp - 1] > vm.stack[vm.sp]) ? 1 : 0;
    vm.sp--;
}

void Operations::opCEQ(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Comparar igualdade
    if (vm.sp <= vm.start_sp+1) {
        throw std::runtime_error("Valores insuficientes na stack CEQ");
    }
    vm.stack[vm.sp - 1] = (vm.stack[vm.sp - 1] == vm.stack[vm.sp]) ? 1 : 0;
    vm.sp--;
}

void Operations::opCDIF(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Comparar desigualdade
    if (vm.sp <= vm.start_sp+1) {
        throw std::runtime_error("Valores insuficientes na stack CDIF");
    }
    vm.stack[vm.sp - 1] = (vm.stack[vm.sp - 1] != vm.stack[vm.sp]) ? 1 : 0;
    vm.sp--;
}

void Operations::opCMEQ(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Comparar menor ou igual
    if (vm.sp <= vm.start_sp+1) {
        throw std::runtime_error("Valores insuficientes na stack CMEQ");
    }
    vm.stack[vm.sp - 1] = (vm.stack[vm.sp - 1] <= vm.stack[vm.sp]) ? 1 : 0;
    vm.sp--;
}

void Operations::opCMAQ(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Comparar maior ou igual
    if (vm.sp <= vm.start_sp+1) {
        throw std::runtime_error("Valores insuficientes na stack CMAQ");
    }
    vm.stack[vm.sp - 1] = (vm.stack[vm.sp - 1] >= vm.stack[vm.sp]) ? 1 : 0;
    vm.sp--;
}


void Operations::opJMP(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Desviar sempre para o endereço especificado
    if (std::holds_alternative<std::string>(op1)) {
        std::string label = std::get<std::string>(op1);
        int p = vm.label_cache[label];

        vm.ip = p;
    } else {
        throw std::runtime_error("JMP precisa de um label");
    }
}

void Operations::opJMPF(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Desviar se o topo da pilha for falso (0)
    if (std::holds_alternative<std::string>(op1)) {
        std::string label = std::get<std::string>(op1);
        
        int p = vm.label_cache[label];

        // Verifica se há valores na pilha
        if (vm.sp <= vm.start_sp) {
            throw std::runtime_error("Stack esta vazia para o JMPF");
        }
        
        // Se o topo da pilha for 0 (falso), desvia para p
        if (vm.stack[vm.sp] == 0) {
            vm.ip = p;
        } // Caso contrário, nao faça nada - o ip já irá ter sido acrescentado
                
        // Reduz o stack pointer
        vm.sp--;
    } else {
        throw std::runtime_error("JMPF precisa de uma label");
    }
}


void Operations::opALLOC(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Verifica se os operandos são inteiros
    if (!std::holds_alternative<int>(op1) || !std::holds_alternative<int>(op2)) {
        throw std::runtime_error("ALLOC precisa de 2 argumentos inteiros");
    }

    int m = std::get<int>(op1);  // Endereço base
    int n = std::get<int>(op2);  // Número de elementos a alocar

    // Aloca n elementos a partir do endereço m
    for (int k = 0; k < n; k++) {
        // Incrementa o stack pointer
        vm.sp++;
        
        // Copia o valor de M[m+k] para o topo da pilha
        vm.stack[vm.sp] = vm.stack[m + k];
    }
}

void Operations::opDALLOC(VirtualMachine& vm, Operand& op1, Operand& op2) {
    // Verifica se os operandos são inteiros
    if (!std::holds_alternative<int>(op1) || !std::holds_alternative<int>(op2)) {
        throw std::runtime_error("DALLOC precisa de 2 argumentos inteiros");
    }

    int m = std::get<int>(op1);  // Endereço base
    int n = std::get<int>(op2);  // Número de elementos a desalocar

    // Desaloca n elementos a partir do endereço m
    for (int k = n - 1; k >= 0; k--) {
        // Copia o topo da pilha para M[m+k]
        vm.stack[m + k] = vm.stack[vm.sp];
        
        // Reduz o stack pointer
        vm.sp--;
    }
}

void Operations::opRD(VirtualMachine& vm, Operand& op1, Operand& op2) {
    int var = vm.readFn();

    vm.sp++;
    vm.stack[vm.sp] = var;
}

void Operations::opPRN(VirtualMachine& vm, Operand& op1, Operand& op2) {
    vm.writeFn(vm.stack[vm.sp]);
    vm.sp--;
}

void Operations::opSTR(VirtualMachine& vm, Operand& op1, Operand& op2) {
    
    if (!std::holds_alternative<int>(op1)) {
        throw std::runtime_error("STR precisa de um endereço inteiro");
    }
    
    int n = std::get<int>(op1);
    vm.stack[n] = vm.stack[vm.sp];;
    vm.sp--;
}

void Operations::opCALL(VirtualMachine& vm, Operand& op1, Operand& op2) {
    if (std::holds_alternative<std::string>(op1)) {
        std::string label = std::get<std::string>(op1);
        
        int p = vm.label_cache[label];
        vm.sp++;

        vm.stack[vm.sp] = vm.ip; // (ip++ ja foi feito)
        
        vm.ip = p;

    } else {
        throw std::runtime_error("CALL precisa de uma label");
    }
}

void Operations::opRETURN(VirtualMachine& vm, Operand& op1, Operand& op2) {
    vm.ip = vm.stack[vm.sp];
    vm.sp--;
}