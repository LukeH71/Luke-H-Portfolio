/*

This program is a condensed version of the popular game Tetris that includes
the following features and more:

- All 7 original pieces
- 7 different colors in the linux terminal
- Gravity
- Moving left, right, down, and rotating with keys WITHOUT requiring the
pressing of the enter key.
- Line clearing
- Score
- Next Piece
- Losing detection
- Immunity/sliding frames
- Random peices
- Many other neiche and random QOL features that make up the beloved
game of Tetris.

However, this program is severely limited by the website it was coded on,
including problems like timing out without user input, or freezing.

BUGS:
Due to the time limitations, I was unable to fix a few bugs that I have
encountered while making this. However, this is the list of bugs that
are present:
- If you get the yellow Z, and press either left or right while the piece 
is on y = 0 or y=-1 (top), the player will instantly loose
If you wanted to recreate this, I suggest waiting until the next piece
is the yellow piece, and holding down "a"
- Additionally, if you slide off a piece in the same instant that the sliding
immunity runs out, the piece will become frozen in air.


*/

#include <iostream> // Printing and getting lines of text from the terminal
#include <unistd.h> // Defines misc functions: read() function in get_char()
#include <termios.h> // Lower level of I/O stream: terminal settings in get_char()

#include <ctime> // Timing for refreshing the frame

#include <string> // std::to_string
#include <vector> // std::vector<typename>

#include <thread> // Multithreading: one thread for input and one for graphics and logic

// Macros for the colors for ease of development
#define RED "\033[41m"
#define GREEN "\033[42m"
#define YELLOW "\033[43m"
#define BLUE "\033[44m"
#define PURPLE "\033[45m"
#define CYAN "\033[46m"

#define CLEAR "\033[40m"
#define WHITETEXT "\033[37m"

// A function that clears the screen by printing a special code
inline void clearScreen() {
    std::cout << "\x1B[2J\x1B[H";
}

// A simple terimino class that implements the logic for colision
// when moving and rotating
class tetromino {
    public:
    
        // A simple initiator that takes in the type of piece
        tetromino(uint8_t index) : _index(index) {
            
            // Default variables: x = 255 corresponds to the x value
            // needing initiation
            _rotation = 4;
            x = 255;
            
            // Tetrominos have a few immunity frames to move once on
            // the ground
            _slidingTime = 2;
            
            // Each piece has a corresponding color, so this code
            // assigns this by using the default clause to assign
            // the one shared color between the square and the S
            // piece
            switch(_index){
                case 1:
                    _color = 6;
                    break;
                case 2:
                    _color = 5;
                    break;
                case 3:
                    _color = 1;
                    break;
                case 4:
                    _color = 4;
                    break;
                case 5:
                    _color = 2;
                    break;
                default:
                    _color = 3;
            }
        }
        
        // After the first input is regestered, cancel it out on the X
        // value to make it smoother, using default for the pieces that
        // have a 3x3 grid
        void initXY(int8_t addY) {
            switch(_index){
                case 1:
                    x = 3;
                    y = -1-addY;
                    break;
                case 7:
                    x = 4;
                    y = 1-addY;
                    break;
                default:
                    x = 3;
                    y = 0-addY;
            }
        }
        
        // A getter for the Y value
        uint8_t getY(){
            return y;
        }
        
        // A getter for the color value : only used for the next piece
        // indicator
        uint8_t getColor(){
            return _color;
        }
        
