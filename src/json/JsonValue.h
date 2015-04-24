/*=============================================================================
 * TarotClub - JsonValue.h
 *=============================================================================
 * Modelization of a JSON generic value
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef JSONVALUE_H
#define JSONVALUE_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

// Forward declarations to resolve inter-dependency between Array and Object
class JsonArray;
class JsonObject;
class JsonValue;

/*****************************************************************************/
class JsonObject
{
public:
    std::string ToString(std::uint32_t level);
    bool HasValue(const std::string &key);
    JsonValue GetValue(const std::string &key);
    void Clear();
    void AddValue(const std::string &name, const JsonValue &value);

private:
    std::map<std::string, JsonValue> mObject;
};

/*****************************************************************************/
class JsonArray
{
public:
    std::string ToString(std::uint32_t level);
    void Clear();
    // JsonArray
    JsonValue GetEntry(std::uint32_t index);
    std::uint32_t Size() { return mArray.size(); }
    void AddValue(const JsonValue &value);

    typedef std::vector<JsonValue>::iterator Iterator;
    Iterator Begin() { return mArray.begin(); }
    Iterator End() { return mArray.end(); }

private:
    std::vector<JsonValue> mArray;
};
/*****************************************************************************/
class JsonValue
{
public:
    enum Tag
    {
        INVALID,
        ARRAY,
        OBJECT,
        INTEGER,
        DOUBLE,
        BOOLEAN,
        STRING,
        NULL_VAL
    };

    // From Value class
    JsonValue(std::int32_t value);
    JsonValue(double value);
    JsonValue(const char *value);
    JsonValue(const std::string &value);
    JsonValue(bool value);
    JsonValue(const JsonValue &value);
    JsonValue(); // default constructor creates an invalid value!
    JsonValue(const JsonObject &obj);
    JsonValue(const JsonArray &array);

    // Implemented virtual methods from IJsonNode
    Tag GetTag()
    {
        return mTag;
    }

    std::string ToString(std::uint32_t level);
    void Clear();

    JsonValue &operator = (JsonValue const &rhs);

    bool IsValid() const      { return mTag != INVALID; }
    bool IsArray() const      { return mTag == ARRAY; }
    bool IsObject() const     { return mTag == OBJECT; }
    bool IsNull() const       { return mTag == NULL_VAL; }
    bool IsString() const     { return mTag == STRING; }
    bool IsInteger() const    { return mTag == INTEGER; }
    bool IsBoolean() const    { return mTag == BOOLEAN; }
    bool IsDouble() const     { return mTag == DOUBLE; }

    JsonObject &GetObject() { return mObject; }
    JsonArray &GetArray() { return mArray; }

    std::int32_t    GetInteger()    { return mIntegerValue; }
    double          GetDouble()     { return mDoubleValue; }
    bool            GetBool()       { return mBoolValue; }
    std::string     GetString()     { return mStringValue; }

    bool GetValue(const std::string &nodePath, std::string &value);
    bool GetValue(const std::string &nodePath, std::uint32_t &value);
    bool GetValue(const std::string &nodePath, std::uint16_t &value);
    bool GetValue(const std::string &nodePath, std::int32_t &value);
    bool GetValue(const std::string &nodePath, bool &value);
    bool GetValue(const std::string &nodePath, double &value);

    void SetNull()
    {
        mTag = NULL_VAL;
    }

    JsonValue FindValue(const std::string &keyPath);

private:
    Tag mTag;
    JsonObject mObject;
    JsonArray mArray;
    std::int32_t mIntegerValue;
    double mDoubleValue;
    std::string mStringValue;
    bool mBoolValue;

    std::vector<std::string> Split(const std::string &obj);
};

#endif // JSONVALUE_H

//=============================================================================
// End of file JsonValue.h
//=============================================================================
