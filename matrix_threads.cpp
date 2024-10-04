#include <iostream>
#include <thread>
#include <vector>
#include <ctime>

using namespace std;

int threadSize;
int matrixSize = 3000;

int* matrixA = new int[matrixSize * matrixSize];
int* matrixB = new int[matrixSize * matrixSize];
int* matrixC = new int[matrixSize * matrixSize];
int* matrixBT = new int[matrixSize * matrixSize];

void matrixMultiply(int firstIndex, int lastIndex){
    for (int i = firstIndex; i < lastIndex; i++){
        for (int j = 0; j < matrixSize; j++){
            for (int k = 0; k < matrixSize; k++){
                matrixC[i * matrixSize + j] += matrixA[i * matrixSize + k] * matrixB[k * matrixSize + j]; 
            }
        }
    }
}

void matrixRandom(){
    for (int i = 0; i < matrixSize; i ++){
        for (int j = 0; j < matrixSize; j++){
            matrixA[i * matrixSize + j] = rand() % 10;
            matrixB[i * matrixSize + j] = rand() % 10;
            matrixC[i * matrixSize + j] = 0;
        }
    }
}

void Transposition(int matrix[]){
    for (int i = 0; i < matrixSize; i++){
        for (int j=0; j < matrixSize; j++){
            matrixBT[i * matrixSize + j] = matrix[j * matrixSize + i];
        }
    }
}

int main() {
    matrixRandom();
    Transposition(matrixB);

    for (threadSize = 1; threadSize <= 8; threadSize *= 2){
        clock_t start = clock();

        vector<thread> threads;
        int threadRows = matrixSize / threadSize;

        for (int i = 0; i < threadSize; i++) {
            int firstIndex = i * threadRows;
            int lastIndex = firstIndex + threadRows;

            if (i == threadSize - 1) {
                lastIndex = matrixSize;
            }

            threads.emplace_back(matrixMultiply, firstIndex, lastIndex);
        }

        for (int i = 0; i < threadSize; i++) {
            threads[i].join();
        }

        clock_t end = clock();
        double duration = (static_cast<double>(end - start)) / CLOCKS_PER_SEC;

        if (threadSize == 1){
            cout << "Czas wykonania dla " << threadSize << " wątku wynosi: " << duration << "sekundy" << endl;
        }
        else{
            cout << "Czas wykonania dla " << threadSize << " wątków wynosi: " << duration << "sekundy" << endl;
        }
    }
    
    delete[] matrixA;
    delete[] matrixB;
    delete[] matrixC;
    delete[] matrixBT;
    
    return 0;
}