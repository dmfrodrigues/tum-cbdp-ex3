#include "MessageHeartbeat.h"

using namespace std;

void MessageHeartbeat::serializeContents(stringstream &) const {
}

bool MessageHeartbeat::deserializeContents(stringstream &ss){
    return ss.eof();
}
