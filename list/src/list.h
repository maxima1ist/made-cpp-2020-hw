#pragma once
#include <iterator>
#include <utility>

namespace task {

    template<class T, class Allocator = std::allocator<T>>
    class list {
    private:
        struct Node {
            T data;
            Node* next;
            Node* prev;

            Node() : data(value_type()), next(nullptr), prev(nullptr) {}

            Node(const T& data) : data(data), next(nullptr), prev(nullptr) {}

            Node(T&& data) : data(std::move(data)), next(nullptr), prev(nullptr) {}

            Node(Node&& node)
                : data(std::forward<T>(node.data)), next(std::move(node.next)), prev(std::move(node.prev)) {
                node.data = nullptr;
                node.next = nullptr;
                node.prev = nullptr;
            }

            Node& operator=(Node&& node) {
                if (this == &node) {
                    return *this;
                }
                data = std::move(node.data);
                next = std::move(node.next);
                prev = std::move(node.prev);

                node.data = nullptr;
                node.next = nullptr;
                node.prev = nullptr;

                return *this;
            }

            bool operator==(const Node& node) {
                return data == node.data;
            }

            bool operator!=(const Node& node) {
                return data != node.data;
            }

        };

        using allocator_traits = typename std::allocator_traits<Allocator>;

        using node_allocator_type = typename Allocator::template rebind<Node>::other;
        using node_allocator_traits = typename std::allocator_traits<node_allocator_type>;

    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = typename allocator_traits::pointer;
        using const_pointer = typename allocator_traits::const_pointer;

        static_assert(
            std::is_same<typename Allocator::value_type, value_type>::value,
            "Allocator::value_type must be the same type as value_type"
        );

        class iterator_base {
        public:
            friend class list;
            using value_type = T;
            using reference = T&;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using iterator_category = std::random_access_iterator_tag;

        private:
            Node* ptr;
            explicit iterator_base(Node* ptr) : ptr(ptr) {}

        public:
            iterator_base() = delete;
            iterator_base(const iterator_base& other) = default;
            iterator_base(iterator_base&& other) noexcept = default;

            iterator_base& operator=(const iterator_base& other) = default;
            iterator_base& operator=(iterator_base&& other) noexcept = default;

            ~iterator_base() = default;

            reference operator+=(const difference_type n) {
                Node* result = ptr;
                if (n > 0) {
                    for (difference_type i(0); i < n; ++i) {
                        ptr = ptr->next;
                    }
                } else {
                    for (difference_type i(0); i < (-1) * n; ++i) {
                        ptr = ptr->prev;
                    }
                }
                return ptr->data;
            }

            reference operator-=(const difference_type n) {
                reference result = ptr;
                for (difference_type i(0); i < n; ++i) {
                    ptr = ptr->prev;
                }
                return ptr->data;
            }

            reference operator*() const { return ptr->data; }
            pointer operator->() { return &(ptr->data); }

            iterator_base& operator++() {
                ptr = ptr->next;
                return *this;
            }
            iterator_base& operator--() {
                ptr = ptr->prev;
                return *this;
            }
            iterator_base operator++(int) {
                iterator_base t(*this);
                ptr = ptr->next;
                return t;
            }
            iterator_base operator--(int) {
                iterator_base t(*this);
                ptr = ptr->prev;
                return t;
            }

            iterator_base operator-(const difference_type n) {
                reference result = ptr;
                for (difference_type i(0); i < n; ++i) {
                    result = result->prev;
                }
                return iterator_base(result);
            }
            difference_type operator-(const iterator_base& other) {
                difference_type result(0);
                Node* buffer = ptr;
                while (buffer && buffer != other.ptr) {
                    ++result;
                    buffer = buffer->prev;
                }
                return result;
            }

            bool operator<(const iterator_base& other) { return *this != other; }

            bool operator==(const iterator_base& other) { return ptr == (other.ptr); }
            bool operator!=(const iterator_base& other) { return ptr != (other.ptr); }

        };

        using iterator = iterator_base;
        using const_iterator = iterator_base;
        using reverse_iterator = std::reverse_iterator<iterator_base>;
        using const_reverse_iterator = std::reverse_iterator<iterator_base>;

