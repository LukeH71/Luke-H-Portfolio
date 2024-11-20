## Luke-H-Portfolio

This is a portfolio of unique projects I have worked on inside of my coding history. There are 3 projects inside of this repository: a re-creation of Mastermind and Tetris, as well as my game engine that is yet to be named. 


## Mastermind


This is a simple approach to the Mastermind game that supports 6 colors, deleting, and playing again without having to reset the program. Mastermind is a strategy game in which the computer will generate a list of 4 random colors. To add extra difficulty, I allowed the user to select if they wanted 6 colors instead. The player's job is to guess these colors. You will be told how many existing colors you have and the amount of colors in the right position after each turn. However, Not which ones! The player will have 8 guesses. If you run out, you lose!

# Technical explanation:

This project is written in the python programming language, as that is the language I felt most appropriate for the task. This is because I do not need to edit the settings of the terminal, and the only complex features I use are screen clearing and color codes, which may not work on all terminals. This project first introduces the user to the game, then lets them choose their difficulty. This difficulty value is limited by the number of colors supported by ANSI escape codes. After that it asks the user if they are ready to start, which has no programmatically function. However, this makes sure the player will re-read the instructions if they don't understand.

Every time the player asks a color, the screen clears, prints the list of allowed colors, prints previous turns, and then prints the current turn along with the input question. This makes sure that the screen is neat. This program allows the use of 6 colors: red, green, blue, yellow, purple, and black. Additionally, this program accepts the keyword "del" to delete the last inputted color, except if it is the start of a guessing turn. 

Overall I am proud of how this program turned out, especially considering it took me about 2-3 days, each where I worked on it for about 1 hour. 


## Tetris


This program is a condensed version of the popular game Tetris. Tetris is a computer game originally created by Alexey Pajitnov in 1985. The original Tetris is a lot more complicated than this version. This (more condensed) iteration of the game lacks levels, and additional points for special line clears. However, this version does include the following features and more:

- All 7 original pieces (tetrominoes)
- 7 different colors in the linux terminal
- Gravity
- Moving left, right, down, and rotating with keys WITHOUT requiring the pressing of the enter key
- Line clearing
- Score
- Next Piece
- Losing detection
- Immunity/sliding frames
- Random pieces
- Pausing
- Many other niche and random QOL features that make up the beloved game of Tetris

However, this program is severely limited by the website it was coded on which was provided by the school, including problems like timing out without user input, freezing, or screen tearing while printing to the console.

BUGS:
Due to the time limitations, I was unable to fix a few bugs that I have encountered while making this. However, this is the list of bugs that are present:

- If you get the yellow Z piece, and press either left or right while the piece is on y = 0 or y=-1 (top), the player will instantly lose. If you wanted to recreate this, I suggest waiting until the next piece is the yellow piece, and holding down "a".
- Additionally, if you slide off a piece in the same instant that the sliding immunity runs out, the piece will become frozen in air.

To play this game, you try to fill entire rows with pieces. Once you fill a row, that row will be cleared and every other row will drop by 1 layer. Once clearing 1 row, you will gain 100 points. However, you get more points the more rows you clear at once. For example, if the player clears 4 rows at once, the player will gain 800 points. The goal of this game is to get the highest score possible before running out of room in your game board, by rotating and moving your pieces around it. Every time that you have a falling piece, you may use the keys A or D to move the piece either left or right. The player can also use the S key to hasten the descent of the piece. If you use the left or right arrow keys, the tetromino will rotate. Finally, you may also use the P key to temporarily pause your game.


# Technical explanation:

This program is coded inside of the language C++ because of the requirement of speed and efficiency, need to modify terminal settings, and complex feature requirements. Additionally, C++ is a language that I am very used to, so it would be easier to code this in C++. However, just like Mastermind, this program is built for the Linux terminal, incorporating ANSI escape codes and several unique features.

Upon the start of this game, the program gives a brief introduction to Tetris, and tells the user how to play. To continue, the user needs to type any key.

After the user starts the game, pieces will start to fall. These pieces, otherwise called tetrominoes, are managed by the tetrominoes class. This class is initiated inside of two variables. One of these pieces are the active falling blocks, while the other is used for the "Next:" display which displays your next piece. Once the falling piece is unable to fall anymore, the falling piece is set to the "Next:" piece. Then the "Next:" piece is re-initialized. 

The tetrominoes have a function that is called when the main game loop needs to move the pieces. This function will return true if the sliding immunity (a feature that lets pieces slide along the ground) runs out and the piece is unable to move downwards. This function also does not force the piece to move, and includes collision with other pieces as well as borders.

Every frame, a function is called to draw the screen. This includes the game board, next piece, tetris logo (which gets unpacked from a pre-calculated string), paused state, score, amount of score from last line clear, and a prompt for user input. The main game loop then uses multithreading to have one thread designated for user input, and one thread for game logic. This is because once you ask for user input inside of C++ using the methods I have learned, the current thread is paused. This approach also makes sure no input is lost while drawing the screen. Once user input is collected, a shared variable between these threads is set to a singular inputted character (which is insured because the terminal is not canonical). For aesthetics also, echo is turned off inside of the terminal while getting user input.





