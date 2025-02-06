#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <stdexcept>
#include <iomanip>
#include <sstream>
using namespace std;
// Определение мастей и рангов карт
enum Suit {
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES
};

enum Rank {
    TWO = 2,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    ACE
};

// Перегрузка оператора вывода для Suit
ostream& operator<<(ostream& os, const Suit& suit) {
    switch (suit) {
    case HEARTS:   os << "\u2665"; break; // Черви
    case DIAMONDS: os << "\u2666"; break; // Бубны
    case CLUBS:    os << "\u2663"; break; // Трефы
    case SPADES:   os << "\u2660"; break; // Пики
    default:       os << "?"; break;
    }
    return os;
}

// Класс Card
class Card {
public:
    Card(Suit suit, Rank rank) : suit_(suit), rank_(rank) {}

    Suit getSuit() const { return suit_; }
    Rank getRank() const { return rank_; }

    int getValue() const {
        if (rank_ >= TWO && rank_ <= NINE) {
            return static_cast<int>(rank_);
        }
        else if (rank_ >= TEN && rank_ <= KING) {
            return 10;
        }
        else if (rank_ == ACE) {
            return 11;
        }
        return 0;
    }

    // Перегрузка оператора вывода
    friend ostream& operator<<(ostream& os, const Card& card) {
        string rankStr;
        switch (card.rank_) {
        case JACK:   rankStr = "J"; break;
        case QUEEN:  rankStr = "Q"; break;
        case KING:   rankStr = "K"; break;
        case ACE:    rankStr = "A"; break;
        default:     rankStr = std::to_string(static_cast<int>(card.rank_)); break;
        }
        os << rankStr << card.getSuit();
        return os;
    }

private:
    Suit suit_;
    Rank rank_;
};

// Класс Deck
class Deck {
public:
    Deck(int numDecks = 1) {
        // Генерация карт
        for (int i = 0; i < numDecks; ++i) {
            for (int suit = HEARTS; suit <= SPADES; ++suit) {
                for (int rank = TWO; rank <= ACE; ++rank) {
                    cards_.emplace_back(static_cast<Suit>(suit), static_cast<Rank>(rank));
                }
            }
        }
        shuffle();
    }

    void shuffle() {
        random_device rd;
        mt19937 g(rd());
        std::shuffle(cards_.begin(), cards_.end(), g);
    }

    Card dealCard() {
        if (cards_.empty()) {
            throw runtime_error("Колода пуста!");
        }
        Card card = cards_.back();
        cards_.pop_back();
        return card;
    }

    bool isEmpty() const {
        return cards_.empty();
    }

    const vector<Card>& getCards() const {
        return cards_;
    }

private:
    vector<Card> cards_;
};


// Класс Player
class Player {
public:
    Player() : handValue_(0), hasAce_(false) {}

    void addCard(const Card& card) {
        hand_.push_back(card);
        handValue_ += card.getValue();

        if (card.getRank() == ACE) {
            hasAce_ = true;
        }
        adjustAce();
    }

    void adjustAce() {
        while (handValue_ > 21 && hasAce_) {
            handValue_ -= 10;
            hasAce_ = false;
            for (const Card& card : hand_) {
                if (card.getRank() == ACE)
                    hasAce_ = true;
            }
        }
    }

    int getHandValue() const {
        return handValue_;
    }

    bool hasBlackjack() const {
        return handValue_ == 21 && hand_.size() == 2;
    }

    void clearHand() {
        hand_.clear();
        handValue_ = 0;
        hasAce_ = false;
    }

    void printHand(bool showFirstCard = true) const {
        setlocale(LC_ALL, "ru");
        for (size_t i = 0; i < hand_.size(); ++i) {
            if (!showFirstCard && i == 0) {
                cout << "[скрыто] ";
            }
            else {
                cout << hand_[i] << " ";
            }
        }
        cout << endl;
    }

protected:
    vector<Card> hand_;
    int handValue_;
    bool hasAce_;
};

// Класс Dealer
class Dealer : public Player {
public:
    bool shouldHit() const {
        return getHandValue() < 17;
    }
};

// Класс Game
class Game {
public:
    Game(int numDecks = 4) : deck_(numDecks), player_(), dealer_() {}

    void play() {
        char choice;
        bool gameFinished = false;

        do {
            startNewRound();
            dealInitialCards();

            playerTurn();

            if (player_.getHandValue() <= 21) {
                dealerTurn();

                determineWinner();
            }
            setlocale(LC_ALL, "ru");

            cout << "Сыграть еще раз? (y/n): ";
            cin >> choice;
            gameFinished = (choice != 'y');
            player_.clearHand();
            dealer_.clearHand();
        } while (!gameFinished);

        std::cout << "Игра окончена." << endl;
    }

private:
    void startNewRound() {
        if (deck_.isEmpty()) {
            setlocale(LC_ALL, "ru");
            cout << "Перемешиваем колоду..." << endl;
            deck_ = Deck(4);
        }
    }

