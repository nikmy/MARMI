#include "bd_request.h"

namespace server
{

BDRequest::BDRequest(const char* text, size_t id)
    : data_(text, id)
{ }

RData BDRequest::getData() const
{ return data_; }

}