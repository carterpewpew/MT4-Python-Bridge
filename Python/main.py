# mt4bridge.py
import mmap
import struct
import win32event
import win32con
import pywintypes
import time

class MT4Bridge:
    SHM_NAME = "Local\\MT4PythonBridge"
    EVENT_NAME = "Global\\MT4TickEvent"
    SHM_SIZE = 8 + 8 + 32  # double ask + double bid + 32-byte signal string

    def __init__(self):
        self.mem = None
        self.event = None
        self._connect()

    def _connect(self):
        while True:
            try:
                self.mem = mmap.mmap(-1, self.SHM_SIZE, tagname=self.SHM_NAME)
                self.event = win32event.OpenEvent(win32con.SYNCHRONIZE, False, self.EVENT_NAME)
                break
            except pywintypes.error:
                time.sleep(1)

    def wait_for_tick(self):
        win32event.WaitForSingleObject(self.event, win32event.INFINITE)
        return self.read_tick()

    def read_tick(self):
        self.mem.seek(0)
        ask, bid = struct.unpack("dd", self.mem.read(16))
        signal = self.mem.read(32).split(b'\0', 1)[0].decode()
        return {"ask": ask, "bid": bid, "signal": signal}

    def send_signal(self, signal):
        self.mem.seek(16)
        b = signal.encode()[:31]
        b += b'\0' * (32 - len(b))
        self.mem.write(b)

    def close(self):
        if self.mem:
            self.mem.close()


# Example usage
if __name__ == "__main__":
    bridge = MT4Bridge()
    print("Connected to MT4. Waiting for ticks...")
    try:
        while True:
            tick = bridge.wait_for_tick()
            print(f"Tick: Ask={tick['ask']:.5f}, Bid={tick['bid']:.5f}, Signal={tick['signal']}")
            bridge.send_signal("buy")  # Replace with your strategy logic
    except KeyboardInterrupt:
        print("Exiting...")
        bridge.close()
