// include/Algorithms.hpp
#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include "PagedArray.hpp"

void quickSort(PagedArray& arr, int low, int high);
// Heap Sort
void heapify(PagedArray& arr, int n, int i);
void heapSort(PagedArray& arr, int n);

// Shell Sort
void shellSort(PagedArray& arr, int n);

// Insertion Sort
void insertionSort(PagedArray& arr, int n);

// Selection Sort
void selectionSort(PagedArray& arr, int n);

#endif