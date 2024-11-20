#include <stdio.h>
#include <math.h>

void DrawRectangleOutline(int width, int height, char ch)
{
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            printf("%c", (x*y*(x-width+1)*(y-height+1)==0)?ch:' ');
        }
    printf("\n");
    }
}

void DrawCheckerboard(int width, int height, int cellSize, char ch1, char ch2)
{
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            printf("%c", (x/cellSize%2==y/cellSize%2)?ch1:ch2);
        }
    printf("\n");
    }
}

void DrawCircle(int radius)
{
    for(int y=0; y<radius*2; y++){
        for(int x=0; x<radius*2; x++){
            int corner=0;
            if(pow(x-radius,2)+pow(y-radius,2)<=pow(radius,2)){
                corner++;
            }
            if(pow(x-radius+1,2)+pow(y-radius,2)<=pow(radius,2)){
                corner++;
            }
            if(pow(x-radius,2)+pow(y-radius+1,2)<=pow(radius,2)){
                corner++;
            }
            if(pow(x-radius+1,2)+pow(y-radius+1,2)<=pow(radius,2)){
                corner++;
            }
            if(corner>=3){
                printf("#");
            }else if(corner==2){
                printf("+");
            }else{
                printf(" ");
            }
        }
    printf("\n");
    }
}

int main(void)
{
	DrawRectangleOutline(10, 5, '*');
	putchar('\n');
	DrawRectangleOutline(3, 3, '#');
	putchar('\n');
	DrawRectangleOutline(1, 5, '@');
	putchar('\n');
	
	DrawCheckerboard(10, 8, 2, 'O', '#');
	putchar('\n');
	DrawCheckerboard(12, 10, 3, ' ', '#');
	putchar('\n');
	DrawCheckerboard(15, 15, 4, '-', '|');
	putchar('\n');

	DrawCircle(4);
	putchar('\n');
	DrawCircle(6);
	putchar('\n');
	DrawCircle(12);
	putchar('\n');
}
