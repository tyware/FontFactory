#pragma

//#ifdef FONTENGINE_EXPORTS
//#define FONTENGINE_API __declspec(dllexport)
//#else
//#define FONTENGINE_API __declspec(dllimport)
//#endif

#ifdef FONTENGINE_EXPORTS
#define FONTENGINE_API
#else
#define FONTENGINE_API
#endif

//#ifdef FONTENGINE_EXPORTS
//#define DLL_SO_EXPORT __declspec(dllexport)
//#else
//#define DLL_SO_EXPORT __declspec(dllimport)
//#endif
