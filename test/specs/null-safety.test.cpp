#include "doctest.h"
#include "general-tree.h"

TEST_CASE("node null safety")
{
    SUBCASE("null node methods return null safely")
    {
        general_tree<int> gt;
        auto null_node = gt.root(); // empty tree returns null node
        
        CHECK(null_node.is_null());
        CHECK_FALSE(null_node.is_root());
        CHECK_FALSE(null_node.is_leaf());
        CHECK_FALSE(null_node.has_left_child());
        CHECK_FALSE(null_node.has_right_sibling());
        
        // These should return null nodes without crashing
        auto left_child = null_node.left_child();
        auto right_sibling = null_node.right_sibling();
        auto parent = null_node.parent();
        
        CHECK(left_child.is_null());
        CHECK(right_sibling.is_null());
        CHECK(parent.is_null());
    }
    
    SUBCASE("chained null node calls are safe")
    {
        general_tree<int> gt;
        auto null_node = gt.root();
        
        // Multiple chained calls should not crash
        auto result = null_node.left_child().right_sibling().parent().left_child();
        CHECK(result.is_null());
    }
}
