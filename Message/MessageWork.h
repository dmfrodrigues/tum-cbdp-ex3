#include "Message.h"

#include <vector>

class MessageWork : public Message {
public:
    std::vector<std::string> chunkURLs;
    ssize_t result = -1;
private:
    virtual void serializeContents(std::stringstream &ss) const;
    virtual bool deserializeContents(std::stringstream &ss);
};
