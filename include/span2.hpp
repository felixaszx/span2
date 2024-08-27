#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <array>

namespace spn2
{
    class circular_span
    {
      public:
        struct block
        {
            std::byte* data_ = nullptr;
            std::size_t size_ = 0;
        };

      private:
        std::byte* begin_ = nullptr;
        std::byte* end_ = nullptr;
        std::size_t front_ = 0;
        std::size_t size_ = 0;
        std::queue<block> blocks_{};

      public:
        circular_span() = default;
        circular_span(void* data, std::size_t size);
        template <typename T>
        circular_span(const T& continues_container)
            : circular_span(continues_container.data(), sizeof(continues_container[0]) * continues_container.size())
        {
        }

        void reference(void* data, std::size_t size);
        bool push_back(void* data, std::size_t new_size);
        void pop_front();
        void pop_front_cleared();
        bool copy_front_block_to(std::byte* dst);
        std::array<block, 2> front_block_region();
        std::size_t front_block_size() { return blocks_.front().size_; }
        std::size_t front() { return front_; }
        std::size_t empty() { return !size_; }
        std::size_t size() { return size_; }
        std::size_t capacity() { return end_ - begin_; }
        std::size_t remainning() { return capacity() - size(); }
    };

    circular_span::circular_span(void* data, std::size_t size)
    {
        reference(data, size);
    }

    void circular_span::reference(void* data, std::size_t size)
    {
        begin_ = (std::byte*)data;
        end_ = (std::byte*)data + size;
        front_ = 0;
        size_ = 0;
        while (!blocks_.empty())
        {
            blocks_.pop();
        }
    }

    bool circular_span::push_back(void* data, std::size_t new_size)
    {
        if (remainning() >= new_size)
        {
            std::size_t seek0 = (front_ + size_) % capacity();
            std::size_t seek0_size = std::min(new_size, capacity() - seek0);
            std::size_t seek1_size = new_size - seek0_size;
            memcpy(begin_ + seek0, data, seek0_size);
            if (seek1_size)
            {
                memcpy(begin_, (std::byte*)data + seek0_size, seek1_size);
            }

            size_ += new_size;
            blocks_.emplace(begin_ + seek0, new_size);
            return true;
        }
        return false;
    }

    void circular_span::pop_front()
    {
        if (size_)
        {
            size_ -= blocks_.front().size_;
            front_ += blocks_.front().size_;
            front_ %= capacity();
            blocks_.pop();
        }
    }

    void circular_span::pop_front_cleared()
    {
        block& block = blocks_.front();
        std::size_t seek0_size = std::min(block.size_, capacity() - front_);
        std::size_t seek1_size = block.size_ - seek0_size;
        memset(block.data_, 0x0, seek0_size);
        if (seek1_size)
        {
            memset(block.data_ + seek0_size, 0x0, seek0_size);
        }
        pop_front();
    }

    bool circular_span::copy_front_block_to(std::byte* dst)
    {
        if (size_)
        {
            block& block = blocks_.front();
            std::size_t seek0_size = std::min(block.size_, capacity() - front_);
            std::size_t seek1_size = block.size_ - seek0_size;
            memcpy(dst, block.data_, seek0_size);
            if (seek1_size)
            {
                memcpy(dst + seek0_size, begin_, seek1_size);
            }
            return true;
        }
        return false;
    }

    std::array<circular_span::block, 2> circular_span::front_block_region()
    {
        block& b = blocks_.front();
        std::array<block, 2> seeks;
        seeks[0].data_ = begin_ + front_;
        seeks[0].size_ = std::min(b.size_, capacity() - front_);
        seeks[1].data_ = begin_;
        seeks[1].size_ = b.size_ - seeks[0].size_;
        return seeks;
    }
}; // namespace spn2