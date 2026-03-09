#include "os_file.h"

#include <cstring>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

struct Node {
    std::string name;
    bool is_file = false;
    int size = 0;
    Node* parent = nullptr;
    std::vector<std::unique_ptr<Node>> children;
};

std::unique_ptr<Node> g_root;
Node* g_current = nullptr;
int g_disk_size = 0;
int g_used_size = 0;

bool is_created() { return g_root != nullptr; }

bool is_valid_name_char(char ch)
{
    return (ch >= 'a' && ch <= 'z') ||
           (ch >= 'A' && ch <= 'Z') ||
           (ch >= '0' && ch <= '9') ||
           ch == '_' || ch == '.';
}

bool is_valid_name(const std::string& name)
{
    if (name.empty() || name.size() > 32 || name == "." || name == "..") {
        return false;
    }

    for (char ch : name) {
        if (!is_valid_name_char(ch)) {
            return false;
        }
    }

    return true;
}

std::vector<std::string> split_path(const char* path)
{
    std::vector<std::string> parts;
    std::string current;

    if (path == nullptr) {
        return parts;
    }

    for (const char* it = path; *it != '\0'; ++it) {
        if (*it == '/') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
            continue;
        }
        current.push_back(*it);
    }

    if (!current.empty()) {
        parts.push_back(current);
    }

    return parts;
}

Node* find_child(Node* parent, const std::string& name)
{
    if (parent == nullptr || parent->is_file) {
        return nullptr;
    }

    for (const auto& child : parent->children) {
        if (child->name == name) {
            return child.get();
        }
    }

    return nullptr;
}

std::string build_path(const Node* node)
{
    if (node == nullptr) {
        return "";
    }

    std::vector<std::string> parts;
    const Node* current = node;

    while (current != nullptr && current->parent != nullptr) {
        parts.push_back(current->name);
        current = current->parent;
    }

    if (parts.empty()) {
        return "/";
    }

    std::string result;
    for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
        result.push_back('/');
        result += *it;
    }
    return result;
}

std::string build_child_path(Node* parent, const std::string& name)
{
    std::string base = build_path(parent);
    if (base != "/") {
        base.push_back('/');
    }
    base += name;
    return base;
}

Node* resolve_node(const char* path)
{
    if (!is_created() || path == nullptr || *path == '\0') {
        return nullptr;
    }

    if (std::strcmp(path, "/") == 0) {
        return g_root.get();
    }

    Node* current = path[0] == '/' ? g_root.get() : g_current;
    const auto parts = split_path(path);

    for (const std::string& part : parts) {
        if (part == ".") {
            continue;
        }
        if (part == "..") {
            if (current->parent != nullptr) {
                current = current->parent;
            }
            continue;
        }

        current = find_child(current, part);
        if (current == nullptr) {
            return nullptr;
        }
    }

    return current;
}

bool resolve_parent_and_name(const char* path, Node*& parent, std::string& name)
{
    if (!is_created() || path == nullptr || *path == '\0') {
        return false;
    }

    const auto parts = split_path(path);
    if (parts.empty()) {
        return false;
    }

    name = parts.back();
    if (!is_valid_name(name)) {
        return false;
    }

    parent = path[0] == '/' ? g_root.get() : g_current;

    for (size_t i = 0; i + 1 < parts.size(); ++i) {
        const std::string& part = parts[i];

        if (part == ".") {
            continue;
        }
        if (part == "..") {
            if (parent->parent != nullptr) {
                parent = parent->parent;
            }
            continue;
        }

        Node* next = find_child(parent, part);
        if (next == nullptr || next->is_file) {
            return false;
        }
        parent = next;
    }

    return parent != nullptr && !parent->is_file;
}

bool is_inside_subtree(const Node* root, const Node* node)
{
    const Node* current = node;
    while (current != nullptr) {
        if (current == root) {
            return true;
        }
        current = current->parent;
    }
    return false;
}

int subtree_file_size(const Node* node)
{
    if (node == nullptr) {
        return 0;
    }

    if (node->is_file) {
        return node->size;
    }

    int total = 0;
    for (const auto& child : node->children) {
        total += subtree_file_size(child.get());
    }
    return total;
}

bool moved_subtree_fits(const Node* node, const std::string& parent_path, const std::string& new_name)
{
    std::string absolute_path = parent_path;
    if (absolute_path != "/") {
        absolute_path.push_back('/');
    }
    absolute_path += new_name;

    if (absolute_path.size() > 128) {
        return false;
    }

    if (node->is_file) {
        return true;
    }

    for (const auto& child : node->children) {
        if (!moved_subtree_fits(child.get(), absolute_path, child->name)) {
            return false;
        }
    }

    return true;
}

