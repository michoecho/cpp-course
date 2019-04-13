#include <algorithm>
#include <cstring>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <cassert>
#include "wallet.h"


static uint64_t globalUnits = 0;

static const uint64_t decimal_shift = 100'000'000;
static const uint64_t maxBC = 21'000'000;


////////////////////////////////////////////////////////////////////////////////
// Formatowanie waluty


static uint64_t getDigitCount(uint64_t n) {
    uint64_t count = 1;
    while (n >= 10) {
        count++;
        n /= 10;
    }
    return count;
}

static uint64_t removeTrailingZeros(uint64_t n) {
    while (n % 10 == 0) {
        n /= 10;
    }
    return n;
}

static void writeCurrency(std::ostream &os, uint64_t amount) {
    uint64_t wholePart = amount / decimal_shift;
    uint64_t fractionalPart = amount % decimal_shift;
    uint64_t fractionalLeadingZerosCount = (getDigitCount(decimal_shift) - 1) - getDigitCount(fractionalPart);
    os << wholePart;
    if (fractionalPart > 0) {
        os << ",";
        for (size_t i = 0; i < fractionalLeadingZerosCount; ++i) {
            os << "0";
        }
        os << removeTrailingZeros(fractionalPart);
    }
}


////////////////////////////////////////////////////////////////////////////////
// Operation


Operation::Operation(uint64_t units) : units(units) {
    using namespace std::chrono;
    timestamp = time_point_cast<milliseconds>(system_clock::now());
}

// Zwraca liczbę jednostek w portfelu po operacji.
uint64_t Operation::getUnits() const {
    return units;
}

// Operatory porównujące czas utworzenia (z dokładnością do milisekund)
// operacji o1 i o2.
bool Operation::operator<(const Operation &o) const {
    return timestamp < o.timestamp;
}

bool Operation::operator>(const Operation &o) const {
    return timestamp > o.timestamp;
}

bool Operation::operator==(const Operation &o) const {
    return timestamp == o.timestamp;
}

bool Operation::operator<=(const Operation &o) const {
    return timestamp <= o.timestamp;
}

bool Operation::operator!=(const Operation &o) const {
    return timestamp != o.timestamp;
}

bool Operation::operator>=(const Operation &o) const {
    return timestamp >= o.timestamp;
}


// Wypisuje na strumień os "Wallet balance is b B after operation made at day d".
// Liczba b jak przy wypisywaniu portfela. Czas d w formacie yyyy-mm-dd.
std::ostream &operator<<(std::ostream &os, const Operation &o) {
    time_t tmp = std::chrono::system_clock::to_time_t(o.timestamp);
    os << "Wallet balance is ";
    writeCurrency(os, o.getUnits());
    os << " B after operation made at day " << std::put_time(std::localtime(&tmp), "%Y-%m-%d");

    return os;
}


////////////////////////////////////////////////////////////////////////////////
// Parsowanie liczb


static uint64_t parseString(const char *str) {
    assert(str != nullptr);
    const char *p = str;
    uint64_t ans = 0;

    while (isspace(*p)) ++p; // Ignorujemy wiodące białe znaki.
    if (*p == '\0') // Były tylko białe znaki.
        throw std::invalid_argument("Whitespace-only B number string.");
    bool leadingDigits = isdigit(*p); // Czy przed separatorem są cyfry?
    bool trailingDigits = true;

    // Część całkowita.
    while (isdigit(*p)) {
        ans *= 10;
        ans += *p - '0';
        // Liczby większe od maxBC uznajemy w tej fazie za poprawne argumenty.
        // Utożsamiamy je jednak z maxBC + 1, by zapobiec przepełnieniu zmiennej.
        if (ans > maxBC)
            ans = maxBC + 1;
        ++p;
    }

    // Część ułamkowa.
    if (*p == ',' || *p == '.') {
        ++p;
        trailingDigits = isdigit(*p); // Czy po separatorze są cyfry?

        unsigned currentShift = 1;
        while (currentShift < decimal_shift && isdigit(*p)) {
            ans *= 10;
            ans += *p - '0';
            currentShift *= 10;
            ++p;
        }

        // Uzupełniamy do 8 miejsc po przecinku.
        while (currentShift < decimal_shift) {
            ans *= 10;
            currentShift *= 10;
        }

        // Zezwalamy na dowolną liczbę zer kończących.
        while (*p == '0') ++p;

    } else {
        // Brak części ułamkowej.
        ans *= decimal_shift;
    }

    while (isspace(*p)) ++p; // Ignorujemy kończące białe znaki.

    if (*p != '\0')
        throw std::invalid_argument("Invalid B number string.");
    if (!leadingDigits && !trailingDigits)
        throw std::invalid_argument("\".\" is not a valid representation of 0 B.");

    return ans;
}

