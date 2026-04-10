#include "../include/Algorithms.hpp"

// Función para particionar el arreglo
int partition(PagedArray& arr, int low, int high) {
    int pivot = arr[high]; // Seleccionar el último elemento como pivote
    int i = (low - 1);     // Índice del elemento más pequeño

    for (int j = low; j <= high - 1; j++) {
        // Si el elemento actual es menor que el pivote
        if (arr[j] < pivot) {
            i++;
            // Intercambio manual (evitamos std::swap por seguridad con la referencia)
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    // Colocar el pivote en su posición correcta
    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    
    return (i + 1);
}

// Función principal de Quicksort
void quickSort(PagedArray& arr, int low, int high) {
    if (low < high) {
        // pi es el índice de partición, arr[pi] está ahora en el lugar correcto
        int pi = partition(arr, low, high);

        // Ordenar los elementos antes y después de la partición
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// Función auxiliar para reestructurar el árbol binario (Heap)
void heapify(PagedArray& arr, int n, int i) {
    int largest = i;         // Inicializar el nodo raíz como el más grande
    int left = 2 * i + 1;    // Hijo izquierdo
    int right = 2 * i + 2;   // Hijo derecho

    // Si el hijo izquierdo es mayor que la raíz
    if (left < n && arr[left] > arr[largest]) {
        largest = left;
    }

    // Si el hijo derecho es mayor que el más grande hasta ahora
    if (right < n && arr[right] > arr[largest]) {
        largest = right;
    }

    // Si el más grande no es la raíz, intercambiamos y aplicamos recursividad
    if (largest != i) {
        int temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;

        // Recursividad para arreglar el subárbol afectado
        heapify(arr, n, largest);
    }
}

// Función principal de Heap Sort
void heapSort(PagedArray& arr, int n) {
    // 1. Construir el Max-Heap inicial
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }

    // 2. Extraer elementos del heap uno por uno
    for (int i = n - 1; i > 0; i--) {
        // Mover la raíz actual (el más grande) al final
        int temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        // Llamar a heapify en el heap reducido
        heapify(arr, i, 0);
    }
}

void shellSort(PagedArray& arr, int n) {
    // Empezar con una brecha (gap) grande y reducirla a la mitad cada vez
    for (int gap = n / 2; gap > 0; gap /= 2) {
        // Hacer un ordenamiento por inserción para la brecha actual
        for (int i = gap; i < n; i += 1) {
            int temp = arr[i]; // Guardamos el valor actual
            int j;
            
            // Desplazar los elementos anteriores que son mayores que temp
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
            }
            // Poner temp en su posición correcta
            arr[j] = temp;
        }
    }
}

void insertionSort(PagedArray& arr, int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i]; // Guardamos el valor a insertar
        int j = i - 1;

        // Mover los elementos de arr[0..i-1] que sean mayores que key
        // una posición adelante de su posición actual
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

void selectionSort(PagedArray& arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        // Intercambio
        int temp = arr[min_idx];
        arr[min_idx] = arr[i];
        arr[i] = temp;
    }
}