#ifndef _LA_CONFFILE_H
#define _LA_CONFFILE_H

#include <QVariant>

namespace la {

/*
All preferences:

[desktop]
number_of_desktops: int [1-n]

[display]
pixels_per_dp: float [1.0 - n]

[keyboard]
delay_until_repeat: int [100-n] ms
key_repeat: [0-n] per/sec

[power]
show_extension_in_menu_bar: bool

 */

class Preferences : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Desktop* desktop READ desktop NOTIFY desktopChanged)
    Q_PROPERTY(Keyboard* keyboard READ keyboard NOTIFY keyboardChanged)
public:
    class Desktop;
    class Keyboard;
    class Display;
private:
    struct Impl;

    // display
    double m_pixels_per_dp;
    // desktop
    int m_number_of_desktops;
    void* m_locales;
    // keyboard
    // power
    bool m_show_extension_in_menu_bar;

    void *pImpl;
public:
    Preferences();
    ~Preferences();

    Impl& impl();

    /// \brief  Start inotify watch loop.
    int run_watch_loop();

    /// \brief  Stop inotify watch loop.
    void exit_loop();

    void sync_with_file();

    void read_conf_file();

    void write_conf_file();

    QVariantMap parse_conf_file(const QString& file_data);

    void set_preference(const char *category, const char *key, QVariant value);

    QVariant get_preference(const char *category, const char *key);

    //===============
    // Getters
    //===============
    double pixels_per_dp() const;

    //===============
    // Properties
    //===============
    Desktop* desktop() const;
    Keyboard* keyboard() const;

    int number_of_desktops() const;
signals:
    void desktopChanged();
    void keyboardChanged();
};




class Preferences::Desktop : public QObject {
    Q_OBJECT

    Q_PROPERTY(int numberOfDesktops READ numberOfDesktops WRITE setNumberOfDesktops NOTIFY numberOfDesktopsChanged)
private:
    int m_number_of_desktops;
public:
    Desktop(QObject *parent);
    ~Desktop();

    //=================
    // Properties
    //=================
    int numberOfDesktops() const;
    void setNumberOfDesktops(int val);
signals:
    void numberOfDesktopsChanged(int num);
};


class Preferences::Keyboard : public QObject {
    Q_OBJECT

    Q_PROPERTY(int delayUntilRepeat READ delayUntilRepeat WRITE setDelayUntilRepeat NOTIFY delayUntilRepeatChanged)
    Q_PROPERTY(int keyRepeat READ keyRepeat WRITE setKeyRepeat NOTIFY keyRepeatChanged)
private:
    int m_delay_until_repeat;
    int m_key_repeat;
public:
    Keyboard(QObject *parent);
    ~Keyboard();

    //==================
    // Properties
    //==================
    int delayUntilRepeat() const;
    void setDelayUntilRepeat(int val);

    int keyRepeat() const;
    void setKeyRepeat(int val);
signals:
    void delayUntilRepeatChanged(int delay);
    void keyRepeatChanged(int repeat);
};


class Preferences::Display : public QObject {
    Q_OBJECT

    Q_PROPERTY(qreal pixelsPerDp)
private:
    qreal m_pixels_per_dp;
public:
    Display(QObject *parent);
    ~Display();

    //==================
    // Properties
    //==================
    qreal pixelsPerDp() const;
    void setPixelsPerDp(qreal val);
signals:
    void pixelsPerDpChanged();
};

}

#endif // _LA_CONFFILE_H
