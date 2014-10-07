#pragma once

template <class T>
class Singleton
{
public:
    T& the() {
        static T;
        return T;
    }
};
