#ifndef WALLET_KEYTREE_H
#define WALLET_KEYTREE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct KeyNode;
struct Key;
struct Error;

/** Constuct a KeyTree from a master key.
 *
 * @param root_key - master key, ownership is transferred.
 * @param root - (out) new KeyTree root, must be freed with free_keynode.
 */
Error* make_key_tree(KeyNode* root_key, Key** root);

/** Add a child node to the parent.
 *
 * @param parent - parent node;
 * @param index - child index;
 * @param key - key of the child, ownership is transferred.
 */
Error* add_child(KeyNode* parent, size_t index, Key* key);

/** Get child node at give index.
 *
 * @param parent - parent node.
 * @param index - index of a child;
 * @param node - (out) child node, null if no child at given index,
 *               ownership is not transferred.
 */
Error* get_child(KeyNode* parent, size_t index, KeyNode** node);

/** Get a key from a node.
 *
 * @param node - pointer to node;
 * @param key - (out) key of the node, ownership is not transferred.
 */
Error* get_key(const KeyNode* node, const Key** key);

/** Remove child node by index.
 *
 * @param parent - parent node;
 * @param index - index of a child node;
 */
Error* remove_child(KeyNode* parent, size_t index);

/// Frees a node and all child nodes, should be called on root only.
void free_keynode(KeyNode*);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // WALLET_KEYTREE_H
