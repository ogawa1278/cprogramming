#include <ctype.h>
#include <stdio.h>
#define COUNTOF(a) ((int)(sizeof(a) / sizeof(a[0])))

void ShowCharArray(const char *a, int count) {
    for (int i = 0; i < count; ++i) {
        putchar(a[i]);
    }
    putchar('\n');
}

// ここにコードを書く

int Count(char *input, int length, char key) {
    int count = 0;
    for (int i = 0; i < length; i++) {
        if (input[i] == key) {
            count++;
        }
    }
    return count;
}

void Replace(char *input, int length, char a, char b) {
    for (int i = 0; i < length; i++) {
        if (input[i] == a) {
            input[i] = b;
        }
    }
}
int CountMissingAlphabet(char *input, int length) {
    char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
    int missing = 26;
    for (int i = 0; i < length; i++) {
        char a;
        if ('a' <= input[i] && input[i] <= 'z') {
            a = input[i];
        } else if ('A' <= input[i] && input[i] <= 'Z') {
            a = 'a' + input[i] - 'A';
        } else {
            continue;
        }

        if (alphabet[a - 'a'] != 'A') {
            alphabet[a - 'a'] = 'A';
            missing--;
        }
    }
    return missing;
}

void invert(char *input, int start, int fin) {
    char buf;
    for (int i = 0; i < (fin - start + 1) / 2; i++) {
        buf = input[i + start];
        input[i + start] = input[fin - i];
        input[fin - i] = buf;
    }
}
void Exchange(char *input, int length, int cut) {
    invert(input, 0, cut - 1);
    invert(input, cut, length - 1);
    invert(input, 0, length - 1);
}

int main(void) {
    {
        puts("[Count]");
        char a[] = {'W', 'a', 's', 'e', 'd', 'a', '!'};

        printf("%d\n", Count(a, COUNTOF(a), 'a'));
        printf("%d\n", Count(a, COUNTOF(a), 'w'));
    }

    {
        puts("[Replace]");
        char a[] = {'W', 'a', 's', 'e', 'd', 'a', '?'};

        Replace(a, COUNTOF(a), 'a', 'A');
        ShowCharArray(a, COUNTOF(a));

        Replace(a, COUNTOF(a), 'A', '*');
        ShowCharArray(a, COUNTOF(a));
    }

    {
        puts("[CountMissingAlphabet]");
        char a[] = {'b', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
                    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S',
                    'T', 'U', 'V', 'W', 'X', 'y', '!'};
        char b[] = {'W', 'a', 's', 'e', 'd', 'a', 'U', 'n', 'i',
                    'v', 'e', 'r', 's', 'i', 't', 'y', '!'};
        char c[] = {"abcdefghijklmnopqrstuvwxy"};

        printf("%d\n", CountMissingAlphabet(a, COUNTOF(a)));
        printf("%d\n", CountMissingAlphabet(b, COUNTOF(b)));
        printf("%d\n", CountMissingAlphabet(c, COUNTOF(c)));
    }

    {
        puts("[Exchange]");
        char a[] = {'W', 'a', 's', 'e', 'd', 'a', 'U', 'n',
                    'i', 'v', 'e', 'r', 's', 'i', 't', 'y'};

        ShowCharArray(a, COUNTOF(a));
        Exchange(a, COUNTOF(a), 6);
        ShowCharArray(a, COUNTOF(a));
        for (int i = 0; i <= 6; i++) {
            char b[] = "ABCDEF";
            Exchange(b, 6, i);
            ShowCharArray(b, 6);
        }
    }
}
