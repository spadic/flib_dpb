//! \author Michael Krieger

#include "MicrosliceContents.hpp"

namespace flib_dpb {

//---- static implementation details ---------------------------------

static const size_t DESC_OFFSET {16 / sizeof(*DTM::data)};
static std::vector<DTM> _get_dtms(const uint16_t *data, size_t size);
static size_t _next_dtm(std::vector<DTM>& dtms, const uint16_t *data);

// extract all DTMs from a raw buffer in "packed DTM" format
std::vector<DTM> _get_dtms(const uint16_t *data, size_t size)
{
    data += DESC_OFFSET;
    size -= DESC_OFFSET;
    auto end = data + size;

    auto dtms = std::vector<DTM> {};
    while (data < end) {
        data += _next_dtm(dtms, data);
    }
    return dtms;
}

// extract a single DTM from a raw buffer in "packed DTM" format
size_t _next_dtm(std::vector<DTM>& dtms, const uint16_t *data)
{
    auto n = size_t {2};
    auto size = size_t {*data++ & 0xFFu};
    if (size) {
        auto addr = uint16_t {*data++};
        dtms.push_back({addr, data, size}); // `size` must not lie!
        n += size;
    }
    n += (~n + 1) % 4; // skip padding (n -> k*4)
    return n;
}

MicrosliceContents::MicrosliceContents(const uint16_t *data, size_t size)
: _dtms {_get_dtms(data, size)} {}

} // namespace
