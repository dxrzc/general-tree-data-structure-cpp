#include "doctest.h"
#include "general-tree.h"

TEST_CASE("deep copy parent pointers")
{
    SUBCASE("copied tree maintains correct parent pointers")
    {
        // Create a tree with multiple levels
        general_tree<int> original(1);
        auto child1 = original.insert_left_child(original.root(), 2);
        auto child2 = original.insert_right_sibling(child1, 3);
        auto grandchild1 = original.insert_left_child(child1, 4);
        auto grandchild2 = original.insert_left_child(child2, 5);
        
        // Copy the tree
        general_tree<int> copy(original);
        
        // Verify parent relationships in copied tree
        auto copy_root = copy.root();
        CHECK_FALSE(copy_root.is_null());
        CHECK(copy_root.is_root());
        CHECK_EQ(copy_root.data(), 1);
        
        auto copy_child1 = copy_root.left_child();
        CHECK_FALSE(copy_child1.is_null());
        CHECK_EQ(copy_child1.data(), 2);
        CHECK(copy_child1.parent() == copy_root);
        
        auto copy_child2 = copy_child1.right_sibling();
        CHECK_FALSE(copy_child2.is_null());
        CHECK_EQ(copy_child2.data(), 3);
        CHECK(copy_child2.parent() == copy_root);
        
        auto copy_grandchild1 = copy_child1.left_child();
        CHECK_FALSE(copy_grandchild1.is_null());
        CHECK_EQ(copy_grandchild1.data(), 4);
        CHECK(copy_grandchild1.parent() == copy_child1);
        
        auto copy_grandchild2 = copy_child2.left_child();
        CHECK_FALSE(copy_grandchild2.is_null());
        CHECK_EQ(copy_grandchild2.data(), 5);
        CHECK(copy_grandchild2.parent() == copy_child2);
    }
    
    SUBCASE("copy assignment maintains correct parent pointers")
    {
        general_tree<int> original(10);
        auto child = original.insert_left_child(original.root(), 20);
        original.insert_left_child(child, 30);
        
        general_tree<int> copy;
        copy = original; // Copy assignment
        
        auto copy_child = copy.root().left_child();
        CHECK(copy_child.parent() == copy.root());
        
        auto copy_grandchild = copy_child.left_child();
        CHECK(copy_grandchild.parent() == copy_child);
    }
}
