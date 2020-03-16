// By Nick van der Merwe - s5151332
// nick.vandermerwe@griffith.edu.au
// Made for 2802ICT's first assignment

#include <iostream>
#include <vector>
#include <chrono>
#include <queue>
#include <stack>
#include <array>
#include <cmath>
#include <algorithm>
#include <set>

/*
 * Comments on code style:
 * - I try to make my variable names informative enough to not explain
 * everything with comments, but it seems the rubric asks for comments to
 * make the code explanable.
 * - I seperated UninformedBoard and LocalBoard out of choice to simplify
 * each class. If they were merged, the order of the functions could be
 * confusing
 */

class UninformedBoard {
public:
    UninformedBoard() = default;

    explicit UninformedBoard(short N) : N{N}, nextRow{0} {}

    UninformedBoard(UninformedBoard const &that) = default;

    std::vector<UninformedBoard> possibleActions() {
        findLegalRows();
        std::vector<UninformedBoard> nextStates;
        for (short i = 0; i < N; i++) {
            if (choices[i] == false) {
                continue;
            }
            UninformedBoard nextState = *this;
            nextState.place(i);
            nextStates.push_back(nextState);
        }
        return nextStates;
    }

    std::vector<UninformedBoard> halfPossibleActionsOnFirstLoop() {
        // only get the first half of the possible actions
        findHalfLegalRows();
        std::vector<UninformedBoard> nextStates;
        for (short i = 0; i < N; i++) {
            if (choices[i] == false) {
                continue;
            }
            UninformedBoard nextState = *this;
            nextState.place(i);
            nextStates.push_back(nextState);
        }
        return nextStates;
    }

    void place(short row) {
        queens.push_back(row);
        nextRow++;
    }

    bool allQueensArePlaced() {
        return queens.size() == N;
    }

    void printBoard() {
        // Note that this actually prints a rotated version of the board,
        // however, this is only for visual checking and the rotated version
        // is still valid. This just proves to be much more readable to print
        std::cout << "=========== N=" << N << " ===========" << std::endl;
        for (int y = 0; y < N; y++) {
            for (int x = 0; x < N; x++) {
                if (x == queens[y]) {
                    std::cout << "1 ";
                } else {
                    std::cout << "0 ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << "==========================" << std::endl;
    }

    std::vector<std::vector<short>> getBoard() {
        /// generate the 2D representation of the board
        // a short vector has less overhead than a boolean vector
        // (24 bytes vs 40 bytes) so I picked a short instead
        std::vector<std::vector<short>> board;
        for (int i = 0; i < N; i++) {
            board.emplace_back(N, 0);
            board[i][queens[i]] = 1;
        }
        return board;
    }

    std::vector<short> getQueens() {
        return queens;
    }

    friend bool operator==(UninformedBoard &L, UninformedBoard const &R);

private:
    std::vector<short> queens;
    std::vector<bool> choices;
    short N{};
    short nextRow{};

    void findHalfLegalRows() {
        choices = std::vector<bool>(N, true);
        if (nextRow == 0) {
            // automatically disable the second half
            for (int i = N / 2; i < N; i++) {
                choices[i] = false;
            }
        }
        for (short i = 0; i < nextRow; i++) {
            choices[queens[i]] = false;
            short temp = queens[i] + nextRow - i;
            if (temp < N) {
                choices[temp] = false;
            }
            temp = queens[i] - nextRow + i;
            if (temp >= 0) {
                choices[temp] = false;
            }
        }
    }

    void findLegalRows() {
        choices = std::vector<bool>(N, true);
        for (short i = 0; i < nextRow; i++) {
            choices[queens[i]] = false;
            short temp = queens[i] + nextRow - i;
            if (temp < N) {
                choices[temp] = false;
            }
            temp = queens[i] - nextRow + i;
            if (temp >= 0) {
                choices[temp] = false;
            }
        }
    }
};

bool operator==(UninformedBoard &L, UninformedBoard const &R) {
    // if their queens and N's are the same the rest will be too
    return L.queens == R.queens && L.N == R.N;
}

std::vector<std::vector<short>> flip(
        std::vector<std::vector<short>> board) {
    //reverse the elements
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board.size() / 2; j++) {
            std::swap(board[i][j], board[i][board.size() - j - 1]);
        }
    }
    return board;
}

std::vector<std::vector<short>> rotate(std::vector<std::vector<short>> board) {
    //transpose it
    for (int i = 0; i < board.size(); i++) {
        for (int j = i; j < board.size(); j++) {
            std::swap(board[i][j], board[j][i]);
        }
    }
    //reverse the elements
    board = flip(board);
    return board;
}

std::vector<short> compressBoard(std::vector<std::vector<short>> board) {
    // reverses the getBoard option
    std::vector<short> compressedBoard;
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board.size(); j++) {
            if (board[i][j] == 1) {
                compressedBoard.push_back(j);
            }
        }
    }
    return compressedBoard;
}

