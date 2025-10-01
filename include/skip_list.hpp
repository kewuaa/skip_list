#pragma once
#include <vector>
#include <format>
#include <iostream>
template<typename Key, typename Value>
concept SkipListRequires = requires (Key key1, Key key2) {
    key1 == key2;
    key1 < key2;
} && std::is_default_constructible_v<Key> && std::is_default_constructible_v<Value>;


template<typename Key, typename Value>
requires SkipListRequires<Key, Value>
class SkipList;


template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& s, SkipList<Key, Value>& tb) noexcept;


template<typename Key, typename Value>
requires SkipListRequires<Key, Value>
class SkipList {
private:
    template<bool Const>
    class _Iterator;
public:
    friend std::ostream& operator<<<>(std::ostream& s, SkipList& tb) noexcept;

    using iterator = _Iterator<false>;
    using const_iterator = _Iterator<true>;

    SkipList(size_t max_level) noexcept: _head(max_level) {
        srand(time(nullptr));
    }

    SkipList(SkipList&) = delete;
    SkipList& operator=(SkipList&) = delete;
    SkipList(SkipList&&) noexcept = default;
    SkipList& operator=(SkipList&&) noexcept = default;

    ~SkipList() noexcept {
        clear();
    }

    void clear() noexcept {
        Node* node = _head.forwards[0];
        while (node) {
            auto fwd = node->forwards[0];
            node = nullptr;
            delete node;
            node = fwd;
        }
        _size = 0;
    }

    [[nodiscard]] iterator find(const Key& key) noexcept {
        return { const_cast<Node*>(const_cast<const SkipList*>(this)->find(key)._node) };
    }

    [[nodiscard]] const_iterator find(const Key& key) const noexcept {
        const Node* node = &_head;
        for (int level = max_level()-1; level >= 0; --level) {
            const Node* fwd;
            while ((fwd = node->forwards[level])) {
                if (fwd->key() == key) {
                    return { fwd };
                } else if (fwd->key() > key) {
                    break;
                }
                node = fwd;
            }
        }
        return { nullptr };
    }

    template<std::same_as<Value> V>
    iterator insert(const Key& key, V&& value) noexcept {
        if (auto it = find(key); it) {
            it->second = std::forward<Value>(value);
            return it;
        }

        int level = 0;
        while (level+1 < (int)max_level()) {
            if (rand() % 2 == 0) {
                ++level;
            } else {
                break;
            }
        }

        auto new_node = new Node(level+1);
        new_node->item = { key, std::forward<Value>(value) };

        for (Node* node = &_head; level >= 0; --level) {
            Node* fwd;
            while (true) {
                if (!(fwd = node->forwards[level]) || fwd->key() > key) {
                    node->forwards[level] = new_node;
                    new_node->forwards[level] = fwd;
                    break;
                }
                node = fwd;
            }
        }

        ++_size;

        return { new_node };
    }

    iterator remove(const Key& key) noexcept {
        Node* target_node { nullptr };
        Node* node = &_head;
        for (int level = max_level()-1; level >= 0; --level) {
            Node* fwd;
            while ((fwd = node->forwards[level])) {
                if (fwd->key() == key) {
                    node->forwards[level] = fwd ? fwd->forwards[level] : nullptr;
                    target_node = fwd;
                    break;
                } else if (fwd->key() > key) {
                    break;
                }
                node = fwd;
            }
        }
        iterator it { nullptr };
        if (target_node) {
            it = { target_node->forwards[0] };
            delete target_node;
            --_size;
        }
        return it;
    }

    [[nodiscard]] inline Value& operator[](const Key& key) noexcept {
        auto it = find(key);
        if (!it) {
            it = insert(key, Value{});
        }
        return it->second;
    }

    [[nodiscard]] inline const Value& operator[](const Key& key) const {
        return find(key)->second;
    }

    inline iterator begin() noexcept {
        return { _head.forwards[0] };
    }

    inline const_iterator begin() const noexcept {
        return { _head.forwards[0] };
    }

    inline iterator end() noexcept {
        return { nullptr };
    }

    inline const_iterator end() const noexcept {
        return { nullptr };
    }

    inline bool empty() const noexcept {
        return _size > 0;
    }

    inline size_t size() const noexcept {
        return _size;
    }

    inline size_t max_level() const noexcept {
        return _head.forwards.size();
    }
private:
    struct Node {
        std::pair<Key, Value> item {};
        std::vector<Node*> forwards {};

        Node(Node&&) noexcept = default;
        Node& operator=(Node&&) noexcept = default;
        Node(Node&) = delete;
        Node& operator=(Node&) = delete;
        Node(size_t max_level) noexcept: forwards(max_level, nullptr) {};
        inline const auto& key() const noexcept { return item.first; }
        inline const auto& value() const noexcept { return item.second; }

        void operator=(std::nullptr_t) noexcept {
            for (auto& fwd : forwards) {
                fwd = nullptr;
            }
        }
    };

    template<bool Const>
    class _Iterator {
        friend SkipList;
    public:
        inline std::conditional_t<
            Const,
            const std::pair<Key, Value>&,
            std::pair<Key, Value>&
        > operator*() const {
            if (!_node) {
                throw std::runtime_error("null pointer");
            }
            return _node->item;
        }

        inline auto operator->() const noexcept {
            return &(**this);
        }

        inline _Iterator& operator++() noexcept {
            _node = _node ? _node->forwards[0] : nullptr;
            return *this;
        }

        inline _Iterator operator++(int) noexcept {
            _Iterator it { _node };
            _node = _node ? _node->forwards[0] : nullptr;
            return it;
        }

        inline bool operator==(const _Iterator& it) const noexcept {
            return _node == it._node;
        }

        inline operator bool() const noexcept {
            return _node != nullptr;
        }
    private:
        std::conditional_t<Const, const Node*, Node*> _node;

        _Iterator(decltype(_node) node): _node(node) {}
    };

    Node _head {};
    size_t _size { 0 };
};


template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& s, SkipList<Key, Value>& tb) noexcept {
    std::string value;
    std::vector<std::string> lines(tb.max_level());
    auto node = tb._head.forwards[0];
    while (node) {
        for (int level = 0; level < (int)node->forwards.size(); ++level) {
            lines[level].append(std::format("|{:^9}|", node->key()));
        }
        for (int level = node->forwards.size(); level < (int)tb.max_level(); ++level) {
            lines[level].append(std::string(11, ' '));
        }
        value.append(std::format("|{:^9}|", node->value()));
        node = node->forwards[0];
    }
    for (int level = tb.max_level()-1; level >= 0; --level) {
        s << std::format("level {:3}: {}\n", level+1, std::move(lines[level]));
    }
    s << std::format("value    : {}\n", std::move(value));
    return s;
}
