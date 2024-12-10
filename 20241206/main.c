#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void ShowBool(bool b) { puts(b ? "true" : "false"); }

bool IsRhyme(const char *input) {
    // ここにコードを書く
    int a = 16, i;
    if (input[0] == 0) {
        return true;
    }
    for (i = 0; input[i] != '\0'; i++) {
        if (input[i + 1] == ' ' || input[i + 1] == '\0') {
            if (a == 16) {
                a = input[i] % 16;
            } else if (a != input[i] % 16) {
                return false;
            }
        }
    }

    return true;
}

int main(void) {
    ShowBool(IsRhyme("banana pizza tapioca pasta"));
    ShowBool(IsRhyme("BUTTER KEBAB CRAB burger"));
    ShowBool(IsRhyme("Egg Pudding Coleslaw Stew"));
    ShowBool(IsRhyme("Cold night wrapped hard ground tight"));

    ShowBool(IsRhyme("banana KEBAB CRAB burger"));
    ShowBool(IsRhyme("Lemon Salmon Bread Salad"));
    ShowBool(IsRhyme("Fries a"));
    ShowBool(IsRhyme("The quick brown fox jumps over the lazy dog"));

    ShowBool(IsRhyme(""));
    ShowBool(IsRhyme("C"));
    ShowBool(IsRhyme("Waseda"));
    ShowBool(IsRhyme("Waseda q"));
}
