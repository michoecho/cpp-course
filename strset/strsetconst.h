#ifndef STRSETCONST_H
#define STRSETCONST_H

#include <stddef.h>

#ifdef __cplusplus
// Zapobiegamy użyciu niezanicjalizowanych strumieni.
#include <iostream>

namespace jnp1 {
extern "C" {
#endif

// Zwraca identyfikator zbioru, którego nie można modyfikować i który zawiera
// jeden element: napis "42". Zbiór jest tworzony przy pierwszym wywołaniu tej
// funkcji i wtedy zostaje ustalony jego numer.
unsigned long strset42();

#ifdef __cplusplus
}
}
#endif

#endif //STRSETCONST_H
