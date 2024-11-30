#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <optional>
#include "vm.hpp"

// Função para ler o conteúdo de um arquivo e retorná-lo como string
std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        throw std::runtime_error("Não foi possível abrir o arquivo: " + filePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Função para fazer o parsing do conteúdo do arquivo em uma lista de instruções
std::vector<Instruction> parseInstructions(const std::string& fileContent) {
    std::vector<Instruction> instructions;
    std::istringstream stream(fileContent);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.empty()) continue; // Ignora linhas vazias

        std::istringstream lineStream(line);
        std::string word1, word2;

        lineStream >> word1 >> word2;

        if (word2 == "NULL") {
            // Linha representa um label
            instructions.push_back({word1, "", std::nullopt, std::nullopt});
        } else {
            // Linha representa uma instrução
            std::optional<std::string> op1 = std::nullopt, op2 = std::nullopt;

            size_t commaPos = word2.find(',');
            if (commaPos != std::string::npos) {
                op1 = word2.substr(0, commaPos);
                op2 = word2.substr(commaPos + 1);
                // Remove espaços extras
                if (op1) *op1 = std::string(op1->begin() + (op1->front() == ' ' ? 1 : 0), op1->end());
                if (op2) *op2 = std::string(op2->begin() + (op2->front() == ' ' ? 1 : 0), op2->end());
            } else if (!word2.empty()) {
                op1 = word2;
            }

            instructions.push_back({
                std::nullopt, 
                word1, 
                op1, 
                op2
            });
        }
    }

    return instructions;
}

void debugInstructions(const std::vector<Instruction>& instructions) {
    std::cout << "Debugging Instructions:\n";
    for (const auto& instr : instructions) {
        if (instr.label) {
            std::cout << "Label: " << *instr.label << "; ";
        }
        std::cout << "Operation: " << instr.operation << "; ";
        if (instr.op1) {
            std::cout << "OP1: " << *instr.op1 << "; ";
        }
        if (instr.op2) {
            std::cout << "OP2: " << *instr.op2 << "; ";
        }
        std::cout << "\n---\n";
    }
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <caminho_do_arquivo>\n";
        return 1;
    }

    try {
        // Passo 1: Ler o arquivo
        std::string fileContent = readFile(argv[1]);

        // Passo 2: Fazer o parsing das instruções
        std::vector<Instruction> instructions = parseInstructions(fileContent);


        // Passo 3: iniciar a VM
        VirtualMachine* vm = new VirtualMachine();
        vm->startVM(instructions);

        // Passo 4: executar o programa
        vm->execute();
        

    } catch (const std::exception& ex) {
        std::cerr << "Erro: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
