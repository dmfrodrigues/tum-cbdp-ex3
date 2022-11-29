#include "MessageFactory.h"

using namespace std;

Message* MessageFactory::factoryMethod(const string &buf) const {
    stringstream ss(buf);
    
    Message::Type type;
    ss.read(reinterpret_cast<char*>(&type), sizeof(type));

    Message::Operation operation;
    ss.read(reinterpret_cast<char*>(&operation), sizeof(operation));

    Message *m = nullptr;

    switch(operation){
        case Message::Operation::HEARTBEAT: m = new MessageHeartbeat; break;
        case Message::Operation::WORK     : m = new MessageWork; break;
        default: throw logic_error("Unknown operation: " + to_string(static_cast<uint8_t>(operation)));
    }

    m->deserializeContents(ss);
    return m;
}