std::unique_ptr<Node> detach_node(Node* node)
{
    if (node == nullptr || node->parent == nullptr) {
        return nullptr;
    }

    auto& siblings = node->parent->children;
    for (auto it = siblings.begin(); it != siblings.end(); ++it) {
        if (it->get() == node) {
            std::unique_ptr<Node> removed = std::move(*it);
            siblings.erase(it);
            return removed;
        }
    }

    return nullptr;
}

}  // namespace

int my_create(int disk_size)
{
    if (disk_size <= 0 || is_created()) {
        return 0;
    }

    g_root = std::make_unique<Node>();
    g_current = g_root.get();
    g_disk_size = disk_size;
    g_used_size = 0;
    return 1;
}

int my_destroy()
{
    if (!is_created()) {
        return 0;
    }

    g_current = nullptr;
    g_root.reset();
    g_disk_size = 0;
    g_used_size = 0;
    return 1;
}

int my_create_dir(const char* path)
{
    Node* parent = nullptr;
    std::string name;

    if (!resolve_parent_and_name(path, parent, name)) {
        return 0;
    }

    if (find_child(parent, name) != nullptr) {
        return 0;
    }

    if (build_child_path(parent, name).size() > 128) {
        return 0;
    }

    auto node = std::make_unique<Node>();
    node->name = name;
    node->is_file = false;
    node->parent = parent;
    parent->children.push_back(std::move(node));
    return 1;
}

int my_create_file(const char* path, int file_size)
{
    Node* parent = nullptr;
    std::string name;

    if (file_size <= 0 || !resolve_parent_and_name(path, parent, name)) {
        return 0;
    }

    if (find_child(parent, name) != nullptr) {
        return 0;
    }

    if (g_used_size + file_size > g_disk_size) {
        return 0;
    }

    if (build_child_path(parent, name).size() > 128) {
        return 0;
    }

    auto node = std::make_unique<Node>();
    node->name = name;
    node->is_file = true;
    node->size = file_size;
    node->parent = parent;
    parent->children.push_back(std::move(node));
    g_used_size += file_size;
    return 1;
}

int my_remove(const char* path, int recursive)
{
    if (!is_created()) {
        return 0;
    }

    Node* node = resolve_node(path);
    if (node == nullptr || node == g_root.get()) {
        return 0;
    }

    if (!node->is_file && !recursive && !node->children.empty()) {
        return 0;
    }

    const bool reset_current = is_inside_subtree(node, g_current);
    g_used_size -= subtree_file_size(node);
    std::unique_ptr<Node> removed = detach_node(node);

    if (!removed) {
        return 0;
    }

    if (reset_current) {
        g_current = g_root.get();
    }

    return 1;
}

int my_change_dir(const char* path)
{
    Node* node = resolve_node(path);
    if (node == nullptr || node->is_file) {
        return 0;
    }

    g_current = node;
    return 1;
}

void my_get_cur_dir(char* dst)
{
    if (dst == nullptr) {
        return;
    }

    if (!is_created()) {
        dst[0] = '\0';
        return;
    }

    const std::string path = build_path(g_current);
    std::strcpy(dst, path.c_str());
}

int my_move(const char* old_path, const char* new_path)
{
    if (!is_created()) {
        return 0;
    }

    Node* node = resolve_node(old_path);
    if (node == nullptr || node == g_root.get()) {
        return 0;
    }

    Node* new_parent = nullptr;
    std::string new_name;
    if (!resolve_parent_and_name(new_path, new_parent, new_name)) {
        return 0;
    }

    if (find_child(new_parent, new_name) != nullptr) {
        return 0;
    }

    if (!node->is_file && is_inside_subtree(node, new_parent)) {
        return 0;
    }

    if (!moved_subtree_fits(node, build_path(new_parent), new_name)) {
        return 0;
    }

    const bool reset_current = is_inside_subtree(node, g_current);
    std::unique_ptr<Node> moved = detach_node(node);
    if (!moved) {
        return 0;
    }

    moved->name = new_name;
    moved->parent = new_parent;
    new_parent->children.push_back(std::move(moved));

    if (reset_current) {
        g_current = g_root.get();
    }

    return 1;
}

void setup_file_manager(file_manager_t* fm)
{
    fm->create = my_create;
    fm->destroy = my_destroy;
    fm->create_dir = my_create_dir;
    fm->create_file = my_create_file;
    fm->remove = my_remove;
    fm->change_dir = my_change_dir;
    fm->get_cur_dir = my_get_cur_dir;
    fm->move = my_move;
}
