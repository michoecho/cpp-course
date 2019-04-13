#ifndef STRSET_H
#define STRSET_H

#include <stddef.h>

#ifdef __cplusplus
// Zapobiegamy użyciu niezanicjalizowanych strumieni.
#include <iostream>

namespace jnp1 {
extern "C" {
#endif

// Tworzy nowy zbiór i zwraca jego identyfikator.
unsigned long strset_new();

// Jeżeli istnieje zbiór o identyfikatorze id, usuwa go, a w przeciwnym
// przypadku nie robi nic.
void strset_delete(unsigned long id);

// Jeżeli istnieje zbiór o identyfikatorze id, zwraca liczbę jego elementów,
// a w przeciwnym przypadku zwraca 0.
size_t strset_size(unsigned long id);

// Jeżeli istnieje zbiór o identyfikatorze id i element value nie należy do
// tego zbioru, to dodaje element do zbioru, a w przeciwnym przypadku nie
// robi nic.
void strset_insert(unsigned long id, const char* value);

// Jeżeli istnieje zbiór o identyfikatorze id i element value należy do tego
// zbioru, to usuwa element ze zbioru, a w przeciwnym przypadku nie robi nic.
void strset_remove(unsigned long id, const char* value);

// Jeżeli istnieje zbiór o identyfikatorze id i element value należy do tego
// zbioru, to zwraca 1, a w przeciwnym przypadku 0.
int strset_test(unsigned long id, const char* value);

// Jeżeli istnieje zbiór o identyfikatorze id, usuwa wszystkie jego elementy,
// a w przeciwnym przypadku nie robi nic.
void strset_clear(unsigned long id);

// Porównuje zbiory o identyfikatorach id1 i id2. Niech sorted(id) oznacza
// posortowany leksykograficznie zbiór o identyfikatorze id. Takie ciągi już
// porównujemy naturalnie: pierwsze miejsce, na którym się różnią, decyduje
// o relacji większości. Jeśli jeden ciąg jest prefiksem drugiego, to ten
// będący prefiks jest mniejszy. Funkcja strset_comp(id1, id2) powinna zwrócić
// -1, gdy sorted(id1) < sorted(id2),
// 0, gdy sorted(id1) = sorted(id2),
// 1, gdy sorted(id1) > sorted(id2).
// Jeżeli zbiór o którymś z identyfikatorów nie istnieje, to jest traktowany
// jako równy zbiorowi pustemu.
int strset_comp(unsigned long id1, unsigned long id2);

#ifdef __cplusplus
}
}
#endif

#endif //STRSET_H
