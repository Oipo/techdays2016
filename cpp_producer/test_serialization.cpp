#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>

#include "message.h"

int main() {
    // This is just used to test serialization.
    Message *msg1 = new Message("Thisis", "a", "test");
    Message *msg2 = new Message();
    std::stringstream stream;
    msg1->Serialize(stream);
    msg2->Deserialize(stream);

    if(msg2->Title != "Thisis") {
        std::cerr << "Title wrong: " << msg2->Title << std::endl;
        exit(1);
    }

    if(msg2->Body != "a") {
        std::cerr << "Body wrong: " << msg2->Body << std::endl;
        exit(1);
    }

    if(msg2->DeviceId != "test") {
        std::cerr << "DeviceId wrong: " << msg2->DeviceId << std::endl;
        exit(1);
    }

    delete msg1;
    delete msg2;

    return 0;
}
