#include <stdio.h>
#include <ctype.h>
#define COUNTOF(a) ((int)(sizeof(a)/sizeof(a[0])))

void ShowCharArray(const char* a, int count)
{
	for (int i = 0; i < count; ++i)
	{
		putchar(a[i]);
	}

	putchar('\n');
}

// ここにコードを書く

int Count(char* input, int length, char key){
    int count =0;
    for(int i=0; i<length; i++){
       if( input[i]==key){
           count++;
       }
    }
    return count;
}

void Replace(char* input, int length, char a, char b){
    for(int i=0; i<length; i++){
        if(input[i]==a){
            input[i]=b;
        }
    }
}
int CountMissingAlphabet(char* input, int length){}
void Exchange(char* input, int length, int location){}

int main(void)
{
	{
		puts("[Count]");
		char a[] = { 'W', 'a', 's', 'e', 'd', 'a', '!'  };

		printf("%d\n", Count(a, COUNTOF(a), 'a'));
		printf("%d\n", Count(a, COUNTOF(a), 'w'));
	}

	{
		puts("[Replace]");
		char a[] = { 'W', 'a', 's', 'e', 'd', 'a', '?'  };

		Replace(a, COUNTOF(a), 'a', 'A');
		ShowCharArray(a, COUNTOF(a));

		Replace(a, COUNTOF(a), 'A', '*');
		ShowCharArray(a, COUNTOF(a));
	}

	{
		puts("[CountMissingAlphabet]");
		char a[] = { 'b', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'y', '!'  };
		char b[] = { 'W', 'a', 's', 'e', 'd', 'a', 'U', 'n', 'i', 'v', 'e', 'r', 's', 'i', 't', 'y', '!'  };

		printf("%d\n", CountMissingAlphabet(a, COUNTOF(a)));
		printf("%d\n", CountMissingAlphabet(b, COUNTOF(b)));
	}

	{
		puts("[Exchange]");
		char a[] = { 'W', 'a', 's', 'e', 'd', 'a', 'U', 'n', 'i', 'v', 'e', 'r', 's', 'i', 't', 'y' };

		ShowCharArray(a, COUNTOF(a));
		Exchange(a, COUNTOF(a), 9);
		ShowCharArray(a, COUNTOF(a));
	}
}
