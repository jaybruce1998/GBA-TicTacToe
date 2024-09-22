#include <gba.h>
#include <gba_video.h>
#include <gba_input.h>

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160
#define CELL_SIZE (SCREEN_HEIGHT / 3) // Size of each cell for a 3x3 grid

// Colors
#define BLACK RGB5(0, 0, 0)
#define WHITE RGB5(31, 31, 31)
//it should be called GRB!
#define RED RGB5(31, 0, 0)
#define GREEN RGB5(0, 31, 0)
#define BLUE RGB5(0, 0, 31)

// Display modes
#define DCNT_MODE3 0x0003
#define DCNT_BG2   0x0400

char board[3][3];
int cur;
char turn, winner;

void drawPixel(int x, int y, u16 color) {
    *((volatile u16*)VRAM + y * SCREEN_WIDTH + x) = color;
}

void clearScreen() {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        *((volatile u16*)VRAM + i) = WHITE;
    }
}

void drawX(int r, int c) {
    // Calculate the top-left corner of the cell
    int startX = c * CELL_SIZE;
    int startY = r * CELL_SIZE;

    // Draw the two diagonal lines of the X
    for (int i = 0; i < CELL_SIZE; i++) {
        drawPixel(startX + i, startY + i, RED); // Draw one diagonal (top-left to bottom-right)
        drawPixel(startX + (CELL_SIZE - 1 - i), startY + i, RED); // Draw the other diagonal (top-right to bottom-left)
    }
}

void drawO(int r, int c) {
    // Calculate the center of the cell
    int centerX = c * CELL_SIZE + CELL_SIZE / 2;
    int centerY = r * CELL_SIZE + CELL_SIZE / 2;

    // Radius for the O
    int radius = CELL_SIZE / 2 - 1;
    int r2 = radius * radius;
	int dif;
    // Draw the O using a simple circle algorithm
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
        	dif = r2 - (x*x+y*y);
            if (dif >= 0 && dif < 50) { // gives the circle some depth
                drawPixel(centerX + x, centerY + y, BLUE);
            }
        }
    }
}

// Character pixel definitions
int X[][2] = {{0, 0}, {1, 1}, {2, 2}, {1, 3}, {0, 4}, {3, 3}, {4, 4}, {3, 1}, {4, 0}, {-1, 0}};
int O[][2] = {{1, 0}, {2, 0}, {3, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 4}, {2, 4}, {3, 4}, {4, 1}, {4, 2}, {4, 3}, {-1, 0}};
int W[][2] = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 4}, {2, 2}, {2, 3}, {2, 4}, {3, 4}, {4, 0}, {4, 1}, {4, 2}, {4, 3}, {4, 4}, {-1, 0}};
int I[][2] = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {-1, 0}};
int N[][2] = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 1}, {2, 2}, {3, 3}, {4, 0}, {4, 1}, {4, 2}, {4, 3}, {4, 4}, {-1, 0}};
int S[][2] = {{1, 0}, {2, 0}, {3, 0}, {0, 1}, {1, 2}, {2, 2}, {3, 2}, {4, 3}, {1, 4}, {2, 4}, {3, 4}, {-1, 0}};
int T[][2] = {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {-1, 0}};
int E[][2] = {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {0, 1}, {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {0, 3}, {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {-1, 0}};

// Function to draw a character
void drawCharacter(int startX, int startY, int character[][2]) {
    for (int i = 0; character[i][0]!=-1; i++) {
        drawPixel(startX + character[i][0], startY + character[i][1], BLACK);
    }
}

// Example usage
void displayWinner(char winner) {
    switch (winner) {
        case 'X':
            drawCharacter(160, 10, X);
            drawCharacter(172, 10, W);
            drawCharacter(178, 10, I);
            drawCharacter(184, 10, N);
            drawCharacter(190, 10, S);
            break;
        case 'O':
            drawCharacter(160, 10, O);
            drawCharacter(172, 10, W);
            drawCharacter(178, 10, I);
            drawCharacter(184, 10, N);
            drawCharacter(190, 10, S);
            break;
        default:
            drawCharacter(160, 10, T);
            drawCharacter(166, 10, I);
            drawCharacter(172, 10, E);
    }
}


void drawBoard() {
	clearScreen();
	
    // Draw vertical lines
    for (int i = 1; i < 3; i++) {
        for (int j = 0; j < SCREEN_HEIGHT; j++) {
            *((volatile u16*)VRAM + (j * SCREEN_WIDTH + (i * CELL_SIZE))) = BLACK;
        }
    }
    // Draw horizontal lines
    for (int i = 1; i < 3; i++) {
        for (int j = 0; j < SCREEN_HEIGHT-1; j++) {
            *((volatile u16*)VRAM + ((i * CELL_SIZE) * SCREEN_WIDTH + j)) = BLACK;
        }
    }
    for(int i=0; i<3; i++)
    	for(int j=0; j<3; j++)
    		if(board[i][j]=='X')
    			drawX(i, j);
    		else if(board[i][j]=='O')
    			drawO(i, j);
}

void drawOutlineSquare(int x, int y) {
	for(int j=0; j<2; j++)
	{
		for (int i = 0; i < CELL_SIZE; i++) {
			drawPixel(x+i, y+j, GREEN);
			drawPixel(x+i, CELL_SIZE+y-j, GREEN);
			drawPixel(x+j, y+i, GREEN);
			drawPixel(CELL_SIZE+x-j, y+i, GREEN);
		}
    }
}

void resetBoard() {
    cur=1;
    turn='X';
    winner='w';
    for(int i=0; i<3; i++)
    	for(int j=0; j<3; j++)
    		board[i][j]=0;
}
int main() {
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2; // Set mode 3 and enable background 2
    
    // Enable interrupts and the VBlank interrupt
    irqInit(); // Initialize interrupt handling
    irqEnable(IRQ_VBLANK); // Enable VBlank interrupt
    REG_IME = 1; // Enable global interrupt
    
    resetBoard();
    drawBoard();
	
    // Initial position of the selected square
    int selectedX = 0; // Column
    int selectedY = 0; // Row

    // Draw the initial outline square
    drawOutlineSquare(selectedX * CELL_SIZE, selectedY * CELL_SIZE);

    while (1) {
        VBlankIntrWait(); // Wait for VBlank

        // Read the input
        u16 keys = ~REG_KEYINPUT; // Invert the key input

        // Save the previous position
        int prevX = selectedX;
        int prevY = selectedY;

        // Update position based on input
        if (keys & KEY_UP) {
            selectedY=selectedY==0?2:selectedY-1;
        }
        if (keys & KEY_DOWN) {
            selectedY=selectedY==2?0:selectedY+1;
        }
        if (keys & KEY_LEFT) {
            selectedX=selectedX==0?2:selectedX-1;
        }
        if (keys & KEY_RIGHT) {
            selectedX=selectedX==2?0:selectedX+1;
        }
		if (keys & KEY_A && board[selectedY][selectedX]==0 && winner=='w') {
			prevX=-1;
			board[selectedY][selectedX]=turn;
			if((board[0][selectedX]==turn&&board[1][selectedX]==turn&&board[2][selectedX]==turn)||(board[selectedY][0]==turn&&board[selectedY][1]==turn&&board[selectedY][2]==turn)||(board[1][1]==turn&&
				((board[0][0]==turn&&board[2][2]==turn)||(board[0][2]==turn&&board[2][0]==turn))))
				winner=turn;
			else if(cur++==9)
				winner='T';
			else
				turn=turn=='X'?'O':'X';
		}
		if (keys & KEY_START && winner != 'w')
		{
			prevX=-1;
			resetBoard();
		}
			
        // Redraw only if the position has changed
        if (selectedX != prevX || selectedY != prevY) {
            // Clear the screen
            clearScreen();
            // Redraw the board and the new outline square
            drawBoard();
            drawOutlineSquare(selectedX * CELL_SIZE, selectedY * CELL_SIZE);
            if(winner!='w')
            	displayWinner(winner);
        }
    }

    return 0;
}

