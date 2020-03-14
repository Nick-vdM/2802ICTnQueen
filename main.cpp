#include <iostream>
#include <vector>
#include <chrono>
#include <queue>
#include <stack>

class Queen {
public:
    Queen() = default;

    explicit Queen(short N) : N{N}, nextRow{0} {}

    Queen(Queen const &that) = default;

    void setnextRow(short nextRow) {
        this->nextRow = nextRow;
    }

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

    void place(short row) {
        queens.push_back(row);
        nextRow++;
    }

    bool allQueensArePlaced() {
        return queens.size() == N;
    }

private:
    std::vector<short> queens;
    std::vector<bool> choices;
    short N;
    short nextRow;

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

int badBFS(int N) {
    /// Includes the search for explored solutions
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

int benchmark() {
    int totalTime = 0;
    int ways;
    for (int i = 1; i <= 20; i++) {
        auto startClock = std::chrono::high_resolution_clock::now();
        // ==================MEASURED TIME=========================
        ways = DFS(i);
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

int main() {
    benchmark();
    return 0;
}
