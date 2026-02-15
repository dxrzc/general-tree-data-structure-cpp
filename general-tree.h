#pragma once

#include <queue>
#include <stdexcept>
#include <utility>

template <typename T>
class general_tree
{
public:
    class node;

    enum class iteration_type
    {
        preorder,
        postorder
    };

private:
    struct private_node
    {
        T m_data;
        private_node* m_right_sibling;
        private_node* m_left_child;
        private_node* m_parent;

        template <typename... Args>
        private_node(Args&&... args)
            : m_right_sibling(nullptr), m_left_child(nullptr), m_parent(nullptr), m_data(std::forward<Args>(args)...)
        {
        }
    };

    private_node* get_initial_node_for_iteration(iteration_type it_type) const
    {
        switch (it_type)
        {
        case iteration_type::preorder:
            return m_root;
        case iteration_type::postorder:
        {
            private_node* current = m_root;
            while (current->m_left_child != nullptr)
                current = current->m_left_child;
            return current;
            break;
        }
        default:
            throw std::invalid_argument("Not implemented");
        }
    }

    template <bool is_const>
    class general_tree_iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = std::conditional_t<is_const, const T&, T&>;
        using pointer = std::conditional_t<is_const, const T*, T*>;

    private:
        using node_ptr = std::conditional_t<is_const, const private_node*, private_node*>;
        node_ptr m_ptr;
        iteration_type m_iteration_type;

        void move_to_the_next_node(iteration_type it_type)
        {
            switch (it_type)
            {
            case iteration_type::preorder:
            {
                if (m_ptr->m_left_child != nullptr)
                    m_ptr = m_ptr->m_left_child;
                else if (m_ptr->m_right_sibling != nullptr)
                    m_ptr = m_ptr->m_right_sibling;
                else
                {
                    // it goes up until it finds either a right sibling or nullptr
                    m_ptr = m_ptr->m_parent;
                    while (m_ptr != nullptr && m_ptr->m_right_sibling == nullptr)
                        m_ptr = m_ptr->m_parent;
                    if (m_ptr != nullptr)
                        m_ptr = m_ptr->m_right_sibling;
                }
                break;
            }
            case iteration_type::postorder:
            {
                if (m_ptr->m_right_sibling != nullptr)
                {
                    m_ptr = m_ptr->m_right_sibling;
                    while (m_ptr->m_left_child != nullptr)
                        m_ptr = m_ptr->m_left_child;
                }
                else
                    m_ptr = m_ptr->m_parent;
                break;
            }
            }
        }

    public:
        explicit general_tree_iterator(
            private_node* node_ptr = nullptr, iteration_type it_type = iteration_type::preorder
        )
            : m_ptr(node_ptr), m_iteration_type(it_type)
        {
        }

        general_tree_iterator& operator++()
        {
            move_to_the_next_node(m_iteration_type);
            return *this;
        }

        general_tree_iterator operator++(int)
        {
            auto aux = *this;
            move_to_the_next_node(m_iteration_type);
            return aux;
        }

        [[nodiscard]] reference operator*() const
        {
            return m_ptr->m_data;
        }

        [[nodiscard]] bool operator==(const general_tree_iterator& other) const
        {
            return m_ptr == other.m_ptr;
        }

