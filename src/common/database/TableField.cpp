#include "TableField.h"

#include <iostream>

std::ostream& operator<<(std::ostream& os, const DatabaseFieldTypes type)
{
    switch(type) {
    case DatabaseFieldTypes::INTEGER:
        os << "INTEGER";
        break;
    case DatabaseFieldTypes::VARCHAR:
        os << "VARCHAR";
        break;
    case DatabaseFieldTypes::TIMESTAMP:
        os << "TIMESTAMP";
        break;
    case DatabaseFieldTypes::DOUBLE:
        os << "DOUBLE";
        break;
    case DatabaseFieldTypes::BLOB:
        os << "BLOB";
        break;
    default:
        os.setstate(std::ios_base::failbit);
    }
    return os;
}

TableField::TableField() : _key(false), _type(DatabaseFieldTypes::VARCHAR), _autoInc(false), notNull_(false), size_(-1)
{
}

void TableField::AddToKey()
{
    _key = true;
    notNull_ = true;
}

void TableField::SetNoKey()
{
    _key = false;
    notNull_ = false;
}

bool TableField::IsKeyField() const
{
    return _key;
}

void TableField::SetType(DatabaseFieldTypes type)
{
    _type = type;
	_autoInc = false;
	_addDefText = "";

    switch(_type) {
    case DatabaseFieldTypes::VARCHAR:
        break;
    case DatabaseFieldTypes::INTEGER:
    case DatabaseFieldTypes::TIMESTAMP:
    case DatabaseFieldTypes::DOUBLE:
        size_ = -1;
        break;
    case DatabaseFieldTypes::BLOB:
        break;
    default:
        throw std::runtime_error{ "FieldType" };
        ;
    }
}

std::string TableField::GetTypeText() const
{
    std::string result("");
    switch(_type) {
    case DatabaseFieldTypes::INTEGER:
        result = "INTEGER";
        break;
    case DatabaseFieldTypes::VARCHAR:
        result = "VARCHAR(" + std::to_string(size_) + ")";
        break;
    case DatabaseFieldTypes::TIMESTAMP:
        result = "TIMESTAMP";
        break;
    case DatabaseFieldTypes::DOUBLE:
        result = "DOUBLE";
        break;
    case DatabaseFieldTypes::BLOB:
        result = "BLOB";
        break;
    default:
        throw std::runtime_error{ "FieldType" };
        ;
    }
    return result;
}

std::string TableField::GetTypeDefText() const
{
    auto result = name_;
    result += " ";
    result += GetTypeText();
    return result;
}

void TableField::SetAutoInc()
{
    _key = false;
    _autoInc = true;
}

bool TableField::GetAutoInc() const
{
	return _autoInc;
}

void TableField::SetDefText(const std::string& text)
{
	_addDefText = text;
}

std::string TableField::GetDefText() const
{
	return _addDefText;
}