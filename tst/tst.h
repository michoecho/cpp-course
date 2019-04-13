#ifndef TST_H_INCLUDED
#define TST_H_INCLUDED

#include <memory>
#include <stdexcept>
#include <cstring>
#include <iterator>

namespace Detail {
// Funkcja fold na zakresie wyznaczonym przez iteratory działa następująco:
//   functor(...functor(functor(acc, *first), *(first + 1))..., *(last - 1))
// W szczególnym przypadku first == last fold zwraca acc.
template<typename Iter, typename Acc, typename Functor>
Acc fold(Iter first, Iter last, Acc acc, Functor functor)
{
  return first == last ? acc : fold(std::next(first), last, functor(acc, *first), functor);
}
}

// Ternary Search Tree
template<typename C = char>
class TST
{
  struct Node {
    const C value;
    const bool word;
    const std::shared_ptr<const Node> left;
    const std::shared_ptr<const Node> center;
    const std::shared_ptr<const Node> right;

    Node (
      const C value,
      const bool word,
      const std::shared_ptr<const Node> &left,
      const std::shared_ptr<const Node> &center,
      const std::shared_ptr<const Node> &right
    ) : value(value), word(word), left(left), center(center), right(right) {}
  };

  std::shared_ptr<const Node> root;

  TST(const C value, const bool word, const TST &left, const TST &center, const TST &right)
      : root(std::make_shared<const Node>(value, word, left.root, center.root, right.root)) {}
  TST(const std::shared_ptr<const Node> &node) : root(node) {}

  // Wyszukuje najdłuższy wspólny prefiks słowa str i słów zawartych w
  // drzewie, po czym zwraca jego długość.
  size_t prefix_len(const C* str) const {
    return
      (empty() || !str[0]) ? (
        0
      ) : (str[0] < value()) ? (
        left().prefix_len(str)
      ) : (str[0] > value()) ? (
        right().prefix_len(str)
      ) : (
        1 + center().prefix_len(str + 1)
      );
  }

public:

  // Tworzy puste drzewo.
  TST() = default;

  // Tworzy drzewo zawierające jedynie słowo str.
  TST(const std::basic_string<C>& str) : TST(str.c_str()) {}

  // Tworzy drzewo zawierające jedynie słowo str.
  TST(const C* str) : TST(!str[0] ? TST() : TST(str[0], !str[1], TST(), TST(str + 1), TST())) {}

  // Dodaje słowo str do drzewa.
  TST operator+(const std::basic_string<C>& str) const {
    return operator+(str.c_str());
  }

  // Dodaje słowo str do drzewa.
  TST operator+(const C* str) const {
    return
      (!str[0]) ? (
        TST(*this)
      ) : (empty()) ? (
        TST(str)
      ) : (str[0] < value()) ? (
        TST(value(), word(), left() + str, center(), right())
      ) : (str[0] > value()) ? (
        TST(value(), word(), left(), center(), right() + str)
      ) : (
        TST(value(), word() || (!str[1]), left(), center() + (str + 1), right())
      );
  }

  // Znak zapisany w korzeniu.
  C value() const {
    return !empty() ? root->value : throw std::logic_error("Empty tree");
  }

  // Czy korzeń jest oznaczony jako węzeł kończący słowo?
  bool word() const {
    return !empty() ? root->word : throw std::logic_error("Empty tree");
  }

  // Prawe poddrzewo.
  TST left() const {
    return !empty() ? TST(root->left) : throw std::logic_error("Empty tree");
  }

  // Środkowe poddrzewo.
  TST center() const {
    return !empty() ? TST(root->center) : throw std::logic_error("Empty tree");
  }

  // Prawe poddrzewo.
  TST right() const {
    return !empty() ? TST(root->right) : throw std::logic_error("Empty tree");
  }

  bool empty() const {
    return root == nullptr;
  }

  // Czy słowo str jest zawarte w drzewie?
  bool exist(const std::basic_string<C>& str) const {
    return exist(str.c_str());
  }

  // Czy słowo str jest zawarte w drzewie?
  bool exist(const C* str) const {
    return
      (empty() || !str[0]) ? (
        false
      ) : (str[0] < value()) ? (
        left().exist(str)
      ) : (str[0] > value()) ? (
        right().exist(str)
      ) : (str[1]) ? (
        center().exist(str + 1)
      ) : (
        word()
      );
  }

  // Wyszukuje najdłuższy wspólny prefiks słowa str i słów zawartych w danym
  // drzewie. Przykład: jeśli tst składa się ze słów "category", "functor"
  // oraz "theory" to tst.prefix("catamorphism") daje rezultat "cat".
  std::basic_string<C> prefix(const std::basic_string<C>& str) const {
    return str.substr(0, prefix_len(str.c_str()));
  }

  // Rekurencyjnie akumuluje wartość w korzeniu i wartości w poddrzewach
  // w sposób definiowany przez functor.
  template<typename Acc, typename Functor>
  Acc fold(Acc acc, Functor functor) const {
    return
      (empty()) ? (
        acc
      ) : (
        functor(left().fold(center().fold(right().fold(acc, functor), functor), functor), value())
      );
  }

  // Liczba węzłów w drzewie.
  size_t size() const
  {
    return this->fold(0, [](size_t acc, C){return 1 + acc;});
  }
};

#endif // TST_H_INCLUDED