        // A simple solution for rotating a tetris piece, while attempting
        // to not clip into other pieces. This approach also includes pushing
        // where if you rotate against a wall, it pushes you until the space
        // is valid
        void rotateDirection(char tetrisBoard[20][10], bool dirPos) {
            
            // Get the orignal (current) piece so that if the opperation
            // fails, the program can reset it.
            std::vector<std::vector<bool>> originalPiece = getPiece();
            
            // The original (current) rotation value so that the
            // program can reset
            uint8_t ogRotation = _rotation;
            
            // Rotate the piece
            if (dirPos) {
                _rotation +=1;
            } else {
                _rotation -=1;
            }
            
            // Get the bitmap of the new piece after the rotation
            std::vector<std::vector<bool>> newPiece = getPiece();
            
            // Clear the old piece off the board
            for(int i = 0; i<newPiece.size(); ++i){
                for(int j = 0; j<newPiece[i].size(); ++j){
                    if (originalPiece[i][j]){
                        tetrisBoard[y+j-1][x+i] = 0;
                    }
                }
            }
            
            // Pushing: if a piece is in a tight location and rotates,
            // it is able to be pushed until the rotation is legal within
            // a range (ex: against a wall)
            
            // Default offsets for the X and Y
            int allowedOffsetX = -255;
            int allowedOffsetY = -255;
            
            // Find any legal x offsets
            for(int k = -1*newPiece.size(); k<abs(newPiece.size()*2); ++k){
                bool allowed = true;
                x += k;
                
                // If its not legal, allowed = false
                for(int i = 0; i<newPiece.size(); ++i){
                    for(int j = 0; j<newPiece[i].size(); ++j){
                        if (newPiece[i][j]&&tetrisBoard[y+j-1][x+i]!=0){
                            allowed = false;
                        }
                        if (x+i>9 || x+i<0) {
                            allowed = false;
                        }
                        if (y+j-1>19 || y+j-1<0) {
                            allowed = false;
                        }
                    }
                }
                
                // Set the offset to the new value if it is closer to 0, and 
                // legal
                if(k==0 && allowed) {
                    allowedOffsetX = 0;
                    break;
                } else if(allowed && abs(k)<abs(allowedOffsetX)) {
                    allowedOffsetX = k;
                }
                x -= k;
            }
            
            // Find any legal y moves
            for(int k = -2; k<3; ++k){
                bool allowed = true;
                y += k;
                
                // If its not legal, allowed = false
                for(int i = 0; i<newPiece.size(); ++i){
                    for(int j = 0; j<newPiece[i].size(); ++j){
                        if (newPiece[i][j]&&tetrisBoard[y+j-1][x+i]!=0){
                            allowed = false;
                        }
                        if (y+j-1>19 || y+j-1<0) {
                            allowed = false;
                        }
                        if (x+i>9 || x+i<0) {
                            allowed = false;
                        }
                    }
                }
                
                // Set the offset to the new value if it is closer to 0, and 
                // legal
                if(k==0 && allowed) {
                    allowedOffsetY = 0;
                    break;
                } else if(allowed && abs(k)<abs(allowedOffsetY)) {
                    allowedOffsetY = k;
                }
                y -= k;
            }
            
            // If both offset variables are -255, than the rotation is
            // illegal no mater what
            if(allowedOffsetX == -255 && allowedOffsetY == -255) {
                for(int i = 0; i<newPiece.size(); ++i){
                    for(int j = 0; j<newPiece[i].size(); ++j){
                        if (originalPiece[i][j]){
                            tetrisBoard[y+j-1][x+i] = _color;
                        }
                    }
                }
                _rotation = ogRotation;
                
            // If the offset has been changed, adjust the piece
            } else {
                if(abs(allowedOffsetX)<abs(allowedOffsetY)){
                    x+=allowedOffsetX;
                } else {
                    y+=allowedOffsetY;
                }
                
                for(int i = 0; i<newPiece.size(); ++i){
                    for(int j = 0; j<newPiece[i].size(); ++j){
                        if (newPiece[i][j]){
                            tetrisBoard[y+j-1][x+i] = _color;
                        }
                    }
                }
            }
            
            
            
        }
        
        // This function returns a value that represents the bitmap value
        // of the current tetromino if it is rotated about the origin
        // 270 degrees
        
