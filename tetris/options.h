#ifndef OPTIONS_H
#define OPTIONS_H

#include <map>
#include <QDate>

class CBestScore
{
public:
    using const_iterator = std::map<int, QDate>::const_reverse_iterator;

    static constexpr size_t MAX_RESULTS = 10;

    const_iterator beign() const {return Results.crbegin();}
    const_iterator end() const {return Results.crend();}

    bool add(int result);
    bool add(int result, const QDate& date);

private:
    std::map<int, QDate> Results;
};

class COptions
{
public:
    COptions();

    void Load();
    void Save();

    //main window layout
    int LayoutLeft;
    int LayoutTop;

    bool ShowGrid{true};

    //keyboard timer
    enum : int
    {
        MIN_KEYBOARD_SPEED = 75,
        DEF_KEYBOARD_SPEED = 100,
        MAX_KEYBOARD_SPEED = 175
    };
    int KeySpeed{DEF_KEYBOARD_SPEED};

    CBestScore BestResults;
};

extern COptions Options;

#endif // OPTIONS_H
