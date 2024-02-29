// Generated by Cap'n Proto compiler, DO NOT EDIT
// source: ndarray.capnp

#pragma once

#include <capnp/generated-header-support.h>
#include <kj/windows-sanity.h>

#ifndef CAPNP_VERSION
#error "CAPNP_VERSION is not defined, is capnp/generated-header-support.h missing?"
#elif CAPNP_VERSION != 1000002
#error "Version mismatch between generated code and library headers.  You must use the same version of the Cap'n Proto compiler and library."
#endif


CAPNP_BEGIN_HEADER

namespace capnp {
namespace schemas {

CAPNP_DECLARE_SCHEMA(b5d9915c41fd9ac2);
CAPNP_DECLARE_SCHEMA(a5083459dbf2dc0e);
enum class DType_a5083459dbf2dc0e: uint16_t {
  BOOL,
  UINT8,
  UINT16,
  UINT32,
  UINT64,
  INT8,
  INT16,
  INT32,
  INT64,
  FLOAT32,
  FLOAT64,
  FLOAT128,
};
CAPNP_DECLARE_ENUM(DType, a5083459dbf2dc0e);

}  // namespace schemas
}  // namespace capnp


struct NDArray {
  NDArray() = delete;

  class Reader;
  class Builder;
  class Pipeline;
  typedef ::capnp::schemas::DType_a5083459dbf2dc0e DType;


  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(b5d9915c41fd9ac2, 2, 2)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

// =======================================================================================

class NDArray::Reader {
public:
  typedef NDArray Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

  inline bool hasShape() const;
  inline  ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>::Reader getShape() const;

  inline  ::NDArray::DType getDtype() const;

  inline bool hasData() const;
  inline  ::capnp::Data::Reader getData() const;

  inline  ::uint64_t getTimestamp() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class NDArray::Builder {
public:
  typedef NDArray Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline bool hasShape();
  inline  ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>::Builder getShape();
  inline void setShape( ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>::Reader value);
  inline void setShape(::kj::ArrayPtr<const  ::uint32_t> value);
  inline  ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>::Builder initShape(unsigned int size);
  inline void adoptShape(::capnp::Orphan< ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>>&& value);
  inline ::capnp::Orphan< ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>> disownShape();

  inline  ::NDArray::DType getDtype();
  inline void setDtype( ::NDArray::DType value);

  inline bool hasData();
  inline  ::capnp::Data::Builder getData();
  inline void setData( ::capnp::Data::Reader value);
  inline  ::capnp::Data::Builder initData(unsigned int size);
  inline void adoptData(::capnp::Orphan< ::capnp::Data>&& value);
  inline ::capnp::Orphan< ::capnp::Data> disownData();

  inline  ::uint64_t getTimestamp();
  inline void setTimestamp( ::uint64_t value);

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class NDArray::Pipeline {
public:
  typedef NDArray Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

// =======================================================================================

inline bool NDArray::Reader::hasShape() const {
  return !_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline bool NDArray::Builder::hasShape() {
  return !_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline  ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>::Reader NDArray::Reader::getShape() const {
  return ::capnp::_::PointerHelpers< ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>>::get(_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>::Builder NDArray::Builder::getShape() {
  return ::capnp::_::PointerHelpers< ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>>::get(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline void NDArray::Builder::setShape( ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), value);
}
inline void NDArray::Builder::setShape(::kj::ArrayPtr<const  ::uint32_t> value) {
  ::capnp::_::PointerHelpers< ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), value);
}
inline  ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>::Builder NDArray::Builder::initShape(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>>::init(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), size);
}
inline void NDArray::Builder::adoptShape(
    ::capnp::Orphan< ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>>::adopt(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>> NDArray::Builder::disownShape() {
  return ::capnp::_::PointerHelpers< ::capnp::List< ::uint32_t,  ::capnp::Kind::PRIMITIVE>>::disown(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}

inline  ::NDArray::DType NDArray::Reader::getDtype() const {
  return _reader.getDataField< ::NDArray::DType>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}

inline  ::NDArray::DType NDArray::Builder::getDtype() {
  return _builder.getDataField< ::NDArray::DType>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}
inline void NDArray::Builder::setDtype( ::NDArray::DType value) {
  _builder.setDataField< ::NDArray::DType>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, value);
}

inline bool NDArray::Reader::hasData() const {
  return !_reader.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS).isNull();
}
inline bool NDArray::Builder::hasData() {
  return !_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS).isNull();
}
inline  ::capnp::Data::Reader NDArray::Reader::getData() const {
  return ::capnp::_::PointerHelpers< ::capnp::Data>::get(_reader.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS));
}
inline  ::capnp::Data::Builder NDArray::Builder::getData() {
  return ::capnp::_::PointerHelpers< ::capnp::Data>::get(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS));
}
inline void NDArray::Builder::setData( ::capnp::Data::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::Data>::set(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS), value);
}
inline  ::capnp::Data::Builder NDArray::Builder::initData(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::Data>::init(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS), size);
}
inline void NDArray::Builder::adoptData(
    ::capnp::Orphan< ::capnp::Data>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::Data>::adopt(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::Data> NDArray::Builder::disownData() {
  return ::capnp::_::PointerHelpers< ::capnp::Data>::disown(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS));
}

inline  ::uint64_t NDArray::Reader::getTimestamp() const {
  return _reader.getDataField< ::uint64_t>(
      ::capnp::bounded<1>() * ::capnp::ELEMENTS);
}

inline  ::uint64_t NDArray::Builder::getTimestamp() {
  return _builder.getDataField< ::uint64_t>(
      ::capnp::bounded<1>() * ::capnp::ELEMENTS);
}
inline void NDArray::Builder::setTimestamp( ::uint64_t value) {
  _builder.setDataField< ::uint64_t>(
      ::capnp::bounded<1>() * ::capnp::ELEMENTS, value);
}


CAPNP_END_HEADER
