# BranchPredictionTest

## 1. Overview

This project investigates the performance impact of branch mispredictions in CPUs by comparing loops with unpredictable (random) branching against predictable patterns. Branch mispredictions disrupt speculative execution, causing pipeline flushes and slowdowns. The project measures this cost, testing random conditions and predictable patterns (e.g., alternating, biased, constant) to demonstrate how predictability improves performance.

## 2. Build & Run

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/AniDashyan/branch_prediction_experiment
   cd branch_prediction_experiment
   ```

2. **Build the Project**:
   ```bash
   cmake -S . -B build
   cmake --build build --config Release
   ```

3. **Run the Executable**:
   ```bash
   ./build/branch --iter [n] --trials [m]
   ```
   - `--iter [n]`: Iterations per test (default: 10,000,000).
   - `--trials [m]`: Number of trials (default: 5).

## 3. Example Output

Sample output for `./build/branch --iter 10000000 --trials 2`:

```
Performance Test: 2 Trials, 5 Predictable Conditions, 1000000 Iterations
------------------------------------------------------------
Trial 1:
  Distribution: 77.69% true, 22.31% false
  Execution Times:
    Unpredictable               12445 us
    Alternating                  1888 us
    3/10 Pattern                 3189 us
    Strong Bias                  1465 us
    Arithmetic                   1734 us
    Constant                     1591 us
  Differences from Unpredictable:
    Alternating                 10557 us
    3/10 Pattern                 9256 us
    Strong Bias                 10980 us
    Arithmetic                  10711 us
    Constant                    10854 us
------------------------------------------------------------
Trial 2:
  Distribution: 12.38% true, 87.62% false
  Execution Times:
    Unpredictable               11582 us
    Alternating                  1955 us
    3/10 Pattern                 3249 us
    Strong Bias                  1536 us
    Arithmetic                   1764 us
    Constant                     1640 us
  Differences from Unpredictable:
    Alternating                  9627 us
    3/10 Pattern                 8333 us
    Strong Bias                 10046 us
    Arithmetic                   9818 us
    Constant                     9942 us
------------------------------------------------------------
```

## 4. Explanation of How the Project Works

### What is Branch Prediction?

Branch prediction is a CPU technique to guess branch outcomes (e.g., if statements) to keep the pipeline running. Mispredictions cause stalls, slowing execution. Predictable branches reduce this penalty.

### How the Project Works

- **Random Conditions**: Generates a vector of random true/false values with a probability `p`, simulating unpredictable branches.
- **Predictable Patterns**: Tests five conditions—Alternating (`i % 2 == 0`), 3/10 Pattern (`i % 10 < 3`), Strong Bias (`i < iterations - 100`), Arithmetic (`(i * i) % 5 == 0`), and Constant (`true`).
- **Measurement**: Runs loops, measures times with `std::chrono`, and compares unpredictable vs. predictable performance.

### Why Some Patterns Are Faster

Predictable patterns are faster because CPUs learn them:
- **Alternating**: Regular true/false cycle, easily predicted.
- **3/10 Pattern**: Longer but predictable pattern.
- **Strong Bias**: Mostly true, few mispredictions.
- **Arithmetic**: Deterministic but complex.
- **Constant**: Always true, minimal mispredictions.

Random conditions (e.g., 50/50) cause frequent mispredictions, while predictable ones optimize pipeline efficiency.
