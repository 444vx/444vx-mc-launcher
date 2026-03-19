#include <gtkmm.h>

static const char* CSS = R"(
window {
    background-color: #ffffff;
    color: #000000;
}

button {
    background: none;
    border: 1px solid #000000;
    color: #000000;
    border-radius: 0;
    padding: 8px 16px;
}

button:hover {
    background-color: #000000;
    color: #ffffff;
}

.sidebar {
    background-color: #ffffff;
    border-right: 1px solid #000000;
}

.title {
    font-size: 18px;
    font-weight: bold;
    color: #000000;
    padding: 20px 10px;
}

.page-title {
    font-size: 22px;
    font-weight: bold;
    color: #000000;
}

.subtitle {
    color: #555555;
    font-size: 13px;
}

entry {
    background-color: #ffffff;
    color: #000000;
    border: 1px solid #000000;
    border-radius: 0;
    padding: 6px;
}

entry:focus {
    border-color: #000000;
    box-shadow: none;
}
)";

// ─── HOME PAGE ───────────────────────────────────────────────
class HomePage : public Gtk::Box {
public:
    HomePage() {
        set_orientation(Gtk::Orientation::VERTICAL);
        set_margin(30);
        set_spacing(20);
        set_valign(Gtk::Align::CENTER);
        set_vexpand(true);

        m_title.set_markup("<span font='20' weight='bold'>444vx Launcher</span>");
        m_title.set_halign(Gtk::Align::START);

        m_status.set_text("Not logged in");
        m_status.set_halign(Gtk::Align::START);
        m_status.add_css_class("subtitle");

        m_mode.set_text("No mode selected");
        m_instance.set_text("Active instance: None");
        m_instance.set_halign(Gtk::Align::START);
        m_instance.add_css_class("subtitle");
        m_mode.set_halign(Gtk::Align::START);
        m_mode.add_css_class("subtitle");

        m_btn_login.set_label("Login with Microsoft");
        m_btn_login.set_halign(Gtk::Align::START);

        m_btn_play.set_label("PLAY");
        m_btn_play.set_halign(Gtk::Align::START);
        m_btn_play.set_size_request(200, 50);

        append(m_title);
        append(m_status);
        append(m_mode);
        append(m_instance);
        append(m_btn_login);
        append(m_btn_play);
    }

private:
    Gtk::Label m_title;
    Gtk::Label m_status;
    Gtk::Label m_mode;
    Gtk::Label m_instance;
    Gtk::Button m_btn_login;
    Gtk::Button m_btn_play;
};

// ─── CATEGORIES PAGE ─────────────────────────────────────────
class CategoriesPage : public Gtk::Box {
public:
    CategoriesPage() {
        set_orientation(Gtk::Orientation::VERTICAL);
        set_margin(30);
        set_spacing(20);

        m_title.set_markup("<span font='20' weight='bold'>Categories</span>");
        m_title.set_halign(Gtk::Align::START);

        m_mode_label.set_text("Game Mode");
        m_mode_label.set_halign(Gtk::Align::START);
        m_mode_label.add_css_class("subtitle");

        m_mode_box.set_orientation(Gtk::Orientation::HORIZONTAL);
        m_mode_box.set_spacing(10);

        m_btn_pvp.set_label("PvP");
        m_btn_vanilla.set_label("Vanilla");
        m_btn_modded.set_label("Modded");
        m_btn_fps.set_label("FPS Boost");

        m_mode_box.append(m_btn_pvp);
        m_mode_box.append(m_btn_vanilla);
        m_mode_box.append(m_btn_modded);
        m_mode_box.append(m_btn_fps);

        m_pc_label.set_text("PC Performance");
        m_pc_label.set_halign(Gtk::Align::START);
        m_pc_label.add_css_class("subtitle");

        m_pc_box.set_orientation(Gtk::Orientation::HORIZONTAL);
        m_pc_box.set_spacing(10);

        m_btn_potato.set_label("Potato");
        m_btn_medium.set_label("Medium");
        m_btn_high.set_label("High End");

        m_pc_box.append(m_btn_potato);
        m_pc_box.append(m_btn_medium);
        m_pc_box.append(m_btn_high);

        append(m_title);
        append(m_mode_label);
        append(m_mode_box);
        append(m_pc_label);
        append(m_pc_box);
    }

private:
    Gtk::Label m_title;
    Gtk::Label m_mode_label;
    Gtk::Label m_pc_label;
    Gtk::Box m_mode_box;
    Gtk::Box m_pc_box;
    Gtk::Button m_btn_pvp;
    Gtk::Button m_btn_vanilla;
    Gtk::Button m_btn_modded;
    Gtk::Button m_btn_fps;
    Gtk::Button m_btn_potato;
    Gtk::Button m_btn_medium;
    Gtk::Button m_btn_high;
};

