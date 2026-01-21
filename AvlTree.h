#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <memory>

using namespace std;

template <class K, class T>
struct Node {
    Node<K, T>* parent;
    unique_ptr<Node<K, T>> left;
    unique_ptr<Node<K, T>> right;
    T value;
    K id;
    int height;
    int weight;

    Node(K id, T value)
        : parent(nullptr), left(nullptr), right(nullptr),
          value(move(value)), id(move(id)), height(0), weight(1) {}

    Node(K id, T value, Node<K, T>* parent)
        : parent(parent), left(nullptr), right(nullptr),
          value(move(value)), id(move(id)), height(0), weight(1) {}
};

template<class K, class T>
class AvlTree {
    unique_ptr<Node<K, T>> root;

    Node<K, T>* searchNode(const K id);

    Node<K, T>* RR(Node<K, T>* oldRoot);
    Node<K, T>* LL(Node<K, T>* d);
    Node<K, T>* LR(Node<K, T>* oldRoot);
    Node<K, T>* RL(Node<K, T>* oldRoot);

    void updateNodeStats(Node<K, T>* t);
    int balance(Node<K, T>* t);

    void replace(Node<K, T>* parent,
                 Node<K, T>* oldSon,
                 unique_ptr<Node<K, T>> newSon);
    T get_ith(Node<K, T>* node, int i);
    K get_ith_id(Node<K, T>* node, int i);

    unique_ptr<Node<K, T>>& link(Node<K, T>* n);

public:
    AvlTree() = default;
    ~AvlTree() = default;

    bool isEmpty();
    void insert(K id, T value);
    void del(const K id);
    auto& search(const K id);
    void rebalance(Node<K, T>* suspect);
    T get_ith_element(int i);
    K get_ith_id(int i);
};

template <class K, class T>
unique_ptr<Node<K, T>>& AvlTree<K, T>::link(Node<K, T>* n) {
    if (!n->parent) return root;
    if (n->parent->left.get() == n) return n->parent->left;
    return n->parent->right;
}

template <class K, class T>
Node<K, T>* AvlTree<K, T>::searchNode(const K id) {
    auto temp = root.get();
    while (temp) {
        if (temp->id == id) return temp;
        else if (id > temp->id) temp = temp->right.get();
        else temp = temp->left.get();
    }
    return nullptr;
}

template <class K, class T>
auto& AvlTree<K, T>::search(const K id) {
    auto temp = root.get();
    while (temp) {
        if (temp->id == id) return *(temp->value);
        else if (id > temp->id) temp = temp->right.get();
        else temp = temp->left.get();
    }
    throw StatusType::FAILURE;
}

template <class K, class T>
void AvlTree<K, T>::insert(K id, T value) {
    if (!root) {
        root = make_unique<Node<K, T>>(move(id), move(value));
        return;
    }
    auto temp = root.get();
    while (temp) {
        if (id == temp->id) {
            throw StatusType::FAILURE;
        }
        if (id > temp->id) {
            if (!temp->right) {
                temp->right = make_unique<Node<K, T>>(move(id), move(value), temp);
                break;
            }
            temp = temp->right.get();
        } else {
            if (!temp->left) {
                temp->left = make_unique<Node<K, T>>(move(id), move(value), temp);
                break;
            }
            temp = temp->left.get();
        }
    }
    rebalance(temp);
}