std::set<std::vector<short>> tripleRotation(
        std::vector<std::vector<short>> board) {
    std::set<std::vector<short>> rotations;
    for (int i = 0; i < 3; i++) {
        rotations.insert(compressBoard(rotate(board)));
    }
    return rotations;
}

std::set<std::vector<short>> permutations(UninformedBoard base) {
    // the maximum number of boards from a single board is 8.
    // base board + 3 rotations
    // flipped board + 3 rotations
    std::set<std::vector<short>> perms;
    perms.insert(base.getQueens());
    std::vector<std::vector<short>> initialBoard = base.getBoard();
    std::set<std::vector<short>> temp; // to fill with rotations and copy in

    temp = tripleRotation(initialBoard);
    perms.insert(temp.begin(), temp.end());

    initialBoard = flip(initialBoard);
    temp = tripleRotation(initialBoard);
    perms.insert(compressBoard(initialBoard));
    perms.insert(temp.begin(), temp.end());

    return perms;
}

int badBFS(int N) {
    // This is the original BFS in the slides with checking for previous
    // positons/
    UninformedBoard INITIAL = UninformedBoard(N);
    std::queue<UninformedBoard> q;
    std::vector<UninformedBoard> found;
    int possibleWays = 0;
    q.push(INITIAL);

    while (!q.empty()) {
        UninformedBoard current = q.front();
        q.pop();
        found.push_back(current);
        std::vector<UninformedBoard> nextBoards = current.possibleActions();

        for (auto &board : nextBoards) {
            if (board.allQueensArePlaced()) {
                possibleWays++;
                if (N <= 6) {
                    board.printBoard();
                }
            } else {
                if (std::find(found.begin(), found.end(), board)
                    == found.end()) {
                    q.push(board);
                }
            }
        }
    }
    return possibleWays;
}

int BFS(int N) {
    UninformedBoard INITIAL = UninformedBoard(N);
    std::queue<UninformedBoard> q;
    int possibleWays = 0;
    q.push(INITIAL);

    while (!q.empty()) {
        UninformedBoard current = q.front();
        q.pop();
        std::vector<UninformedBoard> nextBoards = current.possibleActions();

        for (auto &board : nextBoards) {
            if (board.allQueensArePlaced()) {
                possibleWays++;
                if (N <= 6) {
                    board.printBoard();
                }
            } else {
                q.push(board);
            }
        }
    }
    return possibleWays;
}

int prunedBFS(int N) {
    UninformedBoard INITIAL = UninformedBoard(N);
    std::queue<UninformedBoard> q;
    std::set<std::vector<short>> possibleWays;
    q.push(INITIAL);

    while (!q.empty()) {
        UninformedBoard current = q.front();
        q.pop();
        std::vector<UninformedBoard> nextBoards =
                current.halfPossibleActionsOnFirstLoop();

        for (auto &board : nextBoards) {
            if (board.allQueensArePlaced()) {
                std::set<std::vector<short>> newWays = permutations(board);
                possibleWays.insert(newWays.begin(), newWays.end());
                if (N <= 6) {
                    board.printBoard();
                }
            } else {
                q.push(board);
            }
        }
    }
    return possibleWays.size();
}

int DFS(int N) {
    UninformedBoard INITIAL = UninformedBoard(N);
    std::stack<UninformedBoard> s;
    int possibleWays = 0;
    s.push(INITIAL);

    while (!s.empty()) {
        UninformedBoard current = s.top();
        s.pop();
        std::vector<UninformedBoard> nextBoards = current.possibleActions();

        for (auto &board : nextBoards) {
            if (board.allQueensArePlaced()) {
                possibleWays++;
                if (N <= 6) {
                    board.printBoard();
                }
            } else {
                s.push(board);
            }
        }
    }
    return possibleWays;
}

