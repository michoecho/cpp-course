#include <regex>
#include <algorithm>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>

// Regex definitions. LINE describes the syntax of a valid input line.
#define NUMBER "(0|[1-9][0-9]{0,8})"
#define TYPE "[A-Z0-9][a-zA-Z0-9,/-]*"
#define SPC "\\s+"
#define LINE_DRCE "^\\s*" "[DRCE]" NUMBER SPC TYPE SPC NUMBER SPC NUMBER "\\s*$"
#define LINE_T "^\\s*" "T" NUMBER SPC TYPE SPC NUMBER SPC NUMBER SPC NUMBER "\\s*$"
#define LINE LINE_DRCE "|" LINE_T

using namespace std;

// For each category, we store its elements grouped by type.
// section_t maps a type to the set of elements of that type.
using section_t = unordered_map<string, set<int>>;

// The database of elements is divided into sections by category.
// database_t maps a category symbol to the corresponding section_t.
using database_t = unordered_map<char, section_t>;

// We use a separate container to check for duplicate IDs in each category.
// duplicate_map_t maps a category (symbol) to the set of all its elements.
using duplicate_map_t = unordered_map<char, unordered_set<int>>;

// connection_counter_t maps a node to the number of elements connected to it.
using connection_counter_t = map<int, int>;

void print_error(int line_num, string &line) {
    cerr << "Error in line " << line_num << ": " << line << endl;
}

// If "line" is a valid input line, add a new element to the database
// (and helper containers) and returns true. Else, it returns false.
bool process_line(database_t &database,
                  connection_counter_t &connection_counter,
                  duplicate_map_t &duplicate_checker,
                  string &line) {

    // Ignore empty lines.
    if (line.empty()) {
        return true;
    }

    // Validate syntax.
    static const regex valid_line(LINE);
    if (!regex_match(line, valid_line)) {
        return false;
    }

    // Parse line.
    string type;
    int id, node1, node2, node3; //node3 is unused if category is not 'T'.
    char category;
    stringstream(line) >> category >> id >> type >> node1 >> node2 >> node3;

    // The element must be connected to at least two different nodes.
    // Check node3 only if category is 'T'.
    bool all_nodes_equal = (node1 == node2 && (category != 'T' || node2 == node3));
    if (all_nodes_equal) {
        return false;
    }

    // Try to add the element to duplicate_checker.
    // If an element of the same category and ID had already been added,
    // line is invalid. Else, line is confirmed valid.
    bool duplicate = !duplicate_checker[category].insert(id).second;
    if (duplicate) {
        return false;
    }

    // line is now confirmed valid.
    // Add the new element to database.
    database[category][type].insert(id);

    // Increment the node connection counter. Make sure that each counter is
    // incremented only once, even if some nodes are equal.
    connection_counter[node1]++;
    if (node2 != node1) {
        connection_counter[node2]++;
    }
    if (category == 'T' && node3 != node1 && node3 != node2) {
        connection_counter[node3]++;
    }

    return true;
}

// Prints all elements of given category.
// Referencing elements in section after this function is called
// is undefined behavior.
void print_category(char category, section_t &section) {
    // The type stored inside section_t.
    using record = pair<string, set<int>>;

    // Sort records by smallest contained ID.
    vector<record> sorted;
    move(section.begin(), section.end(), back_inserter(sorted));
    auto comparator = [](record const &a, record const &b) {
        // ID sets are ordered, so their first elements are also the smallest.
        return *a.second.begin() <= *b.second.begin();
    };
    sort(sorted.begin(), sorted.end(), comparator);

    for (auto const &[type, id_set]: sorted) {
        char const *separator = ""; // No separator before first element.
        for (auto const &item : id_set) {
            cout << separator << category << item;
            separator = ", ";
        }
        cout << ": " << type << endl;
    }
}

// Prints the warning about unconnected nodes, if they exist.
void print_warning(connection_counter_t &connection_counter) {
    char static const *warning = "Warning, unconnected node(s): ";
    char const *separator = warning; // Print warning text before elements.

    for (auto const &[node, connections] : connection_counter) {
        if (connections < 2) {
            cerr << separator << node;
            separator = ", ";
        }
    }

    // If anything was printed, finish the line.
    if (separator != warning) {
        cerr << endl;
    }
}

int main() {
    database_t database;
    duplicate_map_t duplicate_checker;
    connection_counter_t connection_counter;

    // Ground node is always present.
    connection_counter[0] = 0;

    string line;
    int line_num = 0;

    // Fill database.
    while (getline(cin, line)) {
        ++line_num;

        if (!process_line(database, connection_counter, duplicate_checker, line)) {
            print_error(line_num, line);
        }
    }

    // Print database.
    for (char category : "TDRCE") {
        print_category(category, database[category]);
    }

    print_warning(connection_counter);

    return 0;
}

