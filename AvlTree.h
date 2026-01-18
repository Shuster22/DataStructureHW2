#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <memory>
// #include "wet1util.h"

using namespace std;

template <class T>
struct Node {
    Node<T>* parent;
    unique_ptr<Node<T>> left;
    unique_ptr<Node<T>> right;
    T value;
    int id;
    int height;
    int weight;

    Node(T value)
        : parent(nullptr), left(nullptr), right(nullptr),
          value(std::move(value)), id(this->value->id), height(0), weight(1) {}

    Node(T value, Node<T>* parent)
        : parent(parent), left(nullptr), right(nullptr),
          value(std::move(value)), id(this->value->id), height(0), weight(1) {}
};

template<class T>
class AvlTree {
    unique_ptr<Node<T>> root;

    Node<T>* searchNode(const int id);

    Node<T>* RR(Node<T>* oldRoot);
    Node<T>* LL(Node<T>* d);
    Node<T>* LR(Node<T>* oldRoot);
    Node<T>* RL(Node<T>* oldRoot);

    void updateNodeStats(Node<T>* t);
    int balance(Node<T>* t);

    void replace(Node<T>* parent,
                 Node<T>* oldSon,
                 unique_ptr<Node<T>> newSon);
    T get_ith(Node<T>* node, int i);

    unique_ptr<Node<T>>& link(Node<T>* n);

public:
    AvlTree() = default;
    ~AvlTree() = default;

    bool isEmpty();
    void insert(T value);
    void del(const int id);
    typename T::element_type& search(const int id);
    void rebalance(Node<T>* suspect);
    T get_ith_element(int i);
};

template <class T>
unique_ptr<Node<T>>& AvlTree<T>::link(Node<T>* n) {
    if (!n->parent) return root;
    if (n->parent->left.get() == n) return n->parent->left;
    return n->parent->right;
}

template <class T>
Node<T>* AvlTree<T>::searchNode(const int id) {
    auto temp = root.get();
    while (temp) {
        if (temp->id == id) return temp;
        else if (id > temp->id) temp = temp->right.get();
        else temp = temp->left.get();
    }
    return nullptr;
}

template <class T>
typename T::element_type& AvlTree<T>::search(const int id) {
    auto temp = root.get();
    while (temp) {
        if (temp->id == id) return *(temp->value);
        else if (id > temp->id) temp = temp->right.get();
        else temp = temp->left.get();
    }
    throw StatusType::FAILURE;
}

template <class T>
bool AvlTree<T>::isEmpty() {
    return root == nullptr;
}

template <class T>
void AvlTree<T>::insert(T value) {
    if (!root) {
        root = make_unique<Node<T>>(std::move(value));
        return;
    }
    auto temp = root.get();
    while (temp) {
        if (value->id == temp->id) {
            throw StatusType::FAILURE;
        }
        if (value->id > temp->id) {
            if (!temp->right) {
                temp->right = make_unique<Node<T>>(std::move(value), temp);
                break;
            }
            temp = temp->right.get();
        } else {
            if (!temp->left) {
                temp->left = make_unique<Node<T>>(std::move(value), temp);
                break;
            }
            temp = temp->left.get();
        }
    }
    rebalance(temp);
}

template <class T>
void AvlTree<T>::rebalance(Node<T>* suspect) {
    while (suspect) {
        updateNodeStats(suspect);
        if (balance(suspect) > 1) {
            if (balance(suspect->left.get()) >= 0) suspect = LL(suspect);
            else suspect = LR(suspect);
        }
        else if (balance(suspect) < -1) {
            if (balance(suspect->right.get()) <= 0) suspect = RR(suspect);
            else suspect = RL(suspect);
        }
        suspect = suspect->parent;
    }
}

template <class T>
Node<T>* AvlTree<T>::RR(Node<T>* oldRoot) {
    auto parent = oldRoot->parent;

    auto& oldLink = link(oldRoot);
    unique_ptr<Node<T>> A = std::move(oldLink);          // oldRoot
    unique_ptr<Node<T>> B = std::move(A->right);         // newRoot
    unique_ptr<Node<T>> b = std::move(B->left);          // B.left subtree

    B->left = std::move(A);
    B->left->parent = B.get();

    B->left->right = std::move(b);
    if (B->left->right) B->left->right->parent = B->left.get();

    B->parent = parent;
    oldLink = std::move(B);

    updateNodeStats(oldLink->left.get());
    updateNodeStats(oldLink.get());
    return oldLink.get();
}

