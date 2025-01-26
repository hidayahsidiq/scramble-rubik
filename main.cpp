char choice;
    do {
        int cubeSize;
        cout << "Choose cube size for scramble (2 for 2x2, 3 for 3x3, 4 for 4x4): ";
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
            default:
                cout << "Invalid choice!" << endl;
                break;
        }

        cout << "Press 'q' to quit or any other key to generate another scramble: ";
        cin >> choice;
    } while (choice != 'q');
