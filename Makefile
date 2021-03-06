CXX=g++-9
CXXFLAGS=-O3 -Wall -Wextra -std=c++17 -I Shared
MAINFILE=main.cpp

KRUSKAL_NAIVE=KruskalNaive
KRUSKAL_UNION_FIND=KruskalUnionFind
KRUSKAL_UNION_FIND_COMPRESSED=KruskalUnionFindCompressed
PRIM_BINARY_HEAP=PrimBinaryHeap
PRIM_K_HEAP=PrimKHeap

OUT_DIR="."
EXT="out"

all: ensure_build_dir algs

algs: ${KRUSKAL_NAIVE} ${KRUSKAL_UNION_FIND} ${KRUSKAL_UNION_FIND_COMPRESSED} ${PRIM_BINARY_HEAP} ${PRIM_K_HEAP}

${KRUSKAL_NAIVE}:
	${CXX} ${CXXFLAGS} ${KRUSKAL_NAIVE}/${MAINFILE} -o ${OUT_DIR}/${KRUSKAL_NAIVE}.${EXT}

${KRUSKAL_UNION_FIND}:
	${CXX} ${CXXFLAGS} ${KRUSKAL_UNION_FIND}/${MAINFILE} -o ${OUT_DIR}/${KRUSKAL_UNION_FIND}.${EXT}

${KRUSKAL_UNION_FIND_COMPRESSED}:
	${CXX} ${CXXFLAGS} ${KRUSKAL_UNION_FIND_COMPRESSED}/${MAINFILE} -o ${OUT_DIR}/${KRUSKAL_UNION_FIND_COMPRESSED}.${EXT}

${PRIM_BINARY_HEAP}:
	${CXX} ${CXXFLAGS} ${PRIM_BINARY_HEAP}/${MAINFILE} -o ${OUT_DIR}/${PRIM_BINARY_HEAP}.${EXT}

${PRIM_K_HEAP}:
	${CXX} ${CXXFLAGS} ${PRIM_K_HEAP}/${MAINFILE} -o ${OUT_DIR}/${PRIM_K_HEAP}.${EXT}

ensure_build_dir:
	mkdir -p ${OUT_DIR}

testall:
	./testall.sh

testall_explicit:
	./test.sh ${KRUSKAL_UNION_FIND}.${EXT}
	./test.sh ${KRUSKAL_UNION_FIND_COMPRESSED}.${EXT}
	./test.sh ${PRIM_BINARY_HEAP}.${EXT}
	./test.sh ${PRIM_K_HEAP}.${EXT}
# TODO: Test KruskalNaive excluded due to its excessively long runtime.
# ./test.sh ${KRUSKAL_NAIVE}.${EXT}

# report:
# 	cd report; make pdf1

.PHONY: all algs ensure_build_dir
.PHONY: ${KRUSKAL_NAIVE} ${KRUSKAL_UNION_FIND} ${KRUSKAL_UNION_FIND_COMPRESSED} ${PRIM_BINARY_HEAP} ${PRIM_K_HEAP} # report
