// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: PowerMonitor.proto

#ifndef PROTOBUF_INCLUDED_PowerMonitor_2eproto
#define PROTOBUF_INCLUDED_PowerMonitor_2eproto

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include <google/protobuf/timestamp.pb.h>
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_PowerMonitor_2eproto 

namespace protobuf_PowerMonitor_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[3];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
}  // namespace protobuf_PowerMonitor_2eproto
class ControlData;
class ControlDataDefaultTypeInternal;
extern ControlDataDefaultTypeInternal _ControlData_default_instance_;
class PowerData;
class PowerDataDefaultTypeInternal;
extern PowerDataDefaultTypeInternal _PowerData_default_instance_;
class PowerData_PowerChannelData;
class PowerData_PowerChannelDataDefaultTypeInternal;
extern PowerData_PowerChannelDataDefaultTypeInternal _PowerData_PowerChannelData_default_instance_;
namespace google {
namespace protobuf {
template<> ::ControlData* Arena::CreateMaybeMessage<::ControlData>(Arena*);
template<> ::PowerData* Arena::CreateMaybeMessage<::PowerData>(Arena*);
template<> ::PowerData_PowerChannelData* Arena::CreateMaybeMessage<::PowerData_PowerChannelData>(Arena*);
}  // namespace protobuf
}  // namespace google

// ===================================================================

class PowerData_PowerChannelData : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:PowerData.PowerChannelData) */ {
 public:
  PowerData_PowerChannelData();
  virtual ~PowerData_PowerChannelData();

  PowerData_PowerChannelData(const PowerData_PowerChannelData& from);

