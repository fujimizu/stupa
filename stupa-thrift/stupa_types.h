/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 */
#ifndef stupa_TYPES_H
#define stupa_TYPES_H

#include <Thrift.h>
#include <protocol/TProtocol.h>
#include <transport/TTransport.h>



namespace stupa { namespace thrift {

class SearchResult {
 public:

  static const char* ascii_fingerprint; // = "C712EF0DA8599E55DF4D0F13415232EF";
  static const uint8_t binary_fingerprint[16]; // = {0xC7,0x12,0xEF,0x0D,0xA8,0x59,0x9E,0x55,0xDF,0x4D,0x0F,0x13,0x41,0x52,0x32,0xEF};

  SearchResult() : name(""), point(0) {
  }

  virtual ~SearchResult() throw() {}

  std::string name;
  double point;

  struct __isset {
    __isset() : name(false), point(false) {}
    bool name;
    bool point;
  } __isset;

  bool operator == (const SearchResult & rhs) const
  {
    if (!(name == rhs.name))
      return false;
    if (!(point == rhs.point))
      return false;
    return true;
  }
  bool operator != (const SearchResult &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const SearchResult & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

}} // namespace

#endif
