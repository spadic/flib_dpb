//! \author Michael Krieger

#include "MicrosliceContents.hpp"

namespace flib_dpb {

//---- static implementation details ---------------------------------

static const size_t DESC_OFFSET {16 / sizeof(*DTM::data)};
static std::vector<DTM> _get_dtms(const uint16_t *data, size_t size);
static size_t _next_dtm(std::vector<DTM>& dtms, const uint16_t *data);
static size_t _num_padding(size_t length, size_t mult=4);

// extract all DTMs from a raw buffer in "packed DTM" format
std::vector<DTM> _get_dtms(const uint16_t *data, size_t size)
{
    if (size < DESC_OFFSET) { return {}; }

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
    n += _num_padding(n, 4); // skip padding (n -> k*4)
    return n;
}

// calculate number of padding words
// length + _num_padding(length, mult) will be a multiple of mult
size_t _num_padding(size_t length, size_t mult)
{
    return (~length + 1) % mult;
}

MicrosliceContents::MicrosliceContents(const uint16_t *data, size_t size)
: _dtms {_get_dtms(data, size)} {}

} // namespace
