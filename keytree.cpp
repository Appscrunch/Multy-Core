#include "keytree.h"

#include "error.h"
#include "keys.h"

#include <memory>
#include <unordered_map>

namespace
{
    struct KeyDeleter
    {
        inline void operator()(Key* key) const
        {
            free_key(key);
        }
    };
    typedef std::unique_ptr<Key, KeyDeleter> KeyPtr;
} // namespace

struct KeyNode
{
    typedef std::unordered_map<size_t, std::unique_ptr<KeyNode>> ChildrenMap;
    KeyPtr key;
    ChildrenMap children;
};

Error* make_key_tree(Key* root_key, KeyNode** root)
{
    if (!root_key || !root)
    {
        return make_error(ERROR_INVALID_ARGUMENT, "Invalid argument");
    }

    std::unique_ptr<KeyNode> result(new KeyNode);
    result->key.reset(root_key);
    *root = result.release();
    return nullptr;
}

Error* add_child(KeyNode* parent, size_t index, Key* child_key)
{
    if (!parent || !child_key)
    {
        return make_error(ERROR_INVALID_ARGUMENT, "Invalid argument");
    }

    if (parent->children.find(index) != parent->children.end())
    {
        return make_error(ERROR_GENERAL_ERROR, "Child with given index exists");
    }

    std::unique_ptr<KeyNode> child_node(new KeyNode);
    child_node->key.reset(child_key);
    parent->children[index].swap(child_node);

    return nullptr;
}

Error* get_child(KeyNode* parent, size_t index, KeyNode** node)
{
    if (!parent)
    {
        return make_error(ERROR_INVALID_ARGUMENT, "Invalid argument");
    }
    auto i = parent->children.find(index);
    if (i == parent->children.end())
    {
        return make_error(ERROR_GENERAL_ERROR, "No child at given index");
    }
    *node = i->second.get();

    return nullptr;
}

Error* get_key(const KeyNode* node, const Key** key)
{
    if (!node || !key)
    {
        return make_error(ERROR_INVALID_ARGUMENT, "Invalid argument");
    }

    *key = node->key.get();

    return nullptr;
}

Error* remove_child(KeyNode* parent, size_t index)
{
    if (!parent)
    {
        return make_error(ERROR_INVALID_ARGUMENT, "Invalid argument");
    }
    auto i = parent->children.find(index);
    if (i == parent->children.end())
    {
        return make_error(ERROR_GENERAL_ERROR, "No child at given index");
    }
    parent->children.erase(i);

    return nullptr;
}

void free_keynode(KeyNode* key_node)
{
    delete key_node;
}
