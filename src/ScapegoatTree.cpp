#include <stack>
#include <iomanip>

template<typename K>
class ScapegoatTree {
    struct Node {
        K key;
        Node* left = nullptr;
        Node* right = nullptr;

        Node(K key) : key(key) {}

        ~Node() {
            delete left;            
            delete right;
        }
    };

    Node* root = nullptr;
    int size = 0;
    int max_size = 0;
    
    //used to analysis
    int comps = 0;
    int restructs = 0;

    float alpha = 0.57;
    int h_alpha() {
        return floor(log(size) / log(1/alpha));
    }

    /**
     * @brief Finds size of a node
     * 
     * @param node 
     * @return int 
     */
    int size_of(Node* node) {
        if(node == nullptr)
            return 0;
        return (size_of(node->left) + size_of(node->right)) + 1;
    }

    /**
     * @brief Builds n number of nodes into a binary tree.
     * 
     * @param n Number of nodes to build.
     * @param x linked list of nodes, must contain n+1 nodes.
     * @return Node* to the node which points to the built tree using its left pointer.
     */
    Node* build(float n, Node* x) {
        if (n == 0.0) {
            x->left = nullptr;
            return x;
        }
        Node* r = build(ceil((n-1)/2), x);
        Node* s = build(floor((n-1)/2), r->right);
        r->right = s->left;
        s->left = r;
        return s;
    }

    /**
     * @brief Creates a linked list of two lists of Node objects and linking them together using the right pointer.
     * 
     * @param x first node of the first list.
     * @param y first node of the second list.
     * @return Node* to the first node of the list
     */
    Node* flatten(Node* x, Node* y) {
        if(x == nullptr)
            return y;
        x->right = flatten(x->right, y);
        return flatten(x->left, x);
    }


    /* used to remove left pointers in the flattened tree */
    /**
     * @brief Calls flatten and removes left pointers in the flattened list.
     * 
     * @param x first node of the first list.
     * @param y first node of the second list.
     * @return Node* to the first node of the list
     */
    Node* flatten_wrapper(Node* x, Node* y) {
        Node* n = flatten(x, y);
        Node* tmp = n;
        while(tmp) {
            tmp->left = nullptr;
            tmp = tmp->right;
        }
        return n;
    }

    /**
     * @brief Checks if the node is the right, left or none of the children of another node. 
     * 
     * @param parent 
     * @param child 
     * @return int - 0 = right child, 1 = left child, -1 not a child.
     */
    int leftOrRightChild(Node* parent, Node* child) {
        if(parent->left && parent->left->key == child->key)
            return 1;
        else if(parent->right && parent->right->key == child->key)
            return 0;
        return -1;
    }

