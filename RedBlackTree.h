#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <string>
#include <vector>

#define COLOR_RED 0
#define COLOR_BLACK 1

struct RBTNode {
    int data;
    int color = COLOR_RED;
    // Links are positions in the tree's vector. -1 means no node.
    int left = -1;
    int right = -1;
    int parent = -1;

    explicit RBTNode(int value) : data(value) {}
};

class RedBlackTree {
public:
    RedBlackTree();
    explicit RedBlackTree(int value);
    RedBlackTree(const RedBlackTree &other);
    RedBlackTree &operator=(const RedBlackTree &other);
    ~RedBlackTree();

    void Insert(int value);
    bool Contains(int value) const;
    int GetMin() const;
    int GetMax() const;
    unsigned long long int Size() const { return nodes.size(); }

    std::string ToInfixString() const { return ToInfixString(root); }
    std::string ToPrefixString() const { return ToPrefixString(root); }
    std::string ToPostfixString() const { return ToPostfixString(root); }

    // Tests can check internal links, ordering, colors, and black heights.
    void PrivateTests() const;

private:
    std::vector<RBTNode> nodes;
    int root = -1;

    void RotateLeft(int node);
    void RotateRight(int node);
    void FixInsertion(int node);
    std::string NodeString(int node) const;
    std::string ToInfixString(int node) const;
    std::string ToPrefixString(int node) const;
    std::string ToPostfixString(int node) const;
    int CheckSubtree(int node, int parent, long long lower, long long upper,
                     unsigned long long int &count) const;
};

#endif
