#ifndef CART_H
#define CART_H

#include <stdint.h>
#include <stdbool.h>

#define CART_SIZE 0xFFFF

typedef struct cart {
    uint8_t data[CART_SIZE];
    long size;
} Cart;

void cart_initialize(Cart* cart);
void cart_load(Cart* cart, const char* filename);

#endif
