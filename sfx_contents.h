#pragma once
#include "sfx.h"
namespace sfx
{
    struct contents:
    widget<contents>
    {
        gui::list list;
        array<int> indices;

        struct record
        {
            str  path;
            str  name;
            int  level = 0;
            bool open = false;
            bool file = false;
            bool operator == (record const&) const = default;
            bool operator != (record const&) const = default;
        };
        typedef array<record> Records;
        binary_property<Records> records;
        binary_property<str> selected;
        property<bool> selected_open = false;

        std::function<str(record)> pretty = [](record r){
            return r.file ? r.name:
            "<b>"+ r.name +"</b>"; };

        void refresh ()
        {
            list.refresh();
        }

        void replane ()
        {
            array<record*> stack;
            if  (selected.now != ""
            and  selected_open.now)
            for (auto& record: records.now)
            {
                if (record.file)
                {
                    if (record.path == selected.now)
                    {
                        while (not
                        stack.empty()) {
                        stack.back()->open = true;
                        stack.pop_back(); }
                        break;
                    }
                }
                else
                if (stack.empty()
                or  stack.back()->level < record.level)
                    stack += &record;
                else
                if (
                not stack.empty()
                and stack.back()->level == record.level)
                    stack.back() = &record;
                else {
                while (
                not stack.empty()
                and stack.back()->level >= record.level)
                    stack.pop_back();
                    stack += &record;
                }
            }

            int n = 0;
            int index =-1;
            int level = 1;
            str list_of_opens;
            indices.clear();
            for (auto& record: records.now)
            {
                if (record.open and not record.file)
                list_of_opens += record.path + ";";

                index++;
                if (record.level > level)
                    continue;

                auto mspace = "\xE2""\x80""\x83";
                auto minus  = (char*)(u8"− ");

                str html;
                for (int i=0; i<record.level-1; i++) html += mspace;
                html += record.file ? mspace : record.open ? minus : "+ ";
                html += pretty(record);

                indices += index;
                auto& it = list.lines(n++);
                it.text.alignment = xy(pix::left, pix::center);
                it.kind = record.file ? gui::button::sticky : gui::button::normal;
                it.on = record.file and record.path == selected.now;
                it.text.html = html;

                if (not record.file)
                level = record.open ?
                        record.level+1:
                        record.level;
            }
            list.lines.truncate(n);
            if (name.now != "") sys::settings::save(
                name.now + "::open", list_of_opens);
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size) list.coord =
                coord.now.local(),
                refresh();

            if (what == &records)
            {
                str path = selected.now;
                std::unordered_set<str> opens;

                if (records.was.empty() and name.now != "")
                {
                    str
                    open = sys::settings::load(name.now+"::open", "");
                    path = sys::settings::load(name.now+"::path", "");
                    for (str s: open.split_by(";"))
                    opens.emplace(s);
                }
                else
                for (auto& record: records.was)
                if (record.open) opens.emplace(
                    record.path);

                bool found = false;
                for (auto& record: records.now) {
                if (opens.contains(record.path)) record.open = true;
                if (record.file and record.path == path) found = true; }
                if (not found)
                    path = "";

                selected = path;
                replane();
                refresh();
            }

            if (what == &list)
            {
                int i = list.selected;
                if (i >= 0 and i < indices.size())
                {
                    i = indices[i];
                    if (i >= 0 and i < records.now.size())
                    {
                        if (records.now[i].file) selected =
                            records.now[i].path; else {
                            records.now[i].open = not
                            records.now[i].open;
                            replane();
                            refresh();
                        }
                    }
                }
            }

            if (what == &selected)
            {
                if (name.now != "") sys::settings::save(
                    name.now + "::path", selected.now);

                replane();
                refresh();
                notify();
            }
        }
    };

    struct dirtree:
    widget<dirtree>
    {
        typedef contents::record record;
        typedef contents::Records Records;
        typedef std::filesystem::path path;

        contents contents;
        binary_property<path> root;
        binary_property<path> selected;

        sys::directory_watcher watcher;
        std::atomic<bool> reload = true;
        gui::timer timer;

        std::function<bool(path)> filter = [](path p){
            return not str(p.filename()).
                starts_with("."); };

        Records fill(path dir, int level = 1)
        {
            Records records;

            using namespace std::filesystem;

            for (directory_iterator next(dir), end; next != end; ++next)
            {
                path p = next->path();
                if (not filter(p))
                    continue;

                record r;
                r.path = str(relative(p));
                r.name = str(p.filename());
                r.open = is_regular_file(p);
                r.file = is_regular_file(p);
                r.level = level;
                records += r;

                if (!r.file)
                records += fill(p, level+1);
            }

            return records;
        }

        void on_change (void* what) override
        {
            if (what == &coord)
                contents.coord =
                coord.now.local();

            if (what == &name)
                contents.
                name =
                name;

            if (what == &root)
            {
                watcher.cancel();
                watcher.dir = root.now;
                watcher.action = [this]
                (path path, str what) {
                if (filter(path))
                reload = true; };
                watcher.watch();
                reload = true;
                timer.start();
            }

            if (what == &timer and reload)
            {
                reload = false;
                contents.records =
                fill(root.now);
            }

            if (what == &contents)
            {
                selected = sys::str2path(contents.
                selected);
            }

            if (what == &selected)
            {
                contents.selected = str(
                relative(selected));
                notify();
            }
        }
    };
}
