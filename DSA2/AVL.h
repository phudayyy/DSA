#ifndef AVL_H
#define AVL_H

#include "IBST.h"

#include <list>
#include <string>
#include <sstream>

template <typename T1, typename T2>
inline std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& p) {
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}

template <typename K, typename V>
class AVL : public IBST<K, V> {
public:
    struct Node {
        K key;
        V value;

        int bfactor; 

        Node* pLeft;
        Node* pRight;

        Node(const K& k, const V& v)
            : key(k), value(v), bfactor(0),
              pLeft(nullptr), pRight(nullptr) {}

        virtual ~Node() = default;

        int balance() const { return bfactor; }

        // Default formatter must compile for any K,V.
        static std::string defaultEntry2Str(const K& k, const V& v) {
            std::ostringstream oss;
            oss << "<" << k << ", " << v << ">";
            return oss.str();
        }

      std::string toString(std::string (*entry2str)(const K&, const V&) = nullptr) const {
      auto getEntry = [&]() {
            return entry2str ? entry2str(key, value) : defaultEntry2Str(key, value);
        };

        std::ostringstream out;
        if (!pLeft && !pRight) {
            out << "[" << getEntry() << ":" << bfactor << "]";
        }
        else if (!pLeft && pRight) {
            out << " (" << getEntry() << ":" << bfactor << "[.]" << pRight->toString(entry2str) << ")";
        }
        else if (pLeft && !pRight) {
            out << " (" << getEntry() << ":" << bfactor << pLeft->toString(entry2str) << "[.])";
        }
        else {
            out << " (" << getEntry() << ":" << bfactor << pLeft->toString(entry2str) << pRight->toString(entry2str) << ") ";
        }
        return out.str();
    }
};

protected:
    Node* pRoot;

    // Hooks for ThreadedAVL
    virtual void onInserted(Node* newNode, Node* pred, Node* succ) {}
    virtual void onErasing(Node* delNode, Node* pred, Node* succ) {}
    virtual void onReplaceBySuccessor(Node* target, Node* successor, Node* successorNext) {}

    // Factory for subclasses to create extended node types
    virtual Node* createNode(const K& key, const V& value) { return new Node(key, value); }

public:
    AVL() : pRoot(nullptr) {}
    virtual ~AVL() { clear(); }

    // =======================
    // IBST implementation (TODO)
    // =======================

    bool insert(const K& key, const V& value) override {
        // TODO
        Node* pred = nullptr;
        Node* succ = nullptr;
        Node* curr = pRoot;
        while (curr) {
            if (key < curr->key) { succ = curr; curr = curr->pLeft; }
            else if (curr->key < key) { pred = curr; curr = curr->pRight; }
            else return false; 
        }

        bool added = false;
        Node* newNode = nullptr;
        pRoot = insertRec(pRoot, key, value, added, newNode);
        
        if (added) {
            onInserted(newNode, pred, succ);
        }
        return added;
    }

    bool erase(const K& key) override {
        // TODO
        bool removed = false;
        pRoot = eraseRec(pRoot, key, removed);
        return removed;
    }

    V* find(const K& key) override {
        // TODO
        Node* current = pRoot;
        while (current) {
            if (key < current->key) current = current->pLeft;
            else if (current->key < key) current = current->pRight;
            else return &(current->value);
        }
        return nullptr;
    }

    bool contains(const K& key) const override {
        // TODO
        Node* current = pRoot;
        while (current) {
            if (key < current->key) current = current->pLeft;
            else if (current->key < key) current = current->pRight;
            else return true;
        }
        return false;
    }

    int size() const override {
        // TODO
        return countNodes(pRoot);
    }

    bool empty() const override {
        // TODO
        return pRoot == nullptr;
    }

    void clear() override {
        // TODO
        clearRec(pRoot);
        pRoot = nullptr;
    }

    int height() const override {
        // TODO
        return getHeight(pRoot);
    }

    std::list<K> ascendingList() const override {
        // TODO
        std::list<K> res;
        inorder(pRoot, res);
        return res;
    }

    std::list<K> descendingList() const override {
        // TODO
        std::list<K> res;
        reverseInorder(pRoot, res);
        return res;
    }

   std::string toString(std::string (*entry2str)(const K&, const V&) = nullptr) const override {
     if (!pRoot) return "(NULL)";
        return pRoot->toString(entry2str);
    }
private:
    // Helper 
    int getHeight(Node* node) const {
        if (!node) return 0;
        int lh = getHeight(node->pLeft);
        int rh = getHeight(node->pRight);
        return 1 + (lh > rh ? lh : rh);
    }

