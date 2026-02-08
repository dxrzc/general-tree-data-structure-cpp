#include "general-tree.h"
#include <doctest.h>

TEST_CASE("general_tree::iterator")
{
    SUBCASE("balanced example")
    {
        /*
            1
           / \
          2   3
         / \   \
        4   5   6
        */
        general_tree<int> mytree;
        mytree.create_root(1);
        auto n2 = mytree.insert_left_child(mytree.root(), 2);
        auto n3 = mytree.insert_right_sibling(n2, 3);
        auto n4 = mytree.insert_left_child(n2, 4);
        mytree.insert_right_sibling(n4, 5);
        mytree.insert_left_child(n3, 6);

        SUBCASE("preorder traversal")
        {
            const auto it_type = general_tree<int>::iteration_type::preorder;
            const std::vector<int> expected_result = {1, 2, 4, 5, 3, 6};

            std::vector<int> traversal;
            for (auto it = mytree.begin(it_type); it != mytree.end(); ++it)
                traversal.push_back(*it);

            REQUIRE_EQ(expected_result, traversal);
        }
    }

    SUBCASE("single node")
    {
        /*
            1
        */
        general_tree<int> mytree;
        mytree.create_root(1);

        SUBCASE("preorder traversal")
        {
            const auto it_type = general_tree<int>::iteration_type::preorder;
            const std::vector<int> expected_result = {1};

            std::vector<int> traversal;
            for (auto it = mytree.begin(it_type); it != mytree.end(); ++it)
                traversal.push_back(*it);

            REQUIRE_EQ(expected_result, traversal);
        }
    }

    SUBCASE("linear chain")
    {
        /*
            1
            |
            2
            |
            3
            |
            4
        */
        general_tree<int> mytree;
        mytree.create_root(1);
        auto n2 = mytree.insert_left_child(mytree.root(), 2);
        auto n3 = mytree.insert_left_child(n2, 3);
        mytree.insert_left_child(n3, 4);

        SUBCASE("preorder traversal")
        {
            const auto it_type = general_tree<int>::iteration_type::preorder;
            const std::vector<int> expected_result = {1, 2, 3, 4};

            std::vector<int> traversal;
            for (auto it = mytree.begin(it_type); it != mytree.end(); ++it)
                traversal.push_back(*it);

            REQUIRE_EQ(expected_result, traversal);
        }
    }

    SUBCASE("wide siblings")
    {
        /*
                1
             /  /  \  \
            2  3   4  5
        */
        general_tree<int> mytree;
        mytree.create_root(1);
        auto n2 = mytree.insert_left_child(mytree.root(), 2);
        auto n3 = mytree.insert_right_sibling(n2, 3);
        auto n4 = mytree.insert_right_sibling(n3, 4);
        mytree.insert_right_sibling(n4, 5);

        SUBCASE("preorder traversal")
        {
            const auto it_type = general_tree<int>::iteration_type::preorder;
            const std::vector<int> expected_result = {1, 2, 3, 4, 5};

            std::vector<int> traversal;
            for (auto it = mytree.begin(it_type); it != mytree.end(); ++it)
                traversal.push_back(*it);

            REQUIRE_EQ(expected_result, traversal);
        }
    }

    SUBCASE("irregular depth")
    {
        /*
                1
               / \
              2   3
             /     \
            4       5
                   |
                   6
        */
        general_tree<int> mytree;
        mytree.create_root(1);

        auto n2 = mytree.insert_left_child(mytree.root(), 2);
        auto n3 = mytree.insert_right_sibling(n2, 3);
        mytree.insert_left_child(n2, 4);
        auto n5 = mytree.insert_left_child(n3, 5);
        mytree.insert_left_child(n5, 6);

        SUBCASE("preorder traversal")
        {
            const auto it_type = general_tree<int>::iteration_type::preorder;
            const std::vector<int> expected_result = {1, 2, 4, 3, 5, 6};

            std::vector<int> traversal;
            for (auto it = mytree.begin(it_type); it != mytree.end(); ++it)
                traversal.push_back(*it);

            REQUIRE_EQ(expected_result, traversal);
        }
    }

    SUBCASE("multiple sibling subtrees")
    {
        /*
                1
              /   \
             2     3
            / \   / \
           4   5 6   7
        */
        general_tree<int> mytree;
        mytree.create_root(1);

        auto n2 = mytree.insert_left_child(mytree.root(), 2);
        auto n3 = mytree.insert_right_sibling(n2, 3);
        auto n4 = mytree.insert_left_child(n2, 4);
        mytree.insert_right_sibling(n4, 5);
        auto n6 = mytree.insert_left_child(n3, 6);
        mytree.insert_right_sibling(n6, 7);

        SUBCASE("preorder traversal")
        {
            const auto it_type = general_tree<int>::iteration_type::preorder;
            const std::vector<int> expected_result = {1, 2, 4, 5, 3, 6, 7};

            std::vector<int> traversal;
            for (auto it = mytree.begin(it_type); it != mytree.end(); ++it)
                traversal.push_back(*it);

            REQUIRE_EQ(expected_result, traversal);
        }
    }
}

TEST_CASE("general_tree::const_iterator")
{
    SUBCASE("for each with const tree")
    {
        const general_tree<int> tree(1);
        for (int v : tree)
        {
        }
    }
}
