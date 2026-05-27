#ifndef __CCEXTSINGLETON_H__
#define __CCEXTSINGLETON_H__

template <typename T>
class CCextSingleton
{
public:
	inline static T* getInstance();
	inline static void release();
private:
	static T* t;
};

template <typename T>
inline T* CCextSingleton<T>::getInstance()
{
	if (!t)
	{
		t = new T;
	}
	return t;
}

template<typename T>
inline void CCextSingleton<T>::release()
{
	if (t)
	{
		delete t;
		t = 0;
	}
}

template <typename T> 
T* CCextSingleton<T>::t = 0;

#endif // __CCEXTSINGLETON_H__