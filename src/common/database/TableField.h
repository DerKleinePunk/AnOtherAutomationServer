#pragma once
#include <string>

enum class DatabaseFieldTypes : unsigned char {
	VARCHAR,
	INTEGER,
	TIMESTAMP,
	DOUBLE,
	BLOB
};


class TableField
{
	bool _key;
	DatabaseFieldTypes _type;
	bool _autoInc;
	std::string _addDefText;
public:
	std::string name_;
	std::string value;
	bool notNull_;
	int size_;
	
	TableField();
	void AddToKey();
	void SetNoKey();
	bool IsKeyField() const;
	void SetType(DatabaseFieldTypes type);
	std::string GetTypeText() const;
	std::string GetTypeDefText() const;
	void SetAutoInc();
	bool GetAutoInc() const;
	void SetDefText(const std::string& text);
	std::string GetDefText() const;
};
