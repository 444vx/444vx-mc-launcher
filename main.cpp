#include <gtkmm.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <chrono>

using json = nlohmann::json;

std::string configPath() {
    return std::string(getenv("HOME")) + "/.config/444vx-launcher/";
}

struct Instance {
    std::string name;
    std::string mcVersion;
    std::string gameMode = "none";
    std::string hardware = "none";
    std::string javaPath = "/usr/bin/java";
    int ram = 4096;
};

void saveInstances(const std::vector<Instance>& instances) {
    system(("mkdir -p " + configPath()).c_str());
    json j = json::array();
    for (auto& i : instances) {
        json inst;
        inst["name"] = i.name;
        inst["mcVersion"] = i.mcVersion;
        inst["gameMode"] = i.gameMode;
        inst["hardware"] = i.hardware;
        inst["javaPath"] = i.javaPath;
        inst["ram"] = i.ram;
        j.push_back(inst);
    }
    std::ofstream f(configPath() + "instances.json");
    f << j.dump(4);
}

std::vector<Instance> loadInstances() {
    std::vector<Instance> instances;
    std::ifstream f(configPath() + "instances.json");
    if (!f.good()) return instances;
    json j; f >> j;
    for (auto& inst : j) {
        Instance i;
        i.name = inst.value("name", "");
        i.mcVersion = inst.value("mcVersion", "");
        i.gameMode = inst.value("gameMode", "none");
        i.hardware = inst.value("hardware", "none");
        i.javaPath = inst.value("javaPath", "/usr/bin/java");
        i.ram = inst.value("ram", 4096);
        instances.push_back(i);
    }
    return instances;
}

struct Account {
    std::string username;
    bool active = false;
};

void saveAccounts(const std::vector<Account>& accounts) {
    system(("mkdir -p " + configPath()).c_str());
    json j = json::array();
    for (auto& a : accounts) {
        json acc;
        acc["username"] = a.username;
        acc["active"] = a.active;
        j.push_back(acc);
    }
    std::ofstream f(configPath() + "accounts.json");
    f << j.dump(4);
}

std::vector<Account> loadAccounts() {
    std::vector<Account> accounts;
    std::ifstream f(configPath() + "accounts.json");
    if (!f.good()) return accounts;
    json j; f >> j;
    for (auto& a : j) {
        Account acc;
        acc.username = a.value("username", "");
        acc.active = a.value("active", false);
        accounts.push_back(acc);
    }
    return accounts;
}

Account getActiveAccount() {
    for (auto& a : loadAccounts())
        if (a.active) return a;
    return {};
}

