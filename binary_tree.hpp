//
// Created by Patrick Thomas on 08/06/2025
//

#ifndef BINARY_TREE_HPP
#define BINARY_TREE_HPP

#include <cstddef>
#include <memory>
#include <initializer_list>
#include <stdexcept>
#include <ranges>
#include <compare>
#include <concepts>


namespace adt {

    template<class T, class Allocator = std::allocator<T>>
    class binary_tree {
    public:
        /* ----------------------------------------------Definitions------------------------------------------------ */
        using value_type = T;

        using allocator_type = Allocator;

        using size_type = std::size_t;

        using difference_type = std::ptrdiff_t;

        using reference = value_type&;

        using const_reference = const value_type&;

        using pointer = typename std::allocator_traits<allocator_type>::pointer;

        using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

    protected:
        /* -------------------------------------------------Node---------------------------------------------------- */
        struct _Node {
            /* --------------------------------------------Fields--------------------------------------------------- */
            value_type value;

            _Node* parent;

            _Node* left;

            _Node* right;

            size_type height;

            /* -----------------------------------------Constructors------------------------------------------------ */
            constexpr _Node() noexcept
                : value(value_type()), parent(nullptr), left(nullptr),right(nullptr), height(0) {}

            constexpr _Node(value_type value) noexcept
                : value(value), parent(nullptr), left(nullptr), right(nullptr), height(0) {}

            constexpr _Node(value_type value, _Node* parent, _Node* left, _Node* right, size_type height) noexcept 
                : value(value), parent(parent), left(left), right(right), height(height) {
                // If there is a left child...
                if (this->left != nullptr) {
                    // If there is a right child...
                    if (this->right != nullptr) {
                        // Set the height to 1 plus the larger height of the two children
                        this->height = 1 + std::max<size_type>({this->left->height, this->right->height});
                    } else {
                        // Otherwise, if there is NO right child, then set 
                        // the height to 1 plus the left child's height
                        this->height = 1 + this->left->height;
                    }
                } else {
                    // Otherwise, if there is a right child, but NO left child...
                    if (this->right != nullptr) {
                        // Set the height to 1 plus the right child's height
                        this->height = 1 + this->right->height;
                    } else {
                        // Otherwise, if there are no children, then set the height to 0
                        this->height = 0;
                    }
                }

                // Do nothing if `*this` is the root node
                if (parent == nullptr) {
                    return;
                }

                // If `*this` is `parent`'s left child...
                if (value < this->parent->value) {
                    // Point `parent`'s left child to `this`
                    this->parent->left = this;
                } else if (value > this->parent->value) {
                    // Otherwise, if `*this` is `parent`'s right child, 
                    // then point `parent`'s right child to `this`
                    this->parent->right = this;
                }

                // Otherwise, do nothing if `this` is the same as `parent`
            }

            constexpr _Node(const _Node&) noexcept = default;

            constexpr _Node(_Node&&) noexcept = default;
            
            /* ------------------------------------------Destructor------------------------------------------------- */
            constexpr ~_Node() noexcept = default;

            /* -------------------------------------Overloaded Operators-------------------------------------------- */
            constexpr _Node& operator=(const _Node&) noexcept = default;

            constexpr _Node& operator=(_Node&&) noexcept = default;

            constexpr bool operator==(const _Node&) const noexcept = default;

            constexpr auto operator<=>(const _Node&) const noexcept = default;

        };

        /* ----------------------------------------------Definitions------------------------------------------------ */
        using allocator_traits = typename std::allocator_traits<allocator_type>;

        using _NodeAllocator = typename allocator_traits::template rebind_alloc<_Node>;

        using node_allocator_traits = typename std::allocator_traits<_NodeAllocator>; 

        /* ------------------------------------------------Fields--------------------------------------------------- */
        _Node* root;

        allocator_type allocator;

        _NodeAllocator node_allocator;

        size_type sz;

        /* ------------------------------------------------Methods-------------------------------------------------- */
        constexpr _Node* _construct_node(const_reference value) noexcept {
            // Create the node
            _Node* node = node_allocator_traits::allocate(this->node_allocator, 1);
            node_allocator_traits::construct(this->node_allocator, node, value);

            return node;
        }

        constexpr _Node* _construct_node(const_reference value,
                                        _Node* parent, 
                                        _Node* left,
                                        _Node* right,
                                        size_type height) noexcept {
            // Create the node
            _Node* node = node_allocator_traits::allocate(this->node_allocator, 1);
            node_allocator_traits::construct(this->node_allocator, node, value, parent, left, right, height);

            return node;
        }