// ─── MODS PAGE ───────────────────────────────────────────────
class ModsPage : public Gtk::Box {
public:
    ModsPage() {
        set_orientation(Gtk::Orientation::VERTICAL);
        set_margin(30);
        set_spacing(20);

        m_title.set_markup("<span font='20' weight='bold'>Mods</span>");
        m_title.set_halign(Gtk::Align::START);

        m_subtitle.set_text("Open your mods folder");
        m_subtitle.set_halign(Gtk::Align::START);
        m_subtitle.add_css_class("subtitle");

        m_btn_open.set_label("Open Mods Folder");
        m_btn_open.set_halign(Gtk::Align::START);
        m_btn_open.signal_clicked().connect([]() {
            system("xdg-open ~/.minecraft/mods");
        });

        append(m_title);
        append(m_subtitle);
        append(m_btn_open);
    }

private:
    Gtk::Label m_title;
    Gtk::Label m_subtitle;
    Gtk::Button m_btn_open;
};

// ─── RESOURCE PACKS PAGE ─────────────────────────────────────
class ResourcePacksPage : public Gtk::Box {
public:
    ResourcePacksPage() {
        set_orientation(Gtk::Orientation::VERTICAL);
        set_margin(30);
        set_spacing(20);

        m_title.set_markup("<span font='20' weight='bold'>Resource Packs</span>");
        m_title.set_halign(Gtk::Align::START);

        m_subtitle.set_text("Open your resource packs folder");
        m_subtitle.set_halign(Gtk::Align::START);
        m_subtitle.add_css_class("subtitle");

        m_btn_open.set_label("Open Resource Packs Folder");
        m_btn_open.set_halign(Gtk::Align::START);
        m_btn_open.signal_clicked().connect([]() {
            system("xdg-open ~/.minecraft/resourcepacks");
        });

        append(m_title);
        append(m_subtitle);
        append(m_btn_open);
    }

private:
    Gtk::Label m_title;
    Gtk::Label m_subtitle;
    Gtk::Button m_btn_open;
};

// ─── MODPACKS PAGE ───────────────────────────────────────────
class ModpacksPage : public Gtk::Box {
public:
    ModpacksPage() {
        set_orientation(Gtk::Orientation::VERTICAL);
        set_margin(30);
        set_spacing(20);

        m_title.set_markup("<span font='20' weight='bold'>Modpacks</span>");
        m_title.set_halign(Gtk::Align::START);

        m_subtitle.set_text("Open your modpacks folder");
        m_subtitle.set_halign(Gtk::Align::START);
        m_subtitle.add_css_class("subtitle");

        m_btn_open.set_label("Open Modpacks Folder");
        m_btn_open.set_halign(Gtk::Align::START);
        m_btn_open.signal_clicked().connect([]() {
            system("xdg-open ~/.minecraft/modpacks");
        });

        append(m_title);
        append(m_subtitle);
        append(m_btn_open);
    }

private:
    Gtk::Label m_title;
    Gtk::Label m_subtitle;
    Gtk::Button m_btn_open;
};

// ─── SETTINGS PAGE ───────────────────────────────────────────
class SettingsPage : public Gtk::Box {
public:
    SettingsPage() {
        set_orientation(Gtk::Orientation::VERTICAL);
        set_margin(30);
        set_spacing(20);

        m_title.set_markup("<span font='20' weight='bold'>Settings</span>");
        m_title.set_halign(Gtk::Align::START);

        m_java_label.set_text("Java Path");
        m_java_label.set_halign(Gtk::Align::START);
        m_java_label.add_css_class("subtitle");
        m_java_entry.set_placeholder_text("/usr/bin/java");
        m_java_entry.set_hexpand(true);

        m_ram_label.set_text("RAM (MB)");
        m_ram_label.set_halign(Gtk::Align::START);
        m_ram_label.add_css_class("subtitle");
        m_ram_entry.set_placeholder_text("4096");
        m_ram_entry.set_hexpand(true);

        m_btn_save.set_label("Save");
        m_btn_save.set_halign(Gtk::Align::START);

        append(m_title);
        append(m_java_label);
        append(m_java_entry);
        append(m_ram_label);
        append(m_ram_entry);
        append(m_btn_save);
    }

private:
    Gtk::Label m_title;
    Gtk::Label m_java_label;
    Gtk::Label m_ram_label;
    Gtk::Entry m_java_entry;
    Gtk::Entry m_ram_entry;
    Gtk::Button m_btn_save;
};