static const char* CSS = R"(
window { background-color: #ffffff; color: #000000; }
button { background: none; border: 1px solid #000000; color: #000000; border-radius: 0; padding: 8px 16px; }
button:hover { background-color: #000000; color: #ffffff; }
.sidebar { background-color: #ffffff; border-right: 1px solid #000000; }
.title { font-size: 18px; font-weight: bold; color: #000000; padding: 20px 10px; }
.subtitle { color: #555555; font-size: 13px; }
entry { background-color: #ffffff; color: #000000; border: 1px solid #000000; border-radius: 0; padding: 6px; }
entry:focus { border-color: #000000; box-shadow: none; }
.instance-card { border: 1px solid #000000; margin: 2px; }
listboxrow { background: none; border-bottom: 1px solid #dddddd; }
listboxrow:hover { background-color: #f5f5f5; }
)";

class AddInstanceWindow : public Gtk::Window {
public:
    std::function<void(Instance)> onAdded;

    AddInstanceWindow() {
        set_title("Add Instance");
        set_default_size(400, 450);
        set_resizable(false);

        m_box.set_orientation(Gtk::Orientation::VERTICAL);
        m_box.set_margin(20);
        m_box.set_spacing(12);
        set_child(m_box);

        m_title.set_markup("<span font='16' weight='bold'>Add Instance</span>");
        m_title.set_halign(Gtk::Align::START);

        m_tabs.append_page(m_custom_box, "Custom");
        m_tabs.append_page(m_modpack_box, "Modpack");

        m_custom_box.set_orientation(Gtk::Orientation::VERTICAL);
        m_custom_box.set_spacing(8);
        m_custom_box.set_margin(10);

        m_name_label.set_text("Instance name");
        m_name_label.set_halign(Gtk::Align::START);
        m_name_label.add_css_class("subtitle");
        m_name_entry.set_placeholder_text("My Instance");

        m_ver_label.set_text("Minecraft version");
        m_ver_label.set_halign(Gtk::Align::START);
        m_ver_label.add_css_class("subtitle");
        m_ver_entry.set_placeholder_text("1.21.11");

        m_mode_label.set_text("Game Mode (optional)");
        m_mode_label.set_halign(Gtk::Align::START);
        m_mode_label.add_css_class("subtitle");
        m_mode_combo.append("none", "None");
        m_mode_combo.append("pvp", "PvP");
        m_mode_combo.append("vanilla", "Vanilla");
        m_mode_combo.append("modded", "Modded");
        m_mode_combo.append("fps", "FPS Boost");
        m_mode_combo.set_active(0);

        m_hw_label.set_text("Hardware (optional)");
        m_hw_label.set_halign(Gtk::Align::START);
        m_hw_label.add_css_class("subtitle");
        m_hw_combo.append("none", "None");
        m_hw_combo.append("potato", "Potato");
        m_hw_combo.append("mid", "Mid");
        m_hw_combo.append("highend", "High End");
        m_hw_combo.set_active(0);

        m_btn_create.set_label("Create Instance");
        m_btn_create.set_halign(Gtk::Align::START);
        m_btn_create.signal_clicked().connect([this]() {
            std::string name = m_name_entry.get_text();
            std::string ver = m_ver_entry.get_text();
            if (name.empty() || ver.empty()) return;
            Instance inst;
            inst.name = name;
            inst.mcVersion = ver;
            inst.gameMode = m_mode_combo.get_active_id();
            inst.hardware = m_hw_combo.get_active_id();
            if (onAdded) onAdded(inst);
            close();
        });

        m_custom_box.append(m_name_label);
        m_custom_box.append(m_name_entry);
        m_custom_box.append(m_ver_label);
        m_custom_box.append(m_ver_entry);
        m_custom_box.append(m_mode_label);
        m_custom_box.append(m_mode_combo);
        m_custom_box.append(m_hw_label);
        m_custom_box.append(m_hw_combo);
        m_custom_box.append(m_btn_create);

        m_modpack_box.set_orientation(Gtk::Orientation::VERTICAL);
        m_modpack_box.set_spacing(8);
        m_modpack_box.set_margin(10);
        m_search_label.set_text("Search Modrinth / CurseForge");
        m_search_label.set_halign(Gtk::Align::START);
        m_search_label.add_css_class("subtitle");
        m_search_entry.set_placeholder_text("Search modpacks...");
        m_coming_soon.set_text("Modpack search — coming soon");
        m_coming_soon.set_halign(Gtk::Align::START);
        m_coming_soon.add_css_class("subtitle");
        m_modpack_box.append(m_search_label);
        m_modpack_box.append(m_search_entry);
        m_modpack_box.append(m_coming_soon);

        m_box.append(m_title);
        m_box.append(m_tabs);
    }

private:
    Gtk::Box m_box;
    Gtk::Label m_title;
    Gtk::Notebook m_tabs;
    Gtk::Box m_custom_box;
    Gtk::Label m_name_label, m_ver_label, m_mode_label, m_hw_label;
    Gtk::Entry m_name_entry, m_ver_entry;
    Gtk::ComboBoxText m_mode_combo, m_hw_combo;
    Gtk::Button m_btn_create;
    Gtk::Box m_modpack_box;
    Gtk::Label m_search_label, m_coming_soon;
    Gtk::Entry m_search_entry;
};

class InstanceSettingsWindow : public Gtk::Window {
public:
    std::function<void(Instance)> onSaved;

    InstanceSettingsWindow(Instance inst) : m_inst(inst) {
        set_title("Settings: " + inst.name);
        set_default_size(400, 400);
        set_resizable(false);

        m_box.set_orientation(Gtk::Orientation::VERTICAL);
        m_box.set_margin(20);
        m_box.set_spacing(12);
        set_child(m_box);

        m_title.set_markup("<span font='16' weight='bold'>" + inst.name + "</span>");
        m_title.set_halign(Gtk::Align::START);
        m_box.append(m_title);

        auto addLabel = [this](const std::string& text) {
            auto lbl = Gtk::make_managed<Gtk::Label>(text);
            lbl->set_halign(Gtk::Align::START);
            lbl->add_css_class("subtitle");
            m_box.append(*lbl);
        };

        addLabel("Minecraft version");
        m_ver_entry.set_text(inst.mcVersion);
        m_box.append(m_ver_entry);

        addLabel("Java path");
        m_java_entry.set_text(inst.javaPath);
        m_box.append(m_java_entry);

        addLabel("RAM (MB)");
        m_ram_entry.set_text(std::to_string(inst.ram));
        m_box.append(m_ram_entry);

        addLabel("Game Mode");
        m_mode_combo.append("none", "None");
        m_mode_combo.append("pvp", "PvP");
        m_mode_combo.append("vanilla", "Vanilla");
        m_mode_combo.append("modded", "Modded");
        m_mode_combo.append("fps", "FPS Boost");
        m_mode_combo.set_active_id(inst.gameMode);
        m_box.append(m_mode_combo);

        addLabel("Hardware");
        m_hw_combo.append("none", "None");
        m_hw_combo.append("potato", "Potato");
        m_hw_combo.append("mid", "Mid");
        m_hw_combo.append("highend", "High End");
        m_hw_combo.set_active_id(inst.hardware);
        m_box.append(m_hw_combo);

        m_btn_save.set_label("Save");
        m_btn_save.set_halign(Gtk::Align::START);
        m_btn_save.signal_clicked().connect([this]() {
            m_inst.mcVersion = m_ver_entry.get_text();
            m_inst.javaPath = m_java_entry.get_text();
            try { m_inst.ram = std::stoi(m_ram_entry.get_text()); } catch(...) { m_inst.ram = 4096; }
            m_inst.gameMode = m_mode_combo.get_active_id();
            m_inst.hardware = m_hw_combo.get_active_id();
            if (onSaved) onSaved(m_inst);
            close();
        });
        m_box.append(m_btn_save);
    }

private:
    Instance m_inst;
    Gtk::Box m_box;
    Gtk::Label m_title;
    Gtk::Entry m_ver_entry, m_java_entry, m_ram_entry;
    Gtk::ComboBoxText m_mode_combo, m_hw_combo;
    Gtk::Button m_btn_save;
};

class HomePage : public Gtk::Box {
public:
    HomePage() {
        set_orientation(Gtk::Orientation::VERTICAL);
        set_margin(20);
        set_spacing(10);

        m_title.set_markup("<span font='20' weight='bold'>Instances</span>");
        m_title.set_halign(Gtk::Align::START);
        m_title.set_margin_bottom(10);

        m_scroll.set_vexpand(true);
        m_scroll.set_child(m_instances_box);
        m_instances_box.set_orientation(Gtk::Orientation::VERTICAL);
        m_instances_box.set_spacing(5);

        m_btn_add.set_label("+ Add Instance");
        m_btn_add.set_halign(Gtk::Align::START);
        m_btn_add.signal_clicked().connect([this]() {
            auto win = new AddInstanceWindow();
            win->onAdded = [this](Instance inst) {
                m_instances.push_back(inst);
                saveInstances(m_instances);
                refresh();
            };
            win->set_transient_for(*dynamic_cast<Gtk::Window*>(get_root()));
            win->present();
        });

        append(m_title);
        append(m_scroll);
        append(m_btn_add);

        m_instances = loadInstances();
        refresh();
    }

    void refresh() {
        while (auto child = m_instances_box.get_first_child())
            m_instances_box.remove(*child);

        if (m_instances.empty()) {
            auto lbl = Gtk::make_managed<Gtk::Label>("No instances yet. Click '+ Add Instance'.");
            lbl->set_halign(Gtk::Align::START);
            lbl->add_css_class("subtitle");
            lbl->set_margin(20);
            m_instances_box.append(*lbl);
            return;
        }

        for (int i = 0; i < (int)m_instances.size(); i++) {
            auto& inst = m_instances[i];

            auto card = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
            card->add_css_class("instance-card");

            auto header = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
            header->set_margin(10);

            auto name_lbl = Gtk::make_managed<Gtk::Label>(inst.name);
            name_lbl->set_halign(Gtk::Align::START);
            name_lbl->set_hexpand(true);

            auto ver_lbl = Gtk::make_managed<Gtk::Label>(inst.mcVersion);
            ver_lbl->add_css_class("subtitle");

            auto arrow = Gtk::make_managed<Gtk::Label>("▼");
            arrow->add_css_class("subtitle");

            header->append(*name_lbl);
            header->append(*ver_lbl);
            header->append(*arrow);

            auto actions = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
            actions->set_margin_start(10);
            actions->set_margin_end(10);
            actions->set_margin_bottom(10);
            actions->set_visible(false);

            auto btn_play = Gtk::make_managed<Gtk::Button>("PLAY");
            auto btn_kill = Gtk::make_managed<Gtk::Button>("KILL");
            auto btn_delete = Gtk::make_managed<Gtk::Button>("DELETE");

            int idx = i;
            btn_delete->signal_clicked().connect([this, idx]() {
                m_instances.erase(m_instances.begin() + idx);
                saveInstances(m_instances);
                refresh();
            });

            actions->append(*btn_play);
            actions->append(*btn_kill);
            actions->append(*btn_delete);

            card->append(*header);
            card->append(*actions);

            auto gesture = Gtk::GestureClick::create();
            gesture->signal_released().connect([actions, arrow](int, double, double) {
                bool vis = !actions->get_visible();
                actions->set_visible(vis);
                arrow->set_text(vis ? "▲" : "▼");
            });
            header->add_controller(gesture);

            m_instances_box.append(*card);
            auto sep = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::HORIZONTAL);
            m_instances_box.append(*sep);
        }
    }

private:
    Gtk::Label m_title;
    Gtk::ScrolledWindow m_scroll;
    Gtk::Box m_instances_box;
    Gtk::Button m_btn_add;
    std::vector<Instance> m_instances;
};

class SettingsPage : public Gtk::Box {
public:
    SettingsPage() {
        set_orientation(Gtk::Orientation::VERTICAL);
        set_margin(20);
        set_spacing(15);

        m_title.set_markup("<span font='20' weight='bold'>Settings</span>");
        m_title.set_halign(Gtk::Align::START);

        m_inst_label.set_markup("<b>Instance Settings</b>");
        m_inst_label.set_halign(Gtk::Align::START);

        m_inst_subtitle.set_text("Double-click an instance to edit");
        m_inst_subtitle.set_halign(Gtk::Align::START);
        m_inst_subtitle.add_css_class("subtitle");

        m_scroll.set_size_request(-1, 200);
        m_scroll.set_child(m_inst_list);
        m_inst_list.set_selection_mode(Gtk::SelectionMode::SINGLE);

        m_global_label.set_markup("<b>Global Settings</b>");
        m_global_label.set_halign(Gtk::Align::START);
        m_global_label.set_margin_top(10);

        m_lang_label.set_text("Language");
        m_lang_label.set_halign(Gtk::Align::START);
        m_lang_label.add_css_class("subtitle");
        m_lang_combo.append("en", "English");
        m_lang_combo.append("pl", "Polski");
        m_lang_combo.set_active(0);

        m_dir_label.set_text("Minecraft directory");
        m_dir_label.set_halign(Gtk::Align::START);
        m_dir_label.add_css_class("subtitle");
        m_dir_entry.set_placeholder_text("~/.minecraft");
        m_dir_entry.set_hexpand(true);

        m_btn_save.set_label("Save Global Settings");
        m_btn_save.set_halign(Gtk::Align::START);

        append(m_title);
        append(m_inst_label);
        append(m_inst_subtitle);
        append(m_scroll);
        append(m_global_label);
        append(m_lang_label);
        append(m_lang_combo);
        append(m_dir_label);
        append(m_dir_entry);
        append(m_btn_save);

        refresh();
    }

    void refresh() {
        while (auto child = m_inst_list.get_first_child())
            m_inst_list.remove(*child);

        m_cached_instances = loadInstances();
        for (auto& inst : m_cached_instances) {
            auto row = Gtk::make_managed<Gtk::ListBoxRow>();
            auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
            box->set_margin(8);
            auto name = Gtk::make_managed<Gtk::Label>(inst.name);
            name->set_halign(Gtk::Align::START);
            name->set_hexpand(true);
            auto ver = Gtk::make_managed<Gtk::Label>(inst.mcVersion);
            ver->add_css_class("subtitle");
            box->append(*name);
            box->append(*ver);
            row->set_child(*box);
            m_inst_list.append(*row);
        }

        m_inst_list.signal_row_activated().connect([this](Gtk::ListBoxRow* row) {
            int idx = row->get_index();
            if (idx < 0 || idx >= (int)m_cached_instances.size()) return;
            auto win = new InstanceSettingsWindow(m_cached_instances[idx]);
            win->onSaved = [this](Instance updated) {
                auto all = loadInstances();
                for (auto& i : all)
                    if (i.name == updated.name) i = updated;
                saveInstances(all);
                refresh();
            };
            win->set_transient_for(*dynamic_cast<Gtk::Window*>(get_root()));
            win->present();
        });
    }

private:
    Gtk::Label m_title, m_inst_label, m_inst_subtitle, m_global_label, m_lang_label, m_dir_label;
    Gtk::ScrolledWindow m_scroll;
    Gtk::ListBox m_inst_list;
    Gtk::ComboBoxText m_lang_combo;
    Gtk::Entry m_dir_entry;
    Gtk::Button m_btn_save;
    std::vector<Instance> m_cached_instances;
};

class AccountsPage : public Gtk::Box {
public:
    AccountsPage() {
        set_orientation(Gtk::Orientation::VERTICAL);
        set_margin(20);
        set_spacing(15);

        m_title.set_markup("<span font='20' weight='bold'>Accounts</span>");
        m_title.set_halign(Gtk::Align::START);

        m_subtitle.set_text("Add your username to play");
        m_subtitle.set_halign(Gtk::Align::START);
        m_subtitle.add_css_class("subtitle");

        m_scroll.set_vexpand(true);
        m_scroll.set_child(m_list);
        m_list.set_selection_mode(Gtk::SelectionMode::NONE);

        // Add account form
        m_form_box.set_orientation(Gtk::Orientation::HORIZONTAL);
        m_form_box.set_spacing(10);

        m_name_entry.set_placeholder_text("Enter username...");
        m_name_entry.set_hexpand(true);

        m_btn_add.set_label("+ Add Account");
        m_btn_add.signal_clicked().connect([this]() {
            std::string name = m_name_entry.get_text();
            if (name.empty()) return;
            Account acc;
            acc.username = name;
            acc.active = true;
            auto all = loadAccounts();
            for (auto& a : all) a.active = false;
            all.push_back(acc);
            saveAccounts(all);
            m_name_entry.set_text("");
            refresh();
        });

        m_form_box.append(m_name_entry);
        m_form_box.append(m_btn_add);

        append(m_title);
        append(m_subtitle);
        append(m_scroll);
        append(m_form_box);

        refresh();
    }

    void refresh() {
        while (auto child = m_list.get_first_child())
            m_list.remove(*child);

        auto accounts = loadAccounts();

        if (accounts.empty()) {
            auto row = Gtk::make_managed<Gtk::ListBoxRow>();
            auto lbl = Gtk::make_managed<Gtk::Label>("No accounts. Add your username below.");
            lbl->set_halign(Gtk::Align::START);
            lbl->add_css_class("subtitle");
            lbl->set_margin(20);
            row->set_child(*lbl);
            m_list.append(*row);
            return;
        }

        for (int i = 0; i < (int)accounts.size(); i++) {
            auto& acc = accounts[i];
            auto row = Gtk::make_managed<Gtk::ListBoxRow>();
            auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
            box->set_margin(10);

            auto star = Gtk::make_managed<Gtk::Label>(acc.active ? "*" : "  ");
            star->set_size_request(20, -1);

            auto name = Gtk::make_managed<Gtk::Label>(acc.username);
            name->set_halign(Gtk::Align::START);
            name->set_hexpand(true);

            auto btn_del = Gtk::make_managed<Gtk::Button>("Remove");
            int idx = i;
            btn_del->signal_clicked().connect([this, idx]() {
                auto all = loadAccounts();
                all.erase(all.begin() + idx);
                saveAccounts(all);
                refresh();
            });

            box->append(*star);
            box->append(*name);
            box->append(*btn_del);
            row->set_child(*box);

            auto gesture = Gtk::GestureClick::create();
            int cidx = i;
            gesture->signal_released().connect([this, cidx](int, double, double) {
                auto all = loadAccounts();
                for (auto& a : all) a.active = false;
                all[cidx].active = true;
                saveAccounts(all);
                refresh();
            });
            box->add_controller(gesture);

            m_list.append(*row);
        }
    }

private:
    Gtk::Label m_title, m_subtitle;
    Gtk::ScrolledWindow m_scroll;
    Gtk::ListBox m_list;
    Gtk::Box m_form_box;
    Gtk::Entry m_name_entry;
    Gtk::Button m_btn_add;
};

class LauncherWindow : public Gtk::Window {
public:
    LauncherWindow() {
        set_title("444vx Launcher");
        set_default_size(900, 600);

        auto css = Gtk::CssProvider::create();
        css->load_from_data(CSS);
        Gtk::StyleContext::add_provider_for_display(
            get_display(), css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
        );

        m_box.set_orientation(Gtk::Orientation::HORIZONTAL);
        set_child(m_box);

        m_sidebar.set_orientation(Gtk::Orientation::VERTICAL);
        m_sidebar.set_size_request(160, -1);
        m_sidebar.set_margin(15);
        m_sidebar.set_spacing(5);
        m_sidebar.add_css_class("sidebar");

        m_label_title.set_text("444vx");
        m_label_title.set_halign(Gtk::Align::START);
        m_label_title.add_css_class("title");

        m_btn_home.set_label("Home");
        m_btn_settings.set_label("Settings");
        m_btn_accounts.set_label("Accounts");

        m_sidebar.append(m_label_title);
        m_sidebar.append(m_btn_home);
        m_sidebar.append(m_btn_settings);
        m_sidebar.append(m_btn_accounts);

        m_separator.set_orientation(Gtk::Orientation::VERTICAL);

        m_stack.add(m_home_page, "home");
        m_stack.add(m_settings_page, "settings");
        m_stack.add(m_accounts_page, "accounts");
        m_stack.set_hexpand(true);
        m_stack.set_vexpand(true);

        m_btn_home.signal_clicked().connect([this]() { m_stack.set_visible_child("home"); });
        m_btn_settings.signal_clicked().connect([this]() {
            m_settings_page.refresh();
            m_stack.set_visible_child("settings");
        });
        m_btn_accounts.signal_clicked().connect([this]() { m_stack.set_visible_child("accounts"); });

        m_box.append(m_sidebar);
        m_box.append(m_separator);
        m_box.append(m_stack);
    }

private:
    Gtk::Box m_box, m_sidebar;
    Gtk::Separator m_separator;
    Gtk::Stack m_stack;
    Gtk::Label m_label_title;
    Gtk::Button m_btn_home, m_btn_settings, m_btn_accounts;
    HomePage m_home_page;
    SettingsPage m_settings_page;
    AccountsPage m_accounts_page;
};

int main(int argc, char* argv[]) {
    curl_global_init(CURL_GLOBAL_ALL);
    auto app = Gtk::Application::create("com.444vx.launcher");
    int result = app->make_window_and_run<LauncherWindow>(argc, argv);
    curl_global_cleanup();
    return result;
}