        constexpr _Node* _destroy_node(_Node* node) noexcept {
            if (node == nullptr) {
                return nullptr;
            }

	        // If the current node being deleted is the root node...
	        if (node->parent == nullptr) {
		        // Delete the root node and exit
		        node_allocator_traits::destroy(this->node_allocator, node);
                node_allocator_traits::deallocate(this->node_allocator, node, 1);
		        return nullptr;
	        }

            // Save a copy of the node's parent
            _Node* parent = node->parent;

	        // If the current node is a left child...
	        if (parent->left == node) {
		        // Point the node's address to nullptr using the parent node's left child pointer
		        parent->left = nullptr;
	        } else if (parent->right == node) {
		        // Otherwise, if the current node is a right child, then point the node's address 
		        // to nullptr using the parent node's right child pointer
		        parent->right = nullptr;
	        }

            // Delete the node
            node_allocator_traits::destroy(this->node_allocator, node);
            node_allocator_traits::deallocate(this->node_allocator, node, 1);

            return parent;
        }
    
    public:
        /* --------------------------------------------Constant Iterator-------------------------------------------- */
        class const_iterator {
        private:
            /* --------------------------------------------Friends-------------------------------------------------- */
            friend class binary_tree;
            
        protected:
            /* ---------------------------------------------Fields-------------------------------------------------- */
            const _Node* node;

            /* ------------------------------------------Constructors----------------------------------------------- */
            constexpr const_iterator(const _Node* const& node) noexcept : node(node) {}

        public:
            /* -------------------------------------------Definitions----------------------------------------------- */
            using iterator_category = std::bidirectional_iterator_tag;

            using value_type = typename binary_tree::value_type;

            using allocator_type = typename binary_tree::allocator_type;

            using size_type = typename binary_tree::size_type;

            using difference_type = typename binary_tree::difference_type;

            using reference = value_type&;

            using const_reference = const value_type&;

            using pointer = typename std::allocator_traits<allocator_type>::pointer;

            using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

            /* ------------------------------------------Constructors----------------------------------------------- */
            constexpr const_iterator() noexcept : node(nullptr) {}

            constexpr const_iterator(std::nullptr_t) noexcept : node(nullptr) {}

            constexpr explicit const_iterator(const binary_tree& tree) noexcept : node(tree.root) {}

            constexpr const_iterator(const const_iterator&) noexcept = default;

            constexpr const_iterator(const_iterator&&) noexcept = default;

            /* -------------------------------------------Destructor------------------------------------------------ */
            constexpr ~const_iterator() noexcept = default;

            /* ---------------------------------------Overloaded Operators------------------------------------------ */
            constexpr const_iterator& operator=(const const_iterator&) noexcept = default;

            constexpr const_iterator& operator=(const_iterator&&) noexcept = default;

            constexpr const_iterator& operator=(std::nullptr_t) noexcept {
                this->node = nullptr;
                return *this;
            }

            [[nodiscard]] constexpr bool operator==(const const_iterator&) const noexcept = default;

            [[nodiscard]] constexpr bool operator==(std::nullptr_t) const noexcept { return this->node == nullptr; }

            [[nodiscard]] constexpr auto operator<=>(const const_iterator&) const noexcept = default;

            [[nodiscard]] constexpr auto operator<=>(std::nullptr_t) const noexcept { return this->node <=> nullptr; }

            [[nodiscard]] const_reference operator*() const {
                if (this->node == nullptr) {
                    throw std::runtime_error("segmentation fault");
                }
                return this->node->value;
            }

            [[nodiscard]] const_pointer operator->() const {
                if (this->node == nullptr) {
                    throw std::runtime_error("segmentation fault");
                }
                return &(this->node->value);
            }

            const_iterator& operator++();

            const_iterator& operator+=(size_type);

            const_iterator& operator--();

            const_iterator& operator-=(size_type);

        };

        /* ------------------------------------------------Iterator------------------------------------------------- */
        class iterator {
        private:
            /* --------------------------------------------Friends-------------------------------------------------- */
            friend class binary_tree;

            friend class const_iterator;

        protected:
            /* ---------------------------------------------Fields-------------------------------------------------- */
            _Node* node;

            /* ------------------------------------------Constructors----------------------------------------------- */
            constexpr iterator(_Node* const& node) noexcept : node(node) {}

        public:
            /* -------------------------------------------Definitions----------------------------------------------- */
            using iterator_category = std::bidirectional_iterator_tag;

