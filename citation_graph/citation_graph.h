#ifndef CITATION_GRAPH_H_
#define CITATION_GRAPH_H_

#include <memory>
#include <map>
#include <set>
#include <vector>

struct PublicationNotFound : std::exception {
    char const *what() const noexcept {
        return "PublicationNotFound";
    }
};

struct PublicationAlreadyCreated : std::exception {
    char const *what() const noexcept {
        return "PublicationAlreadyCreated";
    }
};

struct TriedToRemoveRoot : std::exception {
    char const *what() const noexcept {
        return "TriedToRemoveRoot";
    }
};

template<class Publication>
class CitationGraph {

    struct Node;
    using Id = typename Publication::id_type;
    using Index = std::map<Id, Node *>;

    // The index and the root are allocated dynamically, because otherwise
    // move constructors would be impossible. The index can't change its
    // location in memory, because nodes keep pointers to it, and moving
    // the root would be unsafe, because it keeps a Publication member.
    std::shared_ptr<Index> index;
    std::unique_ptr<Node> root;

    struct Node : std::enable_shared_from_this<Node> {
        Publication publication;
        std::set<std::shared_ptr<Node>> children;
        std::set<Node *> parents;

        // Every node keeps an iterator to its own entry in the index,
        // so that it can safely erase that entry at destruction.
        std::shared_ptr<Index> index = nullptr;
        typename Index::iterator indexEntry;

        Node(Id const &id, std::shared_ptr<Index> index) :
                publication(id),
                index(index) {
            auto[entry, success] = index->emplace(id, this);
            if (!success)
                throw PublicationAlreadyCreated();
            indexEntry = entry;
        }

        ~Node() {
            // A node's destruction happens iff it has no remaining parents,
            // so the only remaining pointers to it are in its children and
            // in the map.
            index->erase(indexEntry);
            for (std::shared_ptr<Node> child : children)
                child->parents.erase(this);
            // All our shared pointers to children are destroyed right here.
            // If this node was their last parent, chain destruction will occur.
        }

        // Adds an incoming edge (from parent to *this).
        void attach(Node &parent) {
            auto[handle, success] = parents.insert(&parent);
            if (!success)
                return; // Edge already exists, so nothing is changed.
            try {
                parent.children.emplace(this->shared_from_this());
            } catch (...) {
                parents.erase(handle);
                throw;
            }
        }

        // Removes all incoming edges.
        void detach() noexcept {
            std::shared_ptr<Node> self = this->shared_from_this();
            for (Node *parent : parents)
                parent->children.erase(self);
            // No shared pointers to this node remain in the graph now.
        }
    };

    Node& get_node(Id const &id) const {
        auto it = index->find(id);
        if (it == index->end())
            throw PublicationNotFound();
        return *it->second;
    }

public:

    CitationGraph(Id const &stem_id) :
            index(std::make_shared<Index>()),
            root(std::make_unique<Node>(stem_id, index)) {}

    CitationGraph(CitationGraph<Publication> &&other) noexcept :
            index(std::move(other.index)),
            root(std::move(other.root)) {}

    CitationGraph<Publication>& operator=(CitationGraph<Publication> &&other) noexcept {
        std::swap(index, other.index);
        std::swap(root, other.root);
        return *this;
    }

    Id get_root_id() const noexcept(noexcept(root->publication.get_id())) {
        return root->publication.get_id();
    }

    std::vector<Id> get_children(Id const &id) const {
        std::vector<Id> ans;
        for (std::shared_ptr<Node> child : get_node(id).children)
            ans.push_back(child->publication.get_id());
        return ans;
    }

    std::vector<Id> get_parents(Id const &id) const {
        std::vector<Id> ans;
        for (Node *parent : get_node(id).parents)
            ans.push_back(parent->publication.get_id());
        return ans;
    }

    bool exists(Id const &id) const {
        return index->find(id) != index->end();
    }

    Publication &operator[](Id const &id) const {
        return get_node(id).publication;
    }

    void create(Id const &id, Id const &parent_id) {
        std::shared_ptr<Node> newNode = std::make_shared<Node>(id, index);
        newNode->attach(get_node(parent_id));
    }

    void create(Id const &id, std::vector<Id> const &parent_ids) {
        if (parent_ids.empty())
            throw PublicationNotFound();
        std::shared_ptr<Node> newNode = std::make_shared<Node>(id, index);
        try {
            for (Id const &parent_id : parent_ids)
                newNode->attach(get_node(parent_id));
        } catch (...) {
            // Failed to add an edge. Delete all edges added so far.
            newNode->detach();
            throw;
        }
    }

    void add_citation(Id const &child_id, Id const &parent_id) {
        get_node(child_id).attach(get_node(parent_id));
    }

    void remove(Id const &id) {
        Node &node = get_node(id);
        if (&node == root.get())
            throw TriedToRemoveRoot();
        node.detach();
        // node is destroyed here.
    }
};

#endif // CITATION_GRAPH_H_
