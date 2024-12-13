#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define COUNTOF(a) ((int)(sizeof(a) / sizeof(a[0])))

typedef struct {
    double x;
    double y;
} Vec2;

void Vec2_Show(const char *s, Vec2 v) { printf("%s: (%f, %f)\n", s, v.x, v.y); }

Vec2 Vec2_Negate(Vec2 v) {
    Vec2 result = {-v.x, -v.y};
    return result;
}

bool Vec2_IsZero(Vec2 v) { return (v.x == 0.0) && (v.y == 0.0); }

// ここにコードを書く

double Vec2_Length(Vec2 v) { return sqrt(pow(v.x, 2) + pow(v.y, 2)); }
double Vec2_Dot(Vec2 v1, Vec2 v2) { return v1.x * v2.x + v1.y * v2.y; }
double Vec2_HasSameDirection(Vec2 v1, Vec2 v2) {
    if (Vec2_IsZero(v1) || Vec2_IsZero(v2)) {
        return false;
    } else {
        return Vec2_Dot(v1, v2) > 0;
    }
}
double Vec2_MaxComponent(Vec2 v) { return (v.x > v.y) ? v.x : v.y; }
double Vec2_ManhattanDistance(Vec2 v1, Vec2 v2) {
    return fabs(v1.x - v2.x) + fabs(v1.y - v2.y);
}
double Vec2_ChebyshevDistance(Vec2 v1, Vec2 v2) {
    double x = fabs(v1.x - v2.x);
    double y = fabs(v1.y - v2.y);
    return (x > y) ? x : y;
}
Vec2 Vec2_Normalize(Vec2 v) {
    Vec2 vreturn;
    if (Vec2_IsZero(v)) {
        vreturn.x = 0;
        vreturn.y = 0;
    } else {
        vreturn.x = v.x / Vec2_Length(v);
        vreturn.y = v.y / Vec2_Length(v);
    }
    return vreturn;
}
Vec2 Vec2_Midpoint(Vec2 v1, Vec2 v2) {
    Vec2 vreturn = {v1.x * 0.5 + v2.x * 0.5, v1.y * 0.5 + v2.y * 0.5};
    return vreturn;
}
Vec2 Vec2_Lerp(Vec2 v1, Vec2 v2, double t) {
    Vec2 vreturn = {v1.x * (1 - t) + v2.x * t, v1.y * (1 - t) + v2.y * t};
    return vreturn;
}
int CompareVec2ByLength(const void *p1, const void *p2) {
    Vec2 *v1 = (Vec2 *)p1;
    Vec2 *v2 = (Vec2 *)p2;
    return ((pow(v1->x, 2) + pow(v1->y, 2)) > (pow(v2->x, 2) + pow(v2->y, 2))) -
           ((pow(v1->x, 2) + pow(v1->y, 2)) < (pow(v2->x, 2) + pow(v2->y, 2)));
}
void Vec2_ShowWithLength(Vec2 v) {
    printf("(%f, %f)\tlength = %f\n", v.x, v.y, Vec2_Length(v));
}

