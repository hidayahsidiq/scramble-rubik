#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// Define the faces and moves of the 2x2 Rubik's cube
enum Move { F, F_PRIME, F2, B, B_PRIME, B2, L, L_PRIME, L2, R, R_PRIME, R2, U, U_PRIME, U2, D, D_PRIME, D2 };
const char* moveNames[] = { "F", "F'", "F2", "B", "B'", "B2", "L", "L'", "L2", "R", "R'", "R2", "U", "U'", "U2", "D", "D'", "D2" };

// Function to get the face of a move
int getFace(Move move) {
    return move / 3;
}

// Function to generate a random move
Move getRandomMove(Move lastMove) {
    Move newMove;
    do {
        newMove = static_cast<Move>(rand() % 18);
    } while (getFace(newMove) == getFace(lastMove) || getFace(newMove) == (getFace(lastMove) ^ 1)); // Check if the move is the same or opposite face
    return newMove;
}

int main() {
    srand(time(0)); // Seed the random number generator
    vector<Move> scramble;
    int numMoves = 20; // Number of moves in the scramble

    Move lastMove = static_cast<Move>(rand() % 18);
    scramble.push_back(lastMove);

    // Generate the scramble ensuring no consecutive or opposite face moves
    for (int i = 1; i < numMoves; ++i) {
        Move newMove = getRandomMove(lastMove);
        scramble.push_back(newMove);
        lastMove = newMove;
    }

    // Output the scramble
    cout << "Scramble: ";
    for (Move move : scramble) {
        cout << moveNames[move] << " ";
    }
    cout << endl;

    return 0;
}