    void dealInitialCards() {
        player_.addCard(deck_.dealCard());
        dealer_.addCard(deck_.dealCard());
        player_.addCard(deck_.dealCard());
        dealer_.addCard(deck_.dealCard());
        setlocale(LC_ALL, "ru");

        cout << "Карты дилера: ";
        dealer_.printHand(false);
        cout << "Карты игрока: ";
        player_.printHand();
    }

    void playerTurn() {
        setlocale(LC_ALL, "ru");

        char choice;
        while (player_.getHandValue() <= 21) {
            cout << "Hit (h) или Stand (s)? ";
            cin >> choice;

            if (choice == 'h') {
                Card card = deck_.dealCard();
                player_.addCard(card);
                cout << "Игрок берет карту: " << card << endl;
                cout << "Карты игрока: ";
                player_.printHand();
                cout << "Очки игрока: " << player_.getHandValue() << endl;

                if (player_.getHandValue() > 21) {
                    cout << "Перебор! Вы проиграли." << endl;
                    return;
                }
            }
            else {
                break;
            }
        }
    }

    void dealerTurn() {
        setlocale(LC_ALL, "ru");

        cout << "Ход дилера:" << endl;
        while (dealer_.shouldHit()) {
            Card card = deck_.dealCard();
            dealer_.addCard(card);
            cout << "Дилер берет карту: " << card << endl;
            cout << "Карты дилера: ";
            dealer_.printHand();
            cout << "Очки дилера: " << dealer_.getHandValue() << endl;
        }
    }

    void determineWinner() {
        setlocale(LC_ALL, "ru");

        cout << "Окончательный результат:" << endl;
        cout << "Карты игрока: ";
        player_.printHand();
        cout << "Очки игрока: " << player_.getHandValue() << endl;
        cout << "Карты дилера: ";
        dealer_.printHand();
        cout << "Очки дилера: " << dealer_.getHandValue() << endl;


        if (player_.hasBlackjack() && dealer_.hasBlackjack()) {
            cout << "Ничья! BlackJack у обоих игроков!" << endl;
        }
        else if (player_.hasBlackjack()) {
            cout << "Игрок выиграл! BlackJack!" << endl;
        }
        else if (dealer_.hasBlackjack()) {
            cout << "Дилер выиграл! BlackJack!" << endl;
        }
        else if (player_.getHandValue() > 21) {
            cout << "Перебор у игрока! Дилер выиграл." << endl;
        }
        else if (dealer_.getHandValue() > 21) {
            cout << "Перебор у дилера! Игрок выиграл." << endl;
        }
        else if (player_.getHandValue() > dealer_.getHandValue()) {
            cout << "Игрок выиграл!" << endl;
        }
        else if (dealer_.getHandValue() > player_.getHandValue()) {
            cout << "Дилер выиграл!" << endl;
        }
        else {
            cout << "Ничья!" << endl;
        }
    }

    Deck deck_;
    Player player_;
    Dealer dealer_;
};

// Интерфейс IFormattable
class IFormattable {
public:
    virtual ~IFormattable() {}
    virtual string format() const = 0;
};

// Функция prettyPrint
void prettyPrint(const IFormattable& object) {
    setlocale(LC_ALL, "ru");

    cout << "Отформатированный вывод: " << object.format() << endl;
}

// Адаптер класса (для Deck)
class DeckAdapter : public IFormattable, public Deck {
public:
    DeckAdapter(int numDecks = 1) : Deck(numDecks) {}

    string format() const override {
        setlocale(LC_ALL, "ru");

        stringstream ss;
        ss << "Содержимое колоды (адаптировано):\n";
        for (const auto& card : getCards()) {
            ss << card << " ";
        }
        ss << "\nКоличество карт: " << getCards().size();
        return ss.str();
    }
};

// Адаптер объекта (для Deck)
class DeckObjectAdapter : public IFormattable {
public:
    DeckObjectAdapter(const Deck& deck) : deck_(deck) {}

    string format() const override {
        setlocale(LC_ALL, "ru");

        stringstream ss;
        ss << "Содержимое колоды (адаптировано через объект):\n";
        for (const auto& card : deck_.getCards()) {
            ss << card << " ";
        }
        ss << "\nКоличество карт: " << deck_.getCards().size();
        return ss.str();
    }

private:
    const Deck& deck_;
};


int main() {
    Game game;
    game.play();
    setlocale(LC_ALL, "ru");
}