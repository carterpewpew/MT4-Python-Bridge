#property strict

#import "bridge.dll"
   void InitBridge();
   void CleanupBridge();
   void SendTick(double ask, double bid);
   void CopySignal(char& buffer[], int maxLen);
#import

int OnInit() {
   InitBridge();
   return INIT_SUCCEEDED;
}

void OnDeinit(const int reason) {
   CleanupBridge();
}

void OnTick() {
   SendTick(Ask, Bid);

   char buffer[32];
   ArrayInitialize(buffer, 0);
   CopySignal(buffer, 32);

   string signal = CharArrayToString(buffer);
   Print("Ask: ", Ask, "  Bid: ", Bid, "  Signal from Python: ", signal);
}