static uint64_t parseBinaryString(const char *str) {
    assert(str != nullptr);
    const char *p = str;
    uint64_t ans = 0;
    if (*p == '\0') // Były tylko białe znaki.
        throw std::invalid_argument("Invalid BC input string format");

    while (*p == '0' || *p == '1') {
        ans *= 2;
        ans += (*p == '1');
        // Liczby większe od maxBC uznajemy w tej fazie za poprawne argumenty.
        // Utożsamiamy je jednak z maxBC + 1, by zapobiec przepełnieniu zmiennej.
        if (ans > maxBC)
            ans = maxBC + 1;
        ++p;
    }

    if (*p != '\0')
        throw std::invalid_argument("Invalid BC input string format");

    return ans * decimal_shift;
}


////////////////////////////////////////////////////////////////////////////////
// Podstawowe operacje na portfelach


static void createNewUnits(uint64_t units) {
    if (globalUnits + units < globalUnits || globalUnits + units > maxBC * decimal_shift)
        throw std::logic_error("BC limit exceeded");
    globalUnits += units;
}

static void destroyUnits(uint64_t units) {
    globalUnits -= units;
}

// Wyczyść zawartość i historię portfela.
// Procedura używana po przeniesieniu portfela.
void Wallet::reset() {
    history.clear();
    updateHistory(0);
}

// Dodaje nowy stan portfela do historii
void Wallet::updateHistory(uint64_t units) {
    history.push_back(Operation(units));
}

uint64_t Wallet::takeAllUnits() {
    uint64_t units = getUnits();
    destroyUnits(units);
    updateHistory(0);
    return units;
}

uint64_t Wallet::takeUnits(uint64_t amount) {
    uint64_t units = getUnits();
    if (units < amount) {
        throw std::logic_error("Insufficient BC for substraction");
    }

    destroyUnits(amount);
    updateHistory(units - amount);
    return amount;
}

uint64_t Wallet::addUnits(uint64_t units) {
    createNewUnits(units);
    updateHistory(getUnits() + units);
    return units;
}

// Zwraca liczbę jednostek w portfelu.
uint64_t Wallet::getUnits() const {
    return history.back().getUnits();
}


////////////////////////////////////////////////////////////////////////////////
// Konstruktory, destruktor i operator przypisania


// Główna metoda konstruująca.
void Wallet::init(uint64_t units) {
    createNewUnits(units);
    updateHistory(units);
}

// Tworzy portfel z n B, gdzie n jest liczbą naturalną. Historia portfela ma
// jeden wpis.
Wallet::Wallet(long long n) {
    if (n < 0)
        throw std::invalid_argument("Attempt to create a wallet with negative BC");
    init(decimal_shift * static_cast<uint64_t>(n));
}
Wallet::Wallet(long n) : Wallet(static_cast<long long>(n)) {}
Wallet::Wallet(int n) : Wallet(static_cast<long long>(n)) {}
Wallet::Wallet(short n) : Wallet(static_cast<long long>(n)) {}

Wallet::Wallet(long long unsigned n) {
    init(decimal_shift * static_cast<uint64_t>(n));
}
Wallet::Wallet(long unsigned n) : Wallet(static_cast<long long unsigned>(n)) {}
Wallet::Wallet(int unsigned n) : Wallet(static_cast<long long unsigned>(n)) {}
Wallet::Wallet(short unsigned n) : Wallet(static_cast<long long unsigned>(n)) {}

// Tworzy pusty portfel. Historia portfela ma jeden wpis.
Wallet::Wallet() : Wallet(0ul) {}