            using value_type = typename binary_tree::value_type;

            using allocator_type = typename binary_tree::allocator_type;

            using size_type = typename binary_tree::size_type;

            using difference_type = typename binary_tree::difference_type;

            using reference = value_type&;

            using const_reference = const value_type&;

            using pointer = typename std::allocator_traits<allocator_type>::pointer;

            using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

            /* ------------------------------------------Constructors----------------------------------------------- */
            constexpr iterator() noexcept : node(nullptr) {}

            constexpr explicit iterator(std::nullptr_t) noexcept : node(nullptr) {}

            constexpr explicit iterator(const binary_tree& tree) noexcept : node(tree.root) {}

            constexpr iterator(const iterator&) noexcept = default;

            constexpr iterator(iterator&&) noexcept = default;

            /* -------------------------------------------Destructor------------------------------------------------ */
            constexpr ~iterator() noexcept = default;

            /* ---------------------------------------Overloaded Operators------------------------------------------ */
            constexpr iterator& operator=(const iterator&) noexcept = default;

            constexpr iterator& operator=(iterator&&) noexcept = default;

            constexpr iterator& operator=(std::nullptr_t) noexcept {
                this->node = nullptr;
                return *this;
            }

            [[nodiscard]] constexpr bool operator==(const iterator&) const noexcept = default;

            [[nodiscard]] constexpr bool operator==(std::nullptr_t) const noexcept { return this->node == nullptr; }

            [[nodiscard]] constexpr auto operator<=>(const iterator&) const noexcept = default;

            [[nodiscard]] constexpr auto operator<=>(std::nullptr_t) const noexcept { return this->node <=> nullptr; }

            [[nodiscard]] reference operator*() const {
                if (this->node == nullptr) {
                    throw std::runtime_error("segmentation fault");
                }
                return this->node->value;
            }

            [[nodiscard]] pointer operator->() const {
                if (this->node == nullptr) {
                    throw std::runtime_error("segmentation fault");
                }
                return &(this->node->value);
            }

            iterator& operator++();

            iterator& operator+=(size_type);

            iterator& operator--();

            iterator& operator-=(size_type);

            [[nodiscard]] constexpr operator const_iterator() const noexcept { return const_iterator(node); }
            
        };

        /* ---------------------------------------Constant Reverse Iterator----------------------------------------- */
        class const_reverse_iterator : public const_iterator {
        private:
            /* --------------------------------------------Friends-------------------------------------------------- */
            friend class binary_tree;

        protected:
            /* ------------------------------------------Constructors----------------------------------------------- */
            constexpr const_reverse_iterator(const _Node* const& node) noexcept : const_iterator(node) {}

        public:
            /* ------------------------------------------Constructors----------------------------------------------- */
            constexpr const_reverse_iterator() noexcept : const_iterator() {}

            constexpr explicit const_reverse_iterator(std::nullptr_t) noexcept : const_iterator(nullptr) {}

            constexpr explicit const_reverse_iterator(const binary_tree& tree) noexcept : const_iterator(tree) {}

            constexpr const_reverse_iterator(const const_reverse_iterator&) noexcept = default;

            constexpr const_reverse_iterator(const_reverse_iterator&&) noexcept = default;

            /* ------------------------------------------Destructor------------------------------------------------- */
            constexpr ~const_reverse_iterator() noexcept = default;

            /* -------------------------------------Overloaded Operators-------------------------------------------- */
            constexpr const_reverse_iterator& operator=(const const_reverse_iterator&) noexcept = default;

            constexpr const_reverse_iterator& operator=(const_reverse_iterator&&) noexcept = default;

            constexpr const_reverse_iterator& operator=(std::nullptr_t) noexcept {
                this->node = nullptr;
                return *this;
            }

            [[nodiscard]] constexpr bool operator==(const const_reverse_iterator&) const noexcept = default;

            [[nodiscard]] constexpr bool operator==(std::nullptr_t) const noexcept { return this->node == nullptr; }

            [[nodiscard]] constexpr auto operator<=>(const const_reverse_iterator&) const noexcept = default;

            [[nodiscard]] constexpr auto operator<=>(std::nullptr_t) const noexcept { return this->node <=> nullptr; }

            const_reverse_iterator& operator++();

            const_reverse_iterator& operator+=(size_type);

            const_reverse_iterator& operator--();

            const_reverse_iterator& operator-=(size_type);

        };

        /* -------------------------------------------Reverse Iterator---------------------------------------------- */
        class reverse_iterator : public iterator {
        private:
            /* --------------------------------------------Friends-------------------------------------------------- */
            friend class binary_tree;

