class CCameraexportLua:public lua::LuaObject
{
protected:
    void DoExport();
public:
     CCameraexportLua():lua::LuaObject(2){};
};