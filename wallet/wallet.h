#ifndef WALLET_H
#define WALLET_H


#include <chrono>
#include <ostream>
#include <string>
#include <vector>


class Operation {

    // Liczba jednostek po operacji.
    uint64_t units;
    // Czas wykonania operacji.
    std::chrono::system_clock::time_point timestamp;

    friend class Wallet;

    Operation(uint64_t units);

public:

    // Zwraca liczbę jednostek w portfelu po operacji.
    uint64_t getUnits() const;

    // Operatory porównujące czas utworzenia (z dokładnością do milisekund)
    // operacji o1 i o2, gdzie op to jeden z: ==, <, <=, != , >, >=.
    bool operator<(const Operation &o) const;
    bool operator>(const Operation &o) const;
    bool operator==(const Operation &o) const;
    bool operator<=(const Operation &o) const;
    bool operator!=(const Operation &o) const;
    bool operator>=(const Operation &o) const;

    friend std::ostream &operator<<(std::ostream &os, const Operation &o);

};

// Wypisuje na strumień os "Wallet balance is b B after operation made at day d".
// Liczba b jak przy wypisywaniu portfela. Czas d w formacie yyyy-mm-dd.
std::ostream &operator<<(std::ostream &os, Operation &o);


class Wallet {

    // Lista operacji na portfelu, uporządkowana chronologicznie.
    std::vector<Operation> history;

    void updateHistory(uint64_t units);

    uint64_t takeAllUnits();

    uint64_t takeUnits(uint64_t amount);

    uint64_t addUnits(uint64_t units);

    void reset();

    void init(uint64_t units);

    static Wallet construct(uint64_t units);

    Wallet move();

    Wallet(uint64_t units, bool);

    Wallet(Wallet &&w, bool) noexcept;


public:

    ~Wallet();

    // Tworzy pusty portfel. Historia portfela ma jeden wpis.
    Wallet();

    // Tworzy portfel z n B, gdzie n jest liczbą naturalną. Historia portfela ma
    // jeden wpis.
    Wallet(long long n);
    Wallet(long n);
    Wallet(int n);
    Wallet(short n);
    Wallet(long long unsigned n);
    Wallet(long unsigned n);
    Wallet(int unsigned n);
    Wallet(short unsigned n);

    // Tworzy portfel na podstawie napisu str określającego ilość B. Napis może
    // zawierać część ułamkową (do 8 miejsc po przecinku). Część ułamkowa jest
    // oddzielona przecinkiem lub kropką. Białe znaki na początku i końcu napisu
    // powinny być ignorowane. Historia portfela ma jeden wpis.
    explicit Wallet(const char *str);

    explicit Wallet(const std::string &str);

    template<typename T>
    Wallet(T) = delete;

    // Konstruktor przenoszący. Historia operacji w1 to historia operacji w2
    // i jeden nowy wpis.
    Wallet(Wallet &&w2) noexcept;

    // Tworzy portfel, którego historia operacji to suma historii operacji w1
    // i w2 plus jeden wpis, całość uporządkowana wg czasów wpisów. Po operacji
    // w w0 jest w1.getUnits() + w2.getUnits() jednostek, a portfele w1 i w2 są
    // puste.
    Wallet(Wallet &&w1, Wallet &&w2) noexcept;

    // Metoda klasowa tworząca portfel na podstawie napisu str, który jest zapisem
    // ilości B w systemie binarnym. Kolejność bajtów jest grubokońcówkowa
    // (ang. big endian).
    static Wallet fromBinary(const char *str);

    // Przypisanie. Jeżeli oba obiekty są tym samym obiektem, to nic nie robi, wpp.
    // historia operacji w1 to historia operacji w2 i jeden nowy wpis. Dostępne
    // jest tylko przypisanie przenoszące, nie przypisanie kopiujące, np.
    // Wallet w1, w2;
    // w1 = Wallet(1); // OK
    // w1 = w2; // błąd kompilacji
    Wallet &&operator=(Wallet &&w) noexcept;

