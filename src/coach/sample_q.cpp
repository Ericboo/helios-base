#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <string.h>

//Definição da estrutura de dados para a tabela Q
std::unordered_map<std::string, std::unordered_map<std::string, double>> q_table;
//typedef std::vector< const rcsc::PlayerType * > PlayerTypePtrCont;

// Função para gerar todas as combinações entre os números de uniforme e as características
std::vector<std::vector<int>> generateCombinations(const std::vector<int>& unum, int hetero) {
    // Inicializar a matriz com zeros
    std::vector<std::vector<int>> q_matrix(unum.size(), std::vector<int>(hetero, 0));

    // Preencher a matriz com combinações
    for (int i = 0; i < unum.size(); ++i) {
        // Para cada número de uniforme, definir a característica de ser o mais rápido (velocidade) como 1
        q_matrix[i][0] = 1; // índice 0 representa a característica de ser o mais rápido

        // Para cada número de uniforme, definir a característica de ter a maior área de chute como 1
        q_matrix[i][1] = 1; // índice 1 representa a característica de ter a maior área de chute
    }

    return q_matrix;
}

// void print_q(int q_table[10][2]) {
//     for(const auto &jogador : q_table) {
//         for (const auto &carcateristica : jogador) {
//             std::cout <<  carcateristica; 
//         }
//         std::cout << std::endl;
//     }
// }

std::vector<std::vector<int>>  initialize_table() { //PlayerTypePtrCont candidates
    
    int q_table[10][2];
    std::vector<std::vector<int>> q_std_table;
    std::vector<int> ordered_unum = {11, 2, 3, 10, 9, 6, 4, 5, 7, 8};
    int num_characteristics = 2; // 0 - Velocidade, 1 - K area
    std::string path = "/home/eric/tcc/times/ibots/src/coach/qMATRIX.txt";
    FILE* file = fopen(path.c_str(), "r+");
    fseek(file, 0, SEEK_END);
    long size = ftell(file);


    if (size == 0) {//Se não possui arquivo da matriz Q

        // Gerar todas as combinações de jogadores
        std::vector<std::vector<int>> combinations = generateCombinations(ordered_unum, num_characteristics);
        // Preencher a matriz Q com combinações
        fprintf(file, "UNUM | Velocidade | K area\n");
        int count = 0;
        for (const auto& combination : combinations) {
            fprintf(file, " %d  |", ordered_unum[count]);
            count += 1;
            int col_comb = 0;
            for (const auto &q_value : combination) {
                if (col_comb == 0) {
                    fprintf(file, "      %d    |", q_value);
                    col_comb += 1;
                } else {
                    fprintf(file, "    %d \n", q_value);
                }
            }
            
        }

    } 

    fseek(file, 0, SEEK_SET);
    int index = 0;
    int subIndex = 0;
    char last[256] = "";
    char file_content[256];
    while(!feof(file)) {
        fscanf(file, "%s", &file_content);
        if(strcmp(file_content, "11") == 0) {
            while(!feof(file)) {
                fscanf(file, "%s", &file_content);
                if (strcmp(file_content, "|") == 0) {
                    last[0] = '|';
                    index +=  1;
                    continue;
                } 

                if (last[0] != '|') {
                    subIndex += 1;
                    index = 0;
                    last[0] = '|';
                    continue;
                }
                int val = atoi(file_content);
                q_table[subIndex][index - 1] = val;
                strcpy(last, file_content);
            }
            break;
        }
        
    }

    index = 0;
    for(const auto &jogador : q_table) {

        std::vector<int> row;
        row.push_back(q_table[index][0]);
        row.push_back(q_table[index][1]);
        q_std_table.push_back(row);
        index += 1;
         
    }

    return q_std_table;

}

// Função para encontrar o índice do maior elemento em um vetor
int findIndexOfMax(const std::vector<int>& vec) {
    if (vec.empty()) {
        // Se o vetor estiver vazio, retorna -1 ou lança uma exceção, dependendo do contexto
        return -1;
    }

    int maxIndex = 0; // Assume que o maior elemento está inicialmente no índice 0

    for (size_t i = 1; i < vec.size(); ++i) {
        if (vec[i] > vec[maxIndex]) {
            maxIndex = i; // Atualiza o índice do maior elemento encontrado
        }
    }

    return maxIndex;
}