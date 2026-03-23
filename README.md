# Limit Order Book Engine (C++)

A C++ implementation of a limit order book supporting price-time priority matching for buy and sell orders.  
Designed as a simplified model of real-world exchange matching engines.

---

## Overview

This project implements the core logic behind a financial exchange's matching engine.  
It maintains buy and sell order books per symbol and processes incoming limit orders by matching them against existing liquidity.

---

## Features

- Limit order matching (buy/sell)
- Price priority:
  - Highest bid matched first
  - Lowest ask matched first
- Partial and full order fills
- Multi-symbol support (e.g., AAPL, TSLA)
- Automatic book updates after matching
- Clean output of remaining orders

---

## Matching Logic

### Buy Orders

A buy order is matched against the lowest available sell orders such that:

```
sell_price ≤ buy_price
```

Matching continues until:
- The order is fully filled, or
- No more matching sell orders are available

---

### Sell Orders

A sell order is matched against the highest available buy orders such that:

```
buy_price ≥ sell_price
```

Matching continues until:
- The order is fully filled, or
- No more matching buy orders are available

---

## Data Structures

For each symbol, the engine maintains:

- **Buy book**
  - Vector of (price, quantity)
  - Sorted in descending order (highest price first)

- **Sell book**
  - Vector of (price, quantity)
  - Sorted in ascending order (lowest price first)

Orders are re-sorted after each insertion to maintain correct priority.

---

## Example

### Input

```cpp
ex.addOrder("AAPL", "buy", 100, 10);
ex.addOrder("AAPL", "sell", 100, 5);
```

### Result

- 5 shares are matched  
- Remaining:
  - Buy: 5 @ 100

---

## Complexity

- Matching: O(n log n)
- Insertions: O(n log n)

> Note: This implementation prioritizes clarity and correctness over optimal performance.

---

## Future Improvements

- Replace sorting with priority queues (heaps) for O(log n) matching
- Add time-priority (FIFO within same price level)
- Introduce order IDs and cancellation support
- Use more efficient containers (e.g., maps or trees per price level)
- Optimize for low-latency environments

---

## Motivation

This project was built to understand the internal mechanics of trading systems and matching engines used in financial markets.

---

## Build & Run

```bash
g++ -std=c++17 main.cpp -o exchange
./exchange
```

---

## License

MIT License
