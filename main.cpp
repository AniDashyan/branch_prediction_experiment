#include <format>
#include <random>
#include <vector>
#include <utility>
#include <string>
#include "kaizen.h"

// Simulate work inside the branch
inline int doWork(int x) {
    volatile int result = x * x;
    return result;
}

std::pair<std::vector<bool>, double> generateRandomConditions(int iterations, std::mt19937& gen) {
    std::uniform_real_distribution<> probDist(0.0, 1.0);
    double p = probDist(gen); // Random probability for this trial
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    std::vector<bool> conditions(iterations);
    int trueCount = 0;
    for (int i = 0; i < iterations; i++) {
        conditions[i] = (dis(gen) < p);
        if (conditions[i]) trueCount++;
    }
    double truePercentage = static_cast<double>(trueCount) / iterations * 100.0;
    
    return {conditions, truePercentage};
}


auto runTest(const std::vector<bool>& conditions, int iterations, int& sum) {
    sum = 0;
    zen::timer t;
    
    t.start();
    for (int i = 0; i < iterations; i++) {
        if (conditions[i]) {
            sum += doWork(i);
        } else {
            sum -= doWork(i);
        }
    }
    t.stop();
    return t.duration<zen::timer::usec>().count();
}


auto runPredictableTest(int iterations, int& sum, int conditionType) {
    zen::timer t;
    sum = 0;
    
    t.start();
    for (int i = 0; i < iterations; i++) {
        bool condition;
        switch (conditionType) {
            case 1: condition = (i % 2 == 0); break;              // Alternating
            case 2: condition = (i % 10 < 3); break;              // 3/10 pattern
            case 3: condition = (i < iterations - 100); break;    // Strong bias
            case 4: condition = ((i * i) % 5 == 0); break;       // Arithmetic
            default: condition = true; break;                     // Constant
        }
        if (condition) {
            sum += doWork(i);
        } else {
            sum -= doWork(i);
        }
    }
    t.stop();
    return t.duration<zen::timer::usec>().count();
}

std::pair<int, int> parse_args(int argc, char** argv) {
    zen::cmd_args args(argv, argc);
    
    int iter = 1'000'000, trial = 3;
    if (!args.is_present("--iter") && !args.is_present("--trial")) {
        zen::log(zen::color::yellow("--iter and --trial options are not provided. Using the default values: " + std::to_string(iter) + " and "+ std::to_string(trial)));
    } 
    else if (!args.is_present("--iter")) {
        zen::log("--iter option is not provided using the default value: ", std::to_string(iter));
        trial = std::stoi(args.get_options("--trial")[0]);
    }
    else if (!args.is_present("--trial")) {
        zen::log("--trial option is not provided using the default value: ", std::to_string(trial));
        trial = std::stoi(args.get_options("--trial")[0]);
    }
    else {
        iter = std::stoi(args.get_options("--iter")[0]);
        trial = std::stoi(args.get_options("--trial")[0]);
    }
    return {iter, trial};
}

int main(int argc, char** argv) {
    const int PREDICTABLE_TYPES = 5;
    auto [ITERS, TRIALS] = parse_args(argc, argv);

    std::random_device rd;
    std::mt19937 gen(rd());

    zen::print(std::format("Performance Test: {} Trials, {} Predictable Conditions, {} Iterations\n", 
                   TRIALS, PREDICTABLE_TYPES, ITERS));
    zen::print("------------------------------------------------------------\n");

    std::vector<std::string> conditionNames = {"Alternating", "3/10 Pattern", "Strong Bias", "Arithmetic", "Constant"};

    // Run trials
    for (int trial = 0; trial < TRIALS; trial++) {
        auto [conditions, truePercentage] = generateRandomConditions(ITERS, gen);
        
        // Run unpredictable test
        int sum1;
        long long unpredictableTime = runTest(conditions, ITERS, sum1);

        // Run all predictable tests for this trial
        std::vector<long long> predictableTimes(PREDICTABLE_TYPES);
        for (int condType = 1; condType <= PREDICTABLE_TYPES; condType++) {
            int sum;
            predictableTimes[condType - 1] = runPredictableTest(ITERS, sum, condType);
        }

        zen::print(std::format("Trial {}:\n", trial + 1));
        zen::print(std::format("  Distribution: {:.2f}% true, {:.2f}% false\n", truePercentage, 100.0 - truePercentage));
        zen::print("  Execution Times:\n");
        zen::print(std::format("    {:<20} {:>12} us\n", "Unpredictable", unpredictableTime));
        for (int condType = 0; condType < PREDICTABLE_TYPES; condType++) {
            zen::print(std::format("    {:<20} {:>12} us\n", conditionNames[condType], predictableTimes[condType]));
        }
        zen::print("  Differences from Unpredictable:\n");
        for (int condType = 0; condType < PREDICTABLE_TYPES; condType++) {
            long long diff = unpredictableTime - predictableTimes[condType];
            zen::print(std::format("    {:<20} {:>12} us\n", conditionNames[condType], diff));
        }
        zen::print("------------------------------------------------------------\n");
    }
    return 0;
}