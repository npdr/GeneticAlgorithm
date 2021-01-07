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
int generations = 10;
int populationSize = 100;
vector<pos>* population = new vector<pos>[populationSize];
int bestFitIndex = 0, secondFitIndex = 0;
int bestFitPopRatio = ceil(1 * populationSize);

// Definicao das funcoes utilizadas
void printPopulation();
int findBestFitIndex();
int findSecondFitIndex();
void buildMap();
pos randomPos(pos currPos);
void defaultPos(vector<pos>* population);
int initialPopulation(vector<pos>* population);
int compareTwoPos(pos pos1, pos pos2);
vector<int> intersection(vector<pos> bestFit, vector<pos> random);
void crossover(vector<pos>* population, int best, int second, int popRatio);
void fitness(vector<pos>* population, int index);
void mutate(vector<pos>* population);
vector<pos> mutateUtil(vector<pos>* population, vector<pos> currPath);
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
    cout << "Iniciando população... " << endl;
    
    // Algoritmo genetico
    while (generations > 0) {
        cout << "INICIAL... " << endl;
        //printPopulation();

        // fitness function
        cout << "FITNESS... " << endl;
        fitness(population, bestFitIndex);
        bestFitIndex = findBestFitIndex();
        secondFitIndex = findSecondFitIndex();
        cout << "best, second: " << population[bestFitIndex].size() << ", " << population[secondFitIndex].size() << endl;
        //printPopulation();

        // crossover
        cout << "CROSSOVER... " << endl;
        crossover(population, bestFitIndex, secondFitIndex, bestFitPopRatio);
        //printPopulation();
        
        // mutacao
        cout << "MUTACAO... " << endl;
        mutate(population);
        //printPopulation();

        cout << "GERACAO " << generations << "... " << endl << endl << endl;
        generations--;
    }

    for(int i = 0; i < population[bestFitIndex].size(); i++)
        cout << "(" << population[bestFitIndex].at(i).x << ", " << population[bestFitIndex].at(i).y << ") ";
    cout << endl; 
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

// Encontra o custo do melhor individuo
int findBestFitIndex() {
    int bestIndex, bestSize = 9999;
    for (int i = 0; i < populationSize; i++) {
        if(population[i].size() == 1) continue; // individuo deletado pelo fitness
        if (population[i].size() < bestSize) {
            bestIndex = i;
            bestSize = population[bestIndex].size();
        }
    }
    return bestIndex;
}

// Encontra o custo do segundo melhor individuo
int findSecondFitIndex() {
    int secondIndex, secondSize = 9999;
    int best = findBestFitIndex();

    for (int i = 0; i < populationSize; i++) {
        if(i == best || population[i].size() == 1) continue; // individuo deletado pelo fitness
        if (population[i].size() < secondSize) {
            secondIndex = i;
            secondSize = population[secondIndex].size();
        }
    }
    return secondIndex;
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
void crossover(vector<pos>* population, int bestIndex, int secondIndex, int popRatio) {
    vector<int> intersec =
        intersection(population[bestIndex], population[secondIndex]);

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
                // Usa o gene do segundo
                while(1) {
                    if (population[secondIndex].at(auxCG).x == objX &&
                        population[secondIndex].at(auxCG).y == objY) {
                        children[iChild].push_back(population[secondIndex].at(auxCG));       
                        break;
                    }

                    if (auxCG == intersec[iIntersec+1]) {
                        if(auxCG == intersec.back());
                        else break;
                    }

                    children[iChild].push_back(population[secondIndex].at(auxCG));
                    auxCG++;
                }
            } else {
                // Usa o gene do melhor
                while(1) {
                    if (population[bestIndex].at(auxCG).x == objX &&
                        population[bestIndex].at(auxCG).y == objY) {
                        children[iChild].push_back(population[bestIndex].at(auxCG));
                        break;
                    }

                    if (auxCG == intersec[iIntersec+1]) break;

                    children[iChild].push_back(population[bestIndex].at(auxCG));
                    auxCG++;
                }
            }
        }
    }

    // Atualiza o vetor de posicoes da populacao anterior
    // ou seja, atualiza a nova geracao de individuos
    for (int i = 0; i < popRatio; i++) {
        population[i] = children[i];
    }
}

// Faz a mutacao de parte do caminho atual
// a intençao é simular uma mutacao verdadeira
// modificando parte do gene do individuo
void mutate(vector<pos>* population) {
    for(int i = 0; i < populationSize; i++) {
        if(i == findBestFitIndex()) continue;
        //cout << "i: " << i << " size: " << population[i].size() << endl;
        population[i] = mutateUtil(population, population[i]);        
    }
}

// Função de utilidade que faz
// a mutação do caminho de um
// individuo escolhido
vector<pos> mutateUtil(vector<pos>* population, vector<pos> currPath) {
    pos defaultPos;
    defaultPos.x = defaultPos.y = 0;
    vector<pos> newPath;
    newPath.push_back(defaultPos);

    // escolhe até onde vai mutar
    // no caminho do individuo
    int maxRange = floor(currPath.size()*0.1);
    if(maxRange == 0) return currPath; // caso encontre um individuo exterminado pelo fitness

    uniform_int_distribution<int> randomGene(1, maxRange);
    int mutationSize = randomGene(gen);

    // mutacao
    pos lastPos = currPath[mutationSize];

    int mutationIt = 0;
    //cout << mutationSize << ": (" << lastPos.x << "," << lastPos.y << ")" << endl ;
    while(mutationIt != mutationSize) {
        pos newPos = randomPos(currPath[mutationIt]);
        //cout << "(" << newPos.x << "," << newPos.y << ")" << endl;
        if(compareTwoPos(newPos, lastPos))
            break;
        newPath.push_back(newPos);
        mutationIt++;
    }

    // coloca os genes restantes
    // que nao foram mutados
    for(int i = mutationSize; i < currPath.size(); i++)
        newPath.push_back(currPath[i]);

    return newPath;
}

// extermina todos os individuos dado um
// threshold cujo valor depende do tamanho
// do melhor individuo ate agora
void fitness(vector<pos>* population, int index) {
    pos iniPos;
    iniPos.x = 0, iniPos.y = 0;

    int sizeThreshold = population[index].size() * 3;
    // exterminio
    for(int i = 0; i < populationSize; i++) {
        if(population[i].size() > sizeThreshold) {
            population[i].clear();
            population[i].push_back(iniPos);
        }
    }

    // substituicao
    for (int i = 0; i < populationSize; i++) {
        int j = 0;
        if(population[i].empty()) {
            while (population[i].at(j).x != objX || population[i].at(j).y != objY) {
                population[i].push_back(randomPos(population[i].at(j)));
                j++;
            }
        }
    }
}