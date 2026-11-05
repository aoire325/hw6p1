#include "RedBlackTree.h"

#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <vector>


template <typename Action>
void ExpectInvalidArgument(Action action) {
    bool caught = false;
    try {
        action();
    } catch (const std::invalid_argument &) {
        caught = true;
    }
    assert(caught);
}

std::vector<int> TraversalValues(const std::string &text) {
    std::istringstream input(text);
    std::vector<int> values;
    char color;
    int value;
    std::string reconstructed;
    while (input >> color) {
        assert(color == 'B' || color == 'R');
        const bool readValue = static_cast<bool>(input >> value);
        assert(readValue);
        (void)readValue;
        values.push_back(value);
        reconstructed += std::string(" ") + color + std::to_string(value) + " ";
    }
    // This also checks the required spaces around every individual node.
    assert(reconstructed == text);
    return values;
}

void CheckContents(const RedBlackTree &tree, const std::set<int> &expected) {
    tree.PrivateTests();
    assert(tree.Size() == expected.size());
    const std::vector<int> sorted(expected.begin(), expected.end());
    assert(TraversalValues(tree.ToInfixString()) == sorted);

    std::vector<int> prefix = TraversalValues(tree.ToPrefixString());
    std::vector<int> postfix = TraversalValues(tree.ToPostfixString());
    if (!sorted.empty()) {
        assert(prefix.front() == postfix.back());
        assert(tree.GetMin() == sorted.front());
        assert(tree.GetMax() == sorted.back());
    }
    std::sort(prefix.begin(), prefix.end());
    std::sort(postfix.begin(), postfix.end());
    assert(prefix == sorted);
    assert(postfix == sorted);
    for (int value : expected) {
        assert(tree.Contains(value));
    }
}

void TestEmptyAndSingleton() {
    RedBlackTree empty;
    CheckContents(empty, {});
    assert(!empty.Contains(0));
    ExpectInvalidArgument([&]() { empty.GetMin(); });
    ExpectInvalidArgument([&]() { empty.GetMax(); });

    RedBlackTree copiedEmpty(empty);
    CheckContents(copiedEmpty, {});
    copiedEmpty.Insert(42);
    CheckContents(empty, {});
    CheckContents(copiedEmpty, {42});

    const RedBlackTree singleton(-17);
    CheckContents(singleton, {-17});
    assert(singleton.ToPrefixString() == " B-17 ");
    assert(singleton.ToPostfixString() == " B-17 ");
}

void TestRotationsAndRecoloring() {
    // Four three-node shapes require the two outside and two inside repairs.
    const int orders[][3] = {{30, 20, 10}, {10, 20, 30},
                             {30, 10, 20}, {10, 30, 20}};
    for (const auto &order : orders) {
        RedBlackTree tree;
        for (int value : order) {
            tree.Insert(value);
            tree.PrivateTests();
        }
        assert(tree.ToPrefixString() == " B20  R10  R30 ");
        assert(tree.ToInfixString() == " R10  B20  R30 ");
        assert(tree.ToPostfixString() == " R10  R30  B20 ");
    }

    const int fourthValues[] = {5, 25, 35, 50};
    const char *expected[] = {
        " B30  B15  R5  B45 ", " B30  B15  R25  B45 ",
        " B30  B15  B45  R35 ", " B30  B15  B45  R50 "
    };
    for (int i = 0; i < 4; ++i) {
        RedBlackTree tree;
        for (int value : {30, 15, 45, fourthValues[i]}) {
            tree.Insert(value);
        }
        tree.PrivateTests();
        assert(tree.ToPrefixString() == expected[i]);
    }
}

