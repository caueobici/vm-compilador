#ifndef OPS_HPP
#define OPS_HPP

#include <string>
#include <stack>
#include <unordered_map>
#include <variant>

class VirtualMachine;

using Operand = std::variant<int, std::string>;

typedef void (*Operation)(VirtualMachine &vm, const Operand &op1, const Operand &op2);

class Operations
{
public:
    static void initialize();
    static Operation getOperation(const std::string &name);

private:
    static std::unordered_map<std::string, Operation> operations;

    // Declaração das operações suportadas
    static void opLDC(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opLDV(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opADD(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opSUB(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opMULT(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opDIVI(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opINV(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opAND(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opOR(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opNEG(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opCME(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opCMA(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opCEQ(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opCDIF(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opCMEQ(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opCMAQ(VirtualMachine &vm, Operand& op1, Operand& op2);
    static void opJMP(VirtualMachine& vm, Operand& op1, Operand& op2);
    static void opJMPF(VirtualMachine& vm, Operand& op1, Operand& op2);
    static void opALLOC(VirtualMachine& vm, Operand& op1, Operand& op2);
    static void opDALLOC(VirtualMachine& vm, Operand& op1, Operand& op2);
    static void opRD(VirtualMachine& vm, Operand& op1, Operand& op2);
    static void opPRN(VirtualMachine& vm, Operand& op1, Operand& op2);
    static void opSTR(VirtualMachine& vm, Operand& op1, Operand& op2);
    static void opCALL(VirtualMachine& vm, Operand& op1, Operand& op2);
    static void opRETURN(VirtualMachine& vm, Operand& op1, Operand& op2);
};

#endif