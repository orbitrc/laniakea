#include "Displays.h"

#include <stdlib.h>

#include <xcb/randr.h>

namespace la {

Display::Mode::Mode(uint32_t width, uint32_t height, const QString& refreshRate)
{
    this->m_width = width;
    this->m_height = height;
    this->m_refreshRate = refreshRate;
}

Display::Display(const QString& output, const Display::Mode& mode,
        uint32_t crtc)
    : m_output(output),
      m_mode(mode)
{
    this->m_crtc = crtc;
}

//=====================
// Display::Output
//=====================
Display::Output::Output(uint32_t id, const QString& name)
{
    this->m_id = id;
    this->m_name = name;
}

uint32_t Display::Output::id() const
{
    return this->m_id;
}

QString Display::Output::name() const
{
    return this->m_name;
}

//===================
// Displays
//===================
Displays::Displays(xcb_connection_t *conn, QObject *parent)
    : QObject(parent)
{
    this->m_connection = conn;

    this->m_screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
}

Displays::~Displays()
{
    xcb_disconnect(this->m_connection);
}

const QList<Display::Output> Displays::outputs() const
{
    auto screen_resources_cookie = xcb_randr_get_screen_resources(
        this->m_connection, this->m_screen->root);
    auto screen_resources_reply = xcb_randr_get_screen_resources_reply(
        this->m_connection, screen_resources_cookie, NULL);

    xcb_randr_output_t *outputs =
        xcb_randr_get_screen_resources_outputs(screen_resources_reply);
    int outputs_length =
        xcb_randr_get_screen_resources_outputs_length(screen_resources_reply);

    QList<Display::Output> ret;
    for (int i = 0; i < outputs_length; ++i) {
        auto output_cookie = xcb_randr_get_output_info(
            this->m_connection,
            outputs[i],
            0
        );
        auto output_reply = xcb_randr_get_output_info_reply(
            this->m_connection,
            output_cookie,
            NULL
        );
        uint8_t *output_name = xcb_randr_get_output_info_name(output_reply);
        Display::Output output(outputs[i], reinterpret_cast<char*>(output_name));
        ret.append(output);
        free(output_reply);
    }
    free(screen_resources_reply);

    return ret;
}

//==================
// Private methods
//==================


} // namespace la
