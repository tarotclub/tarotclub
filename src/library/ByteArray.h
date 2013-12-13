#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <vector>
#include <cstdint>
#include <iostream>

/**
 * @brief The ByteArray class
 * Little endian
 *
 */
class ByteArray
{

public:
    ByteArray(const char *data, std::uint32_t size)
    {
        for (std::uint32_t i = 0U; i < size; i++)
        {
            mData.push_back(static_cast<std::uint8_t>(data[i]));
        }
    }

    ByteArray(const std::string &data)
    {
        for (std::uint32_t i = 0U; i < data.size(); i++)
        {
            mData.push_back(static_cast<std::uint8_t>(data[i]));
        }
    }

    ByteArray()
    {

    }

    void PushBack(std::uint8_t byte)
    {
        mData.push_back(byte);
    }

    std::uint32_t Size() const
    {
        return mData.size();
    }

    void Clear()
    {
        mData.clear();
    }

    ByteArray SubArray(std::uint32_t index, std::uint32_t len)
    {
        ByteArray array;

        if ((index + len) <= mData.size())
        {
            array.mData.insert(array.mData.begin(), mData.begin() + index, mData.end() + index + len);
        }
        return array;
    }

    void Erase(std::uint32_t index, std::uint32_t len)
    {
        mData.erase(mData.begin() + index, mData.begin() + index + len);
    }

    std::uint8_t& operator[] (std::uint32_t i)
    {
        if (i >= mData.size())
        {
            return mData[0];
        }
        else
        {
            return mData[i];
        }
    }

    std::uint8_t operator[] (std::uint32_t i) const
    {
        if (i >= mData.size())
        {
            return mData[0];
        }
        else
        {
            return mData[i];
        }
    }

    ByteArray& operator=(const ByteArray &rhs)
    {
      mData.clear();
      mData = rhs.mData;
      return *this;
    }

    ByteArray& operator += (const ByteArray &rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    ByteArray operator+(const ByteArray &rhs) const
    {
        ByteArray result;
        result.Append(this->mData);
        result.Append(rhs.mData);
        return result;
    }

    std::uint16_t GetUint16 (std::uint32_t index) const
    {
        std::uint16_t data = 0U;

        if (index <= (mData.size() - sizeof(std::uint16_t)))
        {
            for (std::uint8_t i = 0U; i < 2U; i++)
            {
                std::uint16_t hword = mData[index + i];
                data = data + (hword << (i*8U));
            }
        }
        return data;
    }

    std::uint32_t GetUint32 (std::uint32_t index) const
    {
        std::uint32_t data = 0U;

        if (index <= (mData.size() - sizeof(std::uint32_t)))
        {
            for (std::uint8_t i = 0U; i < 2U; i++)
            {
                std::uint32_t word = mData[index + i];
                data = data + (word << (i * 8U));
            }
        }
        return data;
    }

    std::string ToSring() const
    {
        std::string buf;

        for (std::uint32_t i = 0U; i < mData.size(); i++)
        {
            buf.push_back(static_cast<char>(mData[i]));
        }

        return buf;
    }

private:
    std::vector<std::uint8_t> mData;

    void Append (const std::vector<std::uint8_t> &data)
    {
        std::vector<std::uint8_t>::iterator it = mData.end();
        mData.insert(it, data.begin(), data.end());
    }
};


#endif // BYTEARRAY_H