#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// min 以上 max 以下のランダムな整数を返す
int RandomInt(int min, int max) {
    return min + (int)(rand() * (max - min + 1.0) / (RAND_MAX + 1.0));
}

// ここにコードを書いてよい

// 6 個のサイコロを振ったときの目の合計を返す
int Roll6D6(void) {
    // ここにコードを書く
    int sum = 0;
    for (int i = 0; i < 6; i++) {
        sum += RandomInt(1, 6);
    }
    return sum;
}

// ここにコードを書いてよい

int main(void) {
    srand(time(NULL));

    int n;
    scanf("%d", &n);

    // ここにコードを書く

    int result[31] = {0};
    for (int i = 0; i < n; i++) {
        result[Roll6D6() - 6]++;
    }
    for (int i = 0; i < 31; i++) {
        printf("%2d: ", i + 6);
        for (int j = 0; j < result[i] / ((n / 400 > 1) ? n / 400 : 1); j++) {
            printf("*");
        }
        printf("(%d)\n", result[i]);
    }
}
