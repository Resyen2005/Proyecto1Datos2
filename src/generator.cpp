#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;

// Función para imprimir el uso correcto del programa
void printUsage() {
    cout << "Uso: generator -size <SIZE> -output <OUTPUT FILE PATH>\n";
    cout << "Tamanos validos para <SIZE>: TEST (100 nums), SMALL (32 MB), MEDIUM (64 MB), LARGE (128 MB)\n";
}

int main(int argc, char* argv[]) {
    // 1. Validar la cantidad de argumentos de la línea de comandos 
    if (argc != 5) {
        printUsage();
        return 1;
    }

    string sizeStr = "";
    string outputPath = "";

    // 2. Parsear los argumentos 
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-size") == 0 && i + 1 < argc) {
            sizeStr = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-output") == 0 && i + 1 < argc) {
            outputPath = argv[i + 1];
            i++;
        }
    }

    if (sizeStr == "" || outputPath == "") {
        cout << "Error: Faltan argumentos requeridos.\n";
        printUsage();
        return 1;
    }

    // 3. Determinar la cantidad de enteros a generar
    long long totalBytes = 0;

    if (sizeStr == "TEST"){
        totalBytes = 100LL * sizeof(int); // 100 enteros
    } else if (sizeStr == "SMALL") {
        totalBytes = 32LL * 1024 * 1024; // 32 MB
    } else if (sizeStr == "MEDIUM") {
        totalBytes = 64LL * 1024 * 1024; // 64 MB 
    } else if (sizeStr == "LARGE") {
        totalBytes = 128LL * 1024 * 1024; // 128 MB 
    } else {
        cout << "Error: Tamano invalido. Use SMALL, MEDIUM o LARGE.\n";
        return 1;
    }

    long long totalIntegers = totalBytes / sizeof(int);

    // 4. Abrir el archivo en modo binario
    ofstream outFile(outputPath, ios::binary);
    if (!outFile) {
        cout << "Error: No se pudo crear el archivo " << outputPath << "\n";
        return 1;
    }

    // Inicializar los números aleatorios
    srand(time(NULL));

    // 5. Generación eficiente usando un búfer en memoria 
    const int BUFFER_SIZE = 1000000;
    int* buffer = new int[BUFFER_SIZE]; 
    
    long long integersWritten = 0;
    cout << "Generando archivo " << sizeStr << " (" << totalBytes / (1024*1024) << " MB)...\n";

    while (integersWritten < totalIntegers) {
        // Determinar cuántos enteros escribir en esta iteración
        int currentBatchSize = BUFFER_SIZE;
        if (totalIntegers - integersWritten < BUFFER_SIZE) {
            currentBatchSize = totalIntegers - integersWritten;
        }

        // Llenar el búfer con números aleatorios
        for (int i = 0; i < currentBatchSize; i++) {
            buffer[i] = rand();
        }

        // Escribir todo el bloque en binario de una sola vez
        // Se usa reinterpret_cast para convertir el puntero int* a char* que es lo que requiere write()
        outFile.write(reinterpret_cast<const char*>(buffer), currentBatchSize * sizeof(int));
        
        integersWritten += currentBatchSize;

        // Mostrar progreso
        if (integersWritten % (BUFFER_SIZE * 10) == 0) {
            cout << "Progreso: " << (integersWritten * 100) / totalIntegers << "%\r" << flush;
        }
    }

    // 6. Limpieza
    delete[] buffer;
    outFile.close();

    cout << "\nArchivo generado exitosamente en: " << outputPath << "\n";
    cout << "Total de enteros generados: " << totalIntegers << "\n";

    return 0;
}