    // Dodawanie, np.
    // w1 + w2
    // Wallet suma2 = Wallet(2) + w2; // OK, w w2 jest 0 B po operacji
    //                                // i jeden nowy wpis w historii,
    //                                // a w suma2 jest w2.getUnits() + 2 B.
    //                                // Historia operacji powstałego obiektu
    //                                // zależy od implementacji.
    // Wallet suma3 = suma1 + suma2;  // błąd kompilacji
    // Wallet suma4 = Wallet(1) + Wallet(2);  // OK, suma4 ma dwa wpisy
    //                                        // w historii i 3 B
    friend Wallet operator+(Wallet &&w1, Wallet &w2);
    friend Wallet operator+(Wallet &&w1, Wallet &&w2);

    // Odejmowanie, analogicznie jak dodawanie, ale po odejmowaniu w w2 jest dwa
    // razy więcej jednostek, niż było w w2 przed odejmowaniem.
    // Historia operacji powstałego obiektu zależy od implementacji.
    friend Wallet operator-(Wallet &&w1, Wallet &w2);
    friend Wallet operator-(Wallet &&w1, Wallet &&w2);

    // Po operacji w2 ma 0 B i dodatkowy wpis w historii, a w1 ma
    // w1.getUnits() + w2.getUnits() jednostek i jeden dodatkowy wpis w historii.
    Wallet &&operator+=(Wallet &w);
    Wallet &&operator+=(Wallet &&w);


    // Analogicznie do dodawania.
    Wallet &&operator-=(Wallet &w);
    Wallet &&operator-=(Wallet &&w);


    // Pomnożenie zawartości portfela przez liczbę naturalną.
    // Dodaje jeden wpis w historii w1.
    Wallet &&operator*=(uint64_t n);

    friend Wallet operator*(uint64_t n, const Wallet &w);
    friend Wallet operator*(const Wallet &w, uint64_t n);

    // Zwraca liczbę jednostek w portfelu.
    uint64_t getUnits() const;

    // Zwraca liczbę operacji wykonanych na portfelu.
    size_t opSize() const;

    // Zwraca k-tą operację na portfelu. Pod indeksem 0 powinna być najstarsza
    // operacja. Przypisanie do w[k] powinno być zabronione na etapie kompilacji.
    const Operation &operator[](size_t k) const;

};

// Pomnożenie przez liczbę naturalną.
// Historia operacji powstałego obiektu zależy od implementacji.
Wallet operator*(uint64_t n, const Wallet &w);
Wallet operator*(const Wallet &w, uint64_t n);

// Operatory porównujące wartości portfeli w1 i w2.
bool operator==(const Wallet &w1, const Wallet &w2);
bool operator<(const Wallet &w1, const Wallet &w2);
bool operator<=(const Wallet &w1, const Wallet &w2);
bool operator!=(const Wallet &w1, const Wallet &w2);
bool operator>(const Wallet &w1, const Wallet &w2);
bool operator>=(const Wallet &w1, const Wallet &w2);

// Wypisuje "Wallet[b B]" na strumień os, gdzie b to zawartość portfela w B.
// Wypisywana liczba jest bez białych znaków, bez zer wiodących oraz zer na
// końcu w rozwinięciu dziesiętnym oraz z przecinkiem jako separatorem
// dziesiętnym.
std::ostream &operator<<(std::ostream &os, const Wallet &b);

// Należy zaimplementować również globalną funkcję Empty(), która zwróci obiekt
// reprezentujący pusty portfel. Modyfikowanie zwróconego obiektu powinno być
// zabronione. W szczególności konstrukcja "Empty() += Wallet(1);" powinna zostać
// zgłoszone jako błąd kompilacji.
const Wallet &Empty();


#endif // WALLET_H
