
void fatal(const char* message);
void debug(const char* message);
void error(char* message, OBJ object);

LocalContext* newLocalContext(int32_t size);
GlobalContext* newGlobalContext(int32_t size);
FuncContext* newFuncContext(int32_t size);

appContext* initAppContext();


appContext* Program();

void newGlobalVariable(appContext* ctx, const char* name);