        operator general_tree_iterator<true>() const
            requires(!is_const)
        {
            return general_tree_iterator<true>(m_ptr);
        }
    };

    private_node* m_root;

    // - handles null node
    // - sets the pointers to nullptr
    void delete_from_node(private_node* pnode)
    {
        if (pnode == nullptr)
            return;
        const bool is_root = (pnode->m_parent == nullptr);
        const bool is_left_child = !is_root && (pnode == pnode->m_parent->m_left_child);
        const bool is_right_sibling = !is_root && !is_left_child;

        if (is_left_child)
            // set new left child
            pnode->m_parent->m_left_child = pnode->m_right_sibling;

        if (is_right_sibling)
        {
            private_node* aux = pnode->m_parent->m_left_child;
            while (aux->m_right_sibling != pnode)
                aux = aux->m_right_sibling;
            aux->m_right_sibling = pnode->m_right_sibling;
        }

        // Breadth First Algorithm
        // Save the children in the queue and delete the parent

        std::queue<private_node*> queue;
        queue.push(pnode);

        private_node* current = nullptr;
        while (!queue.empty())
        {
            current = queue.front();
            queue.pop();

            for (private_node* child = current->m_left_child; child != nullptr; child = child->m_right_sibling)
                queue.push(child);

            delete current;
        }
    }

    void deep_copy(node n)
    {
        // Breadth First Algorithm
        // Copy children of each node
        if (n.m_node == nullptr)
            return;

        private_node* copy_root = new private_node(n.m_node->m_data);
        m_root = copy_root;

        // keeps track of nodes whose children still need to be copied
        std::queue<std::pair<private_node*, private_node*>> org_copy_relation_queue;
        org_copy_relation_queue.push({n.m_node, copy_root});

        while (!org_copy_relation_queue.empty())
        {
            private_node* original_node = org_copy_relation_queue.front().first;
            private_node* copied_node = org_copy_relation_queue.front().second;
            org_copy_relation_queue.pop();

            private_node* child_original = original_node->m_left_child;
            private_node* prev_copied_child = nullptr;

            // children copy algorithm
            while (child_original != nullptr)
            {
                private_node* child_copy = new private_node(child_original->m_data);
                child_copy->m_parent = copied_node;

                // if it is the first copied child, it goes as left child
                if (prev_copied_child == nullptr)
                    copied_node->m_left_child = child_copy;
                else
                    prev_copied_child->m_right_sibling = child_copy;

                org_copy_relation_queue.push({child_original, child_copy});

                prev_copied_child = child_copy;
                child_original = child_original->m_right_sibling;
            }
        }
    }

