#include <climits>
#include "strset.h"
#include "strsetconst.h"

#ifndef NDEBUG
static const bool debug = true;
#else
static const bool debug = false;
#endif

static unsigned long init42() {
	if (debug) {
		std::cerr << "strsetconst init invoked" << std::endl;
	}
	unsigned long id = jnp1::strset_new();
	jnp1::strset_insert(id, "42");
	if (debug) {
		std::cerr << "strsetconst init finished" << std::endl;
	}
	return id;
}

// Zwraca identyfikator zbioru, którego nie można modyfikować i który zawiera
// jeden element: napis "42". Zbiór jest tworzony przy pierwszym wywołaniu tej
// funkcji i wtedy zostaje ustalony jego numer.
unsigned long jnp1::strset42() {

	// Podczas inicjalizacji stałego zbioru musimy zablokować rekurencyjne
	// wywołania strset42(). Jednak strset_insert() korzysta z strset42()
	// do sprawdzania, czy podane id nie należy do stałego zbioru.
	// Wobec tego podczas blokady funkcja strset42() zwraca liczbę ULONG_MAX,
	// która na pewno jest różna od dowolnego id zwróconego przez strset_new().
	static bool first_call = true;
	static unsigned long id = ULONG_MAX;
	if (first_call) {
		first_call = false;
		id = init42();
	}

	return id;
}