    void updateBalance(Node* node) {
       if (node) {
           // Sửa thành Phải trừ Trái để khớp với testcase
           node->bfactor = getHeight(node->pRight) - getHeight(node->pLeft);
        }
    }

    Node* rotateRight(Node* y) {
        Node* x = y->pLeft;
        Node* T2 = x->pRight;
        x->pRight = y;
        y->pLeft = T2;
        updateBalance(y);
        updateBalance(x);
        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->pRight;
        Node* T2 = y->pLeft;
        y->pLeft = x;
        x->pRight = T2;
        updateBalance(x);
        updateBalance(y);
        return y;
    }

   Node* balanceNode(Node* node) {
   updateBalance(node);
        
        // 1. Trường hợp lệch PHẢI (Right Heavy): bfactor > 1 (vì Phải - Trái > 1)
        if (node->bfactor > 1) {
            // Kiểm tra trường hợp Right-Left (Con phải bị lệch trái)
            if (node->pRight && node->pRight->bfactor < 0) {
                node->pRight = rotateRight(node->pRight);
            }
            // Lệch phải thì xoay TRÁI
            return rotateLeft(node);
        }
        
        // 2. Trường hợp lệch TRÁI (Left Heavy): bfactor < -1 (vì Phải - Trái < -1)
        if (node->bfactor < -1) {
            // Kiểm tra trường hợp Left-Right (Con trái bị lệch phải)
            if (node->pLeft && node->pLeft->bfactor > 0) {
                node->pLeft = rotateLeft(node->pLeft);
            }
            // Lệch trái thì xoay PHẢI
            return rotateRight(node);
        }
        
        return node;
}

    Node* insertRec(Node* node, const K& key, const V& value, bool& added, Node*& newNode) {
        if (!node) {
            added = true;
            newNode = createNode(key, value);
            return newNode;
        }
        if (key < node->key) {
            node->pLeft = insertRec(node->pLeft, key, value, added, newNode);
        } else if (node->key < key) {
            node->pRight = insertRec(node->pRight, key, value, added, newNode);
        } else {
            added = false;
            return node;
        }
        return balanceNode(node);
    }

    Node* eraseRec(Node* node, const K& key, bool& removed) {
        if (!node) return node;
        
        if (key < node->key) {
            node->pLeft = eraseRec(node->pLeft, key, removed);
        } else if (node->key < key) {
            node->pRight = eraseRec(node->pRight, key, removed);
        } else {
            removed = true;
            if (!node->pLeft || !node->pRight) {
                Node* temp = node->pLeft ? node->pLeft : node->pRight;
                
                Node* pred = nullptr; Node* succ = nullptr;
                Node* curr = pRoot;
                while (curr) {
                    if (node->key < curr->key) { succ = curr; curr = curr->pLeft; }
                    else if (curr->key < node->key) { pred = curr; curr = curr->pRight; }
                    else break;
                }
                if (node->pLeft) {
                    pred = node->pLeft;
                    while (pred->pRight) pred = pred->pRight;
                }
                if (node->pRight) {
                    succ = node->pRight;
                    while (succ->pLeft) succ = succ->pLeft;
                }

                onErasing(node, pred, succ);
                delete node;
                return temp;
            } else {
                Node* succ = node->pRight;
                while (succ->pLeft) succ = succ->pLeft;
                
                Node* succNext = nullptr;
                Node* curr = pRoot;
                while (curr) {
                    if (succ->key < curr->key) { succNext = curr; curr = curr->pLeft; }
                    else if (curr->key < succ->key) { curr = curr->pRight; }
                    else break;
                }
                if (succ->pRight) {
                    succNext = succ->pRight;
                    while (succNext->pLeft) succNext = succNext->pLeft;
                }

                onReplaceBySuccessor(node, succ, succNext);

                node->key = succ->key;
                node->value = succ->value;
                bool dummy = false;
                node->pRight = eraseRec(node->pRight, succ->key, dummy);
            }
        }
        if (!node) return node;
        return balanceNode(node);
    }

    int countNodes(Node* node) const {
        if (!node) return 0;
        return 1 + countNodes(node->pLeft) + countNodes(node->pRight);
    }

    void clearRec(Node* node) {
        if (!node) return;
        clearRec(node->pLeft);
        clearRec(node->pRight);
        delete node;
    }

    void inorder(Node* node, std::list<K>& res) const {
        if (!node) return;
        inorder(node->pLeft, res);
        res.push_back(node->key);
        inorder(node->pRight, res);
    }

    void reverseInorder(Node* node, std::list<K>& res) const {
        if (!node) return;
        reverseInorder(node->pRight, res);
        res.push_back(node->key);
        reverseInorder(node->pLeft, res);
    }
};

#endif /* AVL_H */