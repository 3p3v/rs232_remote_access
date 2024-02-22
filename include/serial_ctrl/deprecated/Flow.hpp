#pragma once

class Flow
{
    size_t data{0};

public:
    size_t get() const;

    Flow &operator+=(unsigned int len);
};
