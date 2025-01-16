#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// デフォルトで定義されている演算子の個数
#define DEFAULT_OPERATOR_COUNT 6

void *Malloc(size_t size_bytes) {
    void *p = malloc(size_bytes);

    if (p == NULL) {
        printf("failed to allocate %zu bytes allocation.\n", size_bytes);
        exit(EXIT_FAILURE);
    }

    return p;
}

double Add(double x, double y) { return (x + y); }

double Sub(double x, double y) { return (x - y); }

double Mul(double x, double y) { return (x * y); }

double Div(double x, double y) { return (x / y); }

double Remainder(double x, double y) {
    int q = (int)Div(x, y);
    return (x - q * y);
}

double Power(double x, double y) { return pow(x, y); }

// 演算子の情報
typedef struct {
    // 演算子の記号
    char operator;

    // 演算子に関連付ける関数のポインタ
    double (*function)(double, double);

} Operator;

// 数式処理のコンテキスト
typedef struct {
    // 演算子の情報の配列
    Operator *operators;

    // 登録されている演算子の個数
    int size;

} Context;

int currentmemorysize = DEFAULT_OPERATOR_COUNT;
// 新しいコンテキストを作成する
Context *CreateContext(void) {
    Context *p = (Context *)Malloc(sizeof(Context));

    p->operators =
        (Operator *)Malloc(sizeof(Operator) * DEFAULT_OPERATOR_COUNT);

    p->operators[0].operator= '+';
    p->operators[0].function = Add;

    p->operators[1].operator= '-';
    p->operators[1].function = Sub;

    p->operators[2].operator= '*';
    p->operators[2].function = Mul;

    p->operators[3].operator= '/';
    p->operators[3].function = Div;

    p->operators[4].operator= '%';
    p->operators[4].function = fmod;

    p->operators[5].operator= '^';
    p->operators[5].function = pow;

    p->size = DEFAULT_OPERATOR_COUNT;

    return p;
}

// コンテキストを破棄する
void DestroyContext(Context *c) {
    free(c->operators);
    free(c);
}

// コンテキストに新しい演算子を登録する
void RegisterOperator(Context *c, char op, double (*f)(double, double)) {
    if (currentmemorysize >= c->size + 1) {
        c->operators[c->size].operator= op;
        c->operators[c->size].function = f;
        c->size++;
    } else {
        const int newSize = (c->size + 1);
        Operator *newMemory =
            (Operator *)Malloc(sizeof(Operator) * currentmemorysize * 2);

        // 古い演算子を新しいメモリ領域にコピーする
        for (int i = 0; i < c->size; ++i) {
            newMemory[i] = c->operators[i];
        }

        // 新しい演算子を追加する
        newMemory[c->size].operator= op;
        newMemory[c->size].function = f;

        // 古いメモリ領域を解放する
        free(c->operators);

        c->operators = newMemory;
        c->size = newSize;
        currentmemorysize = currentmemorysize * 2;
    }
    /* printf("%d %d\n", currentmemorysize, c->size); */
}

// 指定した演算子に対応する関数を取得する
double (*GetFunction(const Context *c, char op))(double, double) {
    for (int i = 0; i < c->size; ++i) {
        if (c->operators[i].operator== op) {
            return c->operators[i].function;
        }
    }

    printf("undefined operator: %c\n", op);
    exit(EXIT_FAILURE);
}

// 数式 (x 演算子 y) を評価する
double Eval(const Context *c, double x, char op, double y) {
    return GetFunction(c, op)(x, y);
}

/* library end*/

/* double Min(double x, double y) { return fmin(x,y); } */
/**/
/* double Max(double x, double y) { return fmax(x,y); } */

double Average(double x, double y) { return (x + y) * 0.5; }
double Difference(double x, double y) { return fabs(x - y); }

int main(void) {
    // コンテキストを作成する
    Context *ctx = CreateContext();

    // % 演算子を登録する。関連付ける関数は fmod
    // x を y で割った余りを返す
    /* RegisterOperator(ctx, '%', fmod); */

    // ^ 演算子を登録する。関連付ける関数は pow
    // x の y 乗を返す
    /* RegisterOperator(ctx, '^', pow); */

    // m 演算子を登録する。関連付ける関数は Min
    // 2 つの引数のうち小さい方を返す
    RegisterOperator(ctx, 'm', fmin);

    // M 演算子を登録する。関連付ける関数は Max
    // 2 つの引数のうち大きい方を返す
    RegisterOperator(ctx, 'M', fmax);

    // A 演算子を登録する。関連付ける関数は Average
    // 2つの引数の平均を返す
    RegisterOperator(ctx, 'A', Average);

    // A 演算子を登録する。関連付ける関数は Difference
    // 2つの引数の大きさの差を返す
    RegisterOperator(ctx, 'D', Difference);

    // + を使う式
    printf("2.5 + 1.2 = %f\n", Eval(ctx, 2.5, '+', 1.2)); // 3.7

    // - を使う式
    printf("2.5 - 1.2 = %f\n", Eval(ctx, 2.5, '-', 1.2)); // 1.3

    // * を使う式
    printf("2.4 * 1.5 = %f\n", Eval(ctx, 2.4, '*', 1.5)); // 3.6

    // / を使う式
    printf("2.4 / 1.5 = %f\n", Eval(ctx, 2.4, '/', 1.5)); // 1.6

    // % を使う式
    printf("2.5 %% 1.2 = %f\n", Eval(ctx, 2.5, '%', 1.2)); // 0.1

    // ^ を使う式
    printf("1.5 ^ 2.0 = %f\n", Eval(ctx, 1.5, '^', 2.0)); // 2.25

    // m を使う式
    printf("2.5 m 1.2 = %f\n", Eval(ctx, 2.5, 'm', 1.2)); // 1.2

    // M を使う式
    printf("2.5 M 1.2 = %f\n", Eval(ctx, 2.5, 'M', 1.2)); // 2.5

    printf("2.5 A 1.2 = %f\n", Eval(ctx, 2.5, 'A', 1.2)); // 1.85
    printf("2.5 D 1.2 = %f\n", Eval(ctx, 2.5, 'D', 1.2)); // 1.3
    // コンテキストを破棄する
    DestroyContext(ctx);
}
