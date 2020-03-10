//
// Created by maxwell on 2020-03-02.
//

#ifndef AFORE_TREE_H
#define AFORE_TREE_H

#include "afore/primitives.h"
#include "afore/algorithm/compare.h"

namespace afore
{
    template <Index, typename>
    struct IndexedType;
    template <Index Ia, Index Ib, typename A, typename B>
    struct detail::Compare<IndexedType<Ia, A>, IndexedType<Ib, B>>
    {
        afore_var equals = Ia == Ib;
        afore_var less_than = Ia < Ib;
    };


    inline namespace concepts
    {

        template <typename T>
        concept TreePolicy = true;/*requires
        {
            //detail::impl::is_type_template<T::Insert>;
            //detail::impl::is_type_template<typename T::find>;
        };*/
    }
    struct NilNode{};
    template <typename V, typename Left, typename Right>
    struct Tree;
    template <typename V, typename Left, typename Right, Index left_size = 0ul, Index right_size = 0ul>
    struct CountedTree;

    namespace detail
    {
        template <typename T>
        afore_var is_counted_tree = false;
        template <typename V, typename L, typename R, Index ls, Index rs>
        afore_var is_counted_tree<CountedTree<V, L, R, ls, rs>> = true;
    }

    inline namespace concepts
    {
        template <typename T>
        concept TreeNode = detail::is_same<T, NilNode> || requires
        {
            typename T::left;
            typename T::right;
        };

        template <typename T>
        concept CountedTreeNode = detail::is_same<T, NilNode> || detail::is_counted_tree<T>;
    }

    namespace detail
    {
        template <TreeNode T, typename Node, TreePolicy policy>
        struct Insert;
        template <typename Node, TreePolicy policy>
        struct Insert<NilNode, Node, policy> :
            AFORE_RESULT_T(typename policy::template Create<Node>);
        template <TreeNode Node, TreePolicy policy>
        struct Insert<NilNode, Node, policy> :
            AFORE_RESULT_T(Node);
        template <TreeNode T, TreeNode Node, TreePolicy policy>
        struct Insert<T, Node, policy> :
            AFORE_NEXT(typename policy::template Insert<T, Node>);
    }

    template <typename V, typename Left = NilNode, typename Right = NilNode>
    struct Tree
    {
        afore_type value = V;
        afore_type left = Left;
        afore_type right = Right;

        template <typename Node, TreePolicy policy>
        struct InsertLeft :
            AFORE_RESULT_T(Tree<V, result_t<detail::Insert<Left, Node, policy>>, Right>);
        template <typename Node, TreePolicy policy>
        struct InsertRight :
            AFORE_RESULT_T(Tree<V, Left, result_t<detail::Insert<Right, Node, policy>>>);
    };
    template <typename V, typename Left = NilNode, typename Right = NilNode, Index left_size, Index right_size>
    struct CountedTree
    {
        afore_type value = V;
        afore_type left = Left;
        afore_type right = Right;
        afore_var ls = left_size;
        afore_var rs = right_size;

        template <typename Node, TreePolicy policy>
        struct InsertLeft :
            AFORE_NEXT(CountedTree<V, detail::Insert<Left, Node, policy>, Right, left_size + 1, right_size>);
        template <typename Node, TreePolicy policy>
        struct InsertRight :
            AFORE_NEXT(CountedTree<V, Left, detail::Insert<Right, Node, policy>, left_size, right_size + 1>);
    };



    template <TreePolicy Comp, TreeNode Tree>
    struct TreeBase{};

    namespace tree_policies
    {
        namespace detail
        {
            template <typename A, typename B>
            struct default_comparison_t :
                AFORE_RESULT_V(compare::less_than<A, B>);
            template <typename A, typename B>
            struct max_heap_t :
                AFORE_RESULT_V(compare::greater_than<A, B>);
        }

        template <template <typename, typename> typename comp = detail::default_comparison_t, template <typename> typename TreeType = Tree>
        struct SortedPolicy
        {
            template <typename Node>
            afore_type Create = TreeType<Node>;

            template <TreeNode T, TreeNode Node>
            struct Insert :
                AFORE_NEXT(std::conditional_t<
                        result_v<comp<typename Node::value, typename T::value>>,
                        typename T::template InsertLeft<Node, SortedPolicy>,
                        typename T::template InsertRight<Node, SortedPolicy>
                    >);

        };
        template <template <typename, typename> typename comp = detail::max_heap_t, template <typename> typename TreeType = Tree>
        struct HeapPolicy
        {
            template <typename Node>
            afore_type Create = TreeType<Node>;

            template <TreeNode T, TreeNode Node>
            struct Insert :
            AFORE_RESULT_T(std::conditional_t<
                result_v<comp<typename Node::value, typename T::value>>,
                typename Node::template InsertLeft<T, HeapPolicy>,
                std::conditional_t<
                    afore::detail::is_same<typename T::left, NilNode>,
                    typename T::template InsertLeft<Node, HeapPolicy>,
                    typename T::template InsertRight<Node, HeapPolicy>
                >
            >);
        };
        struct BalancedPolicy
        {
            template <typename Node>
            afore_type Create = CountedTree<Node>;

