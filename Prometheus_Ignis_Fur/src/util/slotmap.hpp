#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cassert>
#include <iostream>

struct SlotKey
{

    bool operator==(const SlotKey &keyB) const
    {
        return (this->id == keyB.id && this->gen == keyB.gen);
    }
    
    bool operator!=(const SlotKey &keyB) const
    {
        return !(*this == keyB);
    }
    
    operator bool() const
    {
        return true;
    }
    int id{-1};
    std::size_t gen{0};
};

template <typename DataType>
struct Slotmap
{

    // using value_type = DataType;
    using index_type = std::size_t;
    using gen_type = index_type;
    using iterator = DataType *;
    using const_iterator = DataType const *;

    struct SlotmapResult
    {
        DataType &data;
        SlotKey key;
    };

    constexpr explicit Slotmap(std::size_t capacity = 0)
    {
        if (capacity != 0)
        {
            capacity_ = capacity;
            indices_.reserve(capacity);
            data_.reserve(capacity);
            erase_.reserve(capacity);
            clear();
        }
    }

    constexpr void reserve(std::size_t capacity)
    {
        if (capacity != 0)
        {
            capacity_ = capacity;
            indices_.reserve(capacity);
            data_.reserve(capacity);
            erase_.reserve(capacity);
            clear();
        }
        else
        {
            throw "\n***Error, asignando 0 capacity slotmap ***\n";
        }
    }

    [[nodiscard]] constexpr std::size_t size() const noexcept { return size_; }
    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return capacity_; }
    [[maybe_unused]] constexpr SlotmapResult push_back(DataType &&newVal)
    {
        auto reserveid = allocate();
        auto &slot = indices_[reserveid];
        // Move Data
        data_.insert(data_.begin() + slot.id, newVal);
        erase_[slot.id] = reserveid;
        // Return key
        auto key{slot};
        key.id = reserveid;

        return {data_[indices_[key.id].id], key};
    }
    [[maybe_unused]] constexpr SlotmapResult push_back(DataType const &newVal)
    {
        return push_back(DataType{newVal});
    }

    template <typename... Args>
    [[maybe_unused]] constexpr SlotmapResult emplace_back(Args &&...args)
    {
        auto reserveid = allocate();
        auto &slot = indices_[reserveid];
        // Move Data
        data_.emplace(data_.begin() + slot.id, std::forward<Args>(args)...);
        erase_[std::size_t(slot.id)] = reserveid;
        // Return key
        auto key{slot};
        key.id = int(reserveid);

        return {data_[std::size_t(indices_[std::size_t(key.id)].id)], key};
    }

    [[nodiscard]] DataType &get(SlotKey key)
    {
        DataType *result{nullptr};
        if (is_valid(key))
        {
            result = &data_[std::size_t(indices_[std::size_t(key.id)].id)];
        }
        return *result;
    }

    [[nodiscard]] DataType &operator[](SlotKey key)
    {
        DataType *result{nullptr};
        if (is_valid(key))
        {
            result = &data_[indices_[key.id].id];
        }
        return *result;
    }

    [[nodiscard]] DataType &operator[](std::size_t id)
    {
        DataType *result{nullptr};
        if (id < data_.size())
        {
            result = &data_[id];
        }
        return *result;
    }

    constexpr void clear() noexcept
    {
        freelist_init();
    }

    constexpr typename std::vector<DataType>::iterator erase(SlotKey const key) noexcept
    {
        if (not is_valid(key) || size_ == 0)
            return data_.end();
        return free(key);
    }

    [[nodiscard]] constexpr bool is_valid(SlotKey key) const noexcept
    {
        if (std::size_t(key.id) >= capacity_ || indices_[std::size_t(key.id)].gen != key.gen)
            return false;
        return true;
    }

    [[nodiscard]] constexpr auto begin() noexcept { return data_.begin(); }
    [[nodiscard]] constexpr auto end() noexcept { return data_.begin() + int(size_); }
    [[nodiscard]] constexpr auto cbegin() const noexcept { return data_.cbegin(); }
    [[nodiscard]] constexpr auto cend() const noexcept { return data_.cbegin() + size_; }

    // Codigo para depurar con el MemViewer
    auto &getData_()
    {
        return data_;
    }
    auto &getIndices_()
    {
        return indices_;
    }
    auto &getErase_()
    {
        return erase_;
    }

private:
    [[nodiscard]] constexpr index_type allocate()
    {
        if (size_ >= capacity_)
            throw std::runtime_error("No space left in the slotmap");
        assert(freelist_ < capacity_);

        // Reserve
        auto slotid = freelist_;
        freelist_ = std::size_t(indices_[slotid].id);

        // Init Slot
        auto &slot = indices_[slotid];
        slot.id = int(size_);
        slot.gen = generation_;

        // Update space and generation
        ++size_;
        ++generation_;

        return slotid;
    }

    constexpr typename std::vector<DataType>::iterator free(SlotKey key) noexcept
    {
        assert(is_valid(key));

        auto &slot = indices_[std::size_t(key.id)];
        std::size_t dataid = std::size_t(slot.id); // to check if is it last
        // Update freelist
        slot.id = int(freelist_);
        slot.gen = generation_;
        freelist_ = std::size_t(key.id);

        // Copy data to free slot
        if (dataid != size_ - 1)
        {
            data_[dataid] = std::move(data_[size_ - 1]);
            erase_[dataid] = std::move(erase_[size_ - 1]);
            indices_[erase_[dataid]].id = int(dataid);
        }

        // Update size
        --size_;
        ++generation_;

        return std::next(data_.begin(), int(dataid));
    }

    constexpr void freelist_init() noexcept
    {
        for (std::size_t i = 0; i < indices_.capacity(); ++i)
        {
            indices_[i].id = int(i) + 1;
        }
        freelist_ = 0;
        size_ = 0;
        generation_ = 0;
    }

    // char const t1[8] = "#SIZE##";
    index_type size_{};
    // char const t2[8] = "#FREEL#";
    index_type freelist_{};
    // char const t3[8] = "#GENER#";
    gen_type generation_{};
    // char const t4[16] = "#INDICES#######";
    std::vector<SlotKey> indices_{};
    // char const t5[16] = "#DATA##########";
    std::vector<DataType> data_{};
    // char const t6[16] = "#ERASE#########";
    std::vector<index_type> erase_{};
    // char const t7[8] = "#CAP###";
    std::size_t capacity_{};
};