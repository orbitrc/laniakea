#include "Displays.h"

#include <stdlib.h>
#include <string.h>

#include <xcb/randr.h>

namespace la {

Display::Display(const Display::Output& output, const QList<Display::Mode>& modes,
        uint32_t crtc)
    : m_output(output)
{
    this->m_modes = modes;
    this->m_crtc = crtc;
    this->m_connection = false;
}

Display::Output Display::output() const
{
    return this->m_output;
}

void Display::setOutput(const Display::Output& output)
{
    if (this->m_output != output) {
        this->m_output = output;
    }
}

Display::Mode Display::mode() const
{
    return this->m_mode;
}

void Display::setMode(const Display::Mode& mode)
{
    if (this->m_mode != mode) {
        this->m_mode = mode;
    }
}

QList<Display::Mode> Display::modes() const
{
    return this->m_modes;
}

uint32_t Display::crtc() const
{
    return this->m_crtc;
}

bool Display::connection() const
{
    return this->m_connection;
}

void Display::setConnection(bool value)
{
    if (this->m_connection != value) {
        this->m_connection = value;
    }
}

QPoint Display::position() const
{
    return this->m_position;
}

void Display::setPosition(const QPoint &position)
{
    if (this->m_position != position) {
        this->m_position = position;
    }
}

//=====================
// Display::Mode
//=====================
Display::Mode::Mode()
{
    this->m_id = 0;
    this->m_width = 0;
    this->m_height = 0;
    this->m_refreshRate = "";
}

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

bool Display::Mode::operator==(const Mode& other) const
{
    return this->m_id == other.id();
}

bool Display::Mode::operator!=(const Mode& other) const
{
    return this->m_id != other.id();
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

bool Display::Output::operator==(const Output& other) const
{
    return this->m_id == other.id();
}

bool Display::Output::operator!=(const Output& other) const
{
    return this->m_id != other.id();
}

//===================
// Display::Edid
//===================
Display::Edid::Edid(uint8_t *data)
{
    this->m_data = data;
}

Display::Edid::~Edid()
{
    free(this->m_data);
}

const uint8_t* Display::Edid::data() const
{
    return this->m_data;
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

void Displays::init()
{
    auto outputs = this->outputs();

    for (int i = 0; i < outputs.length(); ++i) {
        auto modes = this->modes_for_output(outputs[i]);
        auto crtc = this->crtc_for_output(outputs[i]);
        Display display(outputs[i], modes, crtc);
        display.setConnection(this->connection_for_output(outputs[i]));
        display.setPosition(this->position_for_display(display));

        this->m_displays.append(display);
    }
}

QList<Display> Displays::displays() const
{
    return this->m_displays;
}

//==============================
// Displays modifying methods
//==============================
void Displays::applyDisplaySettings(const Display &display,
        const Display::Mode &mode, const QPoint &position,
        Display::Rotation rotation)
{
    xcb_randr_output_t outputs[1];
    outputs[0] = display.output().id();

    xcb_randr_rotation_t rotation_enum = XCB_RANDR_ROTATION_ROTATE_0;
    switch (rotation) {
    case Display::Rotation::Normal:
        rotation_enum = XCB_RANDR_ROTATION_ROTATE_0;
        break;
    case Display::Rotation::Right:
        rotation_enum = XCB_RANDR_ROTATION_ROTATE_90;
        break;
    case Display::Rotation::Left:
        rotation_enum = XCB_RANDR_ROTATION_ROTATE_270;
        break;
    case Display::Rotation::Rotate180:
        rotation_enum = XCB_RANDR_ROTATION_ROTATE_180;
        break;
    case Display::Rotation::UpsideDown:
        rotation_enum = XCB_RANDR_ROTATION_REFLECT_Y;
        break;
    case Display::Rotation::LeftAndRightReversed:
        rotation_enum = XCB_RANDR_ROTATION_REFLECT_X;
        break;
    default:
        break;
    }

    xcb_randr_set_crtc_config(
        this->m_connection,
        display.crtc(),
        XCB_TIME_CURRENT_TIME,
        XCB_TIME_CURRENT_TIME,
        position.x(),
        position.y(),
        mode.id(),
        rotation_enum,
        1,
        outputs
    );

    // Change internal display properties.
    Display *target = this->_display_for_output(display.output());
    target->setPosition(position);
    target->setMode(mode);
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
            // Get refresh rate.
            double vtotal = mode_info->vtotal;
            if (mode_info->mode_flags & XCB_RANDR_MODE_FLAG_DOUBLE_SCAN) {
                vtotal *= 2;
            }
            if (mode_info->mode_flags & XCB_RANDR_MODE_FLAG_INTERLACE) {
                vtotal /= 2;
            }
            double refresh_rate =
                ((double)mode_info->dot_clock) / (mode_info->htotal * vtotal);
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

bool Displays::connection_for_output(const Display::Output &output)
{
    bool connection = false;

    auto output_cookie = xcb_randr_get_output_info(
        this->m_connection, output.id(), 0);
    auto output_reply = xcb_randr_get_output_info_reply(
        this->m_connection, output_cookie, NULL);

    if (output_reply->connection == XCB_RANDR_CONNECTION_CONNECTED) {
        connection = true;
    }
    free(output_reply);

    return connection;
}

QPoint Displays::position_for_display(const Display &display)
{
    QPoint ret;

    auto cookie = xcb_randr_get_crtc_info(
        this->m_connection, display.crtc(), 0);
    auto reply = xcb_randr_get_crtc_info_reply(
        this->m_connection, cookie, NULL);

    ret.setX(reply->x);
    ret.setY(reply->y);

    free(reply);

    return ret;
}

bool Displays::has_edid_for_otuput(const Display::Output &output)
{
    // Get atom EDID.
    xcb_intern_atom_cookie_t intern_atom_cookie =
        xcb_intern_atom(this->m_connection, 1, 4, "EDID");
    xcb_intern_atom_reply_t *intern_atom_reply =
        xcb_intern_atom_reply(this->m_connection, intern_atom_cookie, NULL);
    xcb_atom_t atom_edid = intern_atom_reply->atom;
    free(intern_atom_reply);

    // Get output properties.
    auto list_prop_cookie = xcb_randr_list_output_properties(
        this->m_connection, output.id());
    auto list_prop_reply = xcb_randr_list_output_properties_reply(
        this->m_connection, list_prop_cookie, NULL);
    xcb_atom_t *atoms =
        xcb_randr_list_output_properties_atoms(list_prop_reply);
    int atoms_length =
        xcb_randr_list_output_properties_atoms_length(list_prop_reply);
    QList<xcb_atom_t> atoms_list;
    for (int i = 0; i < atoms_length; ++i) {
        atoms_list.append(atoms[i]);
    }
    free(list_prop_reply);
    if (!atoms_list.contains(atom_edid)) {
        return false;
    }

    return true;
}

Display::Edid* Displays::edid_for_output(const Display::Output &output)
{
    // Get atom EDID.
    xcb_intern_atom_cookie_t intern_atom_cookie =
        xcb_intern_atom(this->m_connection, 1, 4, "EDID");
    xcb_intern_atom_reply_t *intern_atom_reply =
        xcb_intern_atom_reply(this->m_connection, intern_atom_cookie, NULL);
    xcb_atom_t atom_edid = intern_atom_reply->atom;
    free(intern_atom_reply);

    auto property_cookie = xcb_randr_get_output_property(
        this->m_connection,
        output.id(),
        atom_edid,
        XCB_ATOM_ANY,
        0,
        512,
        0,
        0
    );
    auto property_reply = xcb_randr_get_output_property_reply(
        this->m_connection, property_cookie, NULL);
    uint8_t *data = xcb_randr_get_output_property_data(property_reply);
    int data_length = xcb_randr_get_output_property_data_length(property_reply);
    uint8_t *data_copy = (uint8_t*)malloc(128 * sizeof(uint8_t));
    memcpy(data_copy, data, data_length);
    free(property_reply);

    Display::Edid *edid = new Display::Edid(data_copy);

    return edid;
}

Display* Displays::_display_for_output(const Display::Output& output)
{
    Display *ret = nullptr;

    for (int i = 0; i < this->m_displays.length(); ++i) {
        if (output.id() == this->m_displays[i].output().id() &&
                output.name() == this->m_displays[i].output().name()) {
            ret = &(this->m_displays[i]);
            break;
        }
    }

    return ret;
}

} // namespace la
