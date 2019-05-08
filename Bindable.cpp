#include "Bindable.h"
#include <iterator>

void Bindable::_addEventListener(int eventId, MethodContainerBase *container)
{
	map.insert(std::make_pair(eventId, container));
}

void Bindable::_removeEventListener(int eventId, MethodContainerBase *container)
{
	auto iter = map.begin();
	while (iter != map.end()) {
		auto iter1 = iter;
		iter++;
		if (iter1->first == eventId && iter1->second->equals(container)) {
			delete iter1->second;
			map.erase(iter1);
			return;
		}
	}
}

void Bindable::_callEvent(int eventId, void *arg)
{
	for (auto iter = map.begin(); iter != map.end(); ++iter) {
		if (iter->first == eventId)
		{
			iter->second->call(arg);
		}
	}
}

Bindable::~Bindable() {
	auto iter = map.begin();
	while (iter != map.end()) {
		auto iter1 = iter;
		iter++;
		delete iter1->second;
		map.erase(iter1);
	}
}