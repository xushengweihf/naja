#ifndef __XSW_NONCOPYABLE_H__
#define __XSW_NONCOPYABLE_H__

namespace xsw {

class Noncopyable {
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
};

}

#endif
