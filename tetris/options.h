#ifndef OPTIONS_H
#define OPTIONS_H

class COptions
{
public:
    COptions();

    void Load();
    void Save();

    //main window layout
    int LayoutLeft;
    int LayoutTop;

    //keyboard timer
    enum : int
    {
        MIN_KEYBOARD_SPEED = 75,
        DEF_KEYBOARD_SPEED = 100,
        MAX_KEYBOARD_SPEED = 175
    };
    int KeySpeed{DEF_KEYBOARD_SPEED};
};

#endif // OPTIONS_H
