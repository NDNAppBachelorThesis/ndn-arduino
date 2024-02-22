//
// Created by arne- on 21.11.2023.
//

#ifndef NDN_ARDUINO_LOGBUFFER_H
#define NDN_ARDUINO_LOGBUFFER_H

#include <string>
#include <queue>

/**
 * Buffers the latest X log entries for later access
 */
class LogBuffer : public std::queue<std::string> {
public:
    typedef typename std::deque<std::string>::iterator iterator;
    typedef typename std::deque<std::string>::const_iterator const_iterator;

    explicit LogBuffer(size_t limit) : limit(limit), std::queue<std::string>(std::deque<std::string>(limit)) {}

    /**
     * Add a new const log line
     */
    void push(const value_type &x);
    /**
     * Add a new log line
     */
    void push(value_type &&x);

    iterator begin() { return this->c.begin(); }
    iterator end() { return this->c.end(); }
    const_iterator begin() const { return this->c.begin(); }
    const_iterator end() const { return this->c.end(); }

private:
    size_t limit;
};


#endif //NDN_ARDUINO_LOGBUFFER_H
