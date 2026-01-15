#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <memory>
#include "wet1util.h"

using namespace std;

template <class T>
struct Node {
    weak_ptr<Node<T>> parent;
    shared_ptr<Node<T>> left;
    shared_ptr<Node<T>> right;
    T value;
    int id;
    int height;

    Node(T value)
        : parent(), left(nullptr), right(nullptr),
          value(std::move(value)), id(this->value->id), height(0) {}

    Node(T value, shared_ptr<Node<T>> parent)
        : parent(parent), left(nullptr), right(nullptr),
          value(std::move(value)), id(this->value->id), height(0) {}
};

template<class T>
class AvlTree {
    shared_ptr<Node<T>> root;

    shared_ptr<Node<T>> searchNode(const int id);

    shared_ptr<Node<T>> RR(shared_ptr<Node<T>> oldRoot);
    shared_ptr<Node<T>> LL(shared_ptr<Node<T>> d);
    shared_ptr<Node<T>> LR(shared_ptr<Node<T>> oldRoot);
    shared_ptr<Node<T>> RL(shared_ptr<Node<T>> oldRoot);

    int updateHeight(shared_ptr<Node<T>> t);
    int balance(shared_ptr<Node<T>> t);

    void replace(shared_ptr<Node<T>> parent,
                 shared_ptr<Node<T>> oldSon,
                 shared_ptr<Node<T>> newSon);

public:
    AvlTree() = default;
    ~AvlTree() = default;

    bool isEmpty();
    void insert(T value);
    void del(const int id);
    T search(const int id);
    void rebalance(shared_ptr<Node<T>> suspect);
};

template <class T>
shared_ptr<Node<T>> AvlTree<T>::searchNode(const int id) {
    auto temp = root;
    while (temp) {
        if (temp->id == id) return temp;
        else if (id > temp->id) temp = temp->right;
        else temp = temp->left;
    }
    return nullptr;
}

template <class T>
T AvlTree<T>::search(const int id) {
    auto temp = root;
    while (temp) {
        if (temp->id == id) return temp->value;
        else if (id > temp->id) temp = temp->right;
        else temp = temp->left;
    }
    return nullptr;
}

template <class T>
bool AvlTree<T>::isEmpty() {
    return root == nullptr;
}

template <class T>
void AvlTree<T>::insert(T value) {
    if (!root) {
        root = make_shared<Node<T>>(std::move(value));
        return;
    }
    auto temp = root;
    while (temp) {
        if (value->id == temp->id) {
            throw StatusType::FAILURE;
        }
        if (value->id > temp->id) {
            if (!temp->right) {
                temp->right = make_shared<Node<T>>(std::move(value), temp);
                break;
            }
            temp = temp->right;
        } else {
            if (!temp->left) {
                temp->left = make_shared<Node<T>>(std::move(value), temp);
                break;
            }
            temp = temp->left;
        }
    }
    rebalance(temp);
}

template <class T>
void AvlTree<T>::rebalance(shared_ptr<Node<T>> suspect) {
    while (suspect) {
        updateHeight(suspect);
        if (balance(suspect) > 1) {
            if (balance(suspect->left) >= 0) suspect = LL(suspect);
            else suspect = LR(suspect);
        }
        else if (balance(suspect) < -1) {
            if (balance(suspect->right) <= 0) suspect = RR(suspect);
            else suspect = RL(suspect);
        }
        suspect = suspect->parent.lock();
    }
}

template <class T>
shared_ptr<Node<T>> AvlTree<T>::RR(shared_ptr<Node<T>> oldRoot) {
    auto parent = oldRoot->parent.lock();
    auto newRoot = oldRoot->right;
    auto b = newRoot->left;

    newRoot->left = oldRoot;
    oldRoot->parent = newRoot;
    oldRoot->right = b;
    if (b) b->parent = oldRoot;

    newRoot->parent = parent;
    replace(parent, oldRoot, newRoot);

    updateHeight(oldRoot);
    updateHeight(newRoot);
    return newRoot;
}

template <class T>
shared_ptr<Node<T>> AvlTree<T>::LL(shared_ptr<Node<T>> d) {
    auto b = d->left;
    auto c = b->right;
    auto parent = d->parent.lock();

    b->right = d;
    d->parent = b;
    d->left = c;
    if (c) c->parent = d;

    replace(parent, d, b);
    b->parent = parent;

    updateHeight(d);
    updateHeight(b);
    return b;
}

template <class T>
shared_ptr<Node<T>> AvlTree<T>::LR(shared_ptr<Node<T>> oldRoot) {
    RR(oldRoot->left);
    return LL(oldRoot);
}

template <class T>
shared_ptr<Node<T>> AvlTree<T>::RL(shared_ptr<Node<T>> oldRoot) {
    LL(oldRoot->right);
    return RR(oldRoot);
}

template <class T>
int AvlTree<T>::updateHeight(shared_ptr<Node<T>> t) {
    if (!t) return -1;
    int left = (t->left ? t->left->height : -1);
    int right = (t->right ? t->right->height : -1);
    t->height = 1 + max(left, right);
    return t->height;
}

template <class T>
int AvlTree<T>::balance(shared_ptr<Node<T>> t) {
    int lh = t->left ? t->left->height : -1;
    int rh = t->right ? t->right->height : -1;
    return lh - rh;
}

template <class T>
void AvlTree<T>::replace(shared_ptr<Node<T>> parent,
                         shared_ptr<Node<T>> oldSon,
                         shared_ptr<Node<T>> newSon) {
    if (!parent) {
        root = newSon;
        if (newSon) newSon->parent.reset();
        return;
    }
    if (parent->left == oldSon) parent->left = newSon;
    else parent->right = newSon;
    if (newSon) newSon->parent = parent;
}

template <class T>
void AvlTree<T>::del(const int targetId) {
    auto target = searchNode(targetId);
    if (!target) return;

    auto parent = target->parent.lock();

    if (!target->left && !target->right) {
        replace(parent, target, nullptr);
    }
    else if (!target->left && target->right) {
        replace(parent, target, target->right);
    }
    else if (target->left && !target->right) {
        replace(parent, target, target->left);
    }
    else {
        auto replacement = target->right;
        auto repParent = target;

        while (replacement->left) {
            repParent = replacement;
            replacement = replacement->left;
        }

        if (repParent != target) {
            repParent->left = replacement->right;
            if (repParent->left)
                repParent->left->parent = repParent;

            replacement->right = target->right;
            replacement->right->parent = replacement;
        }

        replacement->left = target->left;
        replacement->left->parent = replacement;

        replace(parent, target, replacement);
    }
    rebalance(parent);
}

#endif
