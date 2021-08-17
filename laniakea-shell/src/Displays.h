#ifndef DISPLAYS_H
#define DISPLAYS_H

#include <QObject>
#include <QPoint>

#include <xcb/xcb.h>

namespace la {

class Display
{
public:
    //===================
    // Display::Output
    //===================
    class Output
    {
    public:
        Output(uint32_t id, const QString& name);

        uint32_t id() const;
        QString name() const;

    private:
        uint32_t m_id;
        QString m_name;
    };

    //===================
    // Display::Mode
    //===================
    class Mode
    {
    public:
        Mode();
        Mode(uint32_t id, uint32_t width, uint32_t height,
                const QString& refreshRate);

        uint32_t id() const;
        uint32_t width() const;
        uint32_t height() const;
        QString refreshRate() const;
    private:
        uint32_t m_id;
        uint32_t m_width;
        uint32_t m_height;
        QString m_refreshRate;
    };

public:
    Display(const Display::Output& output, const QList<Display::Mode>& modes,
            uint32_t crtc);

    Display::Output output() const;

    QList<Display::Mode> modes() const;

    uint32_t crtc() const;

    bool connection() const;
    void setConnection(bool value);

    QPoint position() const;
    void setPosition(const QPoint& position);

private:
    Display::Output m_output;
    Display::Mode m_mode;
    uint32_t m_crtc;
    QList<Display::Mode> m_modes;
    bool m_connection;
    QPoint m_position;
};


//==============
// Displays
//==============
class Displays : public QObject
{
    Q_OBJECT
public:
    explicit Displays(xcb_connection_t *conn, QObject *parent = nullptr);
    ~Displays();

    void init();

    QList<Display> displays() const;

    //=============================
    // Displays modifying methods
    //=============================
    void setDisplayOutput(const Display& display, const Display::Mode& mode);

    const QList<Display::Output> outputs() const;

signals:

private:
    QList<Display::Mode> modes_for_output(const Display::Output& output);
    uint32_t crtc_for_output(const Display::Output& output);
    bool connection_for_output(const Display::Output& output);
    QPoint position_for_display(const Display& display);

private:
    xcb_connection_t *m_connection;
    xcb_screen_t *m_screen;
    QList<Display> m_displays;
};

} // namespace la

#endif // DISPLAYS_H
