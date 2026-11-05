#include "RedBlackTree.h"

#include <cassert>
#include <limits>
#include <stdexcept>

RedBlackTree::RedBlackTree() = default;

RedBlackTree::RedBlackTree(int value) {
    Insert(value);
}

// Copying the vector copies every node, including its color and index links.
RedBlackTree::RedBlackTree(const RedBlackTree &other) = default;

RedBlackTree &RedBlackTree::operator=(const RedBlackTree &other) = default;

// The vector automatically releases its storage.
RedBlackTree::~RedBlackTree() = default;

void RedBlackTree::Insert(int value) {
    int parent = -1;
    int current = root;
    while (current != -1) {
        parent = current;
        if (value == nodes[current].data) {
            // Reject duplicates before allocating or changing the tree.
            throw std::invalid_argument("Cannot insert a duplicate value");
        }
        if (value < nodes[current].data) {
            current = nodes[current].left;
        } else {
            current = nodes[current].right;
        }
    }

    // Appending can move the vector's storage, but index numbers stay valid.
    if (nodes.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::length_error("Too many nodes for integer indices");
    }
    int node = static_cast<int>(nodes.size());
    nodes.emplace_back(value);
    nodes[node].parent = parent;
    if (parent == -1) {
        root = node;
    } else if (value < nodes[parent].data) {
        nodes[parent].left = node;
    } else {
        nodes[parent].right = node;
    }
    FixInsertion(node);
}

void RedBlackTree::RotateLeft(int node) {
    int promoted = nodes[node].right;
    nodes[node].right = nodes[promoted].left;
    if (nodes[promoted].left != -1) {
        nodes[nodes[promoted].left].parent = node;
    }

    nodes[promoted].parent = nodes[node].parent;
    if (nodes[node].parent == -1) {
        root = promoted;
    } else if (node == nodes[nodes[node].parent].left) {
        nodes[nodes[node].parent].left = promoted;
    } else {
        nodes[nodes[node].parent].right = promoted;
    }
    nodes[promoted].left = node;
    nodes[node].parent = promoted;
}

void RedBlackTree::RotateRight(int node) {
    int promoted = nodes[node].left;
    nodes[node].left = nodes[promoted].right;
    if (nodes[promoted].right != -1) {
        nodes[nodes[promoted].right].parent = node;
    }

    nodes[promoted].parent = nodes[node].parent;
    if (nodes[node].parent == -1) {
        root = promoted;
    } else if (node == nodes[nodes[node].parent].left) {
        nodes[nodes[node].parent].left = promoted;
    } else {
        nodes[nodes[node].parent].right = promoted;
    }
    nodes[promoted].right = node;
    nodes[node].parent = promoted;
}

void RedBlackTree::FixInsertion(int node) {
    // A red new leaf preserves black heights. Only a red parent needs repair.
    while (node != root && nodes[nodes[node].parent].color == COLOR_RED) {
        int parent = nodes[node].parent;
        int grandparent = nodes[parent].parent;
        if (parent == nodes[grandparent].left) {
            int uncle = nodes[grandparent].right;
            if (uncle != -1 && nodes[uncle].color == COLOR_RED) {
                // Recolor both children and carry the possible conflict upward.
                nodes[parent].color = COLOR_BLACK;
                nodes[uncle].color = COLOR_BLACK;
                nodes[grandparent].color = COLOR_RED;
                node = grandparent;
            } else {
                if (node == nodes[parent].right) {
                    // Turn the inside bend into an outside line first.
                    node = parent;
                    RotateLeft(node);
                }
                // A rotation can change which nodes are the parent and grandparent.
                parent = nodes[node].parent;
                grandparent = nodes[parent].parent;
                nodes[parent].color = COLOR_BLACK;
                nodes[grandparent].color = COLOR_RED;
                RotateRight(grandparent);
            }
        } else {
            // The same cases, reflected across the grandparent.
            int uncle = nodes[grandparent].left;
            if (uncle != -1 && nodes[uncle].color == COLOR_RED) {
                nodes[parent].color = COLOR_BLACK;
                nodes[uncle].color = COLOR_BLACK;
                nodes[grandparent].color = COLOR_RED;
                node = grandparent;
            } else {
                if (node == nodes[parent].left) {
                    node = parent;
                    RotateRight(node);
                }
                parent = nodes[node].parent;
                grandparent = nodes[parent].parent;
                nodes[parent].color = COLOR_BLACK;
                nodes[grandparent].color = COLOR_RED;
                RotateLeft(grandparent);
            }
        }
    }
    nodes[root].color = COLOR_BLACK;
}

