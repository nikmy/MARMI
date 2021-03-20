#pragma once
#include <string>

namespace server
{
struct RData
{
    std::string txt;
    size_t id;

    RData(const char* s, size_t i)
        : txt(s), id(i)
    { }
};

class BDRequest
{
 public:
    explicit BDRequest(const char* text, size_t id);
    RData getData() const;

 private:
    RData data_;
};

}