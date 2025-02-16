#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>

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
            int rowNumber = (player == 'B') ? i + 1 : 8 - i;
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

        if (board[move.to.first][move.to.second] != ' ') return false;

        if (abs(dx) == 1 && abs(dy) == 1) {
            if ((piece == 'W' && dx == -1) || (piece == 'B' && dx == 1) || piece == 'X' || piece == 'Y') {
                return true;
            }
        }

        if (abs(dx) == 2 && abs(dy) == 2) {
            int capX = (move.from.first + move.to.first) / 2;
            int capY = (move.from.second + move.to.second) / 2;
            if (board[capX][capY] == opponent || board[capX][capY] == (opponent == 'B' ? 'Y' : 'X')) {
                return true;
            }
        }

        return false;
    }

    bool canCapture(char piece) {
        int directions[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j] == piece) {
                    for (auto dir : directions) {
                        int x = i + dir[0], y = j + dir[1];
                        int x2 = i + 2 * dir[0], y2 = j + 2 * dir[1];
                        if (x2 >= 0 && x2 < BOARD_SIZE && y2 >= 0 && y2 < BOARD_SIZE &&
                            (board[x][y] == opponent || board[x][y] == (opponent == 'B' ? 'Y' : 'X')) &&
                            board[x2][y2] == ' ') {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    void makeMove(Move move) {
        char piece = board[move.from.first][move.from.second];
        board[move.to.first][move.to.second] = piece;
        board[move.from.first][move.from.second] = ' ';

        if (abs(move.to.first - move.from.first) == 2) {
            int capX = (move.from.first + move.to.first) / 2;
            int capY = (move.from.second + move.to.second) / 2;
            board[capX][capY] = ' ';
        }

        if (piece == 'W' && move.to.first == 0) board[move.to.first][move.to.second] = 'X';
        if (piece == 'B' && move.to.first == BOARD_SIZE - 1) board[move.to.first][move.to.second] = 'Y';
    }

    void opponentMove() {
        bool captured;
        do {
            captured = false;
            Move bestMove;
            for (int i = 0; i < BOARD_SIZE; ++i) {
                for (int j = 0; j < BOARD_SIZE; ++j) {
                    if (board[i][j] == opponent) {
                        int dir = (opponent == 'B') ? 1 : -1;
                        Move possibleMoves[] = {
                            {{i, j}, {i + dir, j + 1}},
                            {{i, j}, {i + dir, j - 1}}
                        };
                        for (Move m : possibleMoves) {
                            if (m.to.first >= 0 && m.to.first < BOARD_SIZE &&
                                m.to.second >= 0 && m.to.second < BOARD_SIZE &&
                                isValidMove(m, opponent)) {
                                makeMove(m);
                                captured = true;
                                break;
                            }
                        }
                    }
                }
            }
        } while (captured);
    }

public:
    CheckersGame(char playerColor) : player(playerColor) {
        opponent = (player == 'W') ? 'B' : 'W';
        initializeBoard();
        if (player == 'B') {
            opponentMove();
        }
        printBoard();
    }

    void play() {
        while (true) {
            string moveStr;
            cout << "Введите ход (например, A6 B5): ";
            getline(cin, moveStr);
            if (moveStr.length() != 5) continue;

            int fromX = 8 - (moveStr[1] - '0');
            int fromY = moveStr[0] - 'A';
            int toX = 8 - (moveStr[4] - '0');
            int toY = moveStr[3] - 'A';

            Move move = { {fromX, fromY}, {toX, toY} };
            if (isValidMove(move, player)) {
                if (canCapture(player) && abs(toX - fromX) != 2) {
                    cout << "Вы обязаны выполнить взятие!" << endl;
                    continue;
                }
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
