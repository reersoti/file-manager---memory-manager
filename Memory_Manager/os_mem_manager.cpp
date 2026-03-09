#include "os_mem.h"

#include <algorithm>
#include <vector>

namespace {

struct Block {
    int addr;
    int size;
};

int g_total_size = 0;
std::vector<Block> g_blocks;

bool is_created() { return g_total_size > 0; }

int get_gap_size(int begin, int end)
{
    return end >= begin ? end - begin : 0;
}

}  // namespace

int my_create(int size, int num_pages)
{
    (void)num_pages;

    if (size <= 0 || is_created()) {
        return 0;
    }

    g_total_size = size;
    g_blocks.clear();
    return 1;
}

int my_destroy()
{
    if (!is_created()) {
        return 0;
    }

    g_total_size = 0;
    g_blocks.clear();
    return 1;
}

int my_get_max_block_size()
{
    if (!is_created()) {
        return 0;
    }

    int max_gap = 0;
    int current_addr = 0;

    for (const Block& block : g_blocks) {
        max_gap = std::max(max_gap, get_gap_size(current_addr, block.addr));
        current_addr = block.addr + block.size;
    }

    max_gap = std::max(max_gap, get_gap_size(current_addr, g_total_size));
    return max_gap;
}

mem_handle_t my_alloc(int block_size)
{
    if (!is_created() || block_size <= 0) {
        return {0, 0};
    }

    int best_gap_addr = -1;
    int best_gap_size = -1;
    int current_addr = 0;
    size_t insert_pos = 0;

    for (size_t i = 0; i < g_blocks.size(); ++i) {
        const int gap_size = get_gap_size(current_addr, g_blocks[i].addr);
        if (gap_size > best_gap_size) {
            best_gap_size = gap_size;
            best_gap_addr = current_addr;
            insert_pos = i;
        }
        current_addr = g_blocks[i].addr + g_blocks[i].size;
    }

    const int tail_gap = get_gap_size(current_addr, g_total_size);
    if (tail_gap > best_gap_size) {
        best_gap_size = tail_gap;
        best_gap_addr = current_addr;
        insert_pos = g_blocks.size();
    }

    if (best_gap_size < block_size) {
        return {0, 0};
    }

    const Block new_block{best_gap_addr, block_size};
    g_blocks.insert(g_blocks.begin() + static_cast<long>(insert_pos), new_block);
    return {new_block.addr, new_block.size};
}

int my_get_free_space()
{
    if (!is_created()) {
        return 0;
    }

    int used = 0;
    for (const Block& block : g_blocks) {
        used += block.size;
    }
    return g_total_size - used;
}

void my_print_blocks()
{
    for (const Block& block : g_blocks) {
        printf("%d %d\n", block.addr, block.size);
    }
}

int my_free(mem_handle_t h)
{
    if (!is_created()) {
        return 0;
    }

    for (auto it = g_blocks.begin(); it != g_blocks.end(); ++it) {
        if (it->addr == h.addr && it->size == h.size) {
            g_blocks.erase(it);
            return 1;
        }
    }

    return 0;
}

mem_handle_t my_get_block(int addr, int size)
{
    if (!is_created() || addr < 0 || size < 0) {
        return {0, 0};
    }

    for (const Block& block : g_blocks) {
        if (block.addr <= addr && addr + size <= block.addr + block.size) {
            return {block.addr, block.size};
        }
    }

    return {0, 0};
}

void setup_memory_manager(memory_manager_t* mm)
{
    mm->create = my_create;
    mm->destroy = my_destroy;
    mm->alloc = my_alloc;
    mm->free = my_free;
    mm->get_block = my_get_block;
    mm->get_max_block_size = my_get_max_block_size;
    mm->get_free_space = my_get_free_space;
    mm->print_blocks = my_print_blocks;
}