            template <CountedTreeNode T, CountedTreeNode Node>
            struct Insert :
                AFORE_RESULT_T(std::conditional_t<
                                T::ls == T::rs,
                                typename T::template InsertLeft<Node, BalancedPolicy>,
                                typename T::template InsertRight<Node, BalancedPolicy>
                           >);
        };

        afore_type DefaultPolicy = SortedPolicy<>;

        namespace detail
        {
            template <typename T>
            afore_var instance_of_sorted_policy = false;
            template <template <typename, typename> typename comp, template <typename> typename TT>
            afore_var instance_of_sorted_policy<SortedPolicy<comp, TT>> = true;

            template <typename T>
            afore_var instance_of_heap_policy = false;
            template <template <typename, typename> typename comp, template <typename> typename TT>
            afore_var instance_of_heap_policy<HeapPolicy<comp, TT>> = true;
        }
    }

    namespace detail::impl
    {
        /*template <typename in, TreeNode tree, TreePolicy policy>
        struct InsertNewNode :
            AFORE_NEXT(result_t<Insert<node, result_t<typename policy::template Create<Value<first_val>>>, policy>>);*/


        template <AforeListType, typename = NilNode, TreePolicy = tree_policies::DefaultPolicy>
        struct MakeTree;
        template <EmptyList empty_input, TreePolicy policy>
        struct MakeTree<empty_input, NilNode, policy> :
            AFORE_RESULT_T(TreeBase<policy, NilNode>);
        template <EmptyList empty_input, typename node, TreePolicy policy>
        struct MakeTree<empty_input, node, policy> :
            AFORE_RESULT_T(TreeBase<policy, node>);


        template <TypeListType list, TreePolicy policy>
        struct MakeTree<list, NilNode, policy> :
            AFORE_NEXT(MakeTree<tail<list>, typename policy::template Create<unwrap_t<head<list>>>, policy>);
        template <auto first_val, auto ...vals, TreePolicy policy>
        struct MakeTree<ValueList<first_val, vals...>, NilNode, policy> :
            AFORE_NEXT(MakeTree<ValueList<vals...>, typename policy::template Create<Value<first_val>>, policy>);

        template <TypeListType list, TreeNode node, TreePolicy policy> requires (NonEmptyList<list>)
        struct MakeTree<list, node, policy> :
            AFORE_NEXT(MakeTree<tail<list>, result_t<Insert<node, typename policy::template Create<unwrap_t<head<list>>>, policy>>, policy>);
        template <auto first_val, auto ...vals, typename tree, TreePolicy policy>
        struct MakeTree<ValueList<first_val, vals...>, tree, policy> :
            AFORE_NEXT(MakeTree<ValueList<vals...>, result_t<Insert<tree, typename policy::template Create<Value<first_val>>, policy>>, policy>);



        template <typename T, typename = NilNode>
        struct FlattenTree;
        template <typename T>
        struct FlattenTree<NilNode, T> :
            AFORE_RESULT_T(TypeList<>);
        template <TreeNode T, TreePolicy policy>
        struct FlattenTree<T, policy> :
            AFORE_RESULT_T(concatenate_lists<result_t<FlattenTree<typename T::left>>, TypeList<typename T::value>, result_t<FlattenTree<typename T::right>>>);
        template <TreePolicy policy, TreeNode T, typename Other>
        struct FlattenTree<TreeBase<policy, T>, Other> :
            AFORE_NEXT(FlattenTree<T, policy>);


        template <typename T, typename = NilNode>
        struct FlattenHeap;
        template <typename T>
        struct FlattenHeap<NilNode, T> :
            AFORE_RESULT_T(TypeList<>);
        template <TreeNode T, TreePolicy policy>
        struct FlattenHeap<T, policy> :
            AFORE_RESULT_T(concatenate_lists<result_t<FlattenHeap<typename T::left>>, result_t<FlattenHeap<typename T::right>>, TypeList<typename T::value>>);
        template <TreePolicy policy, TreeNode T, typename Other>
        struct FlattenHeap<TreeBase<policy, T>, Other> :
            AFORE_NEXT(FlattenHeap<T, policy>);

    }

    template <AforeListType T, TreePolicy policy = tree_policies::DefaultPolicy>
    afore_type make_tree_from_list = result_t<detail::impl::MakeTree<T, NilNode, policy>>;
    template <typename TreeBaseType>
    afore_type flatten_tree = result_t<detail::impl::FlattenTree<TreeBaseType>>;
    template <typename TreeBaseType>
    afore_type flatten_heap = result_t<detail::impl::FlattenHeap<TreeBaseType>>;

}

#endif //AFORE_TREE_H