        protected:
            /* ------------------------------------------Constructors----------------------------------------------- */
            constexpr reverse_iterator(_Node* const& node) noexcept : iterator(node) {}

        public:
            /* ------------------------------------------Constructors----------------------------------------------- */
            constexpr reverse_iterator() noexcept : iterator() {}

            constexpr explicit reverse_iterator(std::nullptr_t) noexcept : iterator(nullptr) {}

            constexpr explicit reverse_iterator(const binary_tree& tree) noexcept : iterator(tree) {}

            constexpr reverse_iterator(const reverse_iterator&) noexcept = default;

            constexpr reverse_iterator(reverse_iterator&&) noexcept = default;

            /* ------------------------------------------Destructor------------------------------------------------- */
            constexpr ~reverse_iterator() noexcept = default;

            /* -------------------------------------Overloaded Operators-------------------------------------------- */
            constexpr reverse_iterator& operator=(const reverse_iterator&) noexcept = default;

            constexpr reverse_iterator& operator=(reverse_iterator&&) noexcept = default;

            constexpr reverse_iterator& operator=(std::nullptr_t) noexcept {
                this->node = nullptr;
                return *this;
            }

            [[nodiscard]] constexpr bool operator==(const reverse_iterator&) const noexcept = default;

            [[nodiscard]] constexpr bool operator==(std::nullptr_t) const noexcept { return this->node == nullptr; }

            [[nodiscard]] constexpr auto operator<=>(const reverse_iterator&) const noexcept = default;

            [[nodiscard]] constexpr auto operator<=>(std::nullptr_t) const noexcept { return this->node <=> nullptr; }

            reverse_iterator& operator++();

            reverse_iterator& operator+=(size_type);

            reverse_iterator& operator--();

            reverse_iterator& operator-=(size_type);

            [[nodiscard]] constexpr operator const_reverse_iterator() const noexcept {
                return const_reverse_iterator(this->node);
            }

        };

        /* -----------------------------------------------Node Type------------------------------------------------- */
        class node_type {
        private:
            /* --------------------------------------------Friends-------------------------------------------------- */
            friend class binary_tree;

        public:
            /* ------------------------------------------Definitions------------------------------------------------ */
            using allocator_type = _NodeAllocator;

        protected:
            /* --------------------------------------------Fields--------------------------------------------------- */
            _Node* node;

            allocator_type allocator;

            /* --------------------------------------------Methods-------------------------------------------------- */
            constexpr void _construct(const _Node* other) noexcept {
                this->node = node_allocator_traits::allocate(this->allocator, 1);
                node_allocator_traits::construct(this->allocator, this->node, other->value);
            }

            constexpr void _destroy() noexcept {
                if (this->node == nullptr) {
                    return;
                }

                node_allocator_traits::destroy(this->allocator, this->node);
                node_allocator_traits::deallocate(this->allocator, this->node, 1);

                this->node = nullptr;
            }

        public:
            /* ------------------------------------------Constructors----------------------------------------------- */
            constexpr node_type() noexcept : node(nullptr) {}

            constexpr node_type(std::nullptr_t) noexcept : node(nullptr) {}

            constexpr node_type(const node_type& other) noexcept {
                if (other.node == nullptr) {
                    this->node = nullptr;
                    return;
                }

                this->_construct(other.node);
                this->allocator = other.allocator;
            }

            constexpr node_type(node_type&& other) noexcept {
                if (other.node == nullptr) {
                    this->node = nullptr;
                    return;
                }

                this->node = other.node;
                other.node = nullptr;

                this->allocator = other.allocator;
                other.allocator = allocator_type();
            }

            /* -------------------------------------------Destructor------------------------------------------------ */
            constexpr ~node_type() noexcept { this->_destroy(); }

            /* ---------------------------------------Overloaded Operators------------------------------------------ */
            constexpr node_type& operator=(const node_type& other) noexcept {
                // Protect against self-assignment
                if (this == &other) {
                    return *this;
                }

                this->_destroy();

                if (other.node == nullptr) {
                    return *this;
                }

                this->_construct(other.node);
                this->allocator = other.allocator;

                return *this;
            }

            constexpr node_type& operator=(node_type&& other) noexcept {
                this->_destroy();

                this->node = other.node;
                other.node = nullptr;

                this->allocator = other.allocator;
                other.allocator = allocator_type();

                return *this;
            }

