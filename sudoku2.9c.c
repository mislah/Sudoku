/*Sudoku v2.9
*Written and compiled on gcc 9.3.0, Ubuntu 20.04
*Code includes certain functions that would require different libraries on windows platform */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <string.h>
#include <limits.h>
struct highscore {// To keep a track of the minimum time the user takes to solve the code
	int score[5];
	char name[5][21];
};

//Declaration of the funcitons used in the code

void display(short[9][9]);//function for displaying the puzzle grid of sudoku
void genpuz(short[9][9], int); 			//this function generates the puzzle for player to play.
void respuz(short[9][9], int);			//this function displays the result of the sudoku problem.
short chkcomp(short[9][9]);				//function to check that all cells are filled or not.
int chksolvable(short[9][9]);			//function to check the solvability of a grid.	
int isallowed(short[9][9], int, int, int);		//function to check whether an element at a certain position satisfies the sudoku rules or not
int solve(short[9][9], int, int);	
int edit(short[9][9], int, int*, int*);
int getin(void);			//this function's job is to get into the sudoku grid and manipulate the grid.
void help(void);			//This function displays the help for user.
void about(void);			//The credits and about of the game.
void prinths(int);				//function to print the highscore.
void writehs(int, int);			//function to update the highscore.
//Declarations end and the program starts with main() definition.
int main(void) {
	
	system("/usr/X11/bin/resize -s 22 87 ");//To resize the user window for better visual experience (Mac only)
	system("printf '\033[8;22;87t'"); //working in Linux: to resize the window for better viewing experience
	short A[9][9];
	int n;
	struct termios def, off;
	tcgetattr(STDIN_FILENO, &def);
	off = def;
	off.c_lflag &= ~(ECHO | ICANON); //switching off the echo so typing charecter doesn't appear
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &off);
	printf("\e[?25l");		//hiding the curser
	do {
	mainmenu:    		 // Code for generating the main menu which will appear at the home screen.
		fflush(stdout);
		system("clear");
		printf("1: Game\n2: Solver\n3: Help\n4: Highscore\n5: About\n6: Exit");		//Home menu options
		n = getin();
		int q, opt, x = 0, y = 0;
		switch (n) {
		case 1:
		newgame:
			respuz(A, 0);
			do {
				fflush(stdout);
				system("clear");
				printf("New Game\n1: Easy\n2: Medium\n3: Hard\n4: Extreme\nq: Main Menu");		//Putting up the options for the users 
				q = getin();
			} while (!(q >= 1 && q <= 4 || q == -2));
			long tstart, ttaken;
			time(&tstart);
			switch (q) {
			case 1:
				genpuz(A, 70);
				break;
			case 2:
				genpuz(A, 45);
				break;
			case 3:
				genpuz(A, 30);
				break;
			case 4:
				genpuz(A, 22);
				break;
			case -2:
				goto mainmenu;
			}
			while (1) {
				display(A);
				if (edit(A, 1, &x, &y)) {
					display(A);
					time(&ttaken);
					ttaken -= tstart;
					printf("\e[8;44fTime taken: %ld mins %ld sec", ttaken / 60, ttaken % 60);			
					printf("\e[9;44f1: Clear Input\e[10;44f2: View Solution\e[11;44f3: New Puzzle\e[12;44f4: Main Menu\e[13;44f5: Quit");	//Whenever the users presses q we put up the pause menu
					opt = getin();
					switch (opt) {
					case 1:
						respuz(A, 1);
						break;
					case 2:
						respuz(A, 1);
						solve(A, 0, 0);
						int c;
						do {
							display(A);
							c = getin();
						} while (c != -2);
						goto mainmenu;
					case 3:
						goto newgame;
					case 4:
						goto mainmenu;
					case 5:
						goto end;
					}
				}
				else {
					break;
				}
			}
			time(&ttaken);
			ttaken -= tstart;
			display(A);
			printf("\e[11;44fCongratulations! You Solved The Sudoku!");
			printf("\e[12;44fTime taken: %ld mins %ld sec", ttaken / 60, ttaken % 60);
			tcsetattr(STDIN_FILENO, TCSAFLUSH, &def); 			//enable canonoical nd echo
			writehs(q, (int)ttaken);
			tcsetattr(STDIN_FILENO, TCSAFLUSH, &off); 			//disable
			getin();
			fflush(stdout);
			break;
		case 2:
			respuz(A, 0);
			while (1) {
				display(A);
				if (edit(A, 0, &x, &y)) {
					printf("\e[8;44f1: Solve\e[9;44f2: Reset\e[10;44f3: Main Menu\e[11;44f4: Exit");
					opt = getin();
					switch (opt) {
					case 1:
						respuz(A, 2);
						if (!chksolvable(A) || !solve(A, 0, 0)) {
							respuz(A, 1);
							respuz(A, 3);
							display(A);
							printf("\e[11;44fNo solution exists!");
							fflush(stdout);
							usleep(2000000);
						}
						else {
							display(A);
							opt = getin();
						}
						break;
					case 2:
						respuz(A, 0);
						break;
					case 3:
						goto mainmenu;
					case 4:
						goto end;
					}
				}
				respuz(A, 3);
			}
			break;
		case 3:
			help();
			break;
		case 4:
			while (1) {
				do {
					fflush(stdout);
					system("clear");
					printf("Highscores\n1: Easy\n2: Medium\n3: Hard\n4: Extreme\nq: Main Menu");
					q = getin();
				} while (!(q >= 1 && q <= 4 || q == -2));
				if (q == -2) {
					break;
				}
				prinths(q);
				getin();
			}
			break;
		case 5:
			about();
			break;
		}
	} while (n != 6);