template <class K, class T>
void AvlTree<K, T>::rebalance(Node<K, T>* suspect) {
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

template <class K, class T>
Node<K, T>* AvlTree<K, T>::RR(Node<K, T>* oldRoot) {
    auto parent = oldRoot->parent;

    auto& oldLink = link(oldRoot);
    unique_ptr<Node<K, T>> A = std::move(oldLink);
    unique_ptr<Node<K, T>> B = std::move(A->right);
    unique_ptr<Node<K, T>> b = std::move(B->left);

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

template <class K, class T>
Node<K, T>* AvlTree<K, T>::LL(Node<K, T>* d) {
    auto parent = d->parent;

    auto& oldLink = link(d);
    unique_ptr<Node<K, T>> A = std::move(oldLink);
    unique_ptr<Node<K, T>> B = std::move(A->left);
    unique_ptr<Node<K, T>> c = std::move(B->right);

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

template <class K, class T>
Node<K, T>* AvlTree<K, T>::LR(Node<K, T>* oldRoot) {
    RR(oldRoot->left.get());
    return LL(oldRoot);
}

template <class K, class T>
Node<K, T>* AvlTree<K, T>::RL(Node<K, T>* oldRoot) {
    LL(oldRoot->right.get());
    return RR(oldRoot);
}

template <class K, class T>
void AvlTree<K, T>::updateNodeStats(Node<K, T>* t) {
    if (!t) return;

    int lh = t->left ? t->left->height : -1;
    int rh = t->right ? t->right->height : -1;
    t->height = 1 + max(lh, rh);

    int lw = t->left ? t->left->weight : 0;
    int rw = t->right ? t->right->weight : 0;
    t->weight = 1 + lw + rw;
}

template <class K, class T>
int AvlTree<K, T>::balance(Node<K, T>* t) {
    if (!t) return 0;
    int lh = t->left ? t->left->height : -1;
    int rh = t->right ? t->right->height : -1;
    return lh - rh;
}

template <class K, class T>
void AvlTree<K, T>::replace(Node<K, T>* parent,
                         Node<K, T>* oldSon,
                         unique_ptr<Node<K, T>> newSon) {
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

template <class K, class T>
void AvlTree<K, T>::del(const K targetId) {
    auto target = searchNode(targetId);
    if (!target) throw StatusType::FAILURE;

    Node<K, T>* nodeToStartRebalanceFrom = nullptr;
    auto parent = target->parent;

    auto& targetLink = link(target);

    if (!target->left || !target->right) {
        unique_ptr<Node<K, T>> child = target->left ? std::move(targetLink->left) : std::move(targetLink->right);
        targetLink = std::move(child);
        if (targetLink) targetLink->parent = parent;
        nodeToStartRebalanceFrom = parent;
    }
    else {
        unique_ptr<Node<K, T>>* succLinkPtr = &targetLink->right;
        Node<K, T>* succParent = target;
        while ((*succLinkPtr)->left) {
            succParent = (*succLinkPtr).get();
            succLinkPtr = &((*succLinkPtr)->left);
        }

        unique_ptr<Node<K, T>> succ = std::move(*succLinkPtr);
        *succLinkPtr = std::move(succ->right);
        if (*succLinkPtr) (*succLinkPtr)->parent = succParent;

        if (succParent != target) {
            nodeToStartRebalanceFrom = succParent;
        } else {
            nodeToStartRebalanceFrom = succ.get();
        }

        succ->left = std::move(targetLink->left);
        if (succ->left) succ->left->parent = succ.get();

        succ->right = std::move(targetLink->right);
        if (succ->right) succ->right->parent = succ.get();

        succ->parent = parent;
        targetLink = std::move(succ);

        updateNodeStats(targetLink.get());
    }

    rebalance(nodeToStartRebalanceFrom);
}

template <class K, class T>
T AvlTree<K, T>::get_ith(Node<K, T>* node, int i) {
    int leftSize = (node->left) ? node->left->weight : 0;
    if (i == leftSize + 1) return node->value;
    if (i <= leftSize) return get_ith(node->left.get(), i);
    return get_ith(node->right.get(), i - leftSize - 1);
}

template <class K, class T>
K AvlTree<K, T>::get_ith_id(Node<K, T>* node, int i) {
    int leftSize = (node->left) ? node->left->weight : 0;
    if (i == leftSize + 1) return node->id;
    if (i <= leftSize) return get_ith_id(node->left.get(), i);
    return get_ith_id(node->right.get(), i - leftSize - 1);
}

template <class K, class T>
T AvlTree<K, T>::get_ith_element(int i) {
    if (!root || i < 1 || i > root->weight) {
        throw StatusType::FAILURE;
    }
    return get_ith(root.get(), i);
}

template <class K, class T>
K AvlTree<K, T>::get_ith_id(int i) {
    if (!root || i < 1 || i > root->weight) {
        throw StatusType::FAILURE;
    }
    return get_ith_id(root.get(), i);
}

#endif