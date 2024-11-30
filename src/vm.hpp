#include <optional>
#include <unordered_map>
#include <string>
#include <vector>
#include <array>

#pragma once

// Estrutura para armazenar uma instrução
struct Instruction {
    std::optional<std::string> label;
    std::string operation;
    std::optional<std::string> op1;
    std::optional<std::string> op2;
};

#ifndef VM_HPP
#define VM_HPP

class VirtualMachine {
private:
    std::vector<Instruction> instructionMemory; // Memória para armazenar as instruções

public:
    VirtualMachine();
    
    std::unordered_map<std::string, int> label_cache;
    std::array<int, 8192> stack = {}; // Stack

    int ip; // Instruction Pointer
    int sp; // Stack Pointer
    int start_sp;

    void startVM(const std::vector<Instruction>& instructions);
    void execute();
    void debug(); // Para depuração e visualização do estado interno
    
};

#endif // VM_HPP