#ifndef DISPLAYS_H
#define DISPLAYS_H

#include <QObject>

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
    Display(const QString& output, const Display::Mode& mode, uint32_t crtc);

private:
    QString m_output;
    Display::Mode m_mode;
    uint32_t m_crtc;
    QList<Display::Mode> m_modes;
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

    const QList<Display::Output> outputs() const;

signals:

private:
    QList<Display::Mode> modes_for_output(const Display::Output& output);
    uint32_t crtc_for_output(const Display::Output& output);

private:
    xcb_connection_t *m_connection;
    xcb_screen_t *m_screen;
};

} // namespace la

#endif // DISPLAYS_H
