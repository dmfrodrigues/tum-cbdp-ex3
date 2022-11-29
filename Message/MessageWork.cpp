#include "MessageWork.h"

using namespace std;

void MessageWork::serializeContents(stringstream &ss) const {
    const size_t &numberChunks = chunkURLs.size();
    ss.write(reinterpret_cast<const char*>(&numberChunks), sizeof(numberChunks));
    for(size_t i = 0; i < numberChunks; ++i){
        const string &chunkURL = chunkURLs[i];
        const size_t &sizeURL = chunkURL.size();
        ss.write(reinterpret_cast<const char*>(&sizeURL), sizeof(sizeURL));
        ss.write(chunkURL.c_str(), sizeURL);
    }
    ss.write(reinterpret_cast<const char*>(&result), sizeof(result));
}

bool MessageWork::deserializeContents(stringstream &ss){
    size_t numberChunks;
    ss.read(reinterpret_cast<char*>(&numberChunks), sizeof(numberChunks));
    chunkURLs.resize(numberChunks);

    for(size_t i = 0; i < numberChunks; ++i){
        size_t sizeURL;
        if(!ss.read(reinterpret_cast<char*>(&sizeURL), sizeof(sizeURL))) return false;
        char buf[Message::MAX_SIZE];
        if(!ss.read(buf, sizeURL)) return false;
        buf[sizeURL] = '\0';
        chunkURLs[i] = string(buf);
    }
    if(!ss.read(reinterpret_cast<char*>(&result), sizeof(result))) return false;

    return ss.eof();
}