        // This function also does not use any algorithm to output the piece
        // due to the imergent complexity that they display when trying
        // to predict them
        std::vector<std::vector<bool>> getPiece() {
            if(_rotation==9){
                _rotation = 1;
            }
            if(_rotation==-1){
                _rotation = 7;
            }
            
            switch(_index) {
                case 1:
                    if(_rotation%2 == 0){
                        std::vector<std::vector<bool>> piece = {{false, false, true, false}, {false, false, true, false}, {false, false, true, false}, {false, false, true, false}};
                        return(piece);
                    } else {
                        if (_rotation%4==1){
                            std::vector<std::vector<bool>> piece = {{false, false, false, false}, {true, true, true, true}, {false, false, false, false}, {false, false, false, false}};
                            return(piece);
                        } else {
                            std::vector<std::vector<bool>> piece = {{false, false, false, false}, {false, false, false, false}, {true, true, true, true}, {false, false, false, false}};
                            return(piece);
                        }
                    }
                    
                    break;
                    
                case 2:
                    switch (_rotation%4){
                        case 0: // 0, 4, and 8
                            {
                                std::vector<std::vector<bool>> piece = {{false, true, false}, {false, true, true}, {false, true, false}};
                                return(piece);
                            }
                        case 1: // 1, and 5
                            {
                                std::vector<std::vector<bool>> piece = {{false, true, false}, {true, true, true}, {false, false, false}};
                                return(piece);
                            }
                        case 2: // 2, and 6
                            {
                                std::vector<std::vector<bool>> piece = {{false, false, true}, {false, true, true}, {false, false, true}};
                                return(piece);
                            }
                        default: // 3, and 7
                            {
                                std::vector<std::vector<bool>> piece = {{false, false, false}, {true, true, true}, {false, true, false}};
                                return(piece);
                            }
                            
                        
                    }
                    break;
                case 3:
                    switch (_rotation%4){
                        case 0: // 0, 4, and 8
                            {
                                std::vector<std::vector<bool>> piece = {{false, true, true}, {false, true, false}, {false, true, false}};
                                return(piece);
                            }
                        case 1: // 1, and 5
                            {
                                std::vector<std::vector<bool>> piece = {{true, false, false}, {true, true, true}, {false, false, false}};
                                return(piece);
                            }
                        case 2: // 2, and 6
                            {
                                std::vector<std::vector<bool>> piece = {{false, false, true}, {false, false, true}, {false, true, true}};
                                return(piece);
                            }
                        default: // 3, and 7
                            {
                                std::vector<std::vector<bool>> piece = {{false, false, false}, {true, true, true}, {false, false, true}};
                                return(piece);
                            }
                            
                        
                    }
                    break;
                case 4:
                    switch (_rotation%4){
                        case 0: // 0, 4, and 8
                            {
                                std::vector<std::vector<bool>> piece = {{false, true, false}, {false, true, false}, {false, true, true}};
                                return(piece);
                            }
                        case 1: // 1, and 5
                            {
                                std::vector<std::vector<bool>> piece = {{false, false, true}, {true, true, true}, {false, false, false}};
                                return(piece);
                            }
                        case 2: // 2, and 6
                            {
                                std::vector<std::vector<bool>> piece = {{false, true, true}, {false, false, true}, {false, false, true}};
                                return(piece);
                            }
                        default: // 3, and 7
                            {
                                std::vector<std::vector<bool>> piece = {{false, false, false}, {true, true, true}, {true, false, false}};
                                return(piece);
                            }
                            
                        
                    }
                    break;
                case 5:
                    switch (_rotation%4){
                        case 0: // 0, 4, and 8
                            {
                                std::vector<std::vector<bool>> piece = {{false, false, true}, {false, true, true}, {false, true, false}};
                                return(piece);
                            }
                        case 1: // 1, and 5
                            {
                                std::vector<std::vector<bool>> piece = {{false, false, false}, {true, true, false}, {false, true, true}};
                                return(piece);
                            }
                        case 2: // 2, and 6
                            {
                                std::vector<std::vector<bool>> piece = {{false, false, true}, {false, true, true}, {false, true, false}};
                                return(piece);
                            }
                        default: // 3, and 7
                            {
                                std::vector<std::vector<bool>> piece = {{true, true, false}, {false, true, true}, {false, false, false}};
                                return(piece);
                            }
                            
                        
                    }
                    break;
                case 6:
                    switch (_rotation%4){
                        case 0: // 0, 4, and 8
                            {
                                std::vector<std::vector<bool>> piece = {{false, true, false}, {false, true, true}, {false, false, true}};
                                return(piece);
                            }
                        case 1: // 1, and 5
                            {
                                std::vector<std::vector<bool>> piece = {{false, false, false}, {false, true, true}, {true, true, false}};
                                return(piece);
                            }
                        case 2: // 2, and 6
                            {
                                std::vector<std::vector<bool>> piece = {{false, true, false}, {false, true, true}, {false, false, true}};
                                return(piece);
                            }
                        default: // 3, and 7
                            {
                                std::vector<std::vector<bool>> piece = {{false, true, true}, {true, true, false}, {false, false, false}};
                                return(piece);
                            }
                            
                        
                    }
                    break;
                    
                default:
                    std::vector<std::vector<bool>> piece = {{true, true}, {true, true}};
                    return(piece);
            }
        }
        
