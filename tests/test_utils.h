#ifndef SCOTTZ0R_TEMPERATURE_TESTS_UTILS
#define SCOTTZ0R_TEMPERATURE_TESTS_UTILS

template <class T> class DoAlways
{
public:
    DoAlways(T action) : m_action(action)
    {
    }

    ~DoAlways()
    {
        m_action();
    }

private:
    T m_action;
};

template <class T> DoAlways<T> make_always(T val)
{
    return DoAlways<T>(val);
}

#endif // SCOTTZ0R_TEMPERATURE_TESTS_UTILS