template <class T>
Node<T>* AvlTree<T>::LL(Node<T>* d) {
    auto parent = d->parent;

    auto& oldLink = link(d);
    unique_ptr<Node<T>> A = std::move(oldLink);          // d
    unique_ptr<Node<T>> B = std::move(A->left);          // b
    unique_ptr<Node<T>> c = std::move(B->right);         // c

    B->right = std::move(A);
    B->right->parent = B.get();

    B->right->left = std::move(c);
    if (B->right->left) B->right->left->parent = B->right.get();

    B->parent = parent;
    oldLink = std::move(B);

    updateNodeStats(oldLink->right.get());
    updateNodeStats(oldLink.get());
    return oldLink.get();
}

template <class T>
Node<T>* AvlTree<T>::LR(Node<T>* oldRoot) {
    RR(oldRoot->left.get());
    return LL(oldRoot);
}

template <class T>
Node<T>* AvlTree<T>::RL(Node<T>* oldRoot) {
    LL(oldRoot->right.get());
    return RR(oldRoot);
}

template <class T>
void AvlTree<T>::updateNodeStats(Node<T>* t) {
    if (!t) return;

    int lh = t->left ? t->left->height : -1;
    int rh = t->right ? t->right->height : -1;
    t->height = 1 + max(lh, rh);

    int lw = t->left ? t->left->weight : 0;
    int rw = t->right ? t->right->weight : 0;
    t->weight = 1 + lw + rw;
}

template <class T>
int AvlTree<T>::balance(Node<T>* t) {
    if (!t) return 0;
    int lh = t->left ? t->left->height : -1;
    int rh = t->right ? t->right->height : -1;
    return lh - rh;
}

template <class T>
void AvlTree<T>::replace(Node<T>* parent,
                         Node<T>* oldSon,
                         unique_ptr<Node<T>> newSon) {
    if (!parent) {
        root = std::move(newSon);
        if (root) root->parent = nullptr;
        return;
    }
    if (parent->left.get() == oldSon) parent->left = std::move(newSon);
    else parent->right = std::move(newSon);
    if (parent->left) parent->left->parent = parent;
    if (parent->right) parent->right->parent = parent;
}

template <class T>
void AvlTree<T>::del(const int targetId) {
    auto target = searchNode(targetId);
    if (!target) throw StatusType::FAILURE;

    Node<T>* nodeToStartRebalanceFrom = nullptr;
    auto parent = target->parent;

    auto& targetLink = link(target);

    if (!target->left || !target->right) {
        unique_ptr<Node<T>> child = target->left ? std::move(targetLink->left) : std::move(targetLink->right);
        targetLink = std::move(child);
        if (targetLink) targetLink->parent = parent;
        nodeToStartRebalanceFrom = parent;
    }
    else {
        // Find successor (by link)
        unique_ptr<Node<T>>* succLinkPtr = &targetLink->right;
        Node<T>* succParent = target;
        while ((*succLinkPtr)->left) {
            succParent = (*succLinkPtr).get();
            succLinkPtr = &((*succLinkPtr)->left);
        }

        unique_ptr<Node<T>> succ = std::move(*succLinkPtr);          // successor node
        // Disconnect successor from its old place
        *succLinkPtr = std::move(succ->right);
        if (*succLinkPtr) (*succLinkPtr)->parent = succParent;

        if (succParent != target) {
            nodeToStartRebalanceFrom = succParent;
        } else {
            nodeToStartRebalanceFrom = succ.get();
        }

        // Move target's children under successor
        succ->left = std::move(targetLink->left);
        if (succ->left) succ->left->parent = succ.get();

        succ->right = std::move(targetLink->right);
        if (succ->right) succ->right->parent = succ.get();

        // Place successor where target was
        succ->parent = parent;
        targetLink = std::move(succ);

        updateNodeStats(targetLink.get());
    }

    rebalance(nodeToStartRebalanceFrom);
}

template <class T>
T AvlTree<T>::get_ith(Node<T>* node, int i) {
    int leftSize = (node->left) ? node->left->weight : 0;
    if (i == leftSize + 1) return node->value;
    if (i <= leftSize) return get_ith(node->left.get(), i);
    return get_ith(node->right.get(), i - leftSize - 1);
}

template <class T>
T AvlTree<T>::get_ith_element(int i) {
    if (!root || i < 1 || i > root->weight) {
        throw StatusType::FAILURE;
    }
    return get_ith(root.get(), i);
}

#endif

