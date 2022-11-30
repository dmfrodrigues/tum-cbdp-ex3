#include "Message.h"

#include <iostream>

using namespace std;

Message::Message(Message::Type t, Message::Operation op) :
    type(t),
    operation(op)
{}

string Message::serialize() const {
    stringstream ss;
    
    ss.write(reinterpret_cast<const char*>(&type), sizeof(type));
    ss.write(reinterpret_cast<const char*>(&operation), sizeof(operation));
    serializeContents(ss);
    return ss.str();
}

bool Message::deserialize(const string &buf){
    stringstream ss(buf);
    ss.read(reinterpret_cast<char*>(&type), sizeof(type));
    ss.read(reinterpret_cast<char*>(&operation), sizeof(operation));
    return deserializeContents(ss);
}

Message::~Message(){}
