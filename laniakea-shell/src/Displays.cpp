#include "Displays.h"

#include <stdlib.h>

#include <xcb/randr.h>

namespace la {

Display::Display(const QString& output, const Display::Mode& mode,
        uint32_t crtc)
    : m_output(output),
      m_mode(mode)
{
    this->m_crtc = crtc;
}

//=====================
// Display::Mode
//=====================
Display::Mode::Mode(uint32_t id, uint32_t width, uint32_t height,
        const QString& refreshRate)
{
    this->m_id = id;
    this->m_width = width;
    this->m_height = height;
    this->m_refreshRate = refreshRate;
}

uint32_t Display::Mode::id() const
{
    return this->m_id;
}

uint32_t Display::Mode::width() const
{
    return this->m_width;
}

uint32_t Display::Mode::height() const
{
    return this->m_height;
}

QString Display::Mode::refreshRate() const
{
    return this->m_refreshRate;
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
QList<Display::Mode> Displays::modes_for_output(const Display::Output &output)
{
    QList<Display::Mode> ret;

    auto output_cookie = xcb_randr_get_output_info(
        this->m_connection, output.id(), 0);
    auto output_reply = xcb_randr_get_output_info_reply(
        this->m_connection, output_cookie, NULL);

    // Get modes for given output.
    xcb_randr_mode_t *modes = xcb_randr_get_output_info_modes(output_reply);
    int modes_length = xcb_randr_get_output_info_modes_length(output_reply);
    QList<xcb_randr_mode_t> modes_list;
    for (int i = 0; i < modes_length; ++i) {
        modes_list.append(modes[i]);
    }
    free(output_reply);

    auto screen_resources_cookie = xcb_randr_get_screen_resources(
        this->m_connection, this->m_screen->root);
    auto screen_resources_reply = xcb_randr_get_screen_resources_reply(
        this->m_connection, screen_resources_cookie, NULL);

    auto all_mode_iter = xcb_randr_get_screen_resources_modes_iterator(
        screen_resources_reply);
    int all_modes_length = xcb_randr_get_screen_resources_modes_length(
        screen_resources_reply);
    for (int i = 0; i < all_modes_length; ++i) {
        xcb_randr_mode_info_t *mode_info = all_mode_iter.data;
        if (modes_list.contains(mode_info->id)) {
            double refresh_rate =
                ((double)mode_info->dot_clock) / (mode_info->htotal * mode_info->vtotal);
            auto refresh_rate_str = QString{"%1"}.arg(refresh_rate, 5, 'f', 2);
            Display::Mode mode(
                mode_info->id,
                mode_info->width,
                mode_info->height,
                refresh_rate_str
            );
            ret.append(mode);
        }

        xcb_randr_mode_info_next(&all_mode_iter);
    }
    free(screen_resources_reply);

    return ret;
}

uint32_t Displays::crtc_for_output(const Display::Output &output)
{
    auto output_cookie = xcb_randr_get_output_info(
        this->m_connection, output.id(), 0);
    auto output_reply = xcb_randr_get_output_info_reply(
        this->m_connection, output_cookie, NULL);

    uint32_t crtc = output_reply->crtc;

    free(output_reply);

    return crtc;
}

} // namespace la