end:
	printf("\e[?25h");
	fflush(stdout);
	system("clear");
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &def);
	return 0;
}
int edit(short A[9][9], int chk, int* x, int* y) {
	printf("\e[?25h");				//show curser
	int in, i, j;
	fflush(stdout);
	for (i = *x;i < 9;i++) {
		for (j = *y;j < 9;) {
			printf("\e[%d;%df", 3 + 2 * i, 5 + 4 * j);
			fflush(stdout);
			in = getin();
			if (in == -2) {
				*x = i;
				*y = j;
				printf("\e[?25l");
				return 1;
			}
			else if (in < 10 && in != -1 && A[i][j] < 10) {
				A[i][j] = in;
				if (in != 0) {
					printf("%d", in);
				}
				if (in == 0) {
					printf(" ");
				}
				fflush(stdout);
			}
			else if (in == 11) {
				if (i != 0) {
					i--;
				}
			}
			else if (in == 22) {
				if (i != 8) {
					i++;
				}
			}
			else if (in == 33) {
				if (!(i == 8 && j == 8)) {
					j++;
				}
			}
			else if (in == 44) {
				if (j != 0) {
					j--;
				}
				else if (i != 0) {
					j = 8;
					i--;
				}
				else {
					j = 0;
				}

			}
			if (chk == 1 && chkcomp(A) == 1) {
				if (chksolvable(A)) {
					printf("\e[?25l");
					return 0;
				}
			}
		}
	}
}
int getin(void) { //function for getting into the grid, navigating through cells and doing operation on the grid  
	char c;//for capturing keypress
	fflush(stdout);
	if (read(STDIN_FILENO, &c, 1) == 1) { //reading character and storing it in variable 'c'
	//checking keypress and cases below
		if (c == '\e') {
			char seq[3];
			if (read(STDIN_FILENO, &seq[0], 1) != 1) {
				return -1;
			}
			if (read(STDIN_FILENO, &seq[1], 1) != 1) {
				return -1;
			}
			if (seq[0] == '[') { //checking for different keypress.
				switch (seq[1]) {
				case 'A':
					return 11; //up
				case 'B':
					return 22; //down
				case 'C':
					return 33; //right
				case 'D':
					return 44; //left
				}
			}
		}
		else if (c == 'q' || c == 'Q') { //checking the keypress, if q is pressed then directing for the appearace of menu wherever one is. 
			return -2;
		}
		else if (c - '0' >= 0 && c - '0' <= 9) { //returning the number inputted by user to be entered in a cell.
			return c - '0';
		}
		else {
			return 0; //does this by default
		}
	}
}
int isallowed(short A[9][9], int m, int  n, int k) {		//function to check if a particular value of A[i][j] is allowed at a particular position (x,y) or not
	for (int i = 0;i < 9;i++) {	//first loop to check for the element in the entire grid
		if (A[i][n] == k || A[i][n] - 10 == k) {
			return 0;
		}
		if (A[m][i] == k || A[m][i] - 10 == k) {
			return 0;
		}
	}
	for (int i = m - m % 3;i < m - m % 3 + 3;i++) {	//second loop to check the element in its particular subgrid
		for (int j = n - n % 3; j < n - n % 3 + 3;j++) {
			if (A[i][j] == k || A[i][j] - 10 == k) {
				return 0;
			}
		}
	}
	return 1;
}
void genpuz(short A[9][9], int d) { //this function generates the puzzle for the player
	int r[9], z = 0, tmp, i, j, k;
	srand(time(0));
	for (i = 0;i < 9;i++) {			//fill array r
		r[i] = i + 1;
	}
	do {						//shuffle array r and enter it to diagonal elements
		for (i = 9;i > 0;i--) {
			k = rand() % i;//generating random numbers between 1 to 9.
			tmp = r[i - 1];
			r[i - 1] = r[k]; 
			r[k] = tmp;
		}
		k = 0;
		for (i = z; i < 3 + z;i++) {
			for (j = z;j < 3 + z;j++) {
				A[i][j] = r[k]; //putting random numbers at each location traversed by loop.
				k++;
			}
		}
		z += 3;
	} while (z != 9);// do while ends here
	solve(A, 0, 0);
	for (int i = 0;i < 81 - d;i++) {			//remove random numbers present in cells.
		int a = rand() % 9;
		int b = rand() % 9;
		if (A[a][b] != 0) {
			A[a][b] = 0;
		}
		else {
			i--;
		}
	}
	respuz(A, 2);
}
void respuz(short A[9][9], int mode) {
	int i, j;
	switch (mode) {
	case 0:				//clear
		for (i = 0;i < 9;i++) {
			for (j = 0;j < 9;j++) {
				A[i][j] = 0;
			}
		}
		break;
	case 1:					//clear user input
		for (i = 0;i < 9;i++) {
			for (j = 0;j < 9;j++) {
				if (A[i][j] < 10) {
					A[i][j] = 0;
				}
			}
		}
		break;
	case 2:					//upgrade sys
		for (i = 0;i < 9;i++) {
			for (j = 0;j < 9;j++) {
				if (A[i][j] != 0) {
					A[i][j] += 10;
				}
			}
		}
		break;
	case 3:					//downgrade sys
		for (int i = 0;i < 9;i++) {
			for (int j = 0;j < 9;j++) {
				if (A[i][j] > 10) {
					A[i][j] -= 10;
				}
			}
		}
		break;
	}
}
int chksolvable(short A[9][9]) {	//function to check the solvability of a grid
	int a;
	for (int i = 0;i < 9;i++) {
		for (int j = 0;j < 9;j++) {
			if (A[i][j] != 0) {
				a = A[i][j];
				A[i][j] = 0;
				if (!isallowed(A, i, j, a)) {	//checking if a particular value of A[i][j] violates the rules at a particular position (x,y) or not
					A[i][j] = a;
					return 0;
				}
				else {
					A[i][j] = a;
				}
			}
		}
	}
	return 1;
}
int solve(short A[9][9], int i, int j) {
	if (i == 8 && j == 9) {
		return 1;
	}
	if (j == 9)
	{
		i++;
		j = 0;
	}
	if (A[i][j] > 0) {
		return solve(A, i, j + 1);
	}
	for (int n = 1; n <= 9; n++)
	{
		if (isallowed(A, i, j, n) == 1)
		{
			A[i][j] = n;
			if (solve(A, i, j + 1) == 1) {
				return 1;
			}
		}
		A[i][j] = 0;
	}
	return 0;
}
short chkcomp(short A[9][9]) {					//checks if all cells are filled
	for (int i = 0;i < 9;i++) {
		for (int j = 0;j < 9;j++) {
			if (A[i][j] == 0) {
				return 0;
			}
		}
	}
	return 1;
}
void prinths(int n) {						//function to print highscore
	n--;
	fflush(stdout);
	system("clear");
	struct highscore hs;
	FILE* fptr;
	if ((fptr = fopen("sudoku.bin", "r")) == NULL) {	//if no file
		printf("No records!");
		return;
	}
	n++;
	while (n--) {
		fread(&hs, sizeof(struct highscore), 1, fptr);
	}
	fclose(fptr);
	if (hs.score[0] == INT_MAX) {
		printf("No records!");
		return;
	}
	for (int i = 0;i < 5;i++) {
		if (hs.score[i] == INT_MAX) {
			return;
		}
		printf("%d. %dmin %dsec %s", i + 1, hs.score[i] / 60, hs.score[i] % 60, hs.name[i]);	//printing highscore
	}
}
void writehs(int n, int score) {		//function for updating highscores
	n--;
	struct highscore d[4];
	int i;
	char name[21];
	FILE* fptr;
	if ((fptr = fopen("sudoku.bin", "rb")) == NULL) {			//if no file
		fptr = fopen("sudoku.bin", "wb");
		for (i = 0;i < 4;i++) {
			for (int j = 0;j < 5;j++) {
				d[i].score[j] = INT_MAX;
				d[i].name[j][0] = '\0';
			}
			fwrite(&d[i], sizeof(struct highscore), 1, fptr);
		}
		fclose(fptr);
		fptr = fopen("sudoku.bin", "rb");
	}
	for (i = 0;i < 4;i++) {
		fread(&d[i], sizeof(struct highscore), 1, fptr);
	}
	for (i = 0;i < 5;i++) {
		if (d[n].score[i] > score) {
			printf("\e[?25h");
			if (i == 0) {
				printf("\e[13;44fNew Highscore!\e[14;44f");
			}
			else {
				printf("\e[13;44f");
			}
			printf("Enter your name : ");
			fgets(name, 21, stdin); 				//supports space & avoids buffer overflow
			printf("\e[?25l"); 						//hides cursor
			for (i;i < 5;i++) {
				d[n].score[4] = d[n].score[i];
				d[n].score[i] = score;
				score = d[n].score[4];
				strcpy(d[n].name[4], d[n].name[i]);
				strcpy(d[n].name[i], name);
				strcpy(name, d[n].name[4]);
			}
			fclose(fptr);
			fptr = fopen("sudoku.bin", "wb");
			for (i = 0;i < 4;i++) {
				fwrite(&d[i], sizeof(struct highscore), 1, fptr);
			}
			fclose(fptr);
			prinths(n + 1);
			break;
		}
	}
}
void help(void) {//This function displays the assistance for the gamer.
	char c; //storing character for keypress
	fflush(stdout);
	system("clear");
	printf("=======================================================================================\n||-------------------------------HELP AND INSTURCTIONS-------------------------------||\n=======================================================================================\n");
	printf("Whenever or wherever you are just press the 'q' key to open a menu that will guide you.\nUse your arrow key to navigate through the grids and hola there you go.\nUse ←↑↓→ for left, up, down and right navigation respectively.\n→ This game consists of a 9x9 grid which is partyly filled.\n→ Complete the incomplete puzzle keeping mind the following rules.\n→ Press the number on your keyboard to fill that number in any cell.\n→ Press any other key to clear any cells.\n");
	//rules
	printf("=======================================================================================\n||---------------------------------------RULES---------------------------------------||\n=======================================================================================\n");
	printf("→ Use only numbers 1 to 9.\n→ Each row and each column should have only one occurence of all nine numbers.\n→ Each 3x3 subgrid should have numbers from 1 to 9 occuring once.\n");
	fflush(stdout);
	read(STDIN_FILENO, &c, 1); //exits this on any keypress.
}
void about(void) { //about function which displays the information of this game
	char c;
	fflush(stdout);
	system("clear");
	printf("Sudoku v2.9\n\nDeveloped as a computer science project by the students of sem-I of batch 2020-2024,\nIndian Institute of Information Technology Kalyani\n\n");
	printf("Inspired by Dr. Bhaskar Biswas, Innovated by Team Brogrammers\n\n");
	printf("Credits:\nAli Asad Quasim\nApurba Nath\nDevadi Yekaditya\nHritwik Ghosh\nMislah Rahman\nSoumalya Biswas\nSriramsetty Bhanu Teja\nSuryansh Sisodia\nVemana Joshua Immanuel\nYashraj Singh");
	fflush(stdout);
	read(STDIN_FILENO, &c, 1);
}
void display(short A[9][9]) { //this function everytime shows the updated grid of sudoku
	fflush(stdout);
	system("clear");
	//printf("\e[38;5;166m");//add for color

	//Printing the Grid

	printf("\n  ╔═══╤═══╤═══╦═══╤═══╤═══╦═══╤═══╤═══╗\n  ║   │   │   ║   │   │   ║   │   │   ║\n  ╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n  ║   │   │   ║   │   │   ║   │   │   ║\n  ╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n  ║   │   │   ║   │   │   ║   │   │   ║\n  ╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣\n  ║   │   │   ║   │   │   ║   │   │   ║\n  ╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n  ║   │   │   ║   │   │   ║   │   │   ║\n  ╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n  ║   │   │   ║   │   │   ║   │   │   ║\n  ╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣\n  ║   │   │   ║   │   │   ║   │   │   ║\n  ╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n  ║   │   │   ║   │   │   ║   │   │   ║\n  ╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n  ║   │   │   ║   │   │   ║   │   │   ║\n  ╚═══╧═══╧═══╩═══╧═══╧═══╩═══╧═══╧═══╝\n\n");
	//printf("\e[m");//add for color
	for (int i = 0;i < 9;i++) {
		for (int j = 0;j < 9;j++) {
			if (A[i][j] == 0) {//if there is 0 value here in that place of 2d array then nothing is to be filled in the grid displayed. 
				continue;
			}
			if (A[i][j] < 10) { //just traversing the whole grid(a 2-d Array) and putting the respective element in the specific grid  
				printf("\e[%d;%df%hu", 3 + 2 * i, 5 + 4 * j, A[i][j]); //putting cursor at position and printing the number
			}
			else {
				char Bold[] = { "𝟬" };
				Bold[3] += A[i][j] - 10;
				printf("\e[%d;%df%s", 3 + 2 * i, 5 + 4 * j, Bold);
			}
		}
	}
	printf("\e[22;0f");
}