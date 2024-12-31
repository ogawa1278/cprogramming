void printword(Word w) {
    for (int i = 0; i < 10; i++) {
        printf("%c", w.s[i]);
    }
    printf("\n");
}
// 逐次探索
void code_a(Word *l1, Word *l2, Word *l3, Word *l4) {
    for (int a = 0; a < 400; a++) {
        if (LinearSearchWord(l2, 300, l1[a]) &&
            LinearSearchWord(l3, 200, l1[a]) &&
            LinearSearchWord(l4, 100, l1[a])) {
            printword(l1[a]);
        }
    }
}
// 二分探索
void code_b(Word *l1, Word *l2, Word *l3, Word *l4) {
    for (int a = 0; a < 400; a++) {
        if (BinarySearchWord(l2, 300, l1[a]) &&
            BinarySearchWord(l3, 200, l1[a]) &&
            BinarySearchWord(l4, 100, l1[a])) {
            printword(l1[a]);
        }
    }
}
// 二分探索 一番要素数の少ないwordlist4をベースとした
void code_c(Word *l1, Word *l2, Word *l3, Word *l4) {
    for (int a = 0; a < 100; a++) {
        if (BinarySearchWord(l3, 200, l4[a]) &&
            BinarySearchWord(l2, 300, l4[a]) &&
            BinarySearchWord(l1, 400, l4[a])) {
            printword(l4[a]);
        }
    }
}