int prunedDFS(int N) {
    UninformedBoard INITIAL = UninformedBoard(N);
    std::stack<UninformedBoard> stack;
    std::set<std::vector<short>> possibleWays;
    stack.push(INITIAL);

    while (!stack.empty()) {
        UninformedBoard current = stack.top();
        stack.pop();
        std::vector<UninformedBoard> nextBoards =
                current.halfPossibleActionsOnFirstLoop();

        for (auto &board : nextBoards) {
            if (board.allQueensArePlaced()) {
                std::set<std::vector<short>> newWays = permutations(board);
                possibleWays.insert(newWays.begin(), newWays.end());
                if (N <= 6) {
                    board.printBoard();
                }
            } else {
                stack.push(board);
            }
        }
    }
    return possibleWays.size();
}

int benchmarkUninformed() {
    // This is purely here for debugging specific cases. Ignore me!
    int totalTime = 0;
    int ways;
    for (int i = 1; i <= 20; i++) {
        auto startClock = std::chrono::high_resolution_clock::now();
        // ==================MEASURED TIME=========================
        //ways = badBFS(i);
//        ways = BFS(i);
        ways = prunedBFS(i);
//        ways = DFS(i);
//        ways = prunedDFS(i);
        // ========================================================
        auto endClock = std::chrono::high_resolution_clock::now();
        double microseconds =
                std::chrono::duration_cast<std::chrono::microseconds>
                        (endClock - startClock).count();

        totalTime += microseconds;
        std::cout << "Took " << double(microseconds) / 1000000
                  << " seconds to find that N = "
                  << i << " has " << ways << " ways of being solved"
                  << std::endl;
    }
    std::cout << "The total time was " << totalTime << std::endl;
    return 0;
}

class LocalBoard {
public:
    LocalBoard() = default; // just here so that nullptr is valid

    explicit LocalBoard(const short N) : N{N}, value{-1} {
        randomFll();
    }

    LocalBoard(LocalBoard const &that) = default;

    LocalBoard bestNextBoard() {
        std::vector<std::array<short, 2>> bestMoveYX = findBestMoves();
        auto nextBoard = *this; // copy ctor does deep copy
        int randMove = rand() % bestMoveYX.size();
        nextBoard.move(bestMoveYX[randMove][0],
                       bestMoveYX[randMove][1]);
        return nextBoard;
    }

    LocalBoard randomNeighbour() {
        // Fully random shift
        auto nextBoard = *this;
        nextBoard.move(rand() % N, rand() % N);
        return nextBoard;
    }

    int getValue() {
        if (value == -1) { // if the current value isn't valid
            value = evaluatePosition(queens);
        }
        return value;
    }