    private:

        node_allocator_type nodeAllocator;

        Node* first;
        Node* last;
        Node* head;
        Node* tail;
        size_type listSize;

        void destroy(Node* buffer) {
            node_allocator_traits::destroy(nodeAllocator, buffer);
            node_allocator_traits::deallocate(nodeAllocator, buffer, sizeof(Node));
            buffer = nullptr;
        }

        Node* create(Node* buffer) {
            buffer = node_allocator_traits::allocate(nodeAllocator, sizeof(Node));
            node_allocator_traits::construct(nodeAllocator, buffer);
            return buffer;
        }

        template<class ...Args>
        Node* create(Node* buffer, Args& ...args) {
            buffer = node_allocator_traits::allocate(nodeAllocator, sizeof(Node));
            node_allocator_traits::construct(nodeAllocator, buffer, args...);
            return buffer;
        }

        template<class ...Args>
        Node* create(Node* buffer, Args&& ...args) {
            buffer = node_allocator_traits::allocate(nodeAllocator, sizeof(Node));
            node_allocator_traits::construct(nodeAllocator, buffer, std::forward<Args>(args)...);
            return buffer;
        }

        void destroy() {
            while (first != nullptr && listSize > 0) {
                Node* buffer = first;
                first = first->next;
                destroy(buffer);
                --listSize;
            }
            tail = nullptr;
            first = last = nullptr;
        }

        void copy(const list& other) {
            listSize = other.listSize;
            Node* buffer;
            if (listSize > 0) {
                last = first = node_allocator_traits::allocate(nodeAllocator, sizeof(Node));
                node_allocator_traits::construct(nodeAllocator, last, other.first->data);
                buffer = other.first->next;
            }
            for (size_type i(1); i < listSize; ++i) {
                last->next = create(last->next, buffer->data);

                Node* bufferForLast = last;
                last = last->next;
                last->prev = bufferForLast;

                buffer = buffer->next;
            }
        }

        void move(list&& other) {
            listSize = other.listSize;
            first = std::move(std::forward<Node*>(other.first));
            Node* buffer = first;
            for (size_type i(1); i < listSize; ++i) {
                buffer = buffer->next;
            }
            last = buffer;
            other.first = nullptr;
            other.last = nullptr;
            other.listSize = 0;
        }

    public:

        list() :
            first(nullptr), last(nullptr), listSize(0), nodeAllocator(allocator_type()), tail(nullptr), head(nullptr) {}

        explicit list(const Allocator& alloc) :
            first(nullptr), last(nullptr), listSize(0), nodeAllocator(alloc), tail(nullptr), head(nullptr) {}

        list(size_type count, const T& value, const Allocator& alloc = Allocator()) :
            listSize(count), nodeAllocator(alloc), tail(nullptr), head(nullptr) {
            if (count > 0) {
                last = first = node_allocator_traits::allocate(nodeAllocator, sizeof(Node));
                node_allocator_traits::construct(nodeAllocator, last, value);
            }
            for (size_type i(1); i < listSize; ++i) {
                last->next = create(last->next, value);

                Node* buffer = last;
                last = last->next;
                last->prev = buffer;
            }
        }

        explicit list(size_type count, const Allocator& alloc = Allocator()) :
            listSize(count), nodeAllocator(alloc), tail(nullptr), head(nullptr) {
            if (count > 0) {
                last = first = node_allocator_traits::allocate(nodeAllocator, sizeof(Node));
                node_allocator_traits::construct(nodeAllocator, last);
            }
            for (size_type i(1); i < count; ++i) {
                last->next = create(last->next);

                Node* buffer = last;
                last = last->next;
                last->prev = buffer;
            }
        }

        ~list() {
            destroy();
        }

        list(const list& other) {
            copy(other);
        }

        list(list&& other) {
            move(std::forward<list>(other));
        }

        list& operator=(const list& other) {
            if (this == &other) {
                return *this;
            }
            destroy();
            copy(other);

            return *this;
        }