// ─── MAIN WINDOW ─────────────────────────────────────────────
class LauncherWindow : public Gtk::Window {
public:
    LauncherWindow() {
        set_title("444vx Launcher");
        set_default_size(900, 600);

        // Load CSS
        auto css = Gtk::CssProvider::create();
        css->load_from_data(CSS);
        Gtk::StyleContext::add_provider_for_display(
            get_display(),
            css,
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
        );

        m_box.set_orientation(Gtk::Orientation::HORIZONTAL);
        set_child(m_box);

        // Sidebar
        m_sidebar.set_orientation(Gtk::Orientation::VERTICAL);
        m_sidebar.set_size_request(160, -1);
        m_sidebar.set_margin(15);
        m_sidebar.set_spacing(5);
        m_sidebar.add_css_class("sidebar");

        m_label_title.set_text("444vx");
        m_label_title.set_halign(Gtk::Align::START);
        m_label_title.add_css_class("title");

        m_btn_home.set_label("Home");
        m_btn_categories.set_label("Categories");
        m_btn_mods.set_label("Mods");
        m_btn_resourcepacks.set_label("Resource Packs");
        m_btn_modpacks.set_label("Modpacks");
        m_btn_settings.set_label("Settings");

        m_sidebar.append(m_label_title);
        m_sidebar.append(m_btn_home);
        m_sidebar.append(m_btn_categories);
        m_sidebar.append(m_btn_mods);
        m_sidebar.append(m_btn_resourcepacks);
        m_sidebar.append(m_btn_modpacks);
        m_sidebar.append(m_btn_settings);

        // Separator
        m_separator.set_orientation(Gtk::Orientation::VERTICAL);

        // Stack
        m_stack.add(m_home_page, "home");
        m_stack.add(m_categories_page, "categories");
        m_stack.add(m_mods_page, "mods");
        m_stack.add(m_resourcepacks_page, "resourcepacks");
        m_stack.add(m_modpacks_page, "modpacks");
        m_stack.add(m_settings_page, "settings");
        m_stack.set_hexpand(true);
        m_stack.set_vexpand(true);

        m_btn_home.signal_clicked().connect([this]() {
            m_stack.set_visible_child("home");
        });
        m_btn_categories.signal_clicked().connect([this]() {
            m_stack.set_visible_child("categories");
        });
        m_btn_mods.signal_clicked().connect([this]() {
            m_stack.set_visible_child("mods");
        });
        m_btn_resourcepacks.signal_clicked().connect([this]() {
            m_stack.set_visible_child("resourcepacks");
        });
        m_btn_modpacks.signal_clicked().connect([this]() {
            m_stack.set_visible_child("modpacks");
        });
        m_btn_settings.signal_clicked().connect([this]() {
            m_stack.set_visible_child("settings");
        });

        m_box.append(m_sidebar);
        m_box.append(m_separator);
        m_box.append(m_stack);
    }

private:
    Gtk::Box m_box;
    Gtk::Box m_sidebar;
    Gtk::Separator m_separator;
    Gtk::Stack m_stack;
    Gtk::Label m_label_title;
    Gtk::Button m_btn_home;
    Gtk::Button m_btn_categories;
    Gtk::Button m_btn_mods;
    Gtk::Button m_btn_resourcepacks;
    Gtk::Button m_btn_modpacks;
    Gtk::Button m_btn_settings;

    HomePage m_home_page;
    CategoriesPage m_categories_page;
    ModsPage m_mods_page;
    ResourcePacksPage m_resourcepacks_page;
    ModpacksPage m_modpacks_page;
    SettingsPage m_settings_page;
};

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("com.444vx.launcher");
    return app->make_window_and_run<LauncherWindow>(argc, argv);
}
