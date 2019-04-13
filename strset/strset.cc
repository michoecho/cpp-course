#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <string>
#include <climits>
#include <cassert>
#include "strset.h"
#include "strsetconst.h"

#ifndef NDEBUG
static const bool debug = true;
#else
static const bool debug = false;
#endif

using std::cerr;
using std::endl;
using std::string;

// Typ przechowywanych zbiorów.
using set_t = std::set<string>;

// Spis wszyskich zbiorów przechowywanych w module.
using index_t = std::unordered_map<unsigned long, set_t>;

// Utwórz spis zbiorów przy pierwszym użyciu.
static index_t& index() {
	static index_t index;
	return index;
}

// Rozpoznaj, czy dany zbiór jest zbiorem stałym.
static bool is_const(unsigned long id) {
	return id == jnp1::strset42();
}

// Podaj nazwę zbioru do użycia w logach.
// Ponieważ do ustalenia nazwy zbioru konieczne jest wywołanie strset42(),
// musimy zapewnić, że pierwsze wywołanie strset42() nastąpi przed pierwszym
// wywołaniem name_set(). W przeciwnym razie wersja diagnostyczna
// stanie się istotnie różna od wersji niediagnostycznej.
static string name_set(unsigned long id) {
	if (is_const(id)) {
		return "the 42 Set";
	} else {
		return "set " + std::to_string(id);
	}
}

// Przygotuj przetwarzany napis do wypisania w logach.
static string quote(const char *arg) {
	if (arg == nullptr) {
		return "NULL";
	} else {
		return '"' + string(arg) + '"';
	}
}

// Odnotuj, czy zbiór id zawiera value.
static void log_value_present(const char *func, unsigned long id,
                              const char *value, bool present) {
	const char *info = present ? " contains" : " does not contain";
	cerr << func << ": " << name_set(id) << info << " the element "
		<< quote(value) << endl;
}

// Odnotuj niepoprawną wartość argumentu.
static void log_invalid_value(const char *func) {
	cerr << func << ": invalid value (NULL)" << endl;
}

// Odnotuj zapytanie o nieistniejący zbiór.
static void log_missing_id(const char *func, unsigned long id) {
	cerr << func << ": set " << id << " does not exist" << endl;
}

// Odnotuj informację o zbiorze.
static void log_id_info(const char *func, unsigned long id, const char *info) {
	cerr << func << ": " << name_set(id) << " " << info << endl;
}

// Odnotuj informację o elemencie zbioru.
static void log_value_info(const char* func, unsigned long id,
                           const char *value, const char *info) {
	cerr << func << ": " << name_set(id) << ", element " <<
		quote(value) << ' ' << info << endl;
}

// Odnotuj próbę modyfikacji stałego zbioru.
static void log_const_violation(const char *func, const char *info) {
	cerr << func << ": attempt to " << info << " the 42 Set" << endl;
}

// Odnotuj wywołanie funkcji i jej argumenty.
static void log_call(const char *func) {
	cerr << func << "()" << endl;
}

// Odnotuj wywołanie funkcji i jej argumenty.
static void log_call(const char *func, unsigned long id) {
	cerr << func << '(' << id << ')' << endl;
}

// Odnotuj wywołanie funkcji i jej argumenty.
static void log_call(const char *func, unsigned long id, const char *value) {
	cerr << func << "(" << id << ", " << quote(value) << ')' << endl;
}

// Odnotuj wywołanie funkcji i jej argumenty.
static void log_call(const char *func, unsigned long id1, unsigned long id2) {
	cerr << func << "(" << id1 << ", " << id2 << ")" << endl;
}

// Tworzy nowy zbiór i zwraca jego identyfikator.
unsigned long jnp1::strset_new() {
	static unsigned long id = 0;
	assert(id < ULONG_MAX);

	if (debug) {
		log_call(__func__);
		cerr << __func__ << ": set " << id << " created" << endl;
	}
	index()[id++];
	return id - 1;
}

// Jeżeli istnieje zbiór o identyfikatorze id, usuwa go, a w przeciwnym
// przypadku nie robi nic.
void jnp1::strset_delete(unsigned long id) {
	if (debug) log_call(__func__, id);

	const auto it = index().find(id);
	if (it != index().end()) {
		if (is_const(id)) {
			if (debug) log_const_violation(__func__, "remove");
			return;
		}

		index().erase(it);
		if (debug) log_id_info(__func__, id, "deleted");
	} else {
		if (debug) log_missing_id(__func__, id);
	}


}

