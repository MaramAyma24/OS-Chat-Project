
#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

/**
 * @struct Message
 * @brief هيكل يمثّل رسالة دردشة موحّدة تُستخدم في النظامين:
 *        - عبر الشبكة (Sockets)
 *        - عبر الذاكرة المشتركة (Shared Memory)
 */
struct Message {
    std::string sender;   ///< اسم المستخدم المرسِل
    std::string content;  ///< محتوى الرسالة
    double timestamp;     ///< وقت الإرسال (ثواني منذ 1 يناير 1970)
};

#endif // MESSAGE_H