bool RedBlackTree::Contains(int value) const {
    int node = root;
    while (node != -1) {
        if (value == nodes[node].data) {
            return true;
        }
        if (value < nodes[node].data) {
            node = nodes[node].left;
        } else {
            node = nodes[node].right;
        }
    }
    return false;
}

int RedBlackTree::GetMin() const {
    if (root == -1) {
        throw std::invalid_argument("An empty tree has no minimum");
    }
    int node = root;
    while (nodes[node].left != -1) {
        node = nodes[node].left;
    }
    return nodes[node].data;
}

int RedBlackTree::GetMax() const {
    if (root == -1) {
        throw std::invalid_argument("An empty tree has no maximum");
    }
    int node = root;
    while (nodes[node].right != -1) {
        node = nodes[node].right;
    }
    return nodes[node].data;
}

std::string RedBlackTree::NodeString(int node) const {
    return std::string(" ") + (nodes[node].color == COLOR_RED ? "R" : "B")
           + std::to_string(nodes[node].data) + " ";
}

std::string RedBlackTree::ToInfixString(int node) const {
    if (node == -1) {
        return "";
    }
    return ToInfixString(nodes[node].left) + NodeString(node)
           + ToInfixString(nodes[node].right);
}

std::string RedBlackTree::ToPrefixString(int node) const {
    if (node == -1) {
        return "";
    }
    return NodeString(node) + ToPrefixString(nodes[node].left)
           + ToPrefixString(nodes[node].right);
}

std::string RedBlackTree::ToPostfixString(int node) const {
    if (node == -1) {
        return "";
    }
    return ToPostfixString(nodes[node].left) + ToPostfixString(nodes[node].right)
           + NodeString(node);
}

int RedBlackTree::CheckSubtree(int node, int parent, long long lower,
                               long long upper,
                               unsigned long long int &count) const {
    if (node == -1) {
        return 1; // Missing children are black leaves.
    }

    // Check the index before using it to access the vector.
    assert(node >= 0 && static_cast<std::size_t>(node) < nodes.size());
    const RBTNode &current = nodes[node];
    assert(current.parent == parent);
    assert(lower < current.data && current.data < upper);
    assert(current.color == COLOR_RED || current.color == COLOR_BLACK);

    ++count;
    assert(count <= nodes.size());
    const int leftHeight = CheckSubtree(current.left, node, lower,
                                        current.data, count);
    const int rightHeight = CheckSubtree(current.right, node, current.data,
                                         upper, count);
    assert(leftHeight == rightHeight);
    if (current.color == COLOR_RED) {
        assert(current.left == -1 || nodes[current.left].color == COLOR_BLACK);
        assert(current.right == -1 || nodes[current.right].color == COLOR_BLACK);
    }

    (void)parent;
    (void)rightHeight;
    return leftHeight + (current.color == COLOR_BLACK ? 1 : 0);
}

void RedBlackTree::PrivateTests() const {
    if (root == -1) {
        assert(nodes.empty());
        return;
    }

    assert(root >= 0 && static_cast<std::size_t>(root) < nodes.size());
    assert(nodes[root].parent == -1);
    assert(nodes[root].color == COLOR_BLACK);

    unsigned long long int count = 0;
    CheckSubtree(root, -1, std::numeric_limits<long long>::lowest(),
                 std::numeric_limits<long long>::max(), count);
    // Every allocated node must be reachable exactly once from the root.
    assert(count == nodes.size());
}
