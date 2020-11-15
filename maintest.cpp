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
int generations = 1000;
int populationSize = 3;
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

    // Algoritmo genetico
    while (generations > 0) {
        // crossover
        crossover(population, bestFitIndex, bestFitPopRatio);
        cout << "Geracao " << generations << ": " << findBestFitCost() << endl;
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
    int cost = 0;
    for (int i = 0; i < populationSize; i++) {
        if(population[i].size() > cost)
            cost = population[i].size();
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
    if (pos1.x == pos2.x && pos1.y == pos2.y) return 1;
    return 0;
}

// Acha um vector de interseccao entre dois caminhos de formigas diferentes
vector<int> intersection(vector<pos> bestFit, vector<pos> random) {
    vector<int> inCommon;

    for (int i = 0; i < bestFit.size(); i++) {
        for (int j = 0; j < random.size(); j++) {
            if (compareTwoPos(bestFit.at(i), random.at(j))) {
                inCommon.push_back(i);
                break;
            }
        }
    }

    return inCommon;
}

// Faz o cruzamento do individuo "index" com algum aleatorio da populacao
// e cria "popRatio" novos filhos com esse cruzamento
void crossover(vector<pos>* population, int index, int popRatio) {
    // Pega um individuo qualquer que nao seja o melhor
    // e cruza os dois para gerar um novo individuo
    uniform_int_distribution<int> randomNumber(0, populationSize - 1);
    int randomChoosen = index;
    while (randomChoosen == index) randomChoosen = randomNumber(gen);

    vector<int> intersec =
        intersection(population[index], population[randomChoosen]);

    // Resultado do crossover
    // Inicialmente, o 0 tem o mesmo valor do melhor
    vector<pos> children[popRatio];
    for (int i = 0; i < popRatio; i++) {
        for (int j = 0; j < population[index].size(); j++)
            children[i].push_back(population[index].at(j));
    }

    // Gera um conjunto de "populationSize/2" tamanho
    // de novos filhos dos dois individuos escolhidos
    int auxCG = 0;
    for (int it = 0; it < popRatio; it++) {
        for (int i = 0; i < intersec.size(); i++) {
            uniform_int_distribution<int> randomNumber01(0, 1);
            int changeGene = randomNumber01(gen);
            if (changeGene) {
                for (int j = auxCG; j < intersec[auxCG]; j++) {
                    children[it].push_back(population[randomChoosen].at(j));
                }
            }
            auxCG = intersec[i];
        }
    }

    // Atualiza o vetor de posicoes da populacao anterior
    // ou seja, atualiza a nova geracao de individuos
    for (int i = 0; i < popRatio; i++) {
        population[i] = children[i];
    }
    return;
}