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
    class Localization;
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

    void sync_with_file();

    bool read_conf_file();

    bool make_conf_file();

    void write_conf_file();

    QVariantMap parse_conf_file(const QString& file_data);

    //===============
    // Properties
    //===============
    Desktop* desktop() const;
    Keyboard* keyboard() const;

    int number_of_desktops() const;
signals:
    void changed();

    void desktopChanged();
    void keyboardChanged();

public slots:
    void diff();
};




class Preferences::Desktop : public QObject {
    Q_OBJECT

    Q_PROPERTY(int numberOfDesktops READ numberOfDesktops WRITE setNumberOfDesktops NOTIFY numberOfDesktopsChanged)
    Q_PROPERTY(QString wallpaper READ wallpaper WRITE setWallpaper NOTIFY wallpaperChanged)
public:
    Desktop(QObject *parent);
    ~Desktop();

    //=================
    // Properties
    //=================
    int numberOfDesktops() const;
    void setNumberOfDesktops(int val);

    QString wallpaper() const;
    void setWallpaper(const QString& path);

signals:
    void numberOfDesktopsChanged(int num);
    void wallpaperChanged(QString path);

private:
    int m_number_of_desktops;
    QString m_wallpaper;
};


class Preferences::Keyboard : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString capsLockBehavior READ capsLockBehavior WRITE setCapsLockBehavior NOTIFY capsLockBehaviorChanged)
    Q_PROPERTY(int delayUntilRepeat READ delayUntilRepeat WRITE setDelayUntilRepeat NOTIFY delayUntilRepeatChanged)
    Q_PROPERTY(int keyRepeat READ keyRepeat WRITE setKeyRepeat NOTIFY keyRepeatChanged)
private:
    QString m_caps_lock_behavior;
    int m_delay_until_repeat;
    int m_key_repeat;
public:
    Keyboard(QObject *parent);
    ~Keyboard();

    //==================
    // Properties
    //==================
    QString capsLockBehavior() const;
    void setCapsLockBehavior(const QString& behavior);

    int delayUntilRepeat() const;
    void setDelayUntilRepeat(int val);

    int keyRepeat() const;
    void setKeyRepeat(int val);
signals:
    void capsLockBehaviorChanged(QString behavior);
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


class Preferences::Localization : public QObject {
    Q_OBJECT
private:
public:
    Localization(QObject *parent);
    ~Localization();
signals:
};

}

#endif // _LA_CONFFILE_H