        list& operator=(list&& other) {
            destroy();
            move(std::forward<list>(other));

            return *this;
        }

        Allocator get_allocator() const {
            return nodeAllocator;
        }

        T& front() {
            return first->data;
        }

        const T& front() const {
            return first->data;
        }

        T& back() {
            return last->data;
        }

        const T& back() const {
            return last->data;
        }

        iterator begin() {
            return iterator(first);
        }

        iterator end() {
            if (!tail) {
                tail = create(tail);
                tail->prev = last;
                last->next = tail;
            }
            return iterator(tail);
        }

        const_iterator cbegin() const {
            return const_iterator(first);
        }

        const_iterator cend() const {
            if (!tail) {
                tail = create(tail);
                tail->prev = last;
                last->next = tail;
            }
            return const_iterator(tail);
        }

        reverse_iterator rbegin() {
            return reverse_iterator(last);
        }

        reverse_iterator rend() {
            if (!head) {
                head = create(head);
                head->next = first;
                first->next = head;
            }
            return reverse_iterator(head);
        }

        const_reverse_iterator crbegin() {
            if (!tail) {
                tail = create(tail);
                tail->prev = last;
                last->next = tail;
            }
            return const_reverse_iterator(iterator_base(tail));
        }

        const_reverse_iterator crend() {
            return const_reverse_iterator(iterator_base(first));
        }

        bool empty() const {
            return listSize == 0;
        }

        size_t size() const {
            return listSize;
        }

        size_t max_size() const {
            //??
            return SIZE_MAX;
        }

        void clear() {
            destroy();
        }

        iterator insert(iterator it, const value_type& value) {
            Node* buffer = it.ptr;
            if (buffer->next) {
                if (buffer->prev) {
                    Node* newPrev = nullptr;
                    newPrev = create(newPrev, value);
                    buffer->prev->next = newPrev;
                    newPrev->prev = buffer->prev;
                    buffer->prev = newPrev;
                    newPrev->next = buffer;
                    it.ptr = buffer;
                } else {
                    buffer->prev = create(buffer->prev, value);
                    first = buffer->prev;
                    first->next = buffer;
                }
            } else {
                buffer->data = value;
                last = tail;
                tail = nullptr;
                tail = create(tail);
                tail->prev = last;
                last->next = tail;
            }
            ++listSize;
            return it;
        }

        iterator insert(iterator it, value_type&& value) {
            Node* buffer = it.ptr;
            if (buffer) {
                if (buffer->prev) {
                    Node* newPrev = nullptr;
                    newPrev = create(newPrev, std::forward<T>(value));
                    buffer->prev->next = newPrev;
                    newPrev->prev = buffer->prev;
                    buffer->prev = newPrev;
                    newPrev->next = buffer;
                    it.ptr = buffer;
                } else {
                    buffer->prev = create(buffer->prev, std::forward<T>(value));
                    first->prev = buffer->prev;
                    first = buffer->prev;
                    first->next = buffer;
                }
            } else {
                buffer->data = std::forward<value_type>(value);
                last = tail;
                tail = nullptr;
            }
            ++listSize;
            return it;
        }

        iterator insert(iterator it, size_type count, const T& value) {
            for (size_type i(0); i < count; ++i) {
                insert(it, value);
            }
            return it;
        }

        iterator erase(iterator it) {
            Node* buffer = it.ptr;
            if (buffer) {
                if (buffer->next) {
                    buffer->next->prev = buffer->prev;
                } else {
                    last = last->prev;
                }
                if (buffer->prev) {
                    buffer->prev->next = buffer->next;
                } else {
                    first = first->next;
                }
                destroy(buffer);
                --listSize;
            }
            return it;
        }

        iterator erase(iterator first, iterator last) {
            iterator it = first;
            while (true) {
                if (it.ptr) {
                    iterator bufferIt(it.ptr->next);
                    erase(it);
                    it = bufferIt;
                } else {
                    break;
                }
                if (it == last) {
                    break;
                }
            }
            return first;
        }

