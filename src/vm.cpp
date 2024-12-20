#include "vm.hpp"
#include "ops.hpp"
#include <iostream>

// Construtor da classe VirtualMachine
VirtualMachine::VirtualMachine() {}

// Carregar instruções na memória da máquina virtual e verifica se a primeira instrucao é START
void VirtualMachine::startVM(const std::vector<Instruction>& instructions) {
    this->instructionMemory = instructions;

    // setar os registradores
    this->start_sp = -1;
    this->sp = start_sp;
    this->ip = 0;

    // verifica se o programa comeca com a instrucao START
    if (this->instructionMemory[0].operation != "START"){
        throw std::invalid_argument("O programa não começa com START");
    }
    this->ip++;

    // inicializa as instrucoes
    Operations::initialize();

    // cria o label cache (label->endereco)
    for (int i=0; i<this->instructionMemory.size(); i++){
        const auto& instr = this->instructionMemory[i];
        
        if (instr.label){
            this->label_cache[instr.label.value()] = i;
        }
    }
}

// Executar as instruções carregadas
void VirtualMachine::execute(ReadFn readFn, WriteFn writeFn) {
    this->readFn = readFn;
    this->writeFn = writeFn;

    while (this->ip < this->instructionMemory.size()) {
        const auto& instr = this->instructionMemory[this->ip];
        this->ip++; // Avançar o Instruction Pointer

        if (!instr.operation.empty()) {
            // std::cout << "Executando operação: " << instr.operation << std::endl;
            if (instr.operation == "HLT"){
                break;
            } else if (instr.operation == "DEBUG"){
                this->debug();
                continue;
            }

            Operation op = Operations::getOperation(instr.operation);
            
            // operadores string
            if (instr.operation == "JMP" || instr.operation == "JMPF" || instr.operation == "CALL"){
                op(*this, instr.op1.value(), instr.op2.value_or(""));
            }
            else { // operadores int
                int op1, op2;
                if (instr.op1) op1 = std::stoi(instr.op1.value());
                if (instr.op2) op2 = std::stoi(instr.op2.value());

                op(*this, op1, op2);
            }

        } else if (instr.label) {
            // std::cout << "Executando Label: " << *instr.label << std::endl;
        }
    }
}

// Função para depurar o estado interno da máquina virtual
void VirtualMachine::debug() {
    std::cout << "=== Estado da Máquina Virtual ===\n";
    std::cout << "Instruction Pointer: " << ip << "\n";
    std::cout << "Stack Pointer: " << sp << "\n";
    std::cout << "Stack Memory: ";
    std::array<int, 8192> tempStack = this->stack;
    int tempSp = this->sp;
    while (tempSp >= 0) {
        std::cout << tempStack[tempSp] << " ";
        tempSp--;
    }
    std::cout << "\n";

    auto currInst = this->instructionMemory[this->ip];
    std::cout << currInst.label.value_or("") << " " << currInst.operation << " " << currInst.op1.value_or("") << "," << currInst.op2.value_or("");
    
}
