
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#define _CRT_SECURE_NO_WARNINGS

struct memory_page {
    std::size_t min = 0;
    std::size_t sze = 0;
    HANDLE p_handle = 0;
    bool mapped = false;

    memory_page(std::size_t min, std::size_t sze, HANDLE p_handle) {
        this->min = min;
        this->sze = sze;
        this->p_handle = p_handle;
    }
};