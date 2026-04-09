#include "../include/PagedArray.hpp"
#include <iostream>

using namespace std;

// --- CONSTRUCTOR ---
PagedArray::PagedArray(string path, int pSize, int pCount) {
    this->filePath = path;
    this->pageSize = pSize;
    this->pageCount = pCount;
    this->currentTime = 0;
    this->pageFaults = 0;
    this->pageHits = 0;

    // Abrir el archivo en modo lectura/escritura binaria
    file.open(filePath, ios::in | ios::out | ios::binary);
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << filePath << " para paginacion.\n";
        exit(1);
    }

    // Se inicializan los arreglos
    pages = new int*[pageCount];
    loadedPages = new int[pageCount];
    lastUsedTime = new int[pageCount];

    for (int i = 0; i < pageCount; i++) {
        pages[i] = new int[pageSize];
        loadedPages[i] = -1; // -1 indica que este slot/marco está vacío
        lastUsedTime[i] = 0;
    }
}

// --- DESTRUCTOR ---
PagedArray::~PagedArray() {
    // 1. Guardar cualquier página que siga en memoria de vuelta al disco
    for (int i = 0; i < pageCount; i++) {
        if (loadedPages[i] != -1) {
            savePage(i);
        }
    }

    // 2. Cerrar archivo
    if (file.is_open()) {
        file.close();
    }

    // 3. Liberar memoria dinámica
    for (int i = 0; i < pageCount; i++) {
        delete[] pages[i];
    }
    delete[] pages;
    delete[] loadedPages;
    delete[] lastUsedTime;
}

// --- MÉTODOS DE DISCO ---

// Carga una página desde el disco a la memoria RAM
void PagedArray::loadPage(int logicalPage, int frameIndex) {
    // Se calcula el byte exacto donde empieza la página en el disco
    long long offset = (long long)logicalPage * pageSize * sizeof(int);
    
    // Se mueve el puntero de lectura del archivo a ese byte
    file.seekg(offset, ios::beg);
    
    // Se lee el bloque completo de bytes y se guarda en el arreglo
    file.read(reinterpret_cast<char*>(pages[frameIndex]), pageSize * sizeof(int));
    
    // Se actualiza el registro
    loadedPages[frameIndex] = logicalPage;
}

// Guarda una página desde la RAM hacia el disco
void PagedArray::savePage(int frameIndex) {
    if (loadedPages[frameIndex] == -1) return; // No hay nada que guardar

    long long offset = (long long)loadedPages[frameIndex] * pageSize * sizeof(int);
    
    // Se mueve el puntero de escritura del archivo
    file.seekp(offset, ios::beg);
    
    // Se escribe el bloque
    file.write(reinterpret_cast<const char*>(pages[frameIndex]), pageSize * sizeof(int));
}

// --- ALGORITMO LRU ---
// Busca qué marco de memoria (0 a pageCount-1) se debe remplazar
int PagedArray::findLRUFrame() {
    // Primero, se busca si hay algún marco vacío
    for (int i = 0; i < pageCount; i++) {
        if (loadedPages[i] == -1) {
            return i;
        }
    }

    // Si no hay vacíos, buscamos el que tenga el lastUsedTime más pequeño ( que seria el más viejo)
    int lruFrame = 0;
    int oldestTime = lastUsedTime[0];

    for (int i = 1; i < pageCount; i++) {
        if (lastUsedTime[i] < oldestTime) {
            oldestTime = lastUsedTime[i];
            lruFrame = i;
        }
    }

    return lruFrame;
}

// --- EL NÚCLEO: OPERADOR [] ---
int& PagedArray::operator[](int index) {
    currentTime++; // Avanzamos el reloj interno

    int logicalPage = index / pageSize; // ¿En qué página debería estar este índice? 
    int offset = index % pageSize;      // ¿En qué posición exacta dentro de esa página?

    // 1. Verificar si la página ya está en memoria (Page Hit)
    for (int i = 0; i < pageCount; i++) {
        if (loadedPages[i] == logicalPage) {
            pageHits++;
            lastUsedTime[i] = currentTime; // Actualizamos que acaba de ser usada
            return pages[i][offset];       // Retornamos la referencia al dato 
        }
    }

    // 2. Si no estaba, es un Page Fault
    pageFaults++;

    // Encontrar un espacio (ya sea vacío o reemplazando uno viejo)
    int frameToUse = findLRUFrame();

    // Si el espacio estaba ocupado por otra página, la guardamos en el disco antes de borrarla
    if (loadedPages[frameToUse] != -1) {
        savePage(frameToUse);
    }

    // Cargamos la nueva página desde el disco a ese espacio
    loadPage(logicalPage, frameToUse);
    lastUsedTime[frameToUse] = currentTime;

    // Retornamos la referencia
    return pages[frameToUse][offset];
}

// --- GETTERS ---
int PagedArray::getPageFaults() const { return pageFaults; }
int PagedArray::getPageHits() const { return pageHits; }