#include <iostream>
#include <vector>
#include <chrono>
#include <queue>
#include <stack>
#include <array>

class Queen {
public:
    Queen() = default;

    explicit Queen(short N) : N{N}, nextRow{0} {}

    Queen(Queen const &that) = default;

    std::vector<Queen> possibleActions() {
        findLegalRows();
        std::vector<Queen> nextStates;
        for (short i = 0; i < N; i++) {
            if (choices[i] == false) {
                continue;
            }
            Queen nextState = *this;
            nextState.place(i);
            nextStates.push_back(nextState);
        }
        return nextStates;
    }

    std::vector<Queen> halfPossibleActions() {
        // only get the first half of the possible actions
        findLegalRows();
        std::vector<Queen> nextStates;
        for (short i = 0; i < N; i++) {
            if (choices[i] == false) {
                continue;
            }
            Queen nextState = *this;
            nextState.place(i);
            nextStates.push_back(nextState);
        }
        return nextStates;
    }

    std::vector<std::vector<short>> makeEvaluatedBoard
            (std::vector<std::vector<short>> basicBoard) {
        // Returns a vector that represents how many queens are currently
        // attacking that position
        std::vector<std::vector<short>> eval{static_cast<size_t>(N),
                                             std::vector<short>{N, 0}};

        for (int i = 0; i < N; i++) {
            incrementCross(i, queens[i], eval);
        }
        return eval;
    };


    void place(short row) {
        queens.push_back(row);
        nextRow++;
    }

    bool allQueensArePlaced() {
        return queens.size() == N;
    }

    std::vector<std::vector<short>> getBoard() {
        std::vector<std::vector<short>> v;
        for (int i = 0; i < nextRow; i++) {
            std::vector<short> toAdd(N, 0);
            toAdd[queens[i]] = 1;
            v.push_back(toAdd);
        }
        if (nextRow < N) {
            while (v.size() < N) {
                v.emplace_back(N, 0);
            }
        }
        return v;
    }

    int size() {
        return N;
    }

    int getValue() {
        return value;
    }

private:
    std::vector<short> queens;
    std::vector<bool> choices;
    short N;
    short nextRow;
    short value;

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

    void incrementCross(int y, int x, std::vector<std::vector<short>> &v) {
        // Increments a cross shape (diagonal + straight)
        // around the given point and specifically avoid the point itself

        for (int i = 0; i < N; i++) {
            if (i != x) {
                v[y][i]++; // vertical
            }
            if (i != y) {
                v[i][x]++; // horizontal
            }
        }

        // upwards diagonals
        int a = x, b = x;
        for (int i = y; i < N; i++) {
            a++;
            b--;
            if (a > N && b < N) {
                break; // both are out of bounds
            } else if (a > N) {
                v[i][a]++;
            }
            if (b < N) {
                v[i][b]++;
            }
        }

        //downwards diagonals
        a = y, b = y;
        for (int i = y; i >= 0; i--) {
            a++;
            b--;
            if (a > N && b < N) {
                break; // both are out of bounds
            } else if (a > N) {
                v[i][a]++;
            }
            if (b < N) {
                v[i][b]++;
            }
        }
    }
};

std::vector<std::vector<short>> rotate(std::vector<std::vector<short>> v) {
    //transpose it
    for (int i = 0; i < v.size(); i++) {
        for (int j = i; j < v.size(); j++) {
            std::swap(v[i][j], v[j][i]);
        }
    }
    //reverse the elements
    for (int i = 0; i < v.size(); i++) {
        for (int j = 0; j < v.size() / 2; j++) {
            std::swap(v[i][j], v[i][v.size() - j - 1]);
        }
    }
    return v;
}

std::vector<Queen> permutations(Queen toRotate) {
    // TODO: Redefine this as returning the boards that
    std::vector<Queen> perms;
    // are permutations
//    if (toRotate.size() == 1) return 1;
//    int perms = 2; // original + symmetrical
//    auto board = toRotate.getBoard();
//    auto rotatedBoard = rotate(board);
//    if (board != rotatedBoard) {
//        perms += 2; // rotated, flipped
//    }
    return perms;
}

int badBFS(int N) {
    // This is the original BFS in the slides with checking for previous
    // positons
    // TODO: implement checking if the boards already exist lol
    Queen INITIAL = Queen(N);
    std::queue<Queen> q;
    std::vector<Queen> found;
    int possibleWays = 0;
    q.push(INITIAL);
    int pushes = 0;

    while (!q.empty()) {
        Queen current = q.front();
        q.pop();
        std::vector<Queen> nextBoards = current.possibleActions();
        if (pushes % 100000 == 0) {
            std::cout << "Currently holding " << q.size() << " boards"
                      << std::endl;
        }

        for (auto &board : nextBoards) {
            if (board.allQueensArePlaced()) {
                possibleWays++;
            } else {
                q.push(board);
                pushes++;
            }
        }
    }
    std::cout << "Pushed in " << pushes << " times" << std::endl;
    return possibleWays;
}

