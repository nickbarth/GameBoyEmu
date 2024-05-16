#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cart.h"

void cart_initialize(Cart* cart) {
    memset(cart->data, 0, CART_SIZE);
}

void cart_load(Cart* cart, const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: Couldn't open file %s\n", path);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    cart->size = ftell(file);
    rewind(file);

    fread(cart->data, cart->size, 1, file);

    char title[17];
    memcpy(title, cart->data + 0x134, 16);
    printf("Title: %s\n", title);

    char manufacturer[5];
    memcpy(manufacturer, cart->data + 0x13F, 4);
    printf("Manufacturer: %s\n", manufacturer);

    char licensee[3];
    memcpy(licensee, cart->data + 0x14B, 2);
    printf("Licensee: %2.2X\n", licensee[0]);

    printf("Loaded %ld bytes from %s\n", cart->size, path);
    fclose(file);
}
