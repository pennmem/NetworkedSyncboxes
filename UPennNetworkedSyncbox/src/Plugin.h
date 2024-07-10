#ifndef PLUGIN_PCH
#define PLUGIN_PCH

const char* OpenUSB();
const char* CloseUSB();
bool IsUSBOpen();
const char* TurnLEDOn();
const char* TurnLEDOff();
long SyncPulse();
void * ExecuteSyncPulse(void * argument);
const char* StimPulse(float durationMilliseconds, float freqHz, bool doRelay);

#endif
