#ifndef PAGEDARRAY_HPP
#define PAGEDARRAY_HPP

#include <string>
#include <fstream>

class PagedArray {
private:
    std::string filePath;
    int pageSize;
    int pageCount;
    
    std::fstream file; // Objeto para leer y escribir el archivo binario

    // Arreglo 2D de enteros (Las páginas en memoria)
    int** pages; 
    
    // Arreglo para saber qué página lógica está en cada "slot" de memoria
    int* loadedPages;  
    
    // Arreglo para llevar el control de cuándo fue usada cada página (Para el algoritmo LRU)
    int* lastUsedTime;
    int currentTime; // Reloj interno para el LRU

    // Estadísticas
    int pageFaults;
    int pageHits;

    // Métodos auxiliares privados
    void loadPage(int logicalPage, int frameIndex);
    void savePage(int frameIndex);
    int findLRUFrame(); 

public:
    // Constructor y Destructor
    PagedArray(std::string path, int pSize, int pCount);
    ~PagedArray();

    // Sobrecarga del operador []
    // Retornamos una referencia (int&) para que los algoritmos puedan modificar el valor
    int& operator[](int index);

    // Getters para las estadísticas
    int getPageFaults() const;
    int getPageHits() const;
};

#endif