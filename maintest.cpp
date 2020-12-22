#include <bits/stdc++.h>
using namespace std;

// Configura a seed de numeros aleatorios
std::random_device rd;
std::mt19937 gen(rd());

// Estrutura de um ponto no mapa
typedef struct _pos {
    int x, y;
} pos;

// Configuracoes do mapa/programa
int** mapMatrix;
int objX = 4, objY = 4;
int mapSize = 5;

// Configuracoes do algoritmo genetico
int generations = 1;
int populationSize = 2;
vector<pos>* population = new vector<pos>[populationSize];
int bestFitIndex = 0;
int bestFitPopRatio = ceil(1 * populationSize);

// Definicao das funcoes utilizadas
void printPopulation();
int findBestFitCost();
void buildMap();
pos randomPos(pos currPos);
void defaultPos(vector<pos>* population);
int initialPopulation(vector<pos>* population);
int compareTwoPos(pos pos1, pos pos2);
vector<int> intersection(vector<pos> bestFit, vector<pos> random);
void crossover(vector<pos>* population, int index, int popRatio);

// ----- ----- ----- ----- MAIN ----- ----- ----- ----- ----- //

int main() {
    // Mapa por onde as formigas podem andar
    // 0 - pode andar
    // 1 - objetivo
    // 2 - obstaculo
    buildMap();

    // Posicoes default das formigas
    // todas comecam no ponto (0,0)
    defaultPos(population);

    // Construindo o caminho aleatorio de cada formiga
    bestFitIndex = initialPopulation(population);
    cout << "Initial population: " << findBestFitCost() << endl;
    printPopulation();

    // Algoritmo genetico
    cout << "After population: " << findBestFitCost() << endl;
    while (generations > 0) {
        // crossover
        crossover(population, bestFitIndex, bestFitPopRatio);
        cout << "Geracao " << generations << ": " << findBestFitCost() << endl;
        printPopulation();
        // mutacao

        // nova geracao
        generations--;
    }

    return 0;
}

// ----- ----- ----- ----- FUNCOES ----- ----- ----- ----- ----- //

// Funcao para teste: printa a populacao atual
void printPopulation() {
    for (int i = 0; i < populationSize; i++) {
        for (int j = 0; j < population[i].size(); j++) {
            cout << "(" << population[i].at(j).x << "," << population[i].at(j).y
                 << ") ";
        }
        cout << endl << endl << endl;
    }
}

// Funcao para teste: printa o custo do melhor individuo
int findBestFitCost() {
    int cost = 999999;
    for (int i = 0; i < populationSize; i++) {
        if (population[i].size() < cost) cost = population[i].size();
    }
    return cost;
}

// Funcao que aloca e constroi o mapa no qual o algoritmo roda
void buildMap() {
    mapMatrix = (int**)malloc(mapSize * sizeof(int*));
    for (int i = 0; i < mapSize; i++) {
        mapMatrix[i] = (int*)malloc(mapSize * sizeof(int));

        for (int j = 0; j < mapSize; j++) {
            if (i == objX && j == objY)
                mapMatrix[i][j] = 1;  // Objetivo no mapa
            else
                mapMatrix[i][j] = 0;  // Caminho possivel
        }
    }
}

// Faz um movimento aleatorio para uma dada
// formiga de entrada e retorna a nova posicao
pos randomPos(pos currPos) {
    uniform_int_distribution<int> randomNumber(0, 1);
    pos newPos;
    newPos.x = currPos.x;
    newPos.y = currPos.y;

    while (1) {
        int newX = randomNumber(gen);
        int newY = randomNumber(gen);
        if (newX == 0) newX = -1;
        if (newY == 0) newY = -1;

        newPos.x += newX;
        newPos.y += newY;

        // Caso encontre uma posicao nova valida, retorne ela
        if (newPos.x >= 0 && newPos.x < mapSize && newPos.y >= 0 &&
            newPos.y < mapSize) {
            if (mapMatrix[newPos.x][newPos.y] != 2) break;
        }

        newPos.x -= newX;
        newPos.y -= newY;
    }

    return newPos;
}

// Define as posicoes iniciais default das formigas no mapa
void defaultPos(vector<pos>* population) {
    pos defaultPos;
    defaultPos.x = defaultPos.y = 0;
    for (int i = 0; i < populationSize; i++)
        population[i].push_back(defaultPos);
}