    /**
     * @brief Get the Minimum Key.
     * 
     * @param current 
     * @return Node* to the smallest node in the tree.
     */
    Node* getMinimumKey(Node* current) {
        while (current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    /**
     * @brief Recusive function of removing a node in a binary tree. 
     *          Must be called using the public remove wrapper.
     * 
     * @param root Root of the tree.
     * @param key Key to remove.
     * @return Node* to the root of the tree.
     */
    Node* remove_recursive(Node* root, K key) {
        if(root == nullptr)
            return root;
        if(root->key > key) {
            root->left = remove_recursive(root->left, key);
            return root;
        }
        else if(root->key < key) {
            root->right = remove_recursive(root->right, key);
            return root;
        }

        if(root->left == nullptr) {
            Node* tmp = root->right;
            delete root;
            size--;
            return tmp;
        } else if(root->right == nullptr) {
            Node* tmp = root->left;
            delete root;
            size--;
            return tmp;
        } else {
            Node* succParent = root;
            Node* succ = root->right;
            while (succ->left != nullptr) {
                succParent = succ;
                succ = succ->left;
            }

            if (succParent != root)
                succParent->left = succ->right;
            else
                succParent->right = succ->right;

            root->key = succ->key;
            delete succ;
            size--;
            return root;
        }   
    }   

    public:
        ScapegoatTree(float alpha = 0.57) {
            this->alpha = alpha;
        }

        ~ScapegoatTree() {
            delete root;
        }

        /**
         * @brief Inserts a key into the binary tree and checks if the node is too deep.
         *          If the node is too deep, a it rebalances using a scapegoat node.
         * 
         * @param key 
         * @return int - 1 = success, -1 duplicate key. 
         */
        int insert(K key) {
            Node* node = new Node(key);
            Node* tmp = nullptr;
            Node* n = root;
            std::stack<Node*> ancestorStack; 
            while(n) {
                tmp = n;
                ancestorStack.push(tmp);
                if (node->key < n->key)
                    n = n->left;
                else if (node->key == n->key) {
                    delete node;
                    return -1;
                }
                else
                    n = n->right;
            }
            if(!tmp)
                root = node;
            else if(node->key < tmp->key)
                tmp->left = node;
            else if(node->key > tmp->key)
                tmp->right = node;
            size++;
            max_size = std::max(max_size, size);

            //check if too deep
            if((int)(ancestorStack.size()) > h_alpha()) {
                while(!ancestorStack.empty()) {
                    Node* n = ancestorStack.top();
                    int n_size = size_of(n);
                    //find scapegoat node
                    if(!(size_of(n->left) <= alpha * n_size && size_of(n->right) <= alpha * n_size)) {
                        ancestorStack.pop();
                        restructs++;
                        if(ancestorStack.empty()) { //root is scapegoat
                            Node* w = new Node(0);
                            root = flatten_wrapper(root, w);
                            root = build(n_size, root)->left;
                            max_size = size;
                            w->left = nullptr;
                            delete w;
                            return 1;
                        }
                        Node* ancestor = ancestorStack.top();
                        int i = leftOrRightChild(ancestor, n);

                        //Rebuild tree:
                        Node* w = new Node(0);
                        n = flatten_wrapper(n, w);
                        if(i == 1) { // left
                            ancestor->left = n;
                        } else if (i == 0) {// right
                            ancestor->right = n;
                        }
                        n = build(n_size, n)->left;
                        if(i == 1) { // left
                            ancestor->left = n;
                        } else if (i == 0) {// right
                            ancestor->right = n;
                        }
                        max_size = size;
                        w->left = nullptr;
                        delete w;
                        return 1;
                    }
                    ancestorStack.pop();
                }
            }
            return 1;
        }

        /**
         * @brief Remove wrapper. Finds the node with the matching key and deletes it using the remove_recusive function.
         * 
         * @param key 
         * @return int - 0 = fail, 1 = sucess. 
         */
        int remove(K key) {
            int tmp_size = size;
            remove_recursive(root, key);
            if(size == tmp_size) return 0;
            if(size < alpha * max_size) {
                //rebuild tree
                Node* w = new Node(0);
                root = flatten_wrapper(root, w);
                root = build(size, root)->left;
                max_size = size;
                w->left = nullptr;
                delete w;
            } 
            return 1;
        }
        
        /**
         * @brief Searches for the node with matching key.
         * 
         * @param key 
         * @return K* to the key of the node.
         */
        K* search_key(K key) {
            auto tmp = root;
            while(tmp && tmp->key != key) {
                comps += 2;
                if(key < tmp->key)
                    tmp = tmp->left;
                else
                    tmp = tmp->right;
            }
            if(tmp == nullptr)
                return nullptr;
            return &(tmp->key);
        }

        /*
        * ---- Getters:
        */
        Node* getRoot() {
            return root;
        }

        int getSize() {
            return size;
        }

        int getComps() {
            return comps;
        }

        int getRestructs() {
            return restructs;
        }

        /**
         * @brief Resets the number of comparisons done in the tree.
         *          Used for analysis.
         */
        void resetComps() {
            comps = 0;
        }

        /*
        *   ----- Pretty printing functions:
        */
        void pretty_print() {
            std::cout << "\n\n";
            postorder(root);
        }
        private:
            int placeholder = (1<<31);
            void getLine(const Node *root, int depth, std::vector<int>& vals) {
                    if (depth <= 0 && root != nullptr) {
                            vals.push_back(root->key);
                            return;
                    }
                    if (root->left != nullptr)
                            getLine(root->left, depth-1, vals);
                    else if (depth-1 <= 0)
                            vals.push_back(placeholder);
                    if (root->right != nullptr)
                            getLine(root->right, depth-1, vals);
                    else if (depth-1 <= 0)
                            vals.push_back(placeholder);
            }

            void printRow(const Node *p, const int height, int depth) {
                std::vector<int> vec;
                getLine(p, depth, vec);
                std::cout << std::setw((height - depth)*2); // scale setw with depth
                bool toggle = true; // start with left
                if (vec.size() > 1) {
                        for (int v : vec) {
                                if (v != placeholder) {
                                        if (toggle)
                                                std::cout << "/" << "   ";
                                        else
                                                std::cout << "\\" << "   ";
                                }
                                toggle = !toggle;
                        }
                        std::cout << std::endl;
                        std::cout << std::setw((height - depth)*2);
                }
                for (int v : vec) {
                        if (v != placeholder)
                                std::cout << v << "   ";
                }
                std::cout << std::endl;
            }

            /**
            * @brief Returns height of a node
            * 
            * @param node 
            * @return int 
            */
            int height_of(Node* node) {
                if(node == nullptr)
                    return 0;
                return std::max(height_of(node->left), height_of(node->right)) + 1;
            }

            void postorder(Node *p) {
                int height = height_of(p) * 2;
                for (int i = 0 ; i < height; i ++) {
                    printRow(p, height, i);
                }
            }
};