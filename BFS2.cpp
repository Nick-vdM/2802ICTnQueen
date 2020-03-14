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
    // are permutations
//    if (toRotate.size() == 1) return 1;
//    int perms = 2; // original + symmetrical
//    auto board = toRotate.getBoard();
//    auto rotatedBoard = rotate(board);
//    if (board != rotatedBoard) {
//        perms += 2; // rotated, flipped
//    }
//    return perms;
    return;
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
    q.push(INITIAL);

    while (!q.empty()) {
        Queen current = q.top();
        q.pop();

        std::vector<Queen> nextBoards = current.halfPossibleActions();

        for (auto &board : nextBoards) {
            if (board.allQueensArePlaced()) {
                possibleWays += permutations(board);
            } else {
                q.push(board);
            }
        }
    }
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
    LocalQueen() = default;

    explicit LocalQueen(const short N) : N{N}, nextRow{0} {
        for (int i = 0; i < N; i++) {
            board.push_back(std::vector<short>(N, 0));
            evalBoard.push_back(std::vector<short>(N, 0));
        }
        evalBoard[0][0] = -1; // mark it as empty
        randomFll();
    }

    LocalQueen(LocalQueen const &that) = default;

    void placeQueenAt(short const &y, short const &x) {
        board[y][x] = 1;
        queens.push_back(std::array<short, 2>{y, x});
    }

    LocalQueen bestNextMove() {
        // Counts how many positions are still free
        // in each legal move and returns the 'best'
        // legal move

        initEvalBoard();
        auto nextQueen = *this;
        auto queenToMove = leastAttackedQueen(evalBoard);
        auto newPosition = leastAttackedSquare(evalBoard);
        nextQueen.swapQueenTo(queenToMove, newPosition);
        return nextQueen;
    }

    int getValue() {
        if (value == -1) {
            calculateValue();
        }
        return value;
    }

    void printBoard() {
        std::cout << "============" << std::endl;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                std::cout << " " << board[i][j];
            }
            std::cout << std::endl;
        }
        std::cout << "============" << std::endl;
    }

private:
    std::vector<std::vector<short>> board;
    std::vector<std::array<short, 2>> queens; // positional values
    std::vector<std::vector<short>> evalBoard;
    short N;
    short nextRow;
    short value;

    void randomFll() {
        // randomly allocate queens to positions
        srand(42); // TODO: change to time(NULL)
        for (int i = 0; i < N; i++) {
            short y = 0, x = 0;
            do {
                y = rand() % N, x = rand() % N;
            } while (board[y][x] == 1); // if there's already a queen
            placeQueenAt(y, x);
        }
    }

    void incrementCross(const int y, const int x,
                        std::vector<std::vector<short>> &v) {
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

        // downwards diagonals
        int a = x, b = x;
        for (int i = y + 1; i < N; i++) {
            a++;
            b--;
            if (a > N && b < 0) {
                break; // both are out of bounds
            } else if (a < N) {
                v[i][a]++;
            }
            if (b >= 0) {
                v[i][b]++;
            }
        }

        //upwards diagonals
        a = x, b = x;
        for (int i = y - 1; i >= 0; i--) {
            a++;
            b--;
            if (a > N && b < 0) {
                break; // both are out of bounds
            } else if (a < N) {
                v[i][a]++;
            }
            if (b >= 0) {
                v[i][b]++;
            }
        }
    }

    void initEvalBoard() {
        // Returns a vector that represents how many queens are currently
        // attacking that position
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                evalBoard[i][j] = 0;
            }
        }
        for (int i = 0; i < N; i++) {
            incrementCross(queens[i][0], queens[i][1], evalBoard);
        }
    };

    std::array<short, 2> leastAttackedQueen(
            const std::vector<std::vector<short>> &eval) {
        std::array<short, 2> min = queens[0];
        for (int i = 1; i < N; i++) {
            if (evalBoard[queens[i][0]][queens[i][1]] <
                evalBoard[min[0]][min[1]]) {
                min[0] = queens[i][0];
                min[1] = queens[i][1];
            }
        }
        return min;
    }

    std::array<short, 2> leastAttackedSquare(
            const std::vector<std::vector<short>> &eval) {
        std::array<short, 2> position{0, 0}; // y x
        for (int y = 1; y < N; y++) { // skip the first square
            for (int x = 0; x < N; x++) {
                if (eval[y][x] < eval[position[0]][position[1]]) {
                    position[0] = y;
                    position[1] = x;
                }
            }
        }
        return position;
    }

    void swapQueenTo(std::array<short, 2> &from, std::array<short, 2> &to) {
        // board position
        std::swap(board[from[0]][from[1]], board[to[0]][to[1]]);
        // coordinate position
        from[0] = to[0];
        from[1] = to[1];
        // invalidate previous values
        value = -1;
        evalBoard[0][0] = -1;
    }

    void calculateValue() {
        // TODO: Redefine this as 'the number of pairs attacking one another'
        // sets the value variable based on the sum of the number of attacks
        // queens are getting
        if (evalBoard[0][0] == -1) {
            // -1 in the top left signifies the board was 'cleared'
            initEvalBoard();
        }
        value = 0;
        for (int i = 0; i < N; i++) {
            value += evalBoard[queens[i][0]][queens[i][1]];
        }
    }
};

LocalQueen simulatedAnnealing(int N) {
    // TODO: Add temperature
    LocalQueen current = LocalQueen(N);
    while (true) {
        LocalQueen neighbour = current.bestNextMove();
        if (neighbour.getValue() <= current.getValue()) {
            return current;
        }
        current = neighbour;
    }
}

LocalQueen hillClimbingSearch(int N) {
    LocalQueen current = LocalQueen(N);
    std::cout << "value = " << current.getValue() << std::endl;
    current.printBoard();
    while (true) {
        LocalQueen neighbour = current.bestNextMove();
        std::cout << "value = " << current.getValue() << std::endl;
        neighbour.printBoard();
        if (neighbour.getValue() <= current.getValue()) {
            return current;
        }
        current = neighbour;
    }
}

int main() {
    hillClimbingSearch(4);
//    benchmark();
    return 0;
}
