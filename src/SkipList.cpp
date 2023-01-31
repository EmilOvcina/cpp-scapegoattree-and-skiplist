#include <vector>

template <typename K, typename T>
class Skiplist {

    int size = 0;
    float probability = 0.5;
    int MAXLEVEL = 0;
    int levelCap;

    struct Node {
        K key;
        T data;
        std::vector<Node*> next;

        ~Node() {
            if(next.at(0))
                delete next.at(0);
        }
    };

    Node* head;

    //used for analysis
    int comps = 0;

    /**
     * @brief l(size) function from the article. Used to determine the amount of levels for lists.
     */
    float l() {
        return log(size) / log(1/probability);
    }

    /**
     * @brief Create a Node object.
     * 
     * @param level level of created node.
     * @return Node* 
     */
    Node* createNode(int level) {
        Node* node = new Node();
        node->next = std::vector<Node*> (level + 1);
        return node;
    }

    /**
     * @brief Create a Node object with parameters.
     * 
     * @param key 
     * @param data 
     * @param level 
     * @return Node* 
     */
    Node* createNode(K key, T data, int level) {
        Node* node = new Node();
        node->data = data;
        node->key = key;
        node->next = std::vector<Node*> (level + 1);
        return node;
    }

    /**
     * @brief Generates a random level for a node
     * @return random level - int.
     */
    int randomLevel() {
        int level = 0;
        float r = ((double) std::rand() / (RAND_MAX));
        while(r < probability && level < levelCap) {
            level++;
        }
        return level;
    }

    /**
     * @brief Increases the max level of the list. Should only be called when inserting.
     * 
     */
    void increaseMaxLevel() {
        int level = MAXLEVEL;
        Node* p = head;
        Node* q = p->next.at(level);

        while(q != nullptr) {
            if((int)(q->next.size() - 1) > level) {
                p->next.resize(level + 2);
                p->next.at(level + 1) = q;
                p = q;
            }
            q = q->next.at(level);
        }        
        p->next.resize(level + 2);
        p->next.at(level + 1) = nullptr;
        MAXLEVEL++;
        head->next.resize(MAXLEVEL + 1);
        head->next.at(MAXLEVEL) = p;
    }

    public:
        Skiplist(int levelCap, float probability=0.5) {
            // std::srand(time(NULL)); // used to get unique random seed for later calls.
            this->levelCap = levelCap;
            this->probability=probability;
            //Creates a head node with no key/value.
            head = createNode(MAXLEVEL);
        };

        ~Skiplist() {
            delete head;
        }

        /**
         * @brief Inserts a node into the skiplist and increasing maxlevel if necessary.
         * 
         * @param key 
         * @param data 
         * @return int result of operation (-1, 0, 1).
         */
        int insert(K key, T data) {
            std::vector<Node*> update (levelCap);

            Node* current = head;

            // Find the place to insert:
            for(int i = MAXLEVEL; i >= 0; i--) {
                if(current != nullptr) {
                    while(current->next.at(i) != nullptr && current->next.at(i)->key < key) { 
                        current = current->next.at(i);
                    }
                }
                update.at(i) = current;
            }
            if(current != nullptr)
                current = current->next.at(0);

            // update value of key if it already exists
            if(current != nullptr && key == current->key) {
                current->data = data;
                return 1;
            } else {
                int generatedLevel = randomLevel();
                Node* node = createNode(key, data, generatedLevel);
                if(head->next.at(0) == nullptr) { // empty list
                    for(int i = node->next.size()-1; i >= 0; i--)
                        head->next.at(i) = node;
                } else {
                    for(int i = 0; i <= std::min(generatedLevel, MAXLEVEL); i++) {    
                        node->next.at(i) = update.at(i)->next.at(i);
                        update.at(i)->next.at(i) = node;
                    }
                }
                size++;
                // check to see if maxlevel should increase.
                if(floor(l()) > MAXLEVEL + 1)
                    increaseMaxLevel();

                return 0;
            }
            return -1;
        }

        /**
         * @brief Removes a key from skiplist. 
         * 
         * @param key 
         * @return true if element was removed
         * @return false if the element was not found.
         */
        bool remove(K key) {
            std::vector<Node*> update (levelCap);
            Node* current = head; 

            // Find the place to insert:
            for(int i = MAXLEVEL; i >= 0; i--) {
                if(current != nullptr) {
                    while(current->next.at(i) != nullptr && current->next.at(i)->key < key) { 
                        current = current->next.at(i);
                    }
                }
                update.at(i) = current;
            }
            if(current != nullptr)
                current = current->next.at(0);

            if(current != nullptr && current->key == key) {
                int j = std::min(((int)(current->next.size())-1), MAXLEVEL);
                for(int i = 0; i <= j; i++) {
                    update.at(i)->next.at(i) = current->next.at(i);
                }
                for(int i = 0; i < (int)current->next.size(); i++)
                    current->next.at(i) = nullptr;
                delete current;
                size--;
                if(size > 0 && MAXLEVEL > 0)
                    if(ceil(l()) < MAXLEVEL + 1)
                        MAXLEVEL--;
                return true;
            }
            return false;
        }

        /**
         * @brief Searches the skiplist for a key and returns its value.
         * 
         * @param key 
         * @return T* or null if no element with key was found.
         */
        T* search(K key) {
            Node* current = head;
            for(int i = MAXLEVEL; i >= 0; i--) {
                if(current != nullptr) {
                    while(current->next.at(i) != nullptr) { 
                        comps++;
                        if(!(current->next.at(i)->key < key))
                            break;
                        current = current->next.at(i);
                    }
                }
            }   
            if(current != nullptr)
                current = current->next.at(0);

            comps++;
            if(current != nullptr && current->key == key)
                return &(current->data);
            else
                return nullptr;
        }

        /**
         * @brief Get the number of comparisons.
         * @return int 
         */
        int getComps() {
            return comps;
        }

        /**
         * @brief Resets the comparison counter.
         */
        void resetComps() {
            comps = 0;
        }

        /**
         * @brief Get the size of skiplist.
         * @return int 
         */
        int getSize() {
            return size;
        }

        /**
         * @brief Prints the skiplist in layers, like shown in the article.
         *          Prints key|value 
         */
        void print() {
            for (int i = MAXLEVEL; i >= 0; i--) {
                if(head->next.at(i) != nullptr) {
                    std::cout << "Layer "<< i+1 <<": ";
                    for (Node* node = head->next.at(i); node != nullptr; node = node->next.at(i)) {
                        std::cout << "<" << node->key <<"|"<< node->data << "> -> ";
                    }
                    std::cout << "NULL\n";
                }
            }
            std::cout << " " << std::endl;
        }

        /**
         * @brief Prints the skiplist in layers, like shown in the article.
         *          Prints keys only.
         */
        void print_keys_only() {
            for (int i = MAXLEVEL; i >= 0; i--) {
                if(head->next.at(i) != nullptr) {
                    std::cout << "Layer "<< i+1 <<": ";
                    for (Node* node = head->next.at(i); node != nullptr; node = node->next.at(i)) {
                        std::cout << "<" << node->key << "> -> ";
                    }
                    std::cout << "NULL\n";
                }
            }
            std::cout << " " << std::endl;
        }
};