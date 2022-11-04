#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
using namespace std;

using Key = int;

class Node {
public:
    virtual bool isLeafNode() = 0;
    virtual void split() = 0;
    Node* parent = nullptr;
    vector<Key> keys;
    int size() { return keys.size(); }
    virtual void insert(Key key) = 0;
    priority_queue<Key> clear() {
        priority_queue<Key> data_copy;
        while (keys.size()) data_copy.push(-keys[keys.size() - 1]), keys.pop_back();

        return data_copy;
    }
};

class InternalNode : public Node {
public:
    int degree;
    vector<Node*> pointers = {};

    void split() {
        if (parent == nullptr) {
            parent = new InternalNode(degree, nullptr);
        }

        auto node = new InternalNode(degree, (InternalNode*)parent);

        auto data_copy = clear();

        int split_till = ceil((degree + 1) / 2.0);
        int replicated_key = 0;

        while (keys.size() < split_till) {
            replicated_key = -data_copy.top();
            insert(replicated_key);
            data_copy.pop();
        }

        replicated_key = data_copy.top();
        data_copy.pop();

        while (data_copy.size()) {
            node->insert(-data_copy.top());
            data_copy.pop();
        }

        ((InternalNode*)parent)->addKeyWithPointer(replicated_key, node);
        parent->insert(replicated_key);
    }

    bool isLeafNode() {
        return false;
    }

    InternalNode(int degree, InternalNode* parent) {
        this->degree = degree;
        keys.reserve(degree - 1);
        pointers.reserve(degree);
        this->parent = parent;
    }

    Node** getNextNodePointer(Key _key) {
        int i = 0;
        for (Key key : keys) {
            if (_key <= key) {
                return &pointers[i];
            }

            i++;
        }

        return &pointers[pointers.size() - 1];
    }

    void addKeyWithPointer(Key _key, Node* node) {
        int i = 0;
        for (Key key : keys) {
            if (_key <= key) {
                i++;
            }
        }

        keys.push_back(_key);
        pointers.push_back(node);
    }

    void addInitialPointer(Node* node) {
        pointers.push_back(node);
    }

    bool hasOverflown() {
        return size() == degree - 1;
    }

    void insert(Key key) {
        keys.push_back(key);
        if (hasOverflown()) {
            split();
        }
    }
};

class LeafNode : public Node {
public:
    int degree;
    LeafNode* next = nullptr;
    vector<Key> data = {};

    bool isLeafNode() {
        return true;
    }

    priority_queue<Key> clear() {
        priority_queue<Key> data_copy;
        while (keys.size()) data_copy.push(-keys[keys.size() - 1]), keys.pop_back();

        return data_copy;
    }

    void split() {
        if (parent == nullptr) {
            parent = new InternalNode(degree, nullptr);
            ((InternalNode*)parent)->addInitialPointer(this);
        }
        auto node = new LeafNode(degree, (InternalNode*)parent);
        next = node;
        
        auto data_copy = clear();

        int split_till = ceil((degree + 1) / 2.0);
        int replicated_key = 0;

        while (keys.size() < split_till) {
            replicated_key = -data_copy.top();
            insert(replicated_key);
            data_copy.pop();
        }

        while (data_copy.size()) {
            node->insert(-data_copy.top());
            data_copy.pop();
        }

        ((InternalNode*)parent)->addKeyWithPointer(replicated_key, node);

        parent->insert(replicated_key);
    }

    LeafNode(int degree, InternalNode* parent) {
        this->degree = degree;
        this->parent = parent;
        keys.reserve(degree - 1);
        data.reserve(degree);
    }

    bool hasOverflown() {
        return size() == degree;
    }

    void insert(Key key) {
        keys.push_back(key);
        if (hasOverflown()) {
            split();
        }
    }
};

class BPTree {
public:
    int degree;
    Node* root;

    void display() {
        queue<Node*> q;
        q.push(root);
        while (q.size()) {
            int s = q.size();
            while (s--) {
                auto curr = q.front();
                q.pop();
            
                cout << "[ ";

                for (auto x : curr->keys) {
                    cout << x << ' ';
                }
                cout << "]";
            
                if (curr->isLeafNode() == false)
                    for (auto p : ((InternalNode*)curr)->pointers) {
                        q.push(p);
                    }
            }

            cout << endl;
        }
    }

    LeafNode* getLeafNode(Key key) {
        return getLeafNodeImpl(root, key);
    }

    LeafNode* getLeafNodeImpl(Node* node, Key key) {
        if (node->isLeafNode()) {
            return (LeafNode*)node;
        }

        InternalNode* current = (InternalNode*)(node);
        Node* next = *current->getNextNodePointer(key);
        return getLeafNodeImpl(next, key);
    }

    BPTree(int degree) {
        this->degree = degree;
        root = new LeafNode(degree, nullptr);
    }

    void insert(Key key) {
        LeafNode* leafNode = getLeafNode(key);
        leafNode->insert(key);
        if (root->parent) root = root->parent;
    }
};

int main() {
    BPTree* bptree = new BPTree(4);

    for (int i = 0; i < 16; i++) {
        cout << "Inserting " << i << endl;
        bptree->insert(i);
        cout << "Tree Structure: \n";
        bptree->display();
    }

}

