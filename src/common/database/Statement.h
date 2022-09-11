#pragma once

#include <string>
#include <sqlite3.h>
#include <gsl/gsl>


class SqlStatement {
	friend class DatabaseManager;
	sqlite3_stmt* stmt_;
	sqlite3* db_;
	std::string text;
	int paramcount_;
	bool eof_;	
protected:
	explicit SqlStatement(gsl::not_null<sqlite3*> db, gsl::not_null<sqlite3_stmt*> stmt);

public:
	~SqlStatement();
	void checkEof();

	int Execute();
	bool Eof() const;
	bool NextRow();
	int BindBlob(int pos, const gsl::not_null<unsigned char*> buffer, int size);
	int BindInt(int pos, int value);
	std::string GetColumnText(int i) const;
	int GetColumnInt(int i);
	int GetColumnBlob(int column, const void*& data);
};
