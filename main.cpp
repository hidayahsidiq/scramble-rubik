#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <conio.h> // For _getch()

using namespace std;

enum Move {
    U, Up, U2, D, Dp, D2, L, Lp, L2, R, Rp, R2, F, Fp, F2, B, Bp, B2,
    Uw, Uwp, Uw2, Dw, Dwp, Dw2, Lw, Lwp, Lw2, Rw, Rwp, Rw2, Fw, Fwp, Fw2, Bw, Bwp, Bw2
};

const char* moveNames[] = {
    "U", "U'", "U2", "D", "D'", "D2", "L", "L'", "L2", "R", "R'", "R2", "F", "F'", "F2", "B", "B'", "B2",
    "Uw", "Uw'", "Uw2", "Dw", "Dw'", "Dw2", "Lw", "Lw'", "Lw2", "Rw", "Rw'", "Rw2", "Fw", "Fw'", "Fw2", "Bw", "Bw'", "Bw2"
};

int getFace(Move move) {
    if (move <= B2) return move / 3; // U, D, L, R, F, B
    return (move - Uw) / 3; // Uw, Dw, Lw, Rw, Fw, Bw
}

Move getRandomMove(Move lastMove, int maxMove) {
    Move newMove;
    do {
        newMove = static_cast<Move>(rand() % maxMove);
    } while (getFace(newMove) == getFace(lastMove) || getFace(newMove) == (getFace(lastMove) ^ 1)); // Check if the move is the same or opposite face
    return newMove;
}

void generateScramble(int numMoves, int maxMove) {
    vector<Move> scramble;
    Move lastMove = static_cast<Move>(rand() % maxMove);
    scramble.push_back(lastMove);

    // Generate the scramble ensuring no consecutive or opposite face moves
    for (int i = 1; i < numMoves; ++i) {
        Move newMove = getRandomMove(lastMove, maxMove);
        scramble.push_back(newMove);
        lastMove = newMove;
    }

    // Output the scramble
    cout << "Scramble: ";
    for (Move move : scramble) {
        cout << moveNames[move] << " ";
    }
    cout << endl;
}

int main() {
    srand(static_cast<unsigned int>(time(0))); // Seed the random number generator

    char choice;
    int cubeSize;
    do {
        cout << "Please select size cube to scramble (2 for 2x2, 3 for 3x3, 4 for 4x4, 5 for 5x5): ";
        cin >> cubeSize;

        switch (cubeSize) {
            case 2:
                generateScramble(20, 18); // 2x2 uses 18 moves (U, D, L, R, F, B and their variations)
                break;
            case 3:
                generateScramble(20, 18); // 3x3 uses 18 moves (U, D, L, R, F, B and their variations)
                break;
            case 4:
                generateScramble(40, 36); // 4x4 uses 36 moves (U, D, L, R, F, B, Uw, Dw, Lw, Rw, Fw, Bw and their variations)
                break;
            case 5:
                generateScramble(60, 36); // 5x5 uses 36 moves (U, D, L, R, F, B, Uw, Dw, Lw, Rw, Fw, Bw and their variations)
                break;
            default:
                cout << "Invalid choice!" << endl;
                continue;
        }

        cout << "Press 'space' to generate a new scramble, 'Enter' to select another scramble, 'q' to quit: ";
        choice = _getch();

        while (choice == ' ') {
            generateScramble((cubeSize == 2 || cubeSize == 3) ? 20 : (cubeSize == 4) ? 40 : 60, (cubeSize == 2 || cubeSize == 3) ? 18 : 36);
            cout << "Press 'space' to generate a new scramble, 'Enter' to select another scramble, 'q' to quit: ";
            choice = _getch();
        }

    } while (choice != 'q');

    return 0;
}
