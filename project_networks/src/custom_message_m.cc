//
// Generated file, do not edit! Created by opp_msgtool 6.0 from custom_message.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "custom_message_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Custom_message_Base::Custom_message_Base(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

Custom_message_Base::Custom_message_Base(const Custom_message_Base& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

Custom_message_Base::~Custom_message_Base()
{
}

Custom_message_Base& Custom_message_Base::operator=(const Custom_message_Base& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void Custom_message_Base::copy(const Custom_message_Base& other)
{
    this->M_Header = other.M_Header;
    this->M_Payload = other.M_Payload;
    this->M_Trailer = other.M_Trailer;
    this->M_Type = other.M_Type;
    this->M_Ack_Num = other.M_Ack_Num;
}

void Custom_message_Base::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->M_Header);
    doParsimPacking(b,this->M_Payload);
    doParsimPacking(b,this->M_Trailer);
    doParsimPacking(b,this->M_Type);
    doParsimPacking(b,this->M_Ack_Num);
}

void Custom_message_Base::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->M_Header);
    doParsimUnpacking(b,this->M_Payload);
    doParsimUnpacking(b,this->M_Trailer);
    doParsimUnpacking(b,this->M_Type);
    doParsimUnpacking(b,this->M_Ack_Num);
}

int Custom_message_Base::getM_Header() const
{
    return this->M_Header;
}

void Custom_message_Base::setM_Header(int M_Header)
{
    this->M_Header = M_Header;
}

const char * Custom_message_Base::getM_Payload() const
{
    return this->M_Payload.c_str();
}

void Custom_message_Base::setM_Payload(const char * M_Payload)
{
    this->M_Payload = M_Payload;
}

int Custom_message_Base::getM_Trailer() const
{
    return this->M_Trailer;
}

void Custom_message_Base::setM_Trailer(int M_Trailer)
{
    this->M_Trailer = M_Trailer;
}

int Custom_message_Base::getM_Type() const
{
    return this->M_Type;
}

void Custom_message_Base::setM_Type(int M_Type)
{
    this->M_Type = M_Type;
}

int Custom_message_Base::getM_Ack_Num() const
{
    return this->M_Ack_Num;
}

void Custom_message_Base::setM_Ack_Num(int M_Ack_Num)
{
    this->M_Ack_Num = M_Ack_Num;
}

class Custom_messageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_M_Header,
        FIELD_M_Payload,
        FIELD_M_Trailer,
        FIELD_M_Type,
        FIELD_M_Ack_Num,
    };
  public:
    Custom_messageDescriptor();
    virtual ~Custom_messageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(Custom_messageDescriptor)

Custom_messageDescriptor::Custom_messageDescriptor() : omnetpp::cClassDescriptor("Custom_message", "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

Custom_messageDescriptor::~Custom_messageDescriptor()
{
    delete[] propertyNames;
}

bool Custom_messageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Custom_message_Base *>(obj)!=nullptr;
}

const char **Custom_messageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = { "customize",  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *Custom_messageDescriptor::getProperty(const char *propertyName) const
{
    if (!strcmp(propertyName, "customize")) return "true";
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int Custom_messageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 5+base->getFieldCount() : 5;
}

unsigned int Custom_messageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_M_Header
        FD_ISEDITABLE,    // FIELD_M_Payload
        FD_ISEDITABLE,    // FIELD_M_Trailer
        FD_ISEDITABLE,    // FIELD_M_Type
        FD_ISEDITABLE,    // FIELD_M_Ack_Num
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *Custom_messageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "M_Header",
        "M_Payload",
        "M_Trailer",
        "M_Type",
        "M_Ack_Num",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int Custom_messageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "M_Header") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "M_Payload") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "M_Trailer") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "M_Type") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "M_Ack_Num") == 0) return baseIndex + 4;
    return base ? base->findField(fieldName) : -1;
}

const char *Custom_messageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_M_Header
        "string",    // FIELD_M_Payload
        "int",    // FIELD_M_Trailer
        "int",    // FIELD_M_Type
        "int",    // FIELD_M_Ack_Num
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **Custom_messageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *Custom_messageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int Custom_messageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    Custom_message_Base *pp = omnetpp::fromAnyPtr<Custom_message_Base>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void Custom_messageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    Custom_message_Base *pp = omnetpp::fromAnyPtr<Custom_message_Base>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'Custom_message_Base'", field);
    }
}

const char *Custom_messageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    Custom_message_Base *pp = omnetpp::fromAnyPtr<Custom_message_Base>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string Custom_messageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    Custom_message_Base *pp = omnetpp::fromAnyPtr<Custom_message_Base>(object); (void)pp;
    switch (field) {
        case FIELD_M_Header: return long2string(pp->getM_Header());
        case FIELD_M_Payload: return oppstring2string(pp->getM_Payload());
        case FIELD_M_Trailer: return long2string(pp->getM_Trailer());
        case FIELD_M_Type: return long2string(pp->getM_Type());
        case FIELD_M_Ack_Num: return long2string(pp->getM_Ack_Num());
        default: return "";
    }
}

void Custom_messageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Custom_message_Base *pp = omnetpp::fromAnyPtr<Custom_message_Base>(object); (void)pp;
    switch (field) {
        case FIELD_M_Header: pp->setM_Header(string2long(value)); break;
        case FIELD_M_Payload: pp->setM_Payload((value)); break;
        case FIELD_M_Trailer: pp->setM_Trailer(string2long(value)); break;
        case FIELD_M_Type: pp->setM_Type(string2long(value)); break;
        case FIELD_M_Ack_Num: pp->setM_Ack_Num(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Custom_message_Base'", field);
    }
}

omnetpp::cValue Custom_messageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    Custom_message_Base *pp = omnetpp::fromAnyPtr<Custom_message_Base>(object); (void)pp;
    switch (field) {
        case FIELD_M_Header: return pp->getM_Header();
        case FIELD_M_Payload: return pp->getM_Payload();
        case FIELD_M_Trailer: return pp->getM_Trailer();
        case FIELD_M_Type: return pp->getM_Type();
        case FIELD_M_Ack_Num: return pp->getM_Ack_Num();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'Custom_message_Base' as cValue -- field index out of range?", field);
    }
}

void Custom_messageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Custom_message_Base *pp = omnetpp::fromAnyPtr<Custom_message_Base>(object); (void)pp;
    switch (field) {
        case FIELD_M_Header: pp->setM_Header(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_M_Payload: pp->setM_Payload(value.stringValue()); break;
        case FIELD_M_Trailer: pp->setM_Trailer(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_M_Type: pp->setM_Type(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_M_Ack_Num: pp->setM_Ack_Num(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Custom_message_Base'", field);
    }
}

const char *Custom_messageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr Custom_messageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    Custom_message_Base *pp = omnetpp::fromAnyPtr<Custom_message_Base>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void Custom_messageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    Custom_message_Base *pp = omnetpp::fromAnyPtr<Custom_message_Base>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Custom_message_Base'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

