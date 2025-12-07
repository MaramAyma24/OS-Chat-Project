// src/core/message.h
#ifndef MESSAGE_H
#define MESSAGE_H
#include <string>


struct Message {
    std::string sender;
    std::string content;
    double timestamp; // or time_t
};
#endif