#ifndef _ANY_H_
#define _ANY_H_

#include <typeinfo>
#include <memory>
#include <type_traits>
#include <vector>
#include <cstddef>
#include <initializer_list>
#include <array>
#include <numeric>
#include <algorithm>
#include <exception>


namespace buler{


class Any{
protected:

  class any_base{
    public:
      virtual ~any_base(){}
      virtual const std::type_info & type(void) const = 0;
      virtual any_base *clone(void) const = 0;
  };

  template <typename T> class any_object : public any_base {
  public:
    any_object(const T &val) : m_object(val){}
    any_object(T&& val) : m_object(std::forward<T>(val)){}

    any_object() = default;
    any_object(any_object&&) = default;
    any_object& operator=(any_object&&) = default;
    any_object(const any_object&) = default;
    any_object& operator=(const any_object&) = default;
    virtual ~any_object() {}
    virtual const std::type_info & type(void) const { return typeid(T); }
    virtual any_base* clone(void) const { return new any_object(m_object); }

    T & value() { return m_object; }
  private:
    T m_object;
  };

public:
  Any()
  :any_null_error(std::make_exception_ptr(std::runtime_error("any_null_reference"))),
  any_badcast_error(std::make_exception_ptr(std::runtime_error("any_badcast"))){}

  Any(Any&&) = delete;
  Any& operator=(Any&&) = delete;

  Any(const Any &value)
  {
    if(bool(value.m_any))
    {
      m_any.reset(value.m_any->clone());
    }
  }

  template <typename T> Any(const T & val) : m_any(std::make_unique<T>(val)){}

  Any & operator = (const Any &val)
  {
    if(this != &val){
      if(bool(m_any)) {m_any.reset(nullptr);}
      if(bool(val.m_any)) { m_any.reset(val.m_any->clone()); }
    }
    return *this;
  }

  template <typename T> void set(const T &val)
  {
    if(bool(m_any)) { m_any.reset(nullptr); }
    m_any = std::make_unique<T>(val);
  }

  void swap(Any &val)
  {
    m_any.swap(val.m_any);
  }

  template<typename T> T & cast(void) const
  {
    if(bool(m_any)) std::rethrow_exception(any_null_error);
    if(m_any->type() != typeid(T)) std::rethrow_exception(any_badcast_error);
    return static_cast<any_object<T> *>(m_any)->value();
  }

  template<typename T> T & unsafecast(void) const
  {
    if(bool(m_any)) std::rethrow_exception(any_null_error);
    return static_cast<any_object<T> *>(m_any)->value();
  }

  const std::type_info & type() const
  {
    return bool(m_any) ? m_any->type() : typeid(void);
  }

  const char *name() const
  {
    return type().name();
  }

  bool empty() const {
    return bool(m_any);
  }
private:
  std::exception_ptr any_null_error;
  std::exception_ptr any_badcast_error;

  std::unique_ptr<any_base> m_any;
};

}

#endif
