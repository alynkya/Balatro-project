#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

// ---------------------------------------------------------
// Data carried between phases
// ---------------------------------------------------------
struct TurnInput {
    int value;
};

// ---------------------------------------------------------
// IInputGenerator (mutable part #1)
// ---------------------------------------------------------
class IInputGenerator {
public:
    virtual TurnInput generate(int round) = 0;
    virtual ~IInputGenerator() {}
};

// Original / deterministic generator
class FixedInputGenerator : public IInputGenerator {
public:
    TurnInput generate(int round) override {
        // simple deterministic pattern: 3, 5, 7, ...
        TurnInput t;
        t.value = 3 + (round - 1) * 2;
        return t;
    }
};

// Modification 1: replaces FixedInputGenerator WITHOUT touching RunSession
class RandomInputGenerator : public IInputGenerator {
public:
    TurnInput generate(int round) override {
        TurnInput t;
        t.value = 1 + (rand() % 10); // random value 1..10
        return t;
    }
};

// ---------------------------------------------------------
// IScoringRule
// ---------------------------------------------------------
class IScoringRule {
public:
    virtual int computeBaseScore(const TurnInput& input) = 0;
    virtual ~IScoringRule() {}
};

class SimpleScoringRule : public IScoringRule {
public:
    int computeBaseScore(const TurnInput& input) override {
        return input.value;
    }
};

// ---------------------------------------------------------
// IRewardRule (mutable part #2)
// ---------------------------------------------------------
class IRewardRule {
public:
    virtual int computeReward(int baseScore, int round) = 0;
    virtual ~IRewardRule() {}
};

// Original behavior: reward == base score
class EqualRewardRule : public IRewardRule {
public:
    int computeReward(int baseScore, int round) override {
        return baseScore;
    }
};

// Modification 2, option A: reward = baseScore + 2
class BonusRewardRule : public IRewardRule {
public:
    int computeReward(int baseScore, int round) override {
        return baseScore + 2;
    }
};

// Modification 2, option B: reward = baseScore * 2 on even rounds only
class EvenRoundDoubleRewardRule : public IRewardRule {
public:
    int computeReward(int baseScore, int round) override {
        return (round % 2 == 0) ? baseScore * 2 : baseScore;
    }
};

// ---------------------------------------------------------
// ShopSystem (simple / hardcoded, with a runtime decision)
// ---------------------------------------------------------
class ShopSystem {
public:
    void offer(int& money) {
        const int cost = 2;
        const int bonus = 2;
        cout << "[SHOP] offered: Bonus(+" << bonus << ") cost " << cost << endl;
        cout << "Buy the bonus? (1 = yes, 0 = skip): ";

        int choice = 0;
        cin >> choice;

        if (choice == 1 && money >= cost) {
            money -= cost;
            money += bonus;
            cout << "[SHOP] purchased bonus | money: " << money << endl;
        } else {
            cout << "[SHOP] skipped" << endl;
        }
    }
};