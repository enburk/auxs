#pragma once
#include <chrono>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include "doc_text_model.h"
namespace doc::text::repo
{
    using path = std::filesystem::path;
    using time = std::filesystem::file_time_type;

    struct source
    {
        path path;
        time edittime;
        text saved_text;
        std::unique_ptr<model> model;

        expected<nothing> load () try
        {
            time filetime = std::filesystem::last_write_time(path);
            if (filetime <= edittime) return nothing{};
            str s;
            {
                std::ifstream stream(path); s = std::string{(
                std::istreambuf_iterator<char>(stream)),
                std::istreambuf_iterator<char>()};
            }
            s.erase_if([](auto c){ return c == '\r'; });
            if (s.starts_with("\xEF" "\xBB" "\xBF"))
                s.upto(3).erase(); // UTF-8 BOM

            edittime = filetime; // prevents recursive load
            model->set_text(s); // triggers compiling
            saved_text = *model;
            return nothing{};
        }
        catch (const std::exception & e) {
            return aux::error("doc::text::repo::source: "
                + path.string() + " : " + e.what());
        }

        expected<nothing> save () try
        {
            time filetime = std::filesystem::last_write_time(path);
            if (edittime <= filetime) return nothing{};
            if (saved_text == *model) {
                edittime = filetime;
                return nothing{};
            }

            std::filesystem::path temp = path;
            temp.replace_extension(temp.extension().string() + "~");
            {
                std::ofstream stream(temp);
                for (auto & line : model->lines)
                    stream << doc::text::string(line) << "\n";
            }
            std::filesystem::rename(temp, path);
            filetime = std::filesystem::last_write_time(path);
            edittime = filetime;
            saved_text = *model;
            return nothing{};
        }
        catch (const std::exception & e) {
            return aux::error("doc::text::repo::source: "
                + path.string() + " : " + e.what());
        }
    };

    inline std::unordered_map<str, source> map;
    inline doc::text::report report;
    inline doc::text::model error;

    template<class Model> auto load (path path) -> model*
    {
        if (path == std::filesystem::path{}) return &error;

        auto & source = map[std::filesystem::canonical(path).string()];

        if (!source.model) {
             source.model = std::move(std::make_unique<Model>());
             source.model->path = path;
             source.path = path;
        }

        auto rc = source.load();
        if (!rc.ok()) report.error(
             rc.error());

        return source.model.get();
    }

    void edit (path path)
    {
        if (path == std::filesystem::path{}) return;

        auto & source = map[std::filesystem::canonical(path).string()];

        source.edittime = time::clock::now();
    }

    void save ()
    {
        for (auto & [path, source] : map)
        {
            auto rc = source.save();
            if (!rc.ok()) report.error(
                 rc.error());
        }
    }

    void reload ()
    {
        std::erase_if(map, [](auto & pair){ return 
            !std::filesystem::exists(pair.second.path); });

        for (auto & [path, source] : map)
        {
            auto rc = source.load();
            if (!rc.ok()) report.error(
                 rc.error());
        }

        for (auto & [path, source] : map)
            source.model->prereanalyze();

        for (auto & [path, source] : map)
            source.model->reanalyze();
    }

    void tick ()
    {
        for (auto & [path, source] : map)
            source.model->tick();
    }
}

