#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <string.h>

#include "SkipList.cpp"
#include "ScapegoatTree.cpp"

template<typename K, typename V>
void SList(Skiplist<K, V>& list);
template<typename K>
void SGtree(ScapegoatTree<K>& tree);
template<typename K, typename V>
void SListAnalysis(Skiplist<K, V>& list, int n);
template<typename K>
void SGTAnalysis(ScapegoatTree<K>& tree, int n);
std::vector<std::string> tokenize(std::string s, std::string del);

int main(int argc, char **argv) {
    if(argc == 1) {
        std::cout << "Must give command line arguments!" << std::endl;
        return -1;
    }
    //Skiplist commands
    if(strcmp(argv[1], "SL") == 0) {
        if(argc == 2) { //Skiplist int, int, 32 default
            Skiplist<int, int> list (32);
            SList<int, int>(list);
        } else if(argc == 3) {
            Skiplist<int, int> list (32, atof(argv[2]));
            SList<int, int>(list);
        } else if(argc > 4) {
            Skiplist<int, int> list (32, 0.5);
            //insert n elements
            for(int j = 0; j < atoi(argv[2]); j++) {
                list.insert(j, 1);
            }   
            for(int i = 0; i < atoi(argv[3]); i++) {
                list.resetComps();
                SListAnalysis<int, int>(list, atoi(argv[2]));     
            }
        }
    }
    //Scapegoat tree commands
    if(strcmp(argv[1], "SGT") == 0) {
        if(argc == 2) { //ScapegoatTree int
            ScapegoatTree<int> tree;
            SGtree<int>(tree);
        } else if (argc == 3) {
            ScapegoatTree<int> tree (atof(argv[2]));
            SGtree<int>(tree);
        } else if(argc > 4) {
            ScapegoatTree<int> tree (0.60);   
            for(int j = 0; j < atoi(argv[2]); j++) {
                tree.insert((int) (((double) std::rand() / (RAND_MAX)) * atoi(argv[2])));
            }
            for(int i = 0; i < atoi(argv[3]); i++) {
                SGTAnalysis(tree, atoi(argv[2]));
                tree.resetComps();
            }
        }
    }
    std::cout << std::flush;
}

/**
 * @brief Used to search for random nodes in the scapegoat tree. Prints information about the size, 
 *        number of comparisons and amount of rebuilds the tree has gone through.
 * 
 * @tparam K 
 * @param tree Scapegoat tree object reference.
 * @param n  Amount of nodes in the scapegoat tree.
 */
template<typename K>
void SGTAnalysis(ScapegoatTree<K>& tree, int n) {
    auto s1 = (int) (((double) std::rand() / (RAND_MAX)) * n);
    tree.search_key(s1);
    std::cout << "Tree size: " << n << " - Searching for: " << s1 << " - Comparisons: " << tree.getComps() << " - Restructs: " << tree.getRestructs() << std::endl;
    std::cout << std::flush;
}

/**
 * @brief Used to find random elements in the skip list. Prints information about size
 *         and number of comparisons.
 * 
 * @tparam K 
 * @tparam V 
 * @param list Skiplist object reference.
 * @param n Amount of elements in the list.
 */
template<typename K, typename V>
void SListAnalysis(Skiplist<K, V> &list, int n) {
    auto s1 = (int) (((double) std::rand() / (RAND_MAX)) * n);
    list.search(s1);
    std::cout << "List size: " << n << " - Searching for: " << s1 << " - Comparisons: " << list.getComps() << std::endl;
    std::cout << std::flush;
}

/**
 * @brief Takes the stdin/cin and tokenizes the lines so the correct operation is
 *         carried out on the data structure. 
 *         Each operation returns an int which indicates the result. The result is printed to stdout/cout.
 *         Lastly the whole tree is printed using the 'pretty_printing' function.
 * @tparam K 
 * @param tree Scapegoat tree object reference.
 */
template<typename K>
void SGtree(ScapegoatTree<K>& tree) {
    for (std::string line; std::getline(std::cin, line);) { // parse each line
        std::vector<std::string> command = tokenize(line, " ");
        int key = std::stoi(command.at(1));

        if(command.at(0) == "I") { // Inserting
            int f = tree.insert(key);
            if(f == 1)
                std::cout << "S" << std::endl;
            else if (f == -1)
                std::cout << "F - Duplicate key" << std::endl;
            else 
                std::cout << "F - Could not insert key" << std::endl;
        }
        if(command.at(0) == "D") { // Deleting
            int f = tree.remove(key);
            if(f == 1)
                std::cout << "S" << std::endl;
            else 
                std::cout << "F - Key not found" << std::endl;
        }
        if(command.at(0) == "S") { // Searching
            K* v = tree.search_key(key);
            if(v)
                std::cout << "S" << std::endl;
            else 
                std::cout << "F - Key not found" << std::endl;
        }
        std::cout << std::flush;
    }

    std::cout << "\n";
    tree.pretty_print();
}

/**
 * @brief Takes the stdin/cin and tokenizes the lines so the correct operation is
 *         carried out on the data structure.
 *         Each operation returns an int which indicates the result. The result is printed to stdout/cout.
 *         Lastly the whole list is printed using the 'print_keys_only' function.
 * 
 * @tparam K 
 * @tparam V 
 * @param list Skiplist object reference.
 */
template<typename K, typename V>
void SList(Skiplist<K, V>& list) {
    for (std::string line; std::getline(std::cin, line);) { // parse each line
        std::vector<std::string> command = tokenize(line, " ");
        int key = std::stoi(command.at(1));

        if(command.at(0) == "I") { // Inserting
            int f = list.insert(key, 1);
            if(f == 0)
                std::cout << "S" << std::endl;
            else if (f == 1)
                std::cout << "F - Duplicate key" << std::endl;
            else 
                std::cout << "F - Could not insert key" << std::endl;
        }
        if(command.at(0) == "D") { // Deleting
            bool f = list.remove(key);
            if(f)
                std::cout << "S" << std::endl;
            else 
                std::cout << "F - Key not found" << std::endl;
        }
        if(command.at(0) == "S") { // Searching
            V* v = list.search(key);
            if(v)
                std::cout << "S" << std::endl;
            else 
                std::cout << "F - Key not found" << std::endl;
        }
        std::cout << std::flush;
    }

    std::cout << "\n";
    list.print_keys_only();
}

/**
 * @brief Tokenizes the input string using the delimiter.
 * 
 * @param s String to tokenize.
 * @param del Delimiter.
 * @return std::vector<std::string> An array of string tokens.
 */
std::vector<std::string> tokenize(std::string s, std::string del)
{
    std::vector<std::string> res;
    int start = 0;
    int end = s.find(del);
    while (end != -1) {
        res.push_back(s.substr(start, end - start));
        start = end + del.size();
        end = s.find(del, start);
    }
    res.push_back(s.substr(start, end - start));
    return res;
}