        void push_back(const T& value) {
            if (listSize > 0) {
                last->next = create(last->next, value);

                Node* buffer = last;
                last = last->next;
                last->prev = buffer;
            } else {
                first = last = node_allocator_traits::allocate(nodeAllocator, sizeof(Node));
                node_allocator_traits::construct(nodeAllocator, last, value);
            }
            ++listSize;
        }

        void push_back(T&& value) {
            if (listSize > 0) {
                last->next = create(last->next, std::forward<T>(value));

                Node* buffer = last;
                last = last->next;
                last->prev = buffer;
            } else {
                first = last = node_allocator_traits::allocate(nodeAllocator, sizeof(Node));
                node_allocator_traits::construct(nodeAllocator, last, std::forward<T>(value));
            }
            ++listSize;
        }

        void pop_back() {
            if (listSize > 0) {
                Node* buffer = last;
                last = last->prev;
                destroy(buffer);
                --listSize;
            }
            if (listSize == 0) {
                first = last = nullptr;
            }
        }

        void push_front(const T& value) {
            if (listSize > 0) {
                first->prev = create(first->prev, value);

                Node* buffer = first;
                first = first->prev;
                first->next = buffer;
            } else {
                first = last = node_allocator_traits::allocate(nodeAllocator, sizeof(Node));
                node_allocator_traits::construct(nodeAllocator, last, value);
            }
            ++listSize;
        }

        void push_front(T&& value) {
            if (listSize > 0) {
                first->prev = create(first->prev, std::forward<T>(value));

                Node* buffer = first;
                first = first->prev;
                first->next = buffer;
            } else {
                first = last = node_allocator_traits::allocate(nodeAllocator, sizeof(Node));
                node_allocator_traits::construct(nodeAllocator, last, std::forward<T>(value));
            }
            ++listSize;
        }

        void pop_front() {
            if (listSize > 0) {
                Node* buffer = first;
                first = first->next;
                destroy(buffer);
                --listSize;
            }
        }

        template<class... Args>
        iterator emplace(const_iterator pos, Args&& ... args) {
            Node* buffer = pos.it.ptr;
            if (buffer) {
                Node* bufferPrev = buffer->prev;
                Node* bufferNext = buffer->next;
                destroy(buffer);
                buffer = create(buffer, std::forward<Args>(args)...);
                buffer->prev = bufferPrev;
                buffer->next = bufferNext;
                if (bufferPrev) {
                    bufferPrev->next = buffer;
                }
                if (bufferNext) {
                    bufferNext->prev = buffer;
                }
            }
        }

        //wtf constructor????
        template<class... Args>
        void emplace_back(Args&& ... args) {
            if (listSize > 0) {
                last->next = create(last->next, std::forward<Args>(args)...);

                Node* buffer = last;
                last = last->next;
                last->prev = buffer;
            } else {
                first = last = node_allocator_traits::allocate(nodeAllocator, sizeof(Node));
                node_allocator_traits::construct(nodeAllocator, last, std::forward<Args>(args)...);
            }
            ++listSize;
        }

        template<class... Args>
        void emplace_front(Args&& ... args) {
            if (listSize > 0) {
                first->prev = create(last->next, std::forward<Args>(args)...);

                Node* buffer = first;
                first = first->prev;
                first->next = buffer;
            } else {
                first = last = node_allocator_traits::allocate(nodeAllocator, sizeof(Node));
                node_allocator_traits::construct(nodeAllocator, last, std::forward<Args>(args)...);
            }
            ++listSize;

        }

        void resize(size_type count) {
            if (count > listSize) {
                for (size_type i(listSize); i < count; ++i) {
                    push_back(value_type());
                }
            } else {
                if (count < listSize) {
                    size_type bufferSize = listSize;
                    for (size_type i(count); i < bufferSize; ++i) {
                        pop_back();
                    }
                }
            }
        }

        void swap(list& other) {
            std::swap(listSize, other.listSize);
            std::swap(first, other.first);
            Node* buffer = first;
            for (size_type i(1); i < listSize; ++i) {
                buffer = buffer->next;
            }
            last = buffer;
            buffer = other.first;
            for (size_type i(1); i < other.listSize; ++i) {
                buffer = buffer->next;
            }
            other.last = buffer;
        }

