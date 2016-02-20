#ifndef BULKERRPROPERTY
#define BULKERPROPERTY

namespace bulker {

    template < class T, class Object,
              T (Object::*real_getter)() const,
              T (Object::*real_setter)(const T&) >
    class Property{
        Object* my_object;
    public:
        void perator()(Object* obj) { my_object = obj; }
        T operator() () const
        {  return (my_object->*real_getter)(); }
        T operator() (const T& value)
        {  return (my_object->*real_setter)(value); }

        T get() const
        { return (my_object->*real_getter)(); }
        T set() const
        { return (my_object->*real_setter)(value); }

        operator T() const
        { return (my_object->*real_getter)(); }
        T operator=(const T& value)
        { return (my_object->*real_setter)(value); }
        typedef T value_type;
    };

}

#endif // BULKERPROPERTY

