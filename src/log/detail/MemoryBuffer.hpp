#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <string_view>

namespace RST::Log::detail {

    class MemoryBuffer {
    public:
        using value_type = char;

        static constexpr std::size_t kInlineCapacity = 512;

        MemoryBuffer() noexcept = default;
        MemoryBuffer(const MemoryBuffer&) = delete;
        MemoryBuffer& operator=(const MemoryBuffer&) = delete;

        void push_back(char c)
        {
            if (_size == _capacity) {
                grow(_size + 1);
            }
            _data[_size++] = c;
        }

        void append(std::string_view text)
        {
            if (_size + text.size() > _capacity) {
                grow(_size + text.size());
            }
            std::memcpy(_data + _size, text.data(), text.size());
            _size += text.size();
        }

        void clear() noexcept { _size = 0; }

        [[nodiscard]] std::string_view view() const noexcept { return {_data, _size}; }

    private:
        void grow(std::size_t minimumCapacity)
        {
            const std::size_t capacity = std::max(minimumCapacity, _capacity * 2);
            auto heap = std::make_unique_for_overwrite<char[]>(capacity);
            std::memcpy(heap.get(), _data, _size);
            _heap = std::move(heap);
            _data = _heap.get();
            _capacity = capacity;
        }

        char _inline[kInlineCapacity];
        std::unique_ptr<char[]> _heap;
        char* _data = _inline;
        std::size_t _size = 0;
        std::size_t _capacity = kInlineCapacity;
    };

}
