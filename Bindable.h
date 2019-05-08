#include <map>

class MethodContainerBase
{
public:
	virtual void call(void *arg) = 0;
	virtual bool equals(const MethodContainerBase *arg) = 0;
};

template<typename RecClass, typename EventClass>
class MethodContainer : public MethodContainerBase
{
	friend class MethodContainer;
private:
	void(RecClass::*method)(EventClass&);
	RecClass *object;
public:
	void call(void *arg)
	{
		(object->*method)(*reinterpret_cast<EventClass*>(arg));
	}
	bool equals(const MethodContainerBase *arg)
	{
		const MethodContainer<RecClass, EventClass> *argC = dynamic_cast<const MethodContainer<RecClass, EventClass>*>(arg);
		if (argC) {
			return argC->method == method && argC->object == object;
		}
		return false;
	}

	MethodContainer(RecClass *_object, void(RecClass::*_method)(EventClass&))
		: object(_object), method(_method) { }
};

template<typename EventClass>
class FunctionContainer : public MethodContainerBase
{
	friend class FunctionContainer;
private:
	void (*function)(EventClass&);
public:
	void call(void *arg)
	{
		(*function)(*reinterpret_cast<EventClass*>(arg));
	}
	bool equals(const MethodContainerBase *arg)
	{
		const FunctionContainer<EventClass> *argC = dynamic_cast<const FunctionContainer<EventClass>*>(arg);
		if (argC) {
			return argC->function == function;
		}
		return false;
	}

	FunctionContainer(void (*_function)(EventClass&))
		: function(_function) { }
};

class Bindable
{
private:
	typedef std::multimap<int, MethodContainerBase*> EventMap;
	EventMap map;
	void _callEvent(int eventId, void *arg);
	void _removeEventListener(int eventId, MethodContainerBase *container);
	void _addEventListener(int eventId, MethodContainerBase *container);
public:
	template<typename RecClass, typename EventClass>
	void addEventListener(int eventId, void(RecClass::*method)(EventClass&), RecClass *object)
	{
		_addEventListener(eventId, new MethodContainer<RecClass, EventClass>(object, method));
	}

	template<typename EventClass>
	void addEventListener(int eventId, void (*function)(EventClass&))
	{
		_addEventListener(eventId, new FunctionContainer<EventClass>(function));
	}

	template<typename RecClass, typename EventClass>
	void removeEventListener(int eventId, void(RecClass::*method)(EventClass&), RecClass *object)
	{
		_removeEventListener(eventId, &MethodContainer<RecClass, EventClass>(object, method));
	}

	template<typename EventClass>
	void removeEventListener(int eventId, void (*function)(EventClass&))
	{
		_removeEventListener(eventId, &FunctionContainer<EventClass>(function));
	}

	template<typename EventClass>
	void callEvent(int eventId, EventClass &arg)
	{
		return _callEvent(eventId, &arg);
	}

	~Bindable();
};