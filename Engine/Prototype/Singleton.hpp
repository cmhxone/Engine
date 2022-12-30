#ifndef _ENGINE_PROTOTYPE_HEADER_
#define _ENGINE_PROTOTYPE_HEADER_

/**
* Inherit this class, makes Singleton instance
*/
template <typename T>
class Singleton
{
public:
	/**
	* Get singleton instance, lazy load
	*/
	static T* getInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new T();
		}
		return _instance;
	}

	/**
	* Destroy singleton instacne
	*/
	static void destoryInstance()
	{
		if (_instance != nullptr)
		{
			delete _instance;
			_instance = nullptr;
		}
	}

protected:
	Singleton() {};
	virtual ~Singleton() {};

private:
	static T* _instance;
};

template <typename T> T* Singleton<T>::_instance = 0;

#endif