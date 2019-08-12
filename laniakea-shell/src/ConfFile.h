#ifndef _LA_CONFFILE_H
#define _LA_CONFFILE_H

namespace la {


class ConfFile
{
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
    ConfFile();
    ~ConfFile();

    Impl& impl();

    /// \brief  Start inotify watch loop.
    int run_watch_loop();

    /// \brief  Stop inotify watch loop.
    void exit_loop();

    void sync_with_file();

    //===============
    // Getters
    //===============
    double pixels_per_dp() const;

    int number_of_desktops() const;
};

}

#endif // _LA_CONFFILE_H
