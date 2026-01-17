#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

class Player {
public:
    string name;
    int score;
    vector<int> rolls;

    Player(string n) {
        name = n;
        score = 0;
    }

    void addRoll(int roll) {
        rolls.push_back(roll);
        score += roll;
    }

    double getAverageRoll() {
        if (rolls.empty()) return 0;
        double sum = 0;
        for (int r : rolls) sum += r;
        return sum / rolls.size();
    }
};

class DiceGame {
private:
    vector<Player> players;
    int targetScore;
    string historyFile = "dice_game_history.txt";

    void diceRollAnimation() {
        cout << YELLOW << "Rolling";
        for (int i = 0; i < 3; i++) {
            cout << ".";
            cout.flush();     
        }
        cout << RESET << endl;
    }

    int rollDice() {
        return (rand() % 6) + 1;
    }

    void saveHistory(string winnerName) {
        ofstream outFile(historyFile, ios::app);
        if (outFile.is_open()) {
            outFile << "===== New Game =====\n";
            outFile << "Target Score: " << targetScore << "\n";
            for (auto &p : players) {
                outFile << p.name << ": ";
                for (int r : p.rolls) outFile << r << " ";
                outFile << "(Total: " << p.score << ", Avg: " 
                        << fixed << setprecision(2) << p.getAverageRoll() << ")\n";
            }
            outFile << "Winner: " << winnerName << "\n";
            outFile << "====================\n\n";
            outFile.close();
        }
    }

public:
    DiceGame() {
        srand(static_cast<unsigned int>(time(0)));
    }

    void start() {
        int numPlayers;
        do {
            cout << CYAN << "Enter number of players (1-4): " << RESET;
            cin >> numPlayers;
            if (numPlayers <2 || numPlayers > 4)
                cout << RED << "Invalid number of players.\n" << RESET;
        } while (numPlayers < 2 || numPlayers > 4);

        cout << CYAN << "Enter target score: " << RESET;
            cin >> targetScore;
        players.clear();
        for (int i = 0; i < numPlayers; i++) {
            string name;
            cout << CYAN << "Enter name for Player " << (i + 1) << ": " << RESET;
            cin >> name;
            players.push_back(Player(name));
        }

        bool gameWon = false;
        string winnerName;

        cin.ignore(); // clear newline
        while (!gameWon) {
            for (auto &p : players) {
                cout << "\n---- " << p.name << "'s turn ----\n";
                cout << "Press Enter to roll...";
                cin.get();
                diceRollAnimation();
                int roll = rollDice();
                p.addRoll(roll);
                cout << YELLOW << p.name << " rolled a " << roll
                     << ". Total score: " << p.score << RESET << endl;

                if (p.score >= targetScore) {
                    winnerName = p.name;
                    gameWon = true;
                    break;
                }
            }
        }

        cout << GREEN << "\n " << winnerName << " WINS the game! \n" << RESET;

        sort(players.begin(), players.end(), [](Player &a, Player &b) {
            return a.score > b.score;
        });

        cout << "\n" << CYAN << "Leaderboard:\n" << RESET;
        for (size_t i = 0; i < players.size(); i++) {
            cout << i + 1 << ". " << players[i].name << " - " 
                 << players[i].score << " points (Avg roll: " 
                 << fixed << setprecision(2) << players[i].getAverageRoll() << ")\n";
        }

        saveHistory(winnerName);
        cout << GREEN << "\n Game history saved to " << historyFile << RESET << "\n";
    }

    void showHistory() {
        ifstream inFile(historyFile);
        if (!inFile.is_open()) {
            cout << RED << "No history found.\n" << RESET;
            return;
        }
        string line;
        cout << CYAN << "\n📜 Game History:\n" << RESET;
        while (getline(inFile, line)) {
            cout << line << "\n";
        }
        inFile.close();
    }
};

int main() {
    DiceGame game;
    int choice;

    do {
        cout << CYAN << "\n===== Dice Game Menu =====\n" << RESET;
        cout << "1. Start New Game\n";
        cout << "2. View History\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: game.start(); break;
            case 2: game.showHistory(); break;
            case 3: cout << GREEN << "Exiting game. See You Again..\n" << RESET; break;
            default: cout << RED << "Invalid choice.\n" << RESET; break;
        }
    } while (choice != 3);
    return 0;
}