// Jeżeli istnieje zbiór o identyfikatorze id, zwraca liczbę jego elementów,
// a w przeciwnym przypadku zwraca 0.
size_t jnp1::strset_size(unsigned long id) {
	if (debug) log_call(__func__, id);

	const auto it = index().find(id);
	if (it != index().end()) {
		jnp1::strset42(); // Patrz komentarz do name_set().
		const set_t &chosen = it->second;
		size_t size = chosen.size();
		if (debug) {
			cerr << __func__ << ": " << name_set(id) << " contains " << size <<
				" element(s)" << endl;
		}
		return size;
	} else {
		if (debug) log_missing_id(__func__, id);
	}
	return 0;
}

// Jeżeli istnieje zbiór o identyfikatorze id i element value nie należy do
// tego zbioru, to dodaje element do zbioru, a w przeciwnym przypadku nie
// robi nic.
void jnp1::strset_insert(unsigned long id, const char* value) {
	if (debug) log_call(__func__, id, value);

	if (value == nullptr) {
		if (debug) log_invalid_value(__func__);
		return;
	}

	const auto it = index().find(id);
	if (it != index().end()) {
		if (is_const(id)) {
			if (debug) log_const_violation(__func__, "insert into");
			return;
		}

		set_t &chosen = it->second;
		const auto target = chosen.find(value);
		if (target == chosen.end()) {
			chosen.emplace(value);
			if (debug) log_value_info(__func__, id, value, "inserted");
		} else {
			if (debug) log_value_info(__func__, id, value, "was already present");
		}
	} else {
		if (debug) log_missing_id(__func__, id);
	}
}

// Jeżeli istnieje zbiór o identyfikatorze id i element value należy do tego
// zbioru, to usuwa element ze zbioru, a w przeciwnym przypadku nie robi nic.
void jnp1::strset_remove(unsigned long id, const char* value) {
	if (debug) log_call(__func__, id, value);

	if (value == nullptr) {
		if (debug) log_invalid_value(__func__);
		return;
	}

	const auto it = index().find(id);
	if (it != index().end()) {
		if (is_const(id)) {
			if (debug) log_const_violation(__func__, "remove from");
			return;
		}

		set_t &chosen = it->second;
		const auto target = chosen.find(value);
		if (target != chosen.end()) {
			chosen.erase(value);
			if (debug) log_value_info(__func__, id, value, "removed");
		} else {
			if (debug) log_value_present(__func__, id, value, false);
		}

	} else {
		if (debug) log_missing_id(__func__, id);
	}
}

// Jeżeli istnieje zbiór o identyfikatorze id i element value należy do tego
// zbioru, to zwraca 1, a w przeciwnym przypadku 0.
int jnp1::strset_test(unsigned long id, const char* value) {
	if (debug) log_call(__func__, id, value);

	if (value == nullptr) {
		if (debug) log_invalid_value(__func__);
		return 0;
	}

	const auto it = index().find(id);
	if (it != index().end()) {
		jnp1::strset42(); // Patrz komentarz do name_set().
		const set_t &chosen = it->second;
		if (chosen.find(value) != chosen.end()) {
			if (debug) log_value_present(__func__, id, value, true);
			return 1;
		} else {
			if (debug) log_value_present(__func__, id, value, false);
			return 0;
		}
	} else {
		if (debug) log_missing_id(__func__, id);
		return 0;
	}
}

// Jeżeli istnieje zbiór o identyfikatorze id, usuwa wszystkie jego elementy,
// a w przeciwnym przypadku nie robi nic.
void jnp1::strset_clear(unsigned long id) {
	if (debug) log_call(__func__, id);

	const auto it = index().find(id);
	if (it != index().end()) {
		if (is_const(id)) {
			if (debug) log_const_violation(__func__, "clear");
			return;
		}

		set_t &chosen = it->second;
		chosen.clear();
		if (debug) log_id_info(__func__, id, "cleared");
	} else {
		if (debug) log_missing_id(__func__, id);
	}
}

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
int jnp1::strset_comp(unsigned long id1, unsigned long id2) {
	if (debug) log_call(__func__, id1, id2);

	static set_t empty;
	const auto it1 = index().find(id1);
	const auto it2 = index().find(id2);

	const bool s1_exists = it1 != index().end();
	const bool s2_exists = it2 != index().end();
	const set_t &s1 = (s1_exists ? it1->second : empty);
	const set_t &s2 = (s2_exists ? it2->second : empty);

	if (debug) {
		if (!s1_exists) {
			log_missing_id(__func__, id1);
		}
		if (!s2_exists) {
			log_missing_id(__func__, id2);
		}
	}

	jnp1::strset42(); // Patrz komentarz do name_set().

	int ans = 0;
	if (std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end())) {
		ans = -1;
	} else if (std::lexicographical_compare(s2.begin(), s2.end(), s1.begin(), s1.end())) {
		ans = 1;
	}

	if (debug) {
		cerr << __func__ << ": result of comparing " <<
			(s1_exists ? name_set(id1) : "an empty set") << " to " <<
			(s2_exists ? name_set(id2) : "an empty set") << " is " <<
			ans << endl;
	}

	return ans;
}
