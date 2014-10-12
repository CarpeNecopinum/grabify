#pragma once

template <class T>
class Singleton
{
public:
    static T& the() {
        static T instance;
        return instance;
    }
};