void TestDuplicatesAndIntegerLimits() {
    RedBlackTree tree;
    const std::set<int> expected = {INT_MIN, -1, 0, 1, INT_MAX};
    for (int value : {0, INT_MIN, INT_MAX, -1, 1}) {
        tree.Insert(value);
    }
    CheckContents(tree, expected);
    for (int value : expected) {
        const std::string before = tree.ToPrefixString();
        ExpectInvalidArgument([&]() { tree.Insert(value); });
        assert(tree.ToPrefixString() == before);
        CheckContents(tree, expected);
    }
    assert(!tree.Contains(-2));
    assert(!tree.Contains(2));
    assert(!tree.Contains(INT_MIN + 1));
    assert(!tree.Contains(INT_MAX - 1));
}

void TestCopyOwnership() {
    RedBlackTree copy;
    {
        RedBlackTree original;
        for (int value : {11, 23, 9, 52, 31, 4}) {
            original.Insert(value);
        }
        RedBlackTree constructed(original);
        assert(constructed.ToPrefixString() == original.ToPrefixString());
        assert(constructed.ToInfixString() == original.ToInfixString());
        assert(constructed.ToPostfixString() == original.ToPostfixString());
        constructed.Insert(-100);
        original.Insert(200);
        assert(!original.Contains(-100));
        assert(!constructed.Contains(200));
        original.PrivateTests();
        constructed.PrivateTests();

        copy.Insert(-999);
        copy = original; // Replace an already allocated tree.
        assert(copy.ToPrefixString() == original.ToPrefixString());
        RedBlackTree &alias = copy;
        copy = alias; // Self-assignment must preserve ownership and contents.
        assert(copy.ToPrefixString() == original.ToPrefixString());
    }
    // The assigned tree must survive destruction of its source.
    CheckContents(copy, {4, 9, 11, 23, 31, 52, 200});
    copy.Insert(300);
    CheckContents(copy, {4, 9, 11, 23, 31, 52, 200, 300});

    RedBlackTree empty;
    copy = empty;
    CheckContents(copy, {});
    copy.Insert(7);
    CheckContents(copy, {7});
}

void TestAllSmallPermutations() {
    std::vector<int> values = {0, 1, 2, 3, 4, 5, 6};
    do {
        RedBlackTree tree;
        std::set<int> expected;
        for (int value : values) {
            tree.Insert(value);
            expected.insert(value);
            // Validate each intermediate shape, not just the finished tree.
            CheckContents(tree, expected);
        }
    } while (std::next_permutation(values.begin(), values.end()));
}

void TestLargeOrderedAndRandomInsertions() {
    for (bool reversed : {false, true}) {
        RedBlackTree tree;
        std::set<int> expected;
        for (int i = 0; i < 4096; ++i) {
            const int value = reversed ? 4095 - i : i;
            tree.Insert(value);
            expected.insert(value);
            tree.PrivateTests();
            assert(tree.Size() == expected.size());
        }
        CheckContents(tree, expected);
    }

    std::mt19937 generator(603);
    std::uniform_int_distribution<int> distribution(-1000, 1000);
    for (int round = 0; round < 8; ++round) {
        RedBlackTree tree;
        std::set<int> expected;
        for (int i = 0; i < 1200; ++i) {
            const int value = distribution(generator);
            if (expected.insert(value).second) {
                tree.Insert(value);
            } else {
                ExpectInvalidArgument([&]() { tree.Insert(value); });
            }
            tree.PrivateTests();
            assert(tree.Size() == expected.size());
            const int probe = distribution(generator);
            assert(tree.Contains(probe) == (expected.count(probe) != 0));
            assert(tree.GetMin() == *expected.begin());
            assert(tree.GetMax() == *expected.rbegin());
        }
        CheckContents(tree, expected);
        RedBlackTree copy(tree);
        assert(copy.ToPrefixString() == tree.ToPrefixString());
        CheckContents(copy, expected);
    }
}

int main() {
    TestEmptyAndSingleton();
    TestRotationsAndRecoloring();
    TestDuplicatesAndIntegerLimits();
    TestCopyOwnership();
    TestAllSmallPermutations();
    TestLargeOrderedAndRandomInsertions();
    std::cout << "ALL ADDITIONAL TESTS PASSED!!" << std::endl;
}