            constexpr node_type& operator=(const_iterator pos) noexcept {
                this->_destroy();

                if (pos == nullptr) {
                    this->node = nullptr;
                    return *this;
                }

                this->node = pos.node;

                return *this;
            }

            [[nodiscard]] constexpr bool operator==(const node_type&) const noexcept = default;

            [[nodiscard]] constexpr bool operator==(std::nullptr_t) const noexcept { return this->node == nullptr; }

            [[nodiscard]] constexpr auto operator<=>(const node_type&) const noexcept = default;

            [[nodiscard]] constexpr auto operator<=>(std::nullptr_t) const noexcept { return this->node <=> nullptr; }

            [[nodiscard]] reference operator*() const {
                if (this->node == nullptr) {
                    throw std::runtime_error("segmentation fault");
                }
                return this->node->value;
            }

            [[nodiscard]] pointer operator->() const {
                if (this->node == nullptr) {
                    throw std::runtime_error("segmentation fault");
                }

                return &(this->node->value);
            }

            [[nodiscard]] constexpr explicit operator bool() const noexcept { return this->node != nullptr; }

            /* --------------------------------------------Methods-------------------------------------------------- */
            [[nodiscard]] constexpr bool empty() const noexcept { return this->node == nullptr; }

            [[nodiscard]] reference value() const {
                if (this->node == nullptr) {
                        throw std::runtime_error("segmentation fault");
                }
                return this->node->value;
            }

            constexpr void swap(node_type& other) noexcept {
                _Node* temp = this->node;
                this->node = other.node;
                other.node = temp;
            }

            [[nodiscard]] constexpr allocator_type get_allocator() const noexcept { return this->allocator; }

        };
    
        /* -------------------------------------------Insert Return Type-------------------------------------------- */
        template<std::input_iterator InputIt = iterator, class NodeType = node_type>
        struct insert_return_type {
            /* --------------------------------------------Fields--------------------------------------------------- */
            InputIt position;

            bool inserted;

            NodeType node;

            /* -----------------------------------------Constructors------------------------------------------------ */
            constexpr insert_return_type() noexcept : inserted(false) {}

            constexpr insert_return_type(InputIt position, bool inserted, NodeType node) noexcept
                : position(position), inserted(inserted), node(node) {}

            constexpr insert_return_type(const insert_return_type&) noexcept = default;

            constexpr insert_return_type(insert_return_type&&) noexcept = default;

            /* ------------------------------------------Destructor------------------------------------------------- */
            constexpr ~insert_return_type() noexcept = default;

            /* -------------------------------------Overloaded Operators-------------------------------------------- */
            constexpr insert_return_type& operator=(const insert_return_type&) noexcept = default;

            constexpr insert_return_type& operator=(insert_return_type&&) noexcept = default;

            [[nodiscard]] constexpr bool operator==(const insert_return_type&) const noexcept = default;

            [[nodiscard]] constexpr auto operator<=>(const insert_return_type&) const noexcept = default;

        };

        /* ----------------------------------------------Constructors----------------------------------------------- */
        constexpr binary_tree() noexcept : root(nullptr), sz(0) {}

        constexpr explicit binary_tree(const allocator_type& allocator) noexcept 
            : root(nullptr), allocator(allocator), sz(0) {}

        constexpr binary_tree(const binary_tree&) noexcept = default;
        
        constexpr binary_tree(binary_tree&&) noexcept = default;

        /* -----------------------------------------------Destructor------------------------------------------------ */
        constexpr virtual ~binary_tree() noexcept = default;

        /* ------------------------------------------Overloaded Operators------------------------------------------- */
        constexpr binary_tree& operator=(const binary_tree&) noexcept = default;

        constexpr binary_tree& operator=(binary_tree&&) noexcept = default;

        [[nodiscard]] constexpr bool operator==(const binary_tree&) const noexcept = default;

        [[nodiscard]] constexpr auto operator<=>(const binary_tree&) const noexcept = default;

        /* ------------------------------------------------Methods-------------------------------------------------- */
        [[nodiscard]] constexpr size_type size() const noexcept { return this->sz; }

        [[nodiscard]] constexpr size_type max_size() const noexcept {
            return std::numeric_limits<difference_type>::max();
        }

        [[nodiscard]] constexpr bool empty() const noexcept { return this->sz == 0; }

        constexpr virtual void clear() noexcept = 0;

        constexpr virtual void insert(std::initializer_list<value_type>) noexcept = 0;

        [[nodiscard]] virtual constexpr bool contains(const_reference) const noexcept = 0;

    };

} // adt


#endif // BINARY_TREE_HPP