int main(void) {
    {
        const Vec2 v1 = {1.2, 2.4};
        const Vec2 v2 = {-3.6, -4.8};
        const Vec2 v3 = {-1.0, 0.0};
        const Vec2 v4 = {0.0, -4.0};
        const Vec2 v5 = {0.0, 0.0};

        Vec2_Show("v1", v1);
        Vec2_Show("v2", v2);
        Vec2_Show("v3", v3);
        Vec2_Show("v4", v4);
        Vec2_Show("v5", v5);

        puts("--------");

        Vec2_Show("Vec2_Negate(v1)", Vec2_Negate(v1));
        Vec2_Show("Vec2_Negate(v2)", Vec2_Negate(v2));
        Vec2_Show("Vec2_Negate(v3)", Vec2_Negate(v3));
        Vec2_Show("Vec2_Negate(v4)", Vec2_Negate(v4));
        Vec2_Show("Vec2_Negate(v5)", Vec2_Negate(v5));

        puts("--------");

        printf("Vec2_IsZero(v1): %s\n", (Vec2_IsZero(v1) ? "true" : "false"));
        printf("Vec2_IsZero(v2): %s\n", (Vec2_IsZero(v2) ? "true" : "false"));
        printf("Vec2_IsZero(v3): %s\n", (Vec2_IsZero(v3) ? "true" : "false"));
        printf("Vec2_IsZero(v4): %s\n", (Vec2_IsZero(v4) ? "true" : "false"));
        printf("Vec2_IsZero(v5): %s\n", (Vec2_IsZero(v5) ? "true" : "false"));

        puts("--------");

        printf("Vec2_Length(v1): %f\n", Vec2_Length(v1));
        printf("Vec2_Length(v2): %f\n", Vec2_Length(v2));
        printf("Vec2_Length(v3): %f\n", Vec2_Length(v3));
        printf("Vec2_Length(v4): %f\n", Vec2_Length(v4));
        printf("Vec2_Length(v5): %f\n", Vec2_Length(v5));

        puts("--------");

        printf("Vec2_Dot(v1, v2): %f\n", Vec2_Dot(v1, v2));
        printf("Vec2_Dot(v2, v3): %f\n", Vec2_Dot(v2, v3));
        printf("Vec2_Dot(v3, v4): %f\n", Vec2_Dot(v3, v4));
        printf("Vec2_Dot(v4, v5): %f\n", Vec2_Dot(v4, v5));
        printf("Vec2_Dot(v5, v1): %f\n", Vec2_Dot(v5, v1));

        puts("--------");

        printf("Vec2_HasSameDirection(v1, v2): %s\n",
               (Vec2_HasSameDirection(v1, v2) ? "true" : "false"));
        printf("Vec2_HasSameDirection(v2, v3): %s\n",
               (Vec2_HasSameDirection(v2, v3) ? "true" : "false"));
        printf("Vec2_HasSameDirection(v3, v4): %s\n",
               (Vec2_HasSameDirection(v3, v4) ? "true" : "false"));
        printf("Vec2_HasSameDirection(v4, v5): %s\n",
               (Vec2_HasSameDirection(v4, v5) ? "true" : "false"));
        printf("Vec2_HasSameDirection(v5, v1): %s\n",
               (Vec2_HasSameDirection(v5, v1) ? "true" : "false"));

        puts("--------");

        printf("Vec2_MaxComponent(v1): %f\n", Vec2_MaxComponent(v1));
        printf("Vec2_MaxComponent(v2): %f\n", Vec2_MaxComponent(v2));
        printf("Vec2_MaxComponent(v3): %f\n", Vec2_MaxComponent(v3));
        printf("Vec2_MaxComponent(v4): %f\n", Vec2_MaxComponent(v4));
        printf("Vec2_MaxComponent(v5): %f\n", Vec2_MaxComponent(v5));

        puts("--------");

        printf("Vec2_ManhattanDistance(v1, v2): %f\n",
               Vec2_ManhattanDistance(v1, v2));
        printf("Vec2_ManhattanDistance(v2, v3): %f\n",
               Vec2_ManhattanDistance(v2, v3));
        printf("Vec2_ManhattanDistance(v3, v4): %f\n",
               Vec2_ManhattanDistance(v3, v4));
        printf("Vec2_ManhattanDistance(v4, v5): %f\n",
               Vec2_ManhattanDistance(v4, v5));
        printf("Vec2_ManhattanDistance(v5, v1): %f\n",
               Vec2_ManhattanDistance(v5, v1));

        puts("--------");

        printf("Vec2_ChebyshevDistance(v1, v2): %f\n",
               Vec2_ChebyshevDistance(v1, v2));
        printf("Vec2_ChebyshevDistance(v2, v3): %f\n",
               Vec2_ChebyshevDistance(v2, v3));
        printf("Vec2_ChebyshevDistance(v3, v4): %f\n",
               Vec2_ChebyshevDistance(v3, v4));
        printf("Vec2_ChebyshevDistance(v4, v5): %f\n",
               Vec2_ChebyshevDistance(v4, v5));
        printf("Vec2_ChebyshevDistance(v5, v1): %f\n",
               Vec2_ChebyshevDistance(v5, v1));

        puts("--------");

        Vec2_Show("Vec2_Normalize(v1)", Vec2_Normalize(v1));
        Vec2_Show("Vec2_Normalize(v2)", Vec2_Normalize(v2));
        Vec2_Show("Vec2_Normalize(v3)", Vec2_Normalize(v3));
        Vec2_Show("Vec2_Normalize(v4)", Vec2_Normalize(v4));
        Vec2_Show("Vec2_Normalize(v5)", Vec2_Normalize(v5));

        puts("--------");

        Vec2_Show("Vec2_Midpoint(v1, v2)", Vec2_Midpoint(v1, v2));
        Vec2_Show("Vec2_Midpoint(v2, v3)", Vec2_Midpoint(v2, v3));
        Vec2_Show("Vec2_Midpoint(v3, v4)", Vec2_Midpoint(v3, v4));
        Vec2_Show("Vec2_Midpoint(v4, v5)", Vec2_Midpoint(v4, v5));
        Vec2_Show("Vec2_Midpoint(v5, v1)", Vec2_Midpoint(v5, v1));

        puts("--------");

        for (int i = 0; i <= 10; ++i) {
            double t = (i / 10.0);
            Vec2 v = Vec2_Lerp(v1, v2, t);
            printf("Vec2_Lerp(v1, v2, %.1f): (%f, %f)\n", t, v.x, v.y);
        }

        puts("--------");
    }

    {
        // 2 次元ベクトルの配列（採点時は中身が変わる）
        Vec2 vectors[] = {
            {0.1, 0.2}, {2.1, 1.5},   {5.9, 0.6},  {1.2, 3.5},   {-0.3, -0.6},
            {0.0, 0.0}, {-0.1, 0.05}, {10.0, 9.9}, {-6.6, -5.5},
        };

        qsort(vectors, COUNTOF(vectors), sizeof(Vec2), CompareVec2ByLength);

        for (int i = 0; i < COUNTOF(vectors); ++i) {
            Vec2_ShowWithLength(vectors[i]);
        }
    }
}