        void merge(list& other) {
            if (this == &other)
                return;

            Node* newFirst = nullptr;
            Node* newLast = nullptr;
            Node* bufferThis = first;
            Node* bufferOther = other.first;
            if (bufferThis || bufferOther) {
                if (bufferThis) {
                    newLast = newFirst = create(newFirst);
                    newFirst->data = bufferThis->data;
                    bufferThis = bufferThis->next;
                } else { // if (bufferOther)
                    newLast = newFirst = create(newFirst);
                    newFirst->data = bufferOther->data;
                    bufferOther = bufferOther->next;
                }
            }

            while (bufferThis || bufferOther) {
                newLast->next = create(newLast->next);
                if (bufferThis && bufferOther) {
                    if (bufferThis->data <= bufferOther->data) {
                        newLast->next->data = bufferThis->data;
                        bufferThis = bufferThis->next;
                    } else {
                        newLast->next->data = bufferOther->data;
                        bufferOther = bufferOther->next;
                    }
                } else if (bufferThis) {
                    newLast->next->data = bufferThis->data;
                    bufferThis = bufferThis->next;
                } else { // if (bufferOther)
                    newLast->next->data = bufferOther->data;
                    bufferOther = bufferOther->next;
                }
                newLast = newLast->next;
            }
        }

        void splice(const_iterator pos, list& other) {
            Node* buffer = pos.ptr;

            if (buffer && other.listSize > 0) {
                Node* bufferPrev = buffer->prev;
                buffer->prev = other.last;
                other.last->next = buffer;
                bufferPrev->next = other.first;
                other.first->prev = bufferPrev;

                other.listSize = 0;
                other.first = nullptr;
                other.last = nullptr;
            }
        }

        void remove(const T& value) {
            Node* buffer;
            while (buffer != nullptr) {
                if (buffer->data == value) {
                    if (buffer->prev != nullptr) {
                        buffer->prev->next = buffer->next;
                    }
                    if (buffer->next != nullptr) {
                        buffer->next->prev = buffer->prev;
                    }
                    destroy(buffer);
                    return;
                }
                buffer = buffer->next;
            }
        }

        void reverse() {
            Node* newFirst = nullptr;
            Node* newLast = nullptr;
            Node* buffer = first;
            if (listSize > 0) {
                newLast = newFirst = create(newFirst, buffer->data);
                buffer = buffer->next;
            }
            while (buffer != nullptr) {
                newFirst->prev = create(newFirst->prev, buffer->data);
                newFirst = newFirst->prev;
                buffer = buffer->next;
            }

            size_type sizeBuffer = listSize;
            destroy();
            listSize = sizeBuffer;
            first = newFirst;
            last = newLast;
        }

        void unique() {
            Node* currentLeftNode = first;
            Node* currentRightNode = nullptr;
            for (size_type i(0); i < listSize; ++i) {
                currentRightNode = currentLeftNode->next;
                for (size_type j(i); j < listSize; ++j) {
                    if (currentLeftNode->data == currentRightNode->data) {
                        currentLeftNode->next = currentRightNode->next;
                        if (currentRightNode->next) {
                            currentRightNode->next->prev = currentLeftNode;
                        }
                        destroy(currentRightNode);
                        --listSize;
                        currentRightNode = currentLeftNode->next;
                    } else {
                        break;
                    }
                    currentRightNode = currentRightNode->next;
                }
                currentLeftNode = currentLeftNode->next;
            }
        }

        void sort() {
            Node* currentLeftNode = first;
            Node* currentRightNode = nullptr;
            for (size_type i(0); i < listSize; ++i) {
                currentRightNode = currentLeftNode->next;
                for (size_type j(i + 1); j < listSize; ++j) {
                    if (currentLeftNode->data > currentRightNode->data) {
                        std::swap(currentLeftNode->data, currentRightNode->data);
                    }
                    currentRightNode = currentRightNode->next;
                }
                currentLeftNode = currentLeftNode->next;
            }
        }

    };

}  // namespace task

//#include "list.cpp"
