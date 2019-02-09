#ifndef MESSAGEFUNCTIONS_H
#define MESSAGEFUNCTIONS_H

namespace StVirtualMachine {

extern void setupMessageFunctions();

extern void registerObjectH();
extern void unregisterObjectH();

extern void registerObjectW();
extern void unregisterObjectW();

extern void setCapture();
extern void releaseCapture();

extern void sendMessage();

}

#endif // MESSAGEFUNCTIONS_H