    void printBoard() {
        // Note that this actually prints a rotated version of the board,
        // however, this is only for visual checking and the rotated version
        // is still valid. This just proves to be much more readable to print
        std::cout << "=========== N=" << N << " ===========" << std::endl;
        for (int y = 0; y < N; y++) {
            for (int x = 0; x < N; x++) {
                if (x == queens[y]) {
                    std::cout << "1 ";
                } else {
                    std::cout << "0 ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << "==========================" << std::endl;
    }

private:
    std::vector<short> queens;
    short N;
    short value;

    void randomFll() {
        // randomly allocate queens to positions
        srand(42); // TODO: change to time(NULL)
        for (int i = 0; i < N; i++) {
            queens.push_back(rand() % N);
        }
    }

    bool queenExists(int y, int x) {
        // checks if a queen is already at the given position
        for (int i = 0; i < N; i++) {
            if (y == i && x == queens[i]) {
                return true;
            }
        }
        return false;
    }

    short evaluatePosition(std::vector<short> const &queensToEval) {
        // Sets value for the current position
        short boardValue = 0;
        for (int i = 0; i < N; i++) {
            for (int j = i + 1; j < N; j++) {
                // This operation is actually quite important and I
                // benchmarked them for how long it takes to solve N = 50, 150
                // times. It seems the compiler is doing something funky to
                // the if statement to optimise it further. Option 3 ended up
                // being the fastest by a good chunk:

// ====================OPTION 1 == 16.035 seconds=========================
//                if (queens[i] == queens[j] ||
//                    abs(i - j) == abs(queens[i] - queens[j])) {
//                    boardValue++;
//                }

// ====================OPTION 2 == 17.2023 seconds========================
//                if (queens[i] == queens[j]) {
//                    boardValue++;
//                }
//                if (abs(i - j) == abs(queens[i] - queens[j])) {
//                    boardValue++;
//                }

// ============OPTION 3 == 8.86594 seconds - *winner*========================
                if (queens[i] == queens[j]) {
                    boardValue++;
                }
                if (queens[i] == queens[j] - (j - i) ||
                    queens[i] == queens[j] + (j - i)) {
                    boardValue++;
                }

// ===================OPTION 4 == 10.4219 seconds============================
//                if (queens[i] == queens[j] ||
//                    queens[i] == queens[j] - j - i ||
//                    queens[i] == queens[j] - j + i) {
//                    boardValue++;
//                }

            }
        }
        return boardValue;
    }

    short evaluatePosition(int y, int x) {
        // value after moving to a specific piece
        short changedPiece = queens[y];
        queens[y] = x;
        short futureValue = evaluatePosition(queens);
        queens[y] = changedPiece;
        return futureValue;
    }

    std::vector<std::array<short, 2>> findBestMoves() {
        // Generate a matrix of the price of all the moves
        std::vector<std::vector<short>> evalBoard;
        // since this board can be big its better to fill it by reference
        for (int i = 0; i < N; i++) {
            evalBoard.emplace_back(N, std::numeric_limits<short>::max());
        }
        // fill it
        short minPrice = std::numeric_limits<short>::max();
        for (int y = 0; y < N; y++) {
            for (int x = 0; x < N; x++) {
                if (queenExists(y, x)) {
                    continue;
                }
                evalBoard[y][x] = evaluatePosition(y, x);
                minPrice = std::min(evalBoard[y][x], minPrice);
            }
        }
        // compile a list of the lowest price positions
        std::vector<std::array<short, 2>> lowestPrices;
        for (short y = 0; y < N; y++) {
            for (short x = 0; x < N; x++) {
                if (evalBoard[y][x] == minPrice) {
                    std::array<short, 2> a{y, x};
                    lowestPrices.push_back(a);
                }
            }
        }
        return lowestPrices;
    }

    void move(int y, int x) {
        queens[y] = x;
        value = -1; // invalidate value so it must be recalculated
    }
};

int hillClimbingSearch(int N) {
    LocalBoard current = LocalBoard(N);
    while (true) {
        LocalBoard neighbour = current.bestNextBoard();
        if (neighbour.getValue() == 0) {
            if (N <= 6) {
                neighbour.printBoard();
            }
            return -1;
        }
        current = neighbour;
    }
}

int hillClimbingSearchRR(int N) {
    short stuck = 0; // increased every time the cost is the same
    short restartAt = 10;
    short previousCost = std::numeric_limits<short>::max();
    LocalBoard current = LocalBoard(N);
    while (true) {
        LocalBoard neighbour = current.bestNextBoard();
        if (neighbour.getValue() == previousCost) {
            stuck++;
            if (stuck == restartAt) {
                // clear the board and start again
                current = LocalBoard(N);
                neighbour = current.randomNeighbour();
                restartAt++; // increase this so it doesn't keep restarting
                // forever
            }
        } else {
            stuck = 0;
            previousCost = neighbour.getValue();
        }


        if (neighbour.getValue() == 0) {
            if (N <= 6) {
                neighbour.printBoard();
            }
            return -1; // solution was found
        }
        current = neighbour;
    }
}

int simulatedAnnealing(int N) {
    double temp = 2800;
    double cooling = 0.98;
    double delta = 0; // used for calculating temperature
    double prob = 0;
    LocalBoard current = LocalBoard(N);
    while (true) {
        LocalBoard neighbour = current.randomNeighbour();

        if (neighbour.getValue() == 0) {
            if (N <= 6) {
                neighbour.printBoard();
            }
            return -1; // the solution
        }

        delta = (double) (neighbour.getValue() - current.getValue())
                / current.getValue();
        if (neighbour.getValue() <= current.getValue()) {
            current = neighbour;
        } else {
            // if its worse work out a chance to keep it
            double roll = ((double) rand() / RAND_MAX);
            if (temp != 0) {
                // avoid division by 0
                prob = std::pow(std::exp(1.0), -delta / temp);
            } else {
                prob = 0;
            }

            if (roll < prob) {
                current = neighbour; // keep the change
            }
        }
        temp *= cooling;
    }
}

int simulatedAnnealingRR(int N) {
    // Random restarts version
    short stuck = 0; // increased every time the cost is the same
    short restartAt = 10;
    short previousCost = std::numeric_limits<short>::max();
    double temp = 2800;
    double cooling = 0.98;
    double delta = 0; // used for calculating temperature
    double prob = 0;
    LocalBoard current = LocalBoard(N);
    while (true) {
        LocalBoard neighbour = current.randomNeighbour();
        if (neighbour.getValue() == previousCost) {
            stuck++;
            if (stuck == restartAt) {
                // clear the board and start again
                current = LocalBoard(N);
                neighbour = current.randomNeighbour();
                restartAt++; // increase this so it doesn't keep restarting
                // forever
            }
        } else {
            stuck = 0;
            previousCost = neighbour.getValue();
        }

        if (neighbour.getValue() == 0) {
            if (N <= 6) {
                neighbour.printBoard();
            }
            return -1; // the solution got found
        }

        delta = (double) (neighbour.getValue() - current.getValue())
                / current.getValue();
        if (neighbour.getValue() <= current.getValue()) {
            current = neighbour;
        } else {
            // if its worse work out a chance to keep it
            double roll = ((double) rand() / RAND_MAX);
            if (temp != 0) {
                // avoid division by 0
                prob = std::pow(std::exp(1.0), -delta / temp);
            } else {
                prob = 0;
            }

            if (roll < prob) {
                current = neighbour; // keep the change
            }
        }
        temp *= cooling;
    }
}

int benchmarkLocal() {
    // This is purely here for debugging specific cases. Ignore me!
    int totalTime = 0;
    int ways;
    for (int i = 1; i <= 20; i++) {
        auto startClock = std::chrono::high_resolution_clock::now();
        // ==================MEASURED TIME=========================
//        hillClimbingSearch(i);
//        hillClimbingSearchRR(i);
//        simulatedAnnealing(i);
//        simulatedAnnealingRR(i);
        // ========================================================
        auto endClock = std::chrono::high_resolution_clock::now();
        double microseconds =
                std::chrono::duration_cast<std::chrono::microseconds>
                        (endClock - startClock).count();

        totalTime += microseconds;
        std::cout << "Took " << double(microseconds) / 1000000
                  << " seconds to solve when N = "
                  << i << std::endl;
    }
    std::cout << "The total time was " << totalTime << std::endl;
    return 0;
}

int benchmarkFunction(int (*fun)(int), int lowerBound,
                      int upperBound, int increment) {

    double totalTime = 0;
    int ways = -1;
    double minTime = std::numeric_limits<double>::max();
    for (int i = lowerBound; i < upperBound; i += increment) {
        if (i == 2 || i == 3) continue;
        auto startClock = std::chrono::high_resolution_clock::now();
        // ==================MEASURED TIME=========================
        ways = fun(i);
        // ========================================================
        auto endClock = std::chrono::high_resolution_clock::now();
        double microseconds =
                std::chrono::duration_cast<std::chrono::microseconds>
                        (endClock - startClock).count();
        minTime = std::min(minTime, microseconds);
        totalTime += microseconds;
        // two different prints for uninformed and global search
        if (ways == -1) {
            std::cout << "Took " << double(microseconds) / 1000000
                      << " seconds to solve N = " << i
                      << std::endl;
        } else {
            std::cout << "Took " << double(microseconds) / 1000000
                      << " seconds to find that N = "
                      << i << " has " << ways << " ways of being solved"
                      << std::endl;
        }
    }
    std::cout << "The fastest time was " << minTime / 1000000
              << std::endl;
    std::cout << "The total time was " << totalTime / 1000000
              << " seconds" <<
              std::endl;
    return 0;
}

void openMessage() {
    std::cout
            << "================MONSTROUS nQUEEN SOLVER===================="
            << "             ++Nick van der Merwe - s5151332 ++"
            << "Hello and welcome to the MONSTROUS nQueen Solver "
            << std::endl
            << "I hope this gives you the fastest benchmarks out of anyone. "
            << std::endl
            << "No, but really. I'd be surprised if anyone made something "
            << "faster." << std::endl
            << "Also I need a job because I have too much time for "
               "assignments "
            << std::endl << std::endl
            << "To pick an algorithm enter its name (e.g. 'SARR' without quotes)"
            << std::endl;
}

void printAlgorithms() {
    std::cout
            << "=====================Uninformed Searches==================="
            << std::endl
            << "'badBFS' => Breadth first search with checking if it already"
               " exists"
            << std::endl
            << "'BFS' => Breadth first search WITHOUT checking back"
            << std::endl
            << "'pBFS' => prune BFS - BFS with added pruning. Utilises "
               "rotations " << std::endl << "to count more boards"
            << std::endl
            << "'DFS' => DFS without checking back. This bad boy can just "
            << std::endl
            << "keep running to N = 100 (given several years)"
            << "'pDFS' => prune DFS - DFS with added pruning. Utilises "
               "rotations " << std::endl << "to count more boards"
            << std::endl
            << std::endl << std::endl
            << "===================Local Searches========================"
            << std::endl
            << "'HC' => Hill-climbing - exactly what's on the tin"
            << std::endl
            << "'SA' => Simulated Annealing - **DANGER: High chance of "
            << std::endl
            << "getting stuck at N <= 30. Only use 30 x x to not risk it"
            << std::endl
            << "'SARR' => Simulated Annealing Random Restart - use with N=200+"
            << std::endl
            << "'HCRR' => Hill-climbing random restart - it's alright, but eh"
            << std::endl << ">";

}

int handleUserInput() {

    std::string request;
    std::cin >> request;
    // make everything lowercase
    std::transform(request.begin(), request.end(),
                   request.begin(), ::tolower);

    if (request == "badbfs") {
        return 0;
    } else if (request == "bfs") {
        return 1;
    } else if (request == "pbfs") {
        return 2;
    } else if (request == "dfs") {
        return 3;
    } else if (request == "pdfs") {
        return 4;
    } else if (request == "hc") {
        return 5;
    } else if (request == "sa") {
        return 6;
    } else if (request == "sarr") {
        return 7;
    } else if (request == "hcrr") {
        return 8;
    } else {
        std::cout << "That's an invalid choice! If you can't get this program"
                  << std::endl
                  << " to work please email the author. He spent a lot of "
                  << std::endl
                  << "time on it :) and would complain if you gave him a zero"
                  << std::endl
                  << " because it doesn't work. nick.vandermerwe@griffith.edu"
                     ".au" << std::endl;
        return -1;
    }
}

void runRequest(int choice) {
    std::cout
            << "Congratulations! If you made it this far you're doing better"
            << std::endl
            << "than most markers for my assignments!"
            << std::endl << std::endl
            << "============BENCHMARKING REQUESTS==============="
            << std::endl
            << "Now is the tough part! Enter the bounds of boards that you want to "
            << std::endl
            << "test and how much to increment them by. Think of this like the "
            << std::endl
            << "syntax for python's range(), well, without commas"
            << std::endl
            << "\t Example 1: '1 21 1' tests everything from 1 -> 20"
            << std::endl
            << "\t Example 2: '10 151 10' tests every tenth number from "
            << std::endl
            << "\t \t10 -> 150. That is: 10, 20, 30, 40, 50..."
            << std::endl
            << "**Note: if a benchmarkUninformed got skipped its because there is no "
            << std::endl
            << "solution. e.g. 2 4 1 will print nothing :)"
            << std::endl << ">";
    int lowerBound, upperBound, increment;
    std::cin >> lowerBound >> upperBound >> increment;

    if (choice == 0) {
        //badbfs
        benchmarkFunction(badBFS, lowerBound, upperBound, increment);
    } else if (choice == 1) {
        //bfs
        benchmarkFunction(BFS, lowerBound, upperBound, increment);
    } else if (choice == 2) {
        //pbfs
        benchmarkFunction(prunedBFS, lowerBound, upperBound, increment);
    } else if (choice == 3) {
        //dfs
        benchmarkFunction(DFS, lowerBound, upperBound, increment);
    } else if (choice == 4) {
        // pdfs
        benchmarkFunction(prunedDFS, lowerBound, upperBound, increment);
    } else if (choice == 5) {
        // hc
        benchmarkFunction(hillClimbingSearch, lowerBound, upperBound,
                          increment);
    } else if (choice == 6) {
        //sa
        benchmarkFunction(simulatedAnnealing, lowerBound, upperBound,
                          increment);
    } else if (choice == 7) {
        // sarr
        benchmarkFunction(simulatedAnnealingRR, lowerBound, upperBound,
                          increment);
    } else if (choice == 8) {
        // hcrr
        benchmarkFunction(hillClimbingSearchRR, lowerBound, upperBound,
                          increment);
    }
    std::cout << "You can request another function (e.g. bfs)!" <<
              std::endl << ">";
}

int main() {
    openMessage();
    while (true) {
        printAlgorithms();
        int choice = handleUserInput();
        if (choice == -1) return -1;
        runRequest(choice);
    }
}