// Define um caminho aleatorio para cada formiga inicialmente
// output: retorna o index do melhor individuo gerado
int initialPopulation(vector<pos>* population) {
    for (int i = 0; i < populationSize; i++) {
        int j = 0;
        while (population[i].at(j).x != objX || population[i].at(j).y != objY) {
            population[i].push_back(randomPos(population[i].at(j)));
            j++;
        }

        // Atualiza o melhor caminho encontrado
        if (population[bestFitIndex].size() > population[i].size())
            bestFitIndex = i;
    }

    return bestFitIndex;
}

// Compara se duas posicoes no mapa sao iguais
int compareTwoPos(pos pos1, pos pos2) {
    // printf("(%d,%d) == (%d,%d)?\n", pos1.x, pos1.y, pos2.x, pos2.y);
    if ((pos1.x == pos2.x) && (pos1.y == pos2.y)) return 1;
    return 0;
}

// Acha um vector de interseccao entre dois caminhos de formigas diferentes
vector<int> intersection(vector<pos> bestFit, vector<pos> random) {
    vector<int> inCommon;

    for (int i = 0; i < bestFit.size(); i++) {
        if (compareTwoPos(bestFit[i], random[i])) inCommon.push_back(i);
    }

    // caso nao tenha adicionado o objetivo
    if (inCommon.back() != bestFit.size() - 1)
        inCommon.push_back(bestFit.size() - 1);

    /* for(int i = 0; i < inCommon.size(); i++)
        cout << inCommon[i] << " ";
    cout << endl; */
    return inCommon;
}

// Faz o cruzamento do individuo "bestFit" com algum aleatorio da populacao
// e cria "popRatio" novos filhos com esse cruzamento
void crossover(vector<pos>* population, int bestFit, int popRatio) {
    // Pega um individuo qualquer que nao seja o melhor
    // e cruza os dois para gerar um novo individuo
    uniform_int_distribution<int> randomNumber(0, populationSize - 1);
    int randomChoosen = bestFit;
    while (randomChoosen == bestFit) randomChoosen = randomNumber(gen);

    vector<int> intersec =
        intersection(population[bestFit], population[randomChoosen]);

    // Resultado do crossover
    // Inicialmente, as criancas tem o mesmo
    // caminho percorrido do melhor
    vector<pos> children[popRatio];

    // Gera um conjunto de "popRatio" tamanho
    // de novos filhos dos dois individuos escolhidos
    for (int iChild = 0; iChild < popRatio; iChild++) {
        int auxCG = 0;
        for (int iIntersec = 0; iIntersec < intersec.size()-1; iIntersec++) {
            // Roda um numero aleatorio para saber com qual
            // faixa do caminho (do pai ou da mae) o filho vai ficar
            uniform_int_distribution<int> randomNumber01(0, 1);
            int changeGene = randomNumber01(gen);

            if (changeGene) {
                // Usa o gene do random
                while(1) {
                    if (population[randomChoosen].at(auxCG).x == objX &&
                        population[randomChoosen].at(auxCG).y == objY) {
                        children[iChild].push_back(population[randomChoosen].at(auxCG));       
                        break;
                    }

                    if (auxCG == intersec[iIntersec+1]) {
                        if(auxCG == intersec.back());
                        else break;
                    }

                    children[iChild].push_back(population[randomChoosen].at(auxCG));
                    auxCG++;
                }
            } else {
                // Usa o gene do melhor
                while(1) {
                    if (population[bestFit].at(auxCG).x == objX &&
                        population[bestFit].at(auxCG).y == objY) {
                        children[iChild].push_back(population[bestFit].at(auxCG));
                        break;
                    }

                    if (auxCG == intersec[iIntersec+1]) break;

                    children[iChild].push_back(population[bestFit].at(auxCG));
                    auxCG++;
                }
            }
        }
        // adiciona a ultima posicao (o objetivo)
        // na proxima geracao
        //children[iChild].push_back(population[bestFit].back());
    }

    

    // Atualiza o vetor de posicoes da populacao anterior
    // ou seja, atualiza a nova geracao de individuos
    for (int i = 0; i < popRatio; i++) {
        population[i] = children[i];
    }
    return;
}