#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <chrono>
#include "../include/PagedArray.hpp"
#include "../include/Algorithms.hpp"

using namespace std;
using namespace std::chrono;

void printUsage() {
    cout << "Uso: sorter -input <INPUT> -output <OUTPUT> -alg <ALGORITMO> -pageSize <SIZE> -pageCount <COUNT>\n";
    cout << "Algoritmos: QS, HS, SS, IS, SL\n";
}

// Función para copiar el archivo de entrada al archivo de salida de trabajo
bool copyFile(const string& source, const string& destination) {
    ifstream src(source, ios::binary);
    ofstream dst(destination, ios::binary);

    if (!src || !dst) return false;

    // Usar un buffer de 1MB para copiar rápido
    const size_t bufferSize = 1024 * 1024;
    char* buffer = new char[bufferSize];

    while (src.read(buffer, bufferSize)) {
        dst.write(buffer, src.gcount());
    }
    dst.write(buffer, src.gcount()); // Escribir lo que quede

    delete[] buffer;
    return true;
}

// Función para generar el archivo legible en formato separado por comas
void generateReadableFile(const string& binaryPath, const string& txtPath) {
    ifstream binFile(binaryPath, ios::binary);
    ofstream txtFile(txtPath);

    if (!binFile || !txtFile) {
        cout << "Error al crear el archivo legible.\n";
        return;
    }

    int value;
    bool first = true;
    while (binFile.read(reinterpret_cast<char*>(&value), sizeof(int))) {
        if (!first) {
            txtFile << ", ";
        }
        txtFile << value;
        first = false;
    }
}

int main(int argc, char* argv[]) {
    // 1. Variables para los argumentos
    string inputPath = "", outputPath = "", algorithm = "";
    int pageSize = 0, pageCount = 0;

    // 2. Parsear los argumentos de la terminal
    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-input") == 0 || strcmp(argv[i], "input") == 0) && i + 1 < argc) {
            inputPath = argv[++i];
        } else if (strcmp(argv[i], "-output") == 0 && i + 1 < argc) {
            outputPath = argv[++i];
        } else if (strcmp(argv[i], "-alg") == 0 && i + 1 < argc) {
            algorithm = argv[++i];
        } else if (strcmp(argv[i], "-pageSize") == 0 && i + 1 < argc) {
            pageSize = stoi(argv[++i]);
        } else if (strcmp(argv[i], "-pageCount") == 0 && i + 1 < argc) {
            pageCount = stoi(argv[++i]);
        }
    }

    if (inputPath == "" || outputPath == "" || algorithm == "" || pageSize <= 0 || pageCount <= 0) {
        cout << "Error: Faltan argumentos o son invalidos.\n";
        printUsage();
        return 1;
    }

    string workingBinaryPath = outputPath + ".bin";
    string finalTxtPath = outputPath + ".txt";

    cout << "Preparando el entorno...\n";
    // 3. Copiar el archivo original para trabajar sobre la copia
    if (!copyFile(inputPath, workingBinaryPath)) {
        cout << "Error: No se pudo copiar el archivo de entrada.\n";
        return 1;
    }

    // Calcular cuántos enteros hay en el archivo para saber el rango a ordenar
    ifstream checkSize(workingBinaryPath, ios::binary | ios::ate);
    long long totalBytes = checkSize.tellg();
    checkSize.close();
    int totalElements = totalBytes / sizeof(int);

    int faults = 0, hits = 0;
    double durationMs = 0;

    cout << "Iniciando ordenamiento con " << algorithm << "...\n";

    // 4. Bloque principal: Instanciar PagedArray y ejecutar el algoritmo
    {
        PagedArray arr(workingBinaryPath, pageSize, pageCount);

        auto start = high_resolution_clock::now();

        if (algorithm == "QS") {
            quickSort(arr, 0, totalElements - 1);
        } 
        else if (algorithm == "HS") {
            heapSort(arr, totalElements);
        }
        else if (algorithm == "SS") {
            shellSort(arr, totalElements);
        }else if (algorithm == "SL")
        {
            selectionSort(arr, totalElements);
        }
        
        else if (algorithm == "IS") {

            if (totalElements > 134217728) { // Si es más grande que SMALL (512MB)
                cout << "Advertencia: Insertion Sort en un archivo tan grande tomara demasiado tiempo.\n";
            }
            insertionSort(arr, totalElements);
        }
        else {
            cout << "Error: Algoritmo no soportado (" << algorithm << ").\n";
            cout << "Soportados: QS (QuickSort), HS (HeapSort), SS (ShellSort), SL (SelectionSort), IS (InsertionSort)\n";
            return 1;
        }

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        durationMs = duration.count();

        faults = arr.getPageFaults();
        hits = arr.getPageHits();
    }

    // 5. Generar archivo ASCII/Unicode separado por comas
    cout << "Generando version legible...\n";
    generateReadableFile(workingBinaryPath, finalTxtPath);

    // 6. Imprimir resumen
    cout << "\n=== RESUMEN DE EJECUCION ===\n";
    cout << "Algoritmo utilizado:  " << algorithm << "\n";
    cout << "Tiempo durado:        " << durationMs << " ms\n";
    cout << "Page Faults:          " << faults << "\n";
    cout << "Page Hits:            " << hits << "\n";
    cout << "Archivos generados:\n";
    cout << " - Binario: " << workingBinaryPath << "\n";
    cout << " - Legible: " << finalTxtPath << "\n";
    cout << "============================\n";

    return 0;
}