// Tworzy portfel na podstawie napisu str określającego ilość B. Napis może
// zawierać część ułamkową (do 8 miejsc po przecinku). Część ułamkowa jest
// oddzielona przecinkiem lub kropką. Białe znaki na początku i końcu napisu
// powinny być ignorowane. Historia portfela ma jeden wpis.
Wallet::Wallet(const char *str) {
    init(::parseString(str));
}
Wallet::Wallet(const std::string &str) : Wallet(str.c_str()) {}

// Pomocniczy konstruktor przenoszący. Używany wyłącznie we właściwym
// konstruktorze przenoszącym i w Wallet::move(). Nie uaktualnia historii.
// Potrzeba istnienia konstruktora przenoszącego, który nie dodaje nowego
// wpisu do historii, wynika z tego przykładu w treści zadania:
//
// Wallet suma4 = Wallet(1) + Wallet(2);  // OK, suma4 ma dwa wpisy
//
// Użycie w operator+ zwykłego konstruktora przenoszącego skutkowałoby
// trzema wpisami w historii.
Wallet::Wallet(Wallet &&w2, bool) noexcept : history(std::move(w2.history)) {
    w2.reset();
}
// Nakładka na pomocniczy konstruktor przenoszący, ukrywająca jego wymuszony,
// nieużywany drugi argument.
Wallet Wallet::move() {
    return Wallet(std::move(*this), true);
}
// Konstruktor przenoszący. Historia operacji w1 to historia operacji w2
// i jeden nowy wpis.
Wallet::Wallet(Wallet &&w2) noexcept : Wallet(std::move(w2), true) {
    updateHistory(getUnits());
}

// Tworzy portfel, którego historia operacji to suma historii operacji w1
// i w2 plus jeden wpis, całość uporządkowana wg czasów wpisów. Po operacji
// w w0 jest w1.getUnits() + w2.getUnits() jednostek, a portfele w1 i w2 są
// puste.
Wallet::Wallet(Wallet &&w1, Wallet &&w2) noexcept {
    uint64_t sum = w1.getUnits() + w2.getUnits();
    std::merge(
            std::make_move_iterator(w1.history.begin()),
            std::make_move_iterator(w1.history.end()),
            std::make_move_iterator(w2.history.begin()),
            std::make_move_iterator(w2.history.end()),
            std::back_inserter(history));
    updateHistory(sum);
    w2.reset();
    w1.reset();
}

Wallet::~Wallet() {
    takeAllUnits();
}

// Pomocniczy konstruktor. Używany wyłącznie w Wallet::construct().
// Tworzy nowy portfel z podaną liczbą jednostek (nie z liczbą B).
Wallet::Wallet(uint64_t units, bool) {
    init(units);
}
// Nakładka na pomocniczy konstruktor Wallet(uint_64, bool),
// ukrywająca jego wymuszony, nieużywany drugi argument.
Wallet Wallet::construct(uint64_t units) {
    return Wallet(units, true);
}

// Metoda klasowa tworząca portfel na podstawie napisu str, który jest zapisem
// ilości B w systemie binarnym. Kolejność bajtów jest grubokońcówkowa
// (ang. big endian).
Wallet Wallet::fromBinary(const char *str) {
    return construct(::parseBinaryString(str));
}

// Przypisanie. Jeżeli oba obiekty są tym samym obiektem, to nic nie robi, wpp.
// historia operacji w1 to historia operacji w2 i jeden nowy wpis.
Wallet &&Wallet::operator=(Wallet &&w) noexcept {
    if (&w == this) return std::move(*this);
    takeAllUnits();
    history = std::move(w.history);
    updateHistory(getUnits());
    w.reset();
    return std::move(*this);
}


////////////////////////////////////////////////////////////////////////////////
// Operatory arytmetyczne i porównujące dla Wallet


// Dodawanie. W w2 jest 0 B po operacji i jeden nowy wpis w historii,
// a w stworzonym portfelu jest w2.getUnits() + w1.getUnits() jednostek.
Wallet operator+(Wallet &&w1, Wallet &w2) {
    w1 += w2;
    return w1.move();
}
Wallet operator+(Wallet &&w1, Wallet &&w2) { return std::move(w1) + w2; }

// Odejmowanie, analogicznie jak dodawanie, ale po odejmowaniu w w2 jest dwa
// razy więcej jednostek, niż było w w2 przed odejmowaniem.
// Historia operacji powstałego obiektu zależy od implementacji.
Wallet operator-(Wallet &&w1, Wallet &w2) {
    w1 -= w2;
    return w1.move();
}
Wallet operator-(Wallet &&w1, Wallet &&w2) { return std::move(w1) - w2; }

// Po operacji w2 ma 0 B i dodatkowy wpis w historii, a w1 ma
// w1.getUnits() + w2.getUnits() jednostek i jeden dodatkowy wpis w historii.
Wallet &&Wallet::operator+=(Wallet &w) {
    if (&w == this) return std::move(*this);
    addUnits(w.takeAllUnits());
    return std::move(*this);
}
Wallet &&Wallet::operator+=(Wallet &&w) { return operator+=(w); }

// Analogicznie do dodawania.
Wallet &&Wallet::operator-=(Wallet &w) {
    if (&w == this) return std::move(*this);
    w.addUnits(takeUnits(w.getUnits()));
    return std::move(*this);
}
Wallet &&Wallet::operator-=(Wallet &&w) { return operator-=(w); }

// Pomnożenie zawartości portfela przez liczbę naturalną.
// Dodaje jeden wpis w historii w1.
Wallet &&Wallet::operator*=(uint64_t n) {
    uint64_t units = getUnits();
    if (n == 0)
        takeAllUnits();
    else if (units > 0 && n > std::numeric_limits<uint64_t>::max() / units)
        addUnits(maxBC * decimal_shift + 1);
    else
        addUnits(units * (n - 1));
    return std::move(*this);
}

// Pomnożenie przez liczbę naturalną.
// Historia operacji powstałego obiektu zależy od implementacji.
Wallet operator*(uint64_t n, const Wallet &w) {
    uint64_t units = w.getUnits();
    if (units != 0 && n > std::numeric_limits<uint64_t>::max() / units)
        return Wallet::construct(maxBC * decimal_shift + 1);
    return Wallet::construct(n * units);
}
Wallet operator*(const Wallet &w, uint64_t n) { return operator*(n, w); }

// Operatory porównujące wartości portfeli w1 i w2.
bool operator==(const Wallet &w1, const Wallet &w2) {
    return w1.getUnits() == w2.getUnits();
}
bool operator<(const Wallet &w1, const Wallet &w2) {
    return w1.getUnits() < w2.getUnits();
}
bool operator<=(const Wallet &w1, const Wallet &w2) {
    return w1.getUnits() <= w2.getUnits();
}
bool operator!=(const Wallet &w1, const Wallet &w2) {
    return w1.getUnits() != w2.getUnits();
}
bool operator>(const Wallet &w1, const Wallet &w2) {
    return w1.getUnits() > w2.getUnits();
}
bool operator>=(const Wallet &w1, const Wallet &w2) {
    return w1.getUnits() >= w2.getUnits();
}


////////////////////////////////////////////////////////////////////////////////
// Pozostałe


// Zwraca liczbę operacji wykonanych na portfelu.
uint64_t Wallet::opSize() const {
    return history.size();
}

// Zwraca k-tą operację na portfelu. Pod indeksem 0 powinna być najstarsza
// operacja. Przypisanie do w[k] powinno być zabronione na etapie kompilacji.
const Operation &Wallet::operator[](size_t k) const {
    return history.at(k);
}

// Wypisuje "Wallet[b B]" na strumień os, gdzie b to zawartość portfela w B.
// Wypisywana liczba jest bez białych znaków, bez zer wiodących oraz zer na
// końcu w rozwinięciu dziesiętnym oraz z przecinkiem jako separatorem
// dziesiętnym.
std::ostream &operator<<(std::ostream &os, const Wallet &o) {
    os << "Wallet[";
    writeCurrency(os, o.getUnits());
    os << " B]";
    return os;
}

// Należy zaimplementować również globalną funkcję Empty(), która zwróci obiekt
// reprezentujący pusty portfel. Modyfikowanie zwróconego obiektu powinno być
// zabronione. W szczególności konstrukcja "Empty() += Wallet(1);" powinna zostać
// zgłoszone jako błąd kompilacji.
const Wallet &Empty() {
    static Wallet empty_wallet = Wallet();
    return empty_wallet;
}
