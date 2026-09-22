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

// ---------------------------------------------------------
// RunSession = THE INVARIANT
// Only knows the phase order. No scoring/reward/input logic here.
// ---------------------------------------------------------
class RunSession {
private:
    IInputGenerator* inputGen;
    IScoringRule*    scoringRule;
    IRewardRule*     rewardRule;
    ShopSystem*      shop;
    int money;
    int totalRounds;

public:
    RunSession(IInputGenerator* ig, IScoringRule* sr, IRewardRule* rr,
               ShopSystem* s, int rounds = 3)
        : inputGen(ig), scoringRule(sr), rewardRule(rr), shop(s),
          money(0), totalRounds(rounds) {}

    void run() {
        cout << "=== RUN START ===" << endl;

        for (int round = 1; round <= totalRounds; ++round) {
            cout << "\nRound " << round << endl;

            // 1. Generate input
            TurnInput input = inputGen->generate(round);
            cout << "[PLAY] input generated: " << input.value << endl;

            // 2. Compute base score
            int baseScore = scoringRule->computeBaseScore(input);
            cout << "[SCORE] base score: " << baseScore << endl;

            // 3. Compute reward
            int reward = rewardRule->computeReward(baseScore, round);

            // 4. Update money
            money += reward;
            cout << "[REWARD] gain: " << reward << " | money: " << money << endl;

            // 5. Shop phase
            shop->offer(money);

            // 6. Advance round (handled by the for-loop itself)
        }

        cout << "\n=== RUN END ===" << endl;
        cout << "Final money: " << money << endl;
    }
};

// ---------------------------------------------------------
// main(): lets the user DECIDE which mutable parts to plug in.
// RunSession's code never changes based on these choices.
// ---------------------------------------------------------
int main() {
    srand((unsigned int)time(nullptr));

    // Mutable parts are wired here directly (no selection menu).
    // Swap these two lines to switch generator/reward strategy —
    // RunSession itself never has to change.
    IInputGenerator* inputGen  = new RandomInputGenerator();   // Modification 1
    IRewardRule*     rewardRule = new BonusRewardRule();       // Modification 2A
    IScoringRule*    scoringRule = new SimpleScoringRule();
    ShopSystem*      shop = new ShopSystem();

    // RunSession is built once and its logic NEVER depends on which
    // concrete classes were plugged in above.
    RunSession session(inputGen, scoringRule, rewardRule, shop, 3);
    session.run();

    delete inputGen;
    delete scoringRule;
    delete rewardRule;
    delete shop;

    return 0;
}