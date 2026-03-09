#include "os_mem.h"

#include <cassert>

int main()
{
    memory_manager_t mm{};
    setup_memory_manager(&mm);

    assert(mm.create(100, 0) == 1);
    assert(mm.create(50, 0) == 0);

    const mem_handle_t first = mm.alloc(10);
    const mem_handle_t second = mm.alloc(20);
    const mem_handle_t third = mm.alloc(5);

    assert(first.addr == 0 && first.size == 10);
    assert(second.addr == 10 && second.size == 20);
    assert(third.addr == 30 && third.size == 5);
    assert(mm.get_free_space() == 65);
    assert(mm.get_max_block_size() == 65);

    assert(mm.free(second) == 1);
    assert(mm.get_free_space() == 85);
    assert(mm.get_max_block_size() == 65);

    const mem_handle_t fourth = mm.alloc(30);
    assert(fourth.addr == 35 && fourth.size == 30);
    assert(mm.get_block(36, 5).addr == 35);
    assert(mm.get_block(36, 5).size == 30);

    assert(mm.free(first) == 1);
    const mem_handle_t fifth = mm.alloc(15);
    assert(fifth.addr == 65 && fifth.size == 15);

    assert(mm.alloc(0) == mem_handle_t(0, 0));
    assert(mm.destroy() == 1);
    assert(mm.destroy() == 0);
}
