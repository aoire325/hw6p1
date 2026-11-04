CXX = c++
CXXFLAGS = -std=c++11 -g -Wall -Wextra -Wpedantic

TESTS = rbt-tests-first-step rbt-tests rbt-tests-additional
SANITIZED_TESTS = $(addsuffix -sanitized,$(TESTS))
UBSAN_TESTS = $(addsuffix -ubsan,$(TESTS))
SANITIZER_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer

.PHONY: all test sanitize ubsan clean

all: $(TESTS)

rbt-tests-first-step: RedBlackTreeTestsFirstStep.cpp RedBlackTree.cpp RedBlackTree.h
	$(CXX) $(CXXFLAGS) RedBlackTreeTestsFirstStep.cpp RedBlackTree.cpp -o $@

rbt-tests: RedBlackTreeTests.cpp RedBlackTree.cpp RedBlackTree.h
	$(CXX) $(CXXFLAGS) RedBlackTreeTests.cpp RedBlackTree.cpp -o $@

rbt-tests-additional: RedBlackTreeAdditionalTests.cpp RedBlackTree.cpp RedBlackTree.h
	$(CXX) $(CXXFLAGS) RedBlackTreeAdditionalTests.cpp RedBlackTree.cpp -o $@

test: all
	./rbt-tests-first-step
	./rbt-tests
	./rbt-tests-additional

rbt-tests-first-step-sanitized: RedBlackTreeTestsFirstStep.cpp RedBlackTree.cpp RedBlackTree.h
rbt-tests-sanitized: RedBlackTreeTests.cpp RedBlackTree.cpp RedBlackTree.h
rbt-tests-additional-sanitized: RedBlackTreeAdditionalTests.cpp RedBlackTree.cpp RedBlackTree.h

$(SANITIZED_TESTS):
	$(CXX) $(CXXFLAGS) $(SANITIZER_FLAGS) $< RedBlackTree.cpp -o $@

sanitize: $(SANITIZED_TESTS)
	./rbt-tests-first-step-sanitized
	./rbt-tests-sanitized
	./rbt-tests-additional-sanitized

rbt-tests-first-step-ubsan: RedBlackTreeTestsFirstStep.cpp RedBlackTree.cpp RedBlackTree.h
rbt-tests-ubsan: RedBlackTreeTests.cpp RedBlackTree.cpp RedBlackTree.h
rbt-tests-additional-ubsan: RedBlackTreeAdditionalTests.cpp RedBlackTree.cpp RedBlackTree.h

$(UBSAN_TESTS):
	$(CXX) $(CXXFLAGS) -fsanitize=undefined -fno-sanitize-recover=all $< RedBlackTree.cpp -o $@

ubsan: $(UBSAN_TESTS)
	./rbt-tests-first-step-ubsan
	./rbt-tests-ubsan
	./rbt-tests-additional-ubsan

clean:
	$(RM) $(TESTS) $(SANITIZED_TESTS) $(UBSAN_TESTS)
	$(RM) -r $(addsuffix .dSYM,$(TESTS) $(SANITIZED_TESTS) $(UBSAN_TESTS))
