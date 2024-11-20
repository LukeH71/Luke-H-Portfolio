"""
This is a simple approach to the Mastermind game that supports
6 colors, deleting, and playing again without having to reset
the program.
"""


import os # Clearing the screen for better user experiance
import random # Introducing pseudo-randomness into the game

# A corrisponding list of colors to difficulties
difficulties = ["red", "green", "blue", "yellow", "pink", "black"]
difficulty = 4

# Constant values: can be changed if the game is too easy or hard
difficulty_num_colors = 4
num_guesses = 8

# A hashmap of colors for an easier development experiance
colors = {}
colors["red"] = "\033[91m{}"
colors["green"] = "\033[92m{}"
colors["yellow"] = "\033[93m{}"
colors["blue"] = "\033[94m{}"
colors["pink"] = "\033[95m{}"
colors["black"] = "\033[00m{}"

# This function prints a color from the colors above into
# the terminal with a specified message. An invalid color
# will result in returning 0, while a valid color will
# return 1.
def print_color(color, output): 
    print(colors[color].format(output), end = "")
    print(colors["black"].format(""), end = " ")
    
# Clear the screen
def clear_screen():
    os.system('cls' if os.name == 'nt' else 'clear')

# Make sure a color is real or allowed by the difficulty
def validate_color(color):
    color = color.lower()
    
    for i in range(difficulty):
        if difficulties[i] == color:
            return color
    
    return "@"
    
# This code prints the guesses to the screen when provided with the
# already guessed colors, amount of colors guessed, and the list
# of correct colors that the player is trying to guess.
# This function also returns 1 if the player wins the game and 0
# if they lose.
def print_screen(guessed_colors, guess_count, correct_colors):
    clear_screen()
    
    # Print the options that the player can choose
    print("Avalible colors:\n(You can delete with \"del\")")
    for i in range(difficulty):
        print(difficulties[i], end = " ")
        for j in range(6-len(difficulties[i])):
            print("", end = " ")
        print(" (", end = " ")
        print_color(difficulties[i], "██")
        print("), ")
    
    print("\nYou have " + str(num_guesses-guess_count) + "/" + str(num_guesses) + " guesses left!", end = "")
    
    # Print all of the already guessed colors
    for i in range (guess_count+1):
        
        print("\n\nGuess " + str(i+1) + ":")
        
        if guessed_colors[(i*difficulty_num_colors)] != "@":
            for j in range(difficulty_num_colors):
                print_color(guessed_colors[(i*difficulty_num_colors)+j], "██")
                if guessed_colors[(i*difficulty_num_colors)+j+1]=="@":
                    break
        
        # If the guessing round is complete for the individual guess,
        # print how many the player guessed correctly
        if guess_count > i:
            
            num_same_spot_color = 0
            num_same_color = 0
            already_selected = {}
            for k in range(difficulty_num_colors):
                already_selected[k] = 0
            for j in range(difficulty_num_colors):
                for k in range(difficulty_num_colors):
                    if already_selected[k] == 1:
                        continue
                    
                    if correct_colors[j] == guessed_colors[(i*difficulty_num_colors)+k]:
                        num_same_color += 1
                        already_selected[k] = 1
                        break
                if correct_colors[j] == guessed_colors[(i*difficulty_num_colors)+j]:
                    num_same_color -= 1
                    num_same_spot_color += 1
                    
            print (str(num_same_color) + " correct color and wrong spot, " + str(num_same_spot_color) + " corect color and correct spot" , end = "" )
            
            # If the player got the amount of colors in the right spot as
            # there are colors to guess, then return 1 because they won
            if(num_same_spot_color==difficulty_num_colors):
                return 1
                
    # At the end of the function, clean up, and print 0 because the player
    # did not win yet
    print("\n")
    return 0



# The main game loop where the game logic lies
while (1):
    
    # Print instructions and query the user on how hard of a game
    # they want to play
    print("Welcome to Mastermind!\n\nThe computer will generatte a list of " + str(difficulty_num_colors) + " random colors. Your job is to guess these colors. You will be told how many existing colors you have and the amount of colors in the right position. (Not which ones!) You will have "+str(num_guesses)+" guesses. If you run out, you loose!\nHave fun!\n\n(Its colorful wordle)\n\n\n")
    
    # This variable is the number of possible colors
    difficulty = 0
    try:
        difficulty = int(input("How many colors do you want? (At least 4 and up to 6): "))
    except ValueError:
        difficulty = 4
    
    # Add a lower and upper ceiling to difficulty
    if difficulty > 6:
        difficulty = 6
    if difficulty < 4:
        difficulty = 4
    
    # This input is just so that the reader will re-read instructions if
    # they did not understand them the first time
    input("Are you ready to continue?: ")
    clear_screen()
    
    # Create the colors the user needs to guess
    correct_colors = {}
    for i in range(difficulty_num_colors):
        correct_colors[i] = difficulties[random.randint(0,difficulty-1)]
    
    # Create a list of guessed colors
    guessed_colors = {}
    for i in range(difficulty_num_colors*num_guesses+1):
        guessed_colors[i] = "@"
    
    # Query the user for colors
    i = 0
    print_screen(guessed_colors, (0), correct_colors)
    while (i<difficulty_num_colors*num_guesses+1):
        
        # If the user submitted all the colors for a guessing round,
        # query if they want to submit. If they dont, do the same
        # function as inputting "del"
        if(i%difficulty_num_colors==0 and i>0):
            if input("Do you want to submit guess? (y/n): ").lower() != "y":
                i-=1
                guessed_colors[i] = "@"
        
        # Losing Logic
        if(i==num_guesses*difficulty_num_colors):
            clear_screen()
            
            print("Sorry, you lost. You last guessed:")
            for j in range(difficulty_num_colors):
                print_color(guessed_colors[((num_guesses-1)*difficulty_num_colors)+j], "██")
            print("\n\nWhile you needed to guess:")
            for j in range(difficulty_num_colors):
                print_color(correct_colors[j], "██")
                
            break
        
        # Winning logic
        if print_screen(guessed_colors, (i//difficulty_num_colors), correct_colors):
            print("")
            clear_screen()
            clear_screen()
            print("Congrats! You won! The combination was:")
            for j in range(difficulty_num_colors):
                print_color(correct_colors[j], "██")
            break
        
        # Loop until the user types a valid color
        while(1):
            x = input("Type in color: ")
            
            # If the color is not valid
            if validate_color(x) == "@" and x.lower() != "del":
                print("Oops! " + x + " is not a color!")
                
            # If the user wants to delete the last color
            elif x.lower() == "del":
                if(i>0 and i%difficulty_num_colors!=0):
                    i-=1
                    guessed_colors[i] = "@"
                    print_screen(guessed_colors, (i//difficulty_num_colors), correct_colors)
                else:
                    print("Oops! Unable to delete color!")
            
            # If the color is valid
            else:
                guessed_colors[i]=validate_color(x)
                print_screen(guessed_colors, (i//difficulty_num_colors), correct_colors)
                break
        
        i+=1
        
    # At the end of the game loop, if the user wins or looses, they are
    # prompted with this, then they can choose weather they want to play
    # again or not
    if input("\n\nWould you like to play again? (y/n): ").lower() != "y":
        break
    else:
        clear_screen()