public:
    using iterator = general_tree_iterator<false>;
    using const_iterator = general_tree_iterator<true>;

    /**
     * @brief Public node interface
     */
    class node
    {
    private:
        private_node* m_node;
        friend class general_tree;

    public:
        node(private_node* node = nullptr) noexcept : m_node(node) {}

        bool operator==(const node& other) const noexcept
        {
            return m_node == other.m_node;
        }

        /**
         * @brief Retrieves the left child of the current node.
         * @return The left child node, or a null node if the current node has no left child.
         */
        [[nodiscard]] node left_child() const noexcept
        {
            return m_node->m_left_child;
        }

        /**
         * @brief Retrieves the parent of the current node.
         * @return The parent node, or a null node if the current node has no parent.
         */
        [[nodiscard]] node parent() const noexcept
        {
            return m_node->m_parent;
        }

        /**
         * @brief Retrieves the right sibling of the current node.
         * @return The right sibling node, or a null node if the current node has no right sibling.
         */
        [[nodiscard]] node right_sibling() const noexcept
        {
            return m_node->m_right_sibling;
        }

        /**
         * @brief Accesses the data stored in the given node.
         * @return const T& Reference to the data stored in the node.
         * @throws std::invalid_argument If the given node is null.
         */
        [[nodiscard]] const T& data() const
        {
            if (!m_node)
                throw std::invalid_argument("Cannot get data from null node");
            return m_node->m_data;
        }

        /**
         * @brief Accesses the data stored in the given node.
         * @return T& Reference to the data stored in the node.
         * @throws std::invalid_argument If the given node is null.
         */
        [[nodiscard]] T& data()
        {
            return const_cast<T&>(static_cast<const general_tree<T>::node&>(*this).data());
        }

        /*
         * @brief Checks if the node is the root of the tree.
         * @return true if the node is the root, false otherwise.
         */
        bool is_root() const noexcept
        {
            return m_node->m_parent == nullptr;
        }

        /**
         * @brief Checks if the node is a leaf in the tree.
         * @return true if the node is a leaf, false otherwise.
         */
        bool is_leaf() const noexcept
        {
            return m_node->m_left_child == nullptr;
        }

        /**
         * @brief Checks if the node has a right sibling.
         * @return true if the node has a right sibling, false otherwise.
         */
        bool has_right_sibling() const noexcept
        {
            return m_node->m_right_sibling != nullptr;
        }

        /**
         * @brief Checks if the node has a left child.
         * @return true if the node has a left child, false otherwise.
         */
        bool has_left_child() const noexcept
        {
            return m_node->m_left_child != nullptr;
        }

        /**
         * @brief Checks if the node is null.
         * @return true if the node is null, false otherwise.
         */
        bool is_null() const noexcept
        {
            return m_node == nullptr;
        }

        /**
         * @brief Retrieves the child node at the specified index.
         * @param index The zero-based index of the child to retrieve.
         * @return The child node at the specified index, or a null node if the index is out of range.
         * @throws std::invalid_argument If the current node is null.
         */
        [[nodiscard]] node child(std::size_t index) const
        {
            if (m_node == nullptr)
                throw std::invalid_argument("Cannot get child of null node");

            node child = m_node->m_left_child;

            for (std::size_t i = 0; i < index; i++)
            {
                if (child.m_node == nullptr)
                    return nullptr;
                child = child.m_node->m_right_sibling;
            }

            return child;
        }

        /**
         * @brief Counts the number of children of the current node.
         * @return The total number of children of the current node.
         * @throws std::invalid_argument If the current node is null.
         */
        [[nodiscard]] std::size_t children_count() const
        {
            if (m_node == nullptr)
                throw std::invalid_argument("Cannot count children of null node");

            std::size_t count = 0;
            for (const private_node* child = m_node->m_left_child; child != nullptr; child = child->m_right_sibling)
                ++count;

            return count;
        }

        /**
         * @brief Computes the depth of the current node in the tree.
         * @throws std::invalid_argument if the node is null.
         * @return std::size_t The depth of the node.
         */
        [[nodiscard]] std::size_t depth() const
        {
            if (m_node == nullptr)
                throw std::invalid_argument("Cannot get depth of null node");

            private_node* aux = m_node;
            std::size_t depth = 0;

            while (aux->m_parent != nullptr)
            {
                ++depth;
                aux = aux->m_parent;
            }

            return depth;
        }

        /**
         * @brief Computes the total number of descendants of the current node.
         * @return The total number of descendants of the current node.
         * @throws std::invalid_argument If the current node is null.
         */
        [[nodiscard]] std::size_t descendants_count() const
        {
            if (m_node == nullptr)
                throw std::invalid_argument("Cannot get descendants count of null node");

            std::size_t count = 0;
            std::queue<private_node*> q;
            q.push(m_node);

            while (!q.empty())
            {
                private_node* current_node = q.front();
                q.pop();

                for (private_node* left_child = current_node->m_left_child; left_child != nullptr;
                     left_child = left_child->m_right_sibling)
                {
                    q.push(left_child);
                    ++count;
                }
            }

            return count;
        }
    };

    general_tree() noexcept : m_root(nullptr) {}

    general_tree(general_tree<T>&& rhs) noexcept : m_root(std::exchange(rhs.m_root, nullptr)) {}

    general_tree(const general_tree<T>& other) : general_tree<T>()
    {
        deep_copy(other.root());
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    general_tree(U&& root_value) : general_tree()
    {
        m_root = new private_node(std::forward<U>(root_value));
    }

    general_tree& operator=(const general_tree<T>& other)
    {
        if (this != &other)
        {
            clear();
            deep_copy(other.root());
        }
        return *this;
    }

    general_tree& operator=(general_tree&& other) noexcept
    {
        if (this != &other)
        {
            clear();
            m_root = other.m_root;
            other.m_root = nullptr;
        }
        return *this;
    }

    bool operator==(const general_tree<T>& other) const
    {
        if (m_root == other.m_root)
            return true;

        if (m_root == nullptr || other.m_root == nullptr)
            return false;

        if (m_root->m_data != other.m_root->m_data)
            return false;

        // Breadth First Algorithm
        // Compare the children of the equal nodes

        std::queue<std::pair<private_node*, private_node*>> equal_nodes;
        equal_nodes.push({m_root, other.m_root});

        while (!equal_nodes.empty())
        {
            private_node* tree1_node = equal_nodes.front().first;
            private_node* tree2_node = equal_nodes.front().second;
            equal_nodes.pop();

            // compare every child of both nodes
            private_node* tree1_node_current_child = tree1_node->m_left_child;
            private_node* tree2_node_current_child = tree2_node->m_left_child;
            while (tree1_node_current_child != nullptr && tree2_node_current_child != nullptr)
            {
                if (tree1_node_current_child->m_data != tree2_node_current_child->m_data)
                    return false;

                equal_nodes.push({tree1_node_current_child, tree2_node_current_child});

                tree2_node_current_child = tree2_node_current_child->m_right_sibling;
                tree1_node_current_child = tree1_node_current_child->m_right_sibling;
            }

            // a node contains more children than the other
            if (tree1_node_current_child != nullptr || tree2_node_current_child != nullptr)
                return false;
        }

        return true;
    }

    /**
     * @brief Creates and emplaces the root node of the tree with the given arguments.
     * @tparam Args Variadic template parameters representing the types of arguments to be forwarded to the root node
     * constructor.
     * @param args Arguments to forward to the root node constructor.
     * @return node A handle to the newly created root node.
     * @throws std::runtime_error If a root node already exists.
     */
    template <typename... Args>
    node emplace_root(Args&&... args)
    {
        if (m_root != nullptr)
            throw std::runtime_error("Root already exists");
        m_root = new private_node(std::forward<Args>(args)...);
        return m_root;
    }

    /**
     * @brief Creates the root node of the tree
     * @param data The value to store in the root node.
     * @return node A handle to the newly created root node.
     * @throws std::runtime_error If a root node already exists.
     */
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    node create_root(U&& data)
    {
        return emplace_root(std::forward<U>(data));
    }

    /**
     * @brief Creates and emplaces a new left child node for the given destination node.
     * @tparam Args Variadic template parameters for the new node constructor.
     * @param destiny The node to which the new left child will be attached.
     * @param args Arguments to forward to the new node's constructor.
     * @return node A handle to the newly created left child node.
     * @throws std::invalid_argument If the destination node is null.
     */
    template <typename... Args>
    node emplace_left_child(node destiny, Args&&... args)
    {
        if (destiny.m_node == nullptr)
            throw std::invalid_argument("Cannot insert left child to null node");

        private_node* new_node = new private_node(std::forward<Args>(args)...);
        new_node->m_right_sibling = destiny.m_node->m_left_child;
        new_node->m_parent = destiny.m_node;

        destiny.m_node->m_left_child = new_node;
        return new_node;
    }

    /**
     * @brief Inserts a new left child node for the given destination node, with the provided value.
     * @param destiny The node to which the left child will be inserted.
     * @param new_node_value The value to store in the newly created left child node.
     * @return node A handle to the newly created left child node.
     * @throws std::invalid_argument If the destination node is null.
     */
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    node insert_left_child(node destiny, U&& new_node_value)
    {
        return emplace_left_child(destiny, std::forward<U>(new_node_value));
    }

    /**
     * @brief Inserts an entire subtree as the left child of the given destination node.
     * @param destiny The node to which the tree will be inserted as a left child.
     * @param tree The tree to insert as a left child. After insertion, this tree will be empty.
     * @return node A handle to the newly inserted left child node, or a null node if the tree is empty.
     * @throws std::invalid_argument If the destination node is null or if attempting to insert the tree as a child of
     * its own root.
     */
    node insert_left_child(node destiny, general_tree<T>& tree)
    {
        if (!destiny.m_node)
            throw std::invalid_argument("Cannot insert left child to null node");

        if (destiny.m_node == tree.m_root)
            throw std::invalid_argument("Cannot insert a tree as its own child");

        if (tree.m_root)
        {
            tree.m_root->m_parent = destiny.m_node;
            tree.m_root->m_right_sibling = destiny.m_node->m_left_child;
            destiny.m_node->m_left_child = tree.m_root;
            tree.m_root = nullptr;
            return destiny.m_node->m_left_child;
        }

        return node(nullptr);
    }

    /**
     * @brief Inserts an entire subtree as the right sibling of the given destination node.
     * @param destiny The node to which the tree will be inserted as a right sibling.
     * @param tree The tree to insert as a right sibling. After insertion, this tree will be empty.
     * @return node A handle to the newly inserted right sibling node, or a null node if the tree is empty.
     * @throws std::invalid_argument If the destination node is null, is the root (cannot have siblings),
     *                               or if attempting to insert the tree as its own sibling.
     */
    node insert_right_sibling(node destiny, general_tree<T>& tree)
    {
        if (destiny.m_node == nullptr)
            throw std::invalid_argument("Cannot insert right sibling to null node");

        if (destiny.m_node->m_parent == nullptr)
            throw std::invalid_argument("Cannot insert right sibling to root");

        if (destiny.m_node == tree.m_root)
            throw std::invalid_argument("Cannot insert a tree as its own sibling");

        if (tree.m_root)
        {
            tree.m_root->m_parent = destiny.m_node->m_parent;
            tree.m_root->m_right_sibling = destiny.m_node->m_right_sibling;
            destiny.m_node->m_right_sibling = tree.m_root;
            tree.m_root = nullptr;
            return destiny.m_node->m_right_sibling;
        }

        return node(nullptr);
    }

    /**
     * @brief Creates and emplaces a new right sibling for the given destination node.
     * @tparam Args Variadic template parameters for the new node constructor.
     * @param destiny The node to which the new right sibling will be attached.
     * @param args Arguments to forward to the new node's constructor.
     * @return node A handle to the newly created right sibling node.
     * @throws std::invalid_argument If the destination node is null or is the root (cannot have a sibling).
     */
    template <typename... Args>
    node emplace_right_sibling(node destiny, Args&&... args)
    {
        if (destiny.m_node == nullptr)
            throw std::invalid_argument("Cannot insert right sibling to null node");

        if (destiny.m_node->m_parent == nullptr)
            throw std::invalid_argument("Cannot insert right sibling to root");

        private_node* new_node = new private_node(std::forward<Args>(args)...);
        new_node->m_parent = destiny.m_node->m_parent;
        new_node->m_right_sibling = destiny.m_node->m_right_sibling;
        destiny.m_node->m_right_sibling = new_node;
        return new_node;
    }

    /**
     * @brief Inserts a new right sibling node for the given destination node, with the provided value.
     * @param destiny The node to which the right sibling will be inserted.
     * @param new_node_value The value to store in the newly created right sibling node.
     * @return node A handle to the newly created right sibling node.
     * @throws std::invalid_argument If the destination node is null or is the root (cannot have a sibling).
     */
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    node insert_right_sibling(node destiny, U&& new_node_value)
    {
        return emplace_right_sibling(destiny, std::forward<U>(new_node_value));
    }

    /**
     * @brief Returns the root node of the tree.
     */
    [[nodiscard]] node root() const noexcept
    {
        return m_root;
    }

    /**
     * @brief Checks whether the tree is empty.
     */
    bool empty() const noexcept
    {
        return m_root == nullptr;
    }

    /**
     * @brief Clears all nodes from the tree.
     */
    void clear()
    {
        delete_from_node(m_root);
        m_root = nullptr;
    }

    void delete_right_sibling(node n)
    {
        if (n.is_null())
            throw std::invalid_argument("Can not delete right sibling of null node");

        if (n.is_root())
            throw std::invalid_argument("Can not delete right sibling of root node");

        delete_from_node(n.m_node->m_right_sibling);
    }

    void delete_left_child(node n)
    {
        if (n.is_null())
            throw std::invalid_argument("Can not delete left child of null node");

        delete_from_node(n.m_node->m_left_child);
    }

    ~general_tree()
    {
        clear();
    }

    [[nodiscard]] iterator begin(iteration_type it_type = iteration_type::preorder)
    {
        private_node* pnode = get_initial_node_for_iteration(it_type);
        return iterator(pnode, it_type);
    }

    [[nodiscard]] iterator end(iteration_type it_type = iteration_type::preorder)
    {
        return iterator(nullptr, it_type);
    }

    [[nodiscard]] const_iterator begin(iteration_type it_type = iteration_type::preorder) const
    {
        private_node* pnode = get_initial_node_for_iteration(it_type);
        return const_iterator(pnode, it_type);
    }

    [[nodiscard]] const_iterator end(iteration_type it_type = iteration_type::preorder) const
    {
        return const_iterator(nullptr, it_type);
    }

    [[nodiscard]] const_iterator cbegin(iteration_type it_type = iteration_type::preorder) const
    {
        return begin();
    }

    [[nodiscard]] const_iterator cend() const
    {
        return end();
    }
};