        // Logic for moving a piece up, down, left, or right, while taking into
        // account collision. This function returns 1 once the sliding immunity
        // runs out.
        bool stepLogic(char tetrisBoard[20][10], int8_t addX, int8_t addY) {
            std::vector<std::vector<bool>> piece = getPiece();
            
            // If the first step has happened, clear the board of the 
            // piece before moving it
            if(x!=255){
                for(int i = 0; i<piece.size(); ++i){
                    for(int j = 0; j<piece[i].size(); ++j){
                        if (piece[i][j]){
                            tetrisBoard[y+j-1][x+i] = 0;
                        }
                    }
                }
            // If the first step has not happened, there would be nothing
            // to clear, so set the X value to a valid value instead
            } else {
                initXY(addY);
            }
            
            // Declare the variables for the legality of each direction,
            // excluding up because that never happens within the code
            bool inAir = true;
            bool posDirrection = true;
            bool negDirrection = true;
            
            // Check the legality of left, right, and down, while taking
            // into account out of bounds.
            for(int i = 0; i<piece.size(); ++i){
                for(int j = 0; j<piece[i].size(); ++j){
                    if(piece[i][j]){
                        if(x+i+1>9) 
                            posDirrection = false;
                        if (tetrisBoard[y+j-1][x+i+1]!=0)
                            posDirrection = false;
                        if(x+i-1<0)
                            negDirrection = false;
                        if (tetrisBoard[y+j-1][x+i-1]!=0)
                            negDirrection = false;
                        if (tetrisBoard[y+j][x+i]!=0)
                            inAir = false;
                        if(y+j+addY >= 21) {
                            inAir = false;
                        }
                            
                    }
                }
            }
            
            

            // If the direction is legal, set the new x value
            if (addX>0 && posDirrection) {
                x+=addX;
                
            } else if (addX<0 && negDirrection){
                x+=addX;
                
            }
            
            // If the piece is in the air, move the piece
            if(inAir){
                y += addY;
            }
            
            // Put the new piece on to the board, and extra ground
            // detection to check if the new piece is now on the ground
            for(int i = 0; i<piece.size(); ++i){
                for(int j = 0; j<piece[i].size(); ++j){
                    if (piece[i][j]){
                        tetrisBoard[y+j-1][x+i] = _color;
                        if(tetrisBoard[y+j][x+i]!=0){
                            inAir = false;
                            
                        }
                    }
                }
            }
            
            // Decrease the sliding immunity if the piece is in the air
            if(!inAir) {
                _slidingTime -= 1;
                if(addX==0){
                    _slidingTime -= 2;
                }
                if (_slidingTime <= -2){
                    // Return false if sliding immunity ran out
                    return true;
                }
            } else {
                // Regenerate sliding immunity if the piece slides into air
                if(_slidingTime < 2) {
                    _slidingTime += 1;
                }
            }
            
            // Generally return true to cover all cases 
            return false;
        }

    private:
        uint8_t _index; // Type of piece
        int8_t _rotation; // Rotation state (ranges may vary)
        uint8_t _color; // Color (0-7)
        
        int _slidingTime; // How many sliding immunity frames are left
        int x; // The X value
        int y; // The Y value
        
};


// A function that clears the lines, and returns how many lines are cleared
// if it is non zero, and different than the lineClears variable. This
// return value is passed to the printScreen function for the messeges
// like "Triple!!! +800" This is also the reason why the score variable
// is passed in by reference to be changed and not being set to the 
// return value.
int stepClearLines(char tetrisBoard[20][10], int lineClears, int& score) {
    // The amount of line clears
    int numLineClears = 0;
    
    // This function loops over each line, checking if its clearable
    for (int i = 20; i>0; --i){
        bool clearable = true;
        
        // If the current line is no longer clearable throughout the
        // duration of each pixel in the line, set the clearable
        // boolean to false
        for (int j = 0; j<10; ++j){
            
            if(tetrisBoard[i][j] > 6) {
                tetrisBoard[i][j] = 0;
            }
            
            if(tetrisBoard[i][j] == 0) {
                clearable = false;
                
            }
        }
        
        // If the clearable variable remains true
        if (clearable){
            // Increment the line clears variable
            ++numLineClears;
            
            // Override the old values and push every row down by one
            // as long as the row is above the previously cleared line
            for (int k = i; k>0; --k){
                for(int l = 0; l<10; ++l){
                    tetrisBoard[k][l] = tetrisBoard[k-1][l]; 
                }
            }
            
            // Loop back incase if the pushed down line is also clearable
            i+=1;
        }
        
        
    }
    
    // Adjust the score with its corresponding line clear values
    switch(numLineClears) {
    case 1:
        score += 100;
        break;
    case 2:
        score += 300;
        break;
    case 3:
        score += 500;
        break;
    case 4:
        score += 800;
        break;
    }
    
    // Return the value as outlined above this function.
    return (numLineClears == 0) ? lineClears : numLineClears;
}

// A function that prints the screen
void printScreen(char tetrisBoard[20][10], tetromino nextPiece, int score, int message, bool playing) {
    
    // The tetris logo is packed into this string so that the code is more
    // readable
    std::string tetrisLogo = " ... ... ... ... ... ...\n  .  .    .  . .  .  .  \n  .  ..   .  ...  .  ...\n  .  .    .  ..   .    .\n  .  ...  .  . . ... ...\n\n";
    
    // Use a string and output everything at once to slightly reduce
    // the terminal's refresh rate from causing studdering
    std::string output = "";
    
    // Set the format to use white text to pop against the background
    output += WHITETEXT;
    output += "\n";
    
    // This unpacks and prints the tetris logo in the color yellow
    for (int i = 0; i<tetrisLogo.length(); ++i){
        if( tetrisLogo[i] == ' ') {
            output += CLEAR;
            output += "  ";
        } else if (tetrisLogo[i] == '.'){
            output += YELLOW;
            output += "  ";
        } else if (tetrisLogo[i] == '\n'){
            output += "\n";
        }
    }
    
    // Print the underline and the score
    output += CLEAR;
    output += " ================================================\n\n         Score: "+std::to_string(score);
    
    // Print the score so that the message will always end at the end 
    // of the board, and prints PAUSED if the game is paused
    if(playing){
        for (int i = std::to_string(score).length(); i<17; ++i) {
            output += " ";
        }
    } else { // Print paused
        for (int i = std::to_string(score).length(); i<7; ++i) {
            output += " ";
        }
        
        output += "PAUSED";
        
        for (int i = std::to_string(score).length(); i<7; ++i) {
            output += " ";
        }
    }
    
    // Print the message: a short string telling the user how many points
    // their last line clear rewarded them with
    switch(message){
        case 1:
            output += "Single!!! +100";
            break;
        case 2:
            output += "Double!!! +300";
            break;
        case 3:
            output += "Triple!!! +500";
            break;
        case 4:
            output += "Tetris!!! +800";
            break;
    }
    
    // Print the top layer of the gameboard frame
    output += "\n\n         ||====================||============||\n";

    // Print each pixel of the game
    for(int i = 0; i<20; ++i){
        output += "         ||";
        for (int j = 0; j<10; ++j){
            switch (tetrisBoard[i][j]) {
                case 1: output += RED; break;
                case 2: output += GREEN; break;
                case 3: output += YELLOW; break;
                case 4: output += BLUE; break;
                case 5: output += PURPLE; break;
                case 6: output += CYAN; break;
            }
            output += "  ";
            output += CLEAR;
        }
        
        // Print the next piece section
        if (i == 0) output += "||    Next:   ";
        if (i<5 && i>0) {
            output += "||  ";
            
            // Get the bitmap of the next piece to be printed
            std::vector<std::vector<bool>> nextPieceBitmap = nextPiece.getPiece();
            
            // If the piece is the bitmap is 2*2, print spaces to center it
            if(nextPieceBitmap.size() == 2) {
                output+=" ";
                i-=1;
            }
            
            // If the piece is not a 4*4, print spaces to format it
            if(nextPieceBitmap.size() != 4) {
                output+=" ";
            }
            
            // Print the colored piece of the next piece
            if(nextPieceBitmap.size()>=i-1){
                for(int j = 0; j<nextPieceBitmap.size(); ++j){
                    if(nextPieceBitmap[j][i-1]&&i-1< nextPieceBitmap.size()){
                        switch (nextPiece.getColor()) {
                            case 1: output += RED; break;
                            case 2: output += GREEN; break;
                            case 3: output += YELLOW; break;
                            case 4: output += BLUE; break;
                            case 5: output += PURPLE; break;
                            case 6: output += CYAN; break;
                        }
                        
                        
                    }
                    
                    output += "  ";
                    output += CLEAR;
                }
            } 
            
            // Mirror the centering spaces above
            output += "  ";
            if(nextPieceBitmap.size() != 4) {
                output+=" ";
            }
            if(nextPieceBitmap.size() == 2) {
                output+=" ";
                if(i==0) {
                    output += "    ";
                }
                i+=1;
            }
            
        }
        
        // If its the end of the next piece section, add the bottom bar
        if (i == 5) output += "||============";
        
        
        output += "||\n";

    }
    
    // Query the user for input
    output += "         ||====================||\n\n ";
    if(playing){
        output += "Enter key (ASD to move the piece or ← / → (arrow keys) to rotate, or puase with P):\n";
    } else {
        output += "Please unpause (P), or provide other user input within one (1) minute to prevent timing out:\n";
    }
    
    // Set the background to a grayish color
    std::cout << CLEAR;
    clearScreen();
    
    // Print all this at once
    std::cout << output;
    
}

