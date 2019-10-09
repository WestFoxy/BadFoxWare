#pragma once

#include <memory>
#include <iostream>
#include <unordered_map>

#include "Misc/Recv.hpp"
#include "../Singleton.hpp"

class NetvarSys
    : public Singleton<NetvarSys>
{
    struct netvar_table
    {
        std::string               name;
        RecvProp*                 prop;
        uint32_t                  offset;
        std::vector<RecvProp*>    child_props;
        std::vector<netvar_table> child_tables;
    };
public:
    void Initialize();

    void Dump();
    void Dump(std::ostream& stream);

    uint32_t      GetOffset(const std::string& tableName, const std::string& propName);
    RecvProp*     GetNetvarProp(const std::string& tableName, const std::string& propName);

private:
    static netvar_table  LoadTable(RecvTable* clientClass);
    static void          DumpTable(std::ostream& stream, const netvar_table& table, uint32_t indentation);
    static uint32_t      GetOffset(const netvar_table& table, const std::string& propName);
    static RecvProp*     GetNetvarProp(const netvar_table& table, const std::string& propName);

private:
    std::vector<netvar_table>  database;
};



class NetMngr : public Singleton<NetMngr>
{
public:

	void init();
	int getOffs(const char* tableName, const char* propName);
	bool hookProp(const char* tableName, const char* propName, RecvVarProxyFn func);
	bool hookProp(const char* tableName, const char* propName, RecvVarProxyFn func, RecvVarProxyFn& orig);
	void dump();

private:

	std::unordered_map<std::string, RecvTable*> tables;

	int getProp(const char* tableName, const char* propName, RecvProp** prop = 0);
	int getProp(RecvTable* recvTable, const char* propName, RecvProp** prop = 0);
	RecvTable* getTable(const char* tableName);
	void dumpTable(RecvTable* table, std::string tabs);
	std::string type2str(SendPropType t);
};
