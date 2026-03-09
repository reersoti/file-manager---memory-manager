#include "os_file.h"

#include <cassert>
#include <cstring>

int main()
{
    file_manager_t fm{};
    setup_file_manager(&fm);

    char current_dir[256] = {};

    assert(fm.create(10) == 1);
    assert(fm.create(10) == 0);

    fm.get_cur_dir(current_dir);
    assert(std::strcmp(current_dir, "/") == 0);

    assert(fm.create_dir("/docs") == 1);
    assert(fm.create_dir("/docs/sub") == 1);
    assert(fm.create_dir("/.") == 0);
    assert(fm.create_file("/docs/sub/readme.txt", 4) == 1);
    assert(fm.create_file("/docs/sub/data.bin", 7) == 0);

    assert(fm.change_dir("/docs") == 1);
    fm.get_cur_dir(current_dir);
    assert(std::strcmp(current_dir, "/docs") == 0);

    assert(fm.create_file("note.txt", 3) == 1);
    assert(fm.move("/docs/note.txt", "/docs/sub/note.txt") == 1);
    assert(fm.move("/docs", "/docs/sub/archive") == 0);

    assert(fm.change_dir("/docs/sub") == 1);
    fm.get_cur_dir(current_dir);
    assert(std::strcmp(current_dir, "/docs/sub") == 0);

    assert(fm.remove("/docs", 1) == 1);
    fm.get_cur_dir(current_dir);
    assert(std::strcmp(current_dir, "/") == 0);

    assert(fm.create_dir("/src") == 1);
    assert(fm.create_dir("/dst") == 1);
    assert(fm.create_dir("/src/lib") == 1);
    assert(fm.change_dir("/src") == 1);
    assert(fm.move("/src", "/dst/src") == 1);

    fm.get_cur_dir(current_dir);
    assert(std::strcmp(current_dir, "/") == 0);

    assert(fm.change_dir("/dst/src") == 1);
    assert(fm.create_file("../shared.txt", 2) == 1);
    assert(fm.remove("/dst", 0) == 0);

    assert(fm.destroy() == 1);
    assert(fm.destroy() == 0);
}