int BFS(int N) {
    Queen INITIAL = Queen(N);
    std::queue<Queen> q;
    int possibleWays = 0;
    q.push(INITIAL);
    int pushes = 0;

    while (!q.empty()) {
        Queen current = q.front();
        q.pop();
        std::vector<Queen> nextBoards = current.possibleActions();
        if (pushes % 100000 == 0) {
            std::cout << "Currently holding " << q.size() << " boards"
                      << std::endl;
        }

        for (auto &board : nextBoards) {
            if (board.allQueensArePlaced()) {
                possibleWays++;
            } else {
                q.push(board);
                pushes++;
            }
        }
    }
    std::cout << "Pushed in " << pushes << " times" << std::endl;
    return possibleWays;
}

int DFS(int N) {
    Queen INITIAL = Queen(N);
    std::stack<Queen> q;
    int possibleWays = 0;
    q.push(INITIAL);

    while (!q.empty()) {
        Queen current = q.top();
        q.pop();
        std::vector<Queen> nextBoards = current.possibleActions();

        for (auto &board : nextBoards) {
            if (board.allQueensArePlaced()) {
                possibleWays++;
            } else {
                q.push(board);
            }
        }
    }
    return possibleWays;
}

int HalfDFS(int N) {
    Queen INITIAL = Queen(N);
    std::stack<Queen> q;
    int possibleWays = 0;
    std::vector<Queen> foundSolutions;
    q.push(INITIAL);

    while (!q.empty()) {
        Queen current = q.top();
        q.pop();

        std::vector<Queen> nextBoards = current.halfPossibleActions();

        for (auto &board : nextBoards) {
            if (board.allQueensArePlaced()) {
                auto newSolutions = permutations(board);
                foundSolutions.insert(foundSolutions.end(),
                                      newSolutions.begin(),
                                      newSolutions.end());
            } else {
                q.push(board);
            }
        }
    }
    possibleWays = foundSolutions.size();
    return possibleWays;
}

int benchmark() {
    int totalTime = 0;
    int ways;
    for (int i = 1; i <= 20; i++) {
        auto startClock = std::chrono::high_resolution_clock::now();
        // ==================MEASURED TIME=========================
        //ways = badBFS(i);
        //ways = BFS(i);
        ways = DFS(i);
        //ways = HalfDFS(i);
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

class LocalQueen {
public:
    LocalQueen() = default; // just here so that nullptr is valid

    explicit LocalQueen(const short N) : N{N}, value{-1} {
        randomFll();
    }

    LocalQueen(LocalQueen const &that) = default;

    LocalQueen bestNextBoard() {
        std::vector<std::array<short, 2>> bestMoveYX = findBestMoves();
        auto nextBoard = *this; // copy ctor does deep copy
        int randMove = rand() % bestMoveYX.size();
        nextBoard.move(bestMoveYX[randMove][0],
                       bestMoveYX[randMove][1]);
        return nextBoard;
    }

    int getValue() {
        if (value == -1) { // if the current value isn't valid
            value = evaluatePosition(queens);
        }
        return value;
    }

    void printBoard() {
        // TODO: Rotate the print correctly
        // currently printing the board rotated
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
                if (queens[i] == queens[j] ||
                    std::abs(i - j) == abs(queens[i] - queens[j])) {
                    boardValue++;
                }
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

LocalQueen simulatedAnnealing(int N) {
    // TODO: Add temperature
    LocalQueen current = LocalQueen(N);
    while (true) {
        LocalQueen neighbour = current.bestNextBoard();
        if (neighbour.getValue() <= current.getValue()) {
            return current;
        }
        current = neighbour;
    }
}

LocalQueen hillClimbingSearch(int N) {
    LocalQueen current = LocalQueen(N);
    while (true) {
        LocalQueen neighbour = current.bestNextBoard();
        if (neighbour.getValue() == 0) {
            return current;
        }
        current = neighbour;
    }
}

int benchmarkLocal() {
    double totalTime = 0;
    double minTime = std::numeric_limits<double>::max();
    for (int i = 10; i <= 150; i++) {
        auto startClock = std::chrono::high_resolution_clock::now();
        // ==================MEASURED TIME=========================
        hillClimbingSearch(20);
        // ========================================================
        auto endClock = std::chrono::high_resolution_clock::now();
        double microseconds =
                std::chrono::duration_cast<std::chrono::microseconds>
                        (endClock - startClock).count();
        minTime = std::min(minTime, microseconds);
        totalTime += microseconds;
        std::cout << "Took " << double(microseconds) / 1000000
                  << " seconds to find a way for N = " << i
                  << " to get solved" << std::endl;
    }
    std::cout << "The fastest time was" << minTime / 1000000
              << std::endl;
    std::cout << "The total time was " << totalTime / 1000000
              << "microseconds" <<
              std::endl;
    return 0;
}

int main() {
    benchmarkLocal();
//    benchmark();
    return 0;
}
