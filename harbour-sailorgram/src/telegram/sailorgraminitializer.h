#ifndef SAILORGRAMINITIALIZER_H
#define SAILORGRAMINITIALIZER_H

class SailorgramInitializer
{
    private:
        SailorgramInitializer();

    public:
        static void init(const char *uri, int major, int minor);
};

#endif // SAILORGRAMINITIALIZER_H
