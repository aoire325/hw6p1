# Assignment 6, Part 1: Red-Black Tree

## Build and run

Requires a C++11 compiler and Make.

```sh
make
make test
make sanitize
make ubsan
make clean
```

`make test` runs both supplied test programs and the additional tests.
`make sanitize` builds separate programs with AddressSanitizer and
UndefinedBehaviorSanitizer, then runs all three suites. With Valgrind available,
you can also run `valgrind --leak-check=full ./rbt-tests-additional` after `make`.
`make ubsan` runs all suites with only UndefinedBehaviorSanitizer and stops on
the first diagnostic. On this Mac, AddressSanitizer stalled in its own startup
before `main`, so UBSan and macOS `leaks --atExit -- ./rbt-tests-additional`
were used as separate checks.

Verified locally: both supplied suites and the additional suite passed normally
and under UBSan. The macOS leak checker reported 0 leaks and 0 leaked bytes for
the additional suite.

## Interface and behavior

- Empty, single-integer, and deep-copy constructors.
- `Insert(int)` restores red-black properties using recoloring and rotations.
  Duplicate values throw `std::invalid_argument` without changing the tree.
- `Contains(int)` returns whether a value is present.
- `GetMin()` and `GetMax()` return the smallest and largest integers. On an
  empty tree, both throw `std::invalid_argument`.
- `Size()` returns the number of distinct nodes.
- `ToInfixString()`, `ToPrefixString()`, and `ToPostfixString()` recursively
  traverse the tree. Every node includes its color and surrounding spaces,
  for example `" B20  R10  R30 "` in prefix order.
- `PrivateTests()` asserts root color, parent links, BST ordering, node count,
  absence of adjacent red nodes, and equal black heights. Run tests with
  assertions enabled (the Makefile does this).

The implementation stores nodes in a `std::vector<RBTNode>`. The `root`, `left`,
`right`, and `parent` links are integer indices; `-1` means no node. For example,
`nodes[node].left` gives the index of a node's left child. Indices remain valid
when the vector grows. The tree implementation uses no explicit pointers,
`new`, or `delete`.

The vector handles memory cleanup and copies every node when a tree is copied,
so separate trees remain independent. Removal is reserved for Part 2. The
supplied tests remain unchanged, including their use of pointers to tree objects.

Search and finding extrema take O(log n) time; size takes O(1). Insertion takes
O(log n) amortized time, with an occasional O(n) vector reallocation. Deep
copying takes O(n). Traversal string concatenation can take O(n log n) time in a
balanced tree; the recursive traversal itself visits each node once. Integer
indices limit the tree to `std::numeric_limits<int>::max()` nodes.

## Extra checks

The supplied tests remain unchanged. The additional suite covers all four
three-node rotation cases, four-node recoloring, empty trees, integer limits,
duplicate rejection, copy independence and self-assignment, all 5,040 insertion
orders of seven distinct values, ascending and descending sequences of 4,096
values, and reproducible random insertions checked against `std::set`.

## Submission

The assignment also requests a public GitHub repository, a video recording,
and submission of the repository link and video on Canvas. Those submission
steps must be completed separately. For the recording, explain the node links,
insertion repair cases, recursive traversals, copy ownership, and test results.

The implementation and additional tests were written with OpenAI Codex
assistance, as acknowledged in the source comments.
