#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <map>

using namespace std;

const int BOARD_SIZE = 8;
mutex mtx;

struct Move {
    pair<int, int> from;
    pair<int, int> to;
};

class CheckersGame {
private:
    vector<vector<char>> board;
    char player;
    char opponent;
    int noCaptureMoves = 0;

    void initializeBoard() {
        board = vector<vector<char>>(BOARD_SIZE, vector<char>(BOARD_SIZE, ' '));
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if ((i + j) % 2 == 1) {
                    if (i < 3) board[i][j] = 'B';
                    else if (i > 4) board[i][j] = 'W';
                }
            }
        }
    }

    void printBoard() {
        if (player == 'B') {
            cout << "  H G F E D C B A" << endl;
        }
        else {
            cout << "  A B C D E F G H" << endl;
        }
        cout << "  -----------------" << endl;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            int rowNumber = (player == 'B') ? BOARD_SIZE - i : i + 1;
            cout << rowNumber << "|";
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (player == 'B') {
                    cout << board[BOARD_SIZE - 1 - i][BOARD_SIZE - 1 - j] << "|";
                }
                else {
                    cout << board[i][j] << "|";
                }
            }
            cout << endl;
        }
        cout << "  -----------------" << endl;
    }

    bool isValidMove(Move move, char piece) {
        int dx = move.to.first - move.from.first;
        int dy = move.to.second - move.from.second;

        if (piece == 'W' && dx != -1) return false;
        if (piece == 'B' && dx != 1) return false;
        if (abs(dy) != 1) return false;
        if (board[move.to.first][move.to.second] != ' ') return false;

        return true;
    }

    void makeMove(Move move) {
        char piece = board[move.from.first][move.from.second];
        if (isValidMove(move, piece)) {
            board[move.to.first][move.to.second] = piece;
            board[move.from.first][move.from.second] = ' ';
        }
    }

    void opponentMove() {
        lock_guard<mutex> lock(mtx);
        int direction = (opponent == 'W') ? -1 : 1;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j] == opponent) {
                    Move move1 = { {i, j}, {i + direction, j + 1} };
                    Move move2 = { {i, j}, {i + direction, j - 1} };
                    if (isValidMove(move1, opponent)) {
                        makeMove(move1);
                        return;
                    }
                    else if (isValidMove(move2, opponent)) {
                        makeMove(move2);
                        return;
                    }
                }
            }
        }
    }

public:
    CheckersGame(char playerColor) : player(playerColor) {
        opponent = (player == 'W') ? 'B' : 'W';
        initializeBoard();
    }

    void play() {
        if (player == 'B') {
            opponentMove();
            printBoard();
        }

        while (true) {
            string moveStr;
            cout << "Введите ход (например, A6 B5): ";
            getline(cin, moveStr);
            if (moveStr.length() != 5) continue;

            int fromX = moveStr[1] - '1';
            int fromY = moveStr[0] - 'A';
            int toX = moveStr[4] - '1';
            int toY = moveStr[3] - 'A';

            Move move = { {fromX, fromY}, {toX, toY} };
            if (isValidMove(move, player)) {
                makeMove(move);
                printBoard();
                opponentMove();
                printBoard();
            }
            else {
                cout << "Некорректный ход! Попробуйте снова." << endl;
            }
        }
    }
};

int main() {
    char choice;
    cout << "Выберите сторону (W - белые, B - черные): ";
    cin >> choice;
    cin.ignore();
    CheckersGame game(choice);
    game.play();
    return 0;
}
