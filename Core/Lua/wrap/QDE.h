class CQDEexportLua:public lua::LuaObject
{
protected:
    void DoExport();
public:
     CQDEexportLua():lua::LuaObject(2){};
};