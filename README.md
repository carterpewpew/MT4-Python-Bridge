# MT4 ↔ Python Ultra-Fast Bridge (Shared Memory + Events)

This project enables **low-latency, bidirectional communication** between MetaTrader 4 (MT4) and Python using **shared memory and event signaling**. It is designed for high-performance applications like real-time signal generation, algorithmic trading, and custom analytics — all without polling.

---

## 🔧 Directory Structure
```
.
├── mt4
│   └── expert.mq4        # MT4 Expert Advisor to send ticks and receive signals
├── python
│   └── main.py             # Python bridge that receives ticks and sends signals
├── bridge
│   └── bridge.c            # C DLL for shared memory and event coordination
└── README.md
```

---

## ⚙️ How It Works
- MT4 loads a DLL built from `bridge.c`, which creates shared memory and an event.
- Every tick, MT4 writes ask/bid into shared memory and signals Python using `SetEvent()`.
- Python waits (`WaitForSingleObject`) and processes the tick only when there's new data.
- Python writes the signal (`buy`, `sell`, etc.) back into shared memory.
- MT4 reads that signal and prints it in the log.

No polling. Pure event-based signaling. ⚡

---

## 🛠️ Setup Instructions

### 1. 🔨 Compile the DLL from `bridge.c`

**Requirements:**
- Windows
- [MinGW-w64](https://www.mingw-w64.org/) or MSVC compiler

**Using MinGW (32-bit):**
```bash
cd bridge
gcc -shared -o bridge.dll bridge.c -Wl,--output-def,bridge.def -m32
```
> 📌 Ensure MT4 and DLL are both 32-bit. MT4 is always 32-bit even on 64-bit Windows.

---

### 2. 🧠 Install Python Dependencies
In a terminal:
```bash
pip install pywin32
```

---

### 3. 📂 Place Files and Compile
- Copy `bridge/bridge.dll` to your MT4 `Libraries` folder:
- Copy `mt4/expert.mq4` to your MT4 `Experts` folder:
- Compile the EA in MetaEditor.

---

### 4. 🐍 Run the Python Script
In a terminal:
```bash
cd python
python main.py
```

You should see output like:
```text
Connected to MT4. Waiting for ticks...
Tick: Ask=1.34103, Bid=1.34080, Signal=none
```

---

### 5. ▶️ Run the EA in MT4
- Attach `expert` to any chart.
- Open the **Terminal → Experts** log.
- You’ll see Ask/Bid printed along with signals from Python.

---

## ✅ Example Tick Flow
```
MT4 Tick → Write ask/bid → Signal event → Python wakes up
Python reads → Decides signal → Writes signal → MT4 reads signal
```

---

## 📌 Notes
- This setup is optimized for ultra-fast execution (under 1 ms typical).
- You can easily extend the struct to include indicators (EMA, SMA, time).

---