// Get a char from the stream without needing '\n'
void getChar(char &buffer) {
    
    // Define original terminal paramiters
    struct termios old = {0};
    
    // Store the terminal's perameters in the 'old' variable
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    
    // Set flags: disable canonical mode (not requiring '\n' like in st::cin)
    // and disable echo (showing what you typed in the console)
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    
    // Set controll characters
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;

    // Set the terminal attributes to the new attributes after the settings
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    
    // Read from the terminal
    if (read(0, &buffer, 1) < 0)
        perror ("read()");
    
    // Reset flags in the 'old' variable
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    
    // Set them to the terminal 
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
}

// The main function: brings all of these functions together
int main()
{
    // Set the background to gray and greet the user, then query if they
    // want to start
    std::cout << CLEAR;
    clearScreen();
    std::cout << WHITETEXT;
    
    std::cout << "\n  Welcome to Tetris!!!\n\n  Tetris is a computer game originally created by Alexey Pajitnov in 1985.\n  The original Tetris is a lot more complicated than this version.\n  This (more condensed) itteration of the game lacks levels, and additional points for special line clears.\n\n  Its still fun though! :D\n\n  To play, use A/D to move the piece left or right.\n  You may use S to hasten the falling speed of the tetromino (Tetris piece).\n  You can also use the left and right arrow keys to rotate the tetromino.\n\n\n  Type any key to play!: \n";
    
    // Get this character inside of a scope to not have a one time use
    // char value. This function freezes the thread it is called in until
    // the user types any character into the terminal
    {
        char charr = '\0';
        getChar(std::ref(charr));
    }
    
    // Set the seed to the time so that the pieces are random
    srand(time(NULL));
    
    // The value of the score: defined here so it can be displayed after
    // loosing
    int score = 0;
    
    // Main game loop: only broken when the user looses than says they
    // dont want to play again
    while(true){
        
        // A 2D 20*10 array of values representing each pixel on the screen
        char tetrisBoard[20][10];
        
        // Set default values for the socore and message
        score = 0;
        int message = 0;
        
        // Give default values ot the piece and the following nextPiece
        // the next piece is already declared because it is displayed in
        // the next piece box
        tetromino piece = tetromino(rand() % 7 + 1);
        tetromino nextPiece = tetromino(rand() % 7 + 1);
        
        // Set each value in the screen to 0
        for (int i = 0; i<20; ++i){
            for (int j = 0; j<10; ++j){
                tetrisBoard[i][j] = 0;
            }
        }
        
        // Initiate the thread where input is being collected, as well
        // as set the variable that this is collected in to a default
        // value that is impossible to type with a non-custom keyboard
        char userInput = '\0';
        std::thread inputThread (getChar, std::ref(userInput));
        
        // This while loop runs for the duration of the game and gets
        // looped after the piece falls to gravity or is suspected to
        // maybe winning
        while (true){
            
            // Make the piece fall (the higher Y is, the lower it is visually)
            bool onGround = piece.stepLogic(tetrisBoard, 0, 1);
            printScreen(tetrisBoard, nextPiece, score, message, true);
            
            // If the piece is on the ground, check if one of the middle
            // top pixels is full. This means that the player has lost.
            // If they loose, break and query them if they want to play
            // again. Otherwise, generate a new piece and attempt to 
            // clear lines
            if (onGround){
                if(tetrisBoard[0][4] == 0&&tetrisBoard[0][5] == 0) {
                    message = stepClearLines(tetrisBoard, message, std::ref(score));
                    piece = nextPiece;
                    nextPiece = tetromino(rand() % 7 + 1);
                } else {
                    break;
                }
            }
        
            // This variable is used for how fast the pieces fall.
            // fractional values do not seem to work, however, if future
            // development requires changing this speed (levels), than
            // the chronos library may assist in getting more presise
            // counters.
            std::time_t refresh = std::time(nullptr)+1;
            
            // A boolean value of if it is paused or not
            bool playing = true;
            
            // Once the one second of stalling in the air, exit the loop and
            // fall one unit.
            while (refresh>std::time(nullptr)||!playing){
                // If the user provided input
                if(userInput!='\0'){
                    
                    // Check each case of user input
                    if(playing){
                        switch (userInput) {
                            case 's': // the S key: move 1 unit down and reset the timer
                                onGround |= piece.stepLogic(tetrisBoard, 0, 1);
                                refresh = std::time(nullptr)+1;
                                break;
                            case 'a': // the A key: move 1 unit to the left
                                onGround |= piece.stepLogic(tetrisBoard, -1, 0);
                                break;
                            case 'd': // the D key: move one unit to the right
                                onGround |= piece.stepLogic(tetrisBoard, 1, 0);
                                break;
                            case 'C': // the last byte in the 4 byte sequence in
                                // the code for the right arrow key: rotate right
                                piece.rotateDirection(tetrisBoard, true);
                                onGround |= piece.stepLogic(tetrisBoard, 0, 0);
                                break;
                            case 'D': // the last byte in the 4 byte sequence in
                                // the code for the left arrow key: rotate left
                                piece.rotateDirection(tetrisBoard, false);
                                onGround |= piece.stepLogic(tetrisBoard, 0, 0);
                                break;
                        }
                    }
                    if (userInput == 'p'){ // toggle paused state
                        playing ^= true;
                    }
                    
                    
                    // After user input is checked and effected, reset the input
                    // variable, join the joinable thread, and initiate a new thread
                    // to replace the past one
                    inputThread.join();
                    userInput = '\0';
                    inputThread = std::thread(getChar, std::ref(userInput));
                    
                    // If the piece is now on the ground, commence the same logic as
                    // the same code above
                    if (onGround){
                        if(tetrisBoard[0][4] == 0) {
                            message = stepClearLines(tetrisBoard, message, std::ref(score));
                            piece = nextPiece;
                            nextPiece = tetromino(rand() % 7 + 1);
                        } else {
                            break;
                        }
                    }
                    
                    // Print the screen
                    printScreen(tetrisBoard, nextPiece, score, message, playing);
                }
            }
        }
        
        // This code runs if the user looses:
        
        // Clear the screen
        clearScreen();

        // Inform them they lost and join the pottenitally still open thread
        std::cout << "\n  Score: " << score<<"\n\n  You just lost! Would you like to play again? (y/n):\n";
        if(inputThread.joinable()) {inputThread.join();}
        std::string output = "#";
        
        // Make sure the input is either "y" or "n", and repeats if its not
        while (output != "n" && output != "y"){
            
            if(output != "#"){
                std::cout << "\nInvalid input. Only input either \"y\" or \"n\", sorry!:\n";
            }
            std::cin >> output;
        }
        
        // If the input is "n," break and let the program terminate. Otherwise
        // let the program loop
        if(output=="n"){
            break;
        }
    }

    return 0;
}