  inline PowerData_PowerChannelData& operator=(const PowerData_PowerChannelData& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  PowerData_PowerChannelData(PowerData_PowerChannelData&& from) noexcept
    : PowerData_PowerChannelData() {
    *this = ::std::move(from);
  }

  inline PowerData_PowerChannelData& operator=(PowerData_PowerChannelData&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const PowerData_PowerChannelData& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const PowerData_PowerChannelData* internal_default_instance() {
    return reinterpret_cast<const PowerData_PowerChannelData*>(
               &_PowerData_PowerChannelData_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(PowerData_PowerChannelData* other);
  friend void swap(PowerData_PowerChannelData& a, PowerData_PowerChannelData& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline PowerData_PowerChannelData* New() const final {
    return CreateMaybeMessage<PowerData_PowerChannelData>(NULL);
  }

  PowerData_PowerChannelData* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<PowerData_PowerChannelData>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const PowerData_PowerChannelData& from);
  void MergeFrom(const PowerData_PowerChannelData& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(PowerData_PowerChannelData* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string name = 1;
  void clear_name();
  static const int kNameFieldNumber = 1;
  const ::std::string& name() const;
  void set_name(const ::std::string& value);
  #if LANG_CXX11
  void set_name(::std::string&& value);
  #endif
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  ::std::string* mutable_name();
  ::std::string* release_name();
  void set_allocated_name(::std::string* name);

  // double bus_voltage = 2;
  void clear_bus_voltage();
  static const int kBusVoltageFieldNumber = 2;
  double bus_voltage() const;
  void set_bus_voltage(double value);

  // double shunt_voltage = 3;
  void clear_shunt_voltage();
  static const int kShuntVoltageFieldNumber = 3;
  double shunt_voltage() const;
  void set_shunt_voltage(double value);

  // double current = 4;
  void clear_current();
  static const int kCurrentFieldNumber = 4;
  double current() const;
  void set_current(double value);

  // double power = 5;
  void clear_power();
  static const int kPowerFieldNumber = 5;
  double power() const;
  void set_power(double value);

  // @@protoc_insertion_point(class_scope:PowerData.PowerChannelData)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr name_;
  double bus_voltage_;
  double shunt_voltage_;
  double current_;
  double power_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_PowerMonitor_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class PowerData : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:PowerData) */ {
 public:
  PowerData();
  virtual ~PowerData();

  PowerData(const PowerData& from);

  inline PowerData& operator=(const PowerData& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  PowerData(PowerData&& from) noexcept
    : PowerData() {
    *this = ::std::move(from);
  }

  inline PowerData& operator=(PowerData&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const PowerData& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const PowerData* internal_default_instance() {
    return reinterpret_cast<const PowerData*>(
               &_PowerData_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  void Swap(PowerData* other);
  friend void swap(PowerData& a, PowerData& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline PowerData* New() const final {
    return CreateMaybeMessage<PowerData>(NULL);
  }

  PowerData* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<PowerData>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const PowerData& from);
  void MergeFrom(const PowerData& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(PowerData* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  typedef PowerData_PowerChannelData PowerChannelData;

  // accessors -------------------------------------------------------

  // repeated .PowerData.PowerChannelData channels = 1;
  int channels_size() const;
  void clear_channels();
  static const int kChannelsFieldNumber = 1;
  ::PowerData_PowerChannelData* mutable_channels(int index);
  ::google::protobuf::RepeatedPtrField< ::PowerData_PowerChannelData >*
      mutable_channels();
  const ::PowerData_PowerChannelData& channels(int index) const;
  ::PowerData_PowerChannelData* add_channels();
  const ::google::protobuf::RepeatedPtrField< ::PowerData_PowerChannelData >&
      channels() const;

  // .google.protobuf.Timestamp timestamp = 2;
  bool has_timestamp() const;
  void clear_timestamp();
  static const int kTimestampFieldNumber = 2;
  private:
  const ::google::protobuf::Timestamp& _internal_timestamp() const;
  public:
  const ::google::protobuf::Timestamp& timestamp() const;
  ::google::protobuf::Timestamp* release_timestamp();
  ::google::protobuf::Timestamp* mutable_timestamp();
  void set_allocated_timestamp(::google::protobuf::Timestamp* timestamp);

  // @@protoc_insertion_point(class_scope:PowerData)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedPtrField< ::PowerData_PowerChannelData > channels_;
  ::google::protobuf::Timestamp* timestamp_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_PowerMonitor_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class ControlData : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:ControlData) */ {
 public:
  ControlData();
  virtual ~ControlData();

  ControlData(const ControlData& from);

  inline ControlData& operator=(const ControlData& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ControlData(ControlData&& from) noexcept
    : ControlData() {
    *this = ::std::move(from);
  }

  inline ControlData& operator=(ControlData&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const ControlData& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ControlData* internal_default_instance() {
    return reinterpret_cast<const ControlData*>(
               &_ControlData_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  void Swap(ControlData* other);
  friend void swap(ControlData& a, ControlData& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ControlData* New() const final {
    return CreateMaybeMessage<ControlData>(NULL);
  }

  ControlData* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<ControlData>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const ControlData& from);
  void MergeFrom(const ControlData& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ControlData* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string app_name = 1;
  void clear_app_name();
  static const int kAppNameFieldNumber = 1;
  const ::std::string& app_name() const;
  void set_app_name(const ::std::string& value);
  #if LANG_CXX11
  void set_app_name(::std::string&& value);
  #endif
  void set_app_name(const char* value);
  void set_app_name(const char* value, size_t size);
  ::std::string* mutable_app_name();
  ::std::string* release_app_name();
  void set_allocated_app_name(::std::string* app_name);

  // bool start = 2;
  void clear_start();
  static const int kStartFieldNumber = 2;
  bool start() const;
  void set_start(bool value);

  // bool stop = 3;
  void clear_stop();
  static const int kStopFieldNumber = 3;
  bool stop() const;
  void set_stop(bool value);

  // @@protoc_insertion_point(class_scope:ControlData)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr app_name_;
  bool start_;
  bool stop_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_PowerMonitor_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// PowerData_PowerChannelData

// string name = 1;
inline void PowerData_PowerChannelData::clear_name() {
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& PowerData_PowerChannelData::name() const {
  // @@protoc_insertion_point(field_get:PowerData.PowerChannelData.name)
  return name_.GetNoArena();
}
inline void PowerData_PowerChannelData::set_name(const ::std::string& value) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:PowerData.PowerChannelData.name)
}
#if LANG_CXX11
inline void PowerData_PowerChannelData::set_name(::std::string&& value) {
  
  name_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:PowerData.PowerChannelData.name)
}
#endif
inline void PowerData_PowerChannelData::set_name(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:PowerData.PowerChannelData.name)
}
inline void PowerData_PowerChannelData::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:PowerData.PowerChannelData.name)
}
inline ::std::string* PowerData_PowerChannelData::mutable_name() {
  
  // @@protoc_insertion_point(field_mutable:PowerData.PowerChannelData.name)
  return name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* PowerData_PowerChannelData::release_name() {
  // @@protoc_insertion_point(field_release:PowerData.PowerChannelData.name)
  
  return name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void PowerData_PowerChannelData::set_allocated_name(::std::string* name) {
  if (name != NULL) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:PowerData.PowerChannelData.name)
}

// double bus_voltage = 2;
inline void PowerData_PowerChannelData::clear_bus_voltage() {
  bus_voltage_ = 0;
}
inline double PowerData_PowerChannelData::bus_voltage() const {
  // @@protoc_insertion_point(field_get:PowerData.PowerChannelData.bus_voltage)
  return bus_voltage_;
}
inline void PowerData_PowerChannelData::set_bus_voltage(double value) {
  
  bus_voltage_ = value;
  // @@protoc_insertion_point(field_set:PowerData.PowerChannelData.bus_voltage)
}

// double shunt_voltage = 3;
inline void PowerData_PowerChannelData::clear_shunt_voltage() {
  shunt_voltage_ = 0;
}
inline double PowerData_PowerChannelData::shunt_voltage() const {
  // @@protoc_insertion_point(field_get:PowerData.PowerChannelData.shunt_voltage)
  return shunt_voltage_;
}
inline void PowerData_PowerChannelData::set_shunt_voltage(double value) {
  
  shunt_voltage_ = value;
  // @@protoc_insertion_point(field_set:PowerData.PowerChannelData.shunt_voltage)
}

// double current = 4;
inline void PowerData_PowerChannelData::clear_current() {
  current_ = 0;
}
inline double PowerData_PowerChannelData::current() const {
  // @@protoc_insertion_point(field_get:PowerData.PowerChannelData.current)
  return current_;
}
inline void PowerData_PowerChannelData::set_current(double value) {
  
  current_ = value;
  // @@protoc_insertion_point(field_set:PowerData.PowerChannelData.current)
}

// double power = 5;
inline void PowerData_PowerChannelData::clear_power() {
  power_ = 0;
}
inline double PowerData_PowerChannelData::power() const {
  // @@protoc_insertion_point(field_get:PowerData.PowerChannelData.power)
  return power_;
}
inline void PowerData_PowerChannelData::set_power(double value) {
  
  power_ = value;
  // @@protoc_insertion_point(field_set:PowerData.PowerChannelData.power)
}

// -------------------------------------------------------------------

// PowerData

// repeated .PowerData.PowerChannelData channels = 1;
inline int PowerData::channels_size() const {
  return channels_.size();
}
inline void PowerData::clear_channels() {
  channels_.Clear();
}
inline ::PowerData_PowerChannelData* PowerData::mutable_channels(int index) {
  // @@protoc_insertion_point(field_mutable:PowerData.channels)
  return channels_.Mutable(index);
}
inline ::google::protobuf::RepeatedPtrField< ::PowerData_PowerChannelData >*
PowerData::mutable_channels() {
  // @@protoc_insertion_point(field_mutable_list:PowerData.channels)
  return &channels_;
}
inline const ::PowerData_PowerChannelData& PowerData::channels(int index) const {
  // @@protoc_insertion_point(field_get:PowerData.channels)
  return channels_.Get(index);
}
inline ::PowerData_PowerChannelData* PowerData::add_channels() {
  // @@protoc_insertion_point(field_add:PowerData.channels)
  return channels_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::PowerData_PowerChannelData >&
PowerData::channels() const {
  // @@protoc_insertion_point(field_list:PowerData.channels)
  return channels_;
}

// .google.protobuf.Timestamp timestamp = 2;
inline bool PowerData::has_timestamp() const {
  return this != internal_default_instance() && timestamp_ != NULL;
}
inline const ::google::protobuf::Timestamp& PowerData::_internal_timestamp() const {
  return *timestamp_;
}
inline const ::google::protobuf::Timestamp& PowerData::timestamp() const {
  const ::google::protobuf::Timestamp* p = timestamp_;
  // @@protoc_insertion_point(field_get:PowerData.timestamp)
  return p != NULL ? *p : *reinterpret_cast<const ::google::protobuf::Timestamp*>(
      &::google::protobuf::_Timestamp_default_instance_);
}
inline ::google::protobuf::Timestamp* PowerData::release_timestamp() {
  // @@protoc_insertion_point(field_release:PowerData.timestamp)
  
  ::google::protobuf::Timestamp* temp = timestamp_;
  timestamp_ = NULL;
  return temp;
}
inline ::google::protobuf::Timestamp* PowerData::mutable_timestamp() {
  
  if (timestamp_ == NULL) {
    auto* p = CreateMaybeMessage<::google::protobuf::Timestamp>(GetArenaNoVirtual());
    timestamp_ = p;
  }
  // @@protoc_insertion_point(field_mutable:PowerData.timestamp)
  return timestamp_;
}
inline void PowerData::set_allocated_timestamp(::google::protobuf::Timestamp* timestamp) {
  ::google::protobuf::Arena* message_arena = GetArenaNoVirtual();
  if (message_arena == NULL) {
    delete reinterpret_cast< ::google::protobuf::MessageLite*>(timestamp_);
  }
  if (timestamp) {
    ::google::protobuf::Arena* submessage_arena =
      reinterpret_cast<::google::protobuf::MessageLite*>(timestamp)->GetArena();
    if (message_arena != submessage_arena) {
      timestamp = ::google::protobuf::internal::GetOwnedMessage(
          message_arena, timestamp, submessage_arena);
    }
    
  } else {
    
  }
  timestamp_ = timestamp;
  // @@protoc_insertion_point(field_set_allocated:PowerData.timestamp)
}

// -------------------------------------------------------------------

// ControlData

// string app_name = 1;
inline void ControlData::clear_app_name() {
  app_name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& ControlData::app_name() const {
  // @@protoc_insertion_point(field_get:ControlData.app_name)
  return app_name_.GetNoArena();
}
inline void ControlData::set_app_name(const ::std::string& value) {
  
  app_name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:ControlData.app_name)
}
#if LANG_CXX11
inline void ControlData::set_app_name(::std::string&& value) {
  
  app_name_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:ControlData.app_name)
}
#endif
inline void ControlData::set_app_name(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  app_name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:ControlData.app_name)
}
inline void ControlData::set_app_name(const char* value, size_t size) {
  
  app_name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:ControlData.app_name)
}
inline ::std::string* ControlData::mutable_app_name() {
  
  // @@protoc_insertion_point(field_mutable:ControlData.app_name)
  return app_name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* ControlData::release_app_name() {
  // @@protoc_insertion_point(field_release:ControlData.app_name)
  
  return app_name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void ControlData::set_allocated_app_name(::std::string* app_name) {
  if (app_name != NULL) {
    
  } else {
    
  }
  app_name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), app_name);
  // @@protoc_insertion_point(field_set_allocated:ControlData.app_name)
}

// bool start = 2;
inline void ControlData::clear_start() {
  start_ = false;
}
inline bool ControlData::start() const {
  // @@protoc_insertion_point(field_get:ControlData.start)
  return start_;
}
inline void ControlData::set_start(bool value) {
  
  start_ = value;
  // @@protoc_insertion_point(field_set:ControlData.start)
}

// bool stop = 3;
inline void ControlData::clear_stop() {
  stop_ = false;
}
inline bool ControlData::stop() const {
  // @@protoc_insertion_point(field_get:ControlData.stop)
  return stop_;
}
inline void ControlData::set_stop(bool value) {
  
  stop_ = value;
  // @@protoc_insertion_point(field_set:ControlData.stop)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_INCLUDED_PowerMonitor_2eproto