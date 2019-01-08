#include "XEvent.hpp"
#include "XCounter.hpp"

namespace XBoost {

	//////////////////////////////////////////////////////////////////////////

	Event::Event()
		:src(0), dst(0), evt(0), obj(0), id(get_unique_id()), handled(false)
	{
	}

	Event::Event(EvtObject* src, EvtObject* dst, size_t evt, size_t val, boost::any obj)
		: src(src), dst(dst), evt(evt), val(val), obj(obj), id(get_unique_id()), handled(false)
	{
	}

	Event::Event(EvtObject* dst, size_t evt, size_t val, boost::any obj)
		: src(nullptr), dst(dst), evt(evt), val(val), obj(obj), id(get_unique_id()), handled(false)
	{
	}

	Event::Event(size_t evt, size_t val, boost::any obj)
		: src(nullptr), dst(nullptr), evt(evt), val(val), obj(obj), id(get_unique_id()), handled(false)
	{

	}

	Event::~Event()
	{
	}

	Event::Event(const Event& o) :src(o.src), dst(o.dst), evt(o.evt), val(o.val), obj(o.obj), id(o.id), handled(o.handled)
	{
	}

	Event::Event(const Event&& o) : src(o.src), dst(o.dst), evt(o.evt), val(o.val), obj(o.obj), id(o.id), handled(o.handled)
	{
	}

	Event& Event::operator=(const Event& o)
	{
		src = o.src;
		dst = o.dst;
		evt = o.evt;
		val = o.val;
		obj = o.obj;
		id = o.id;
		handled = o.handled;
		return (*this);
	}

	Event& Event::operator=(const Event&& o)
	{
		src = o.src;
		dst = o.dst;
		evt = o.evt;
		val = o.val;
		obj = o.obj;
		id = o.id;
		handled = o.handled;
		return (*this);
	}


	//////////////////////////////////////////////////////////////////////////

	typedef boost::unordered_map<EvtObject*, boost::thread::id> MapEvtObject;
	static  MapEvtObject _mapAllEvtObject;
	static boost::mutex _SectionAllEvtObject;

	static void AddEvtObject(EvtObject* pEvtObject, boost::thread::id ThreadId = boost::this_thread::get_id())
	{
		BOOST_ASSERT(pEvtObject);
		if (pEvtObject) {
			boost::unique_lock<boost::mutex> lock(_SectionAllEvtObject);
			_mapAllEvtObject[pEvtObject] = ThreadId;
		}
	}
	static void RemoveEvtObject(EvtObject* pEvtObject, boost::thread::id ThreadId = boost::this_thread::get_id())
	{
		BOOST_ASSERT(pEvtObject);
		if (pEvtObject) {
			boost::unique_lock<boost::mutex> lock(_SectionAllEvtObject);
			_mapAllEvtObject.erase(pEvtObject);
		}
	}

	static boost::thread::id GetEvtObjectThreadId(EvtObject* pEvtObject)
	{
		BOOST_ASSERT(pEvtObject);
		if (pEvtObject) {
			boost::unique_lock<boost::mutex> lock(_SectionAllEvtObject);
			MapEvtObject::iterator it = _mapAllEvtObject.find(pEvtObject);
			if (it != _mapAllEvtObject.end()) {
				return it->second;
			}
		}
		return boost::thread::id();
	}

	static size_t GetEvtObjectCount(boost::thread::id ThreadId = boost::this_thread::get_id())
	{
		boost::unique_lock<boost::mutex> lock(_SectionAllEvtObject);
		size_t Count = 0;
		MapEvtObject::iterator it = _mapAllEvtObject.begin();
		for (; it != _mapAllEvtObject.end(); ++it) {
			if (it->second == ThreadId) {
				Count++;
			}
		}
		return Count;
	}

	long EvtObject::post_thread_event(boost::thread::id ThreadId, Event& evt)
	{
		EvtDispatcher* pDispatcher = EvtDispatcher::get_evtdispatcher(ThreadId);
		if (pDispatcher) {
			return pDispatcher->post_event(evt);
		}
		else {
			if (evt.dst) {
				evt.dst->call(evt);
			}
			else {
				BOOST_ASSERT(0);
			}
		}
		return RLT_UNKNOWN;
	}

	long EvtObject::post_thread_delay_event(boost::thread::id ThreadId, Event& evt, size_t delay, size_t times)
	{
		EvtDispatcher* pDispatcher = EvtDispatcher::get_evtdispatcher(ThreadId);
		if (pDispatcher) {
			return pDispatcher->post_delay_event(evt, delay, times);
		}
		else {
			BOOST_ASSERT(0);
			if (evt.dst) {
				evt.dst->call(evt);
			}
			else {
				//
			}
		}
		return RLT_UNKNOWN;
	}

	EvtObject::EvtObject()
	{
		AddEvtObject(this, boost::this_thread::get_id());
	}

	EvtObject::~EvtObject()
	{
		RemoveEvtObject(this);
	}

	const char* EvtObject::name()
	{
		return nullptr;
	}

	size_t EvtObject::id()
	{
		return 0;
	}

	boost::thread::id EvtObject::tid()
	{
		return GetEvtObjectThreadId(this);
	}

	long EvtObject::send_event(Event& evt)
	{
		if (!evt.dst) {
			evt.dst = this;
		}
		if (evt.dst == this) {
			return call(evt);
		}
		else {
			return evt.dst->send_event(evt);
		}
		//return evt.dst->call(evt);
	}

	long EvtObject::post_event(Event& evt)
	{
		if (!evt.dst) {
			evt.dst = this;
		}
		if (evt.dst == this) {
			return post_thread_event(tid(), evt);
		}
		else {
			return evt.dst->post_event(evt);
		}
	}

	long EvtObject::post_delay_event(Event& evt, size_t delay, size_t times)
	{
		if (!evt.dst) {
			evt.dst = this;
		}
		if (evt.dst == this) {
			return post_thread_delay_event(tid(), evt, delay, times);
		}
		else {
			return evt.dst->post_delay_event(evt, delay, times);
		}
		return RLT_UNKNOWN;
	}

	long EvtObject::send_event(EvtObject* dst, Event& evt)
	{
		evt.dst = dst;
		return send_event(evt);
	}

	long EvtObject::post_event(EvtObject* dst, Event& evt)
	{
		evt.dst = dst;
		return post_event(evt);
	}

	long EvtObject::post_delay_event(EvtObject* dst, Event& evt, size_t delay, size_t times)
	{
		evt.dst = dst;
		return post_delay_event(evt, delay, times);
	}

	long EvtObject::call(Event& evt)
	{
		if (evt.handled) {
			evt.handled = false;
		}
		return on_event(evt);
	}
	//
	//long EvtObject::call(EvtObject* src, long evt, boost::any obj)
	//{
	//	return call(Event(src,this,evt,value,obj));
	//}
	//
	long EvtObject::on_event(Event& evt)
	{
		//ASSERT(0);
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////


	static boost::thread_specific_ptr<EvtDispatcher> _pDispatcher;

	boost::mutex EvtDispatcher::s_mutex_;
	std::map<boost::thread::id, EvtDispatcher*> EvtDispatcher::s_mapDispatcher;

	void EvtDispatcher::set_evtdispatcher(EvtDispatcher* pDispatcher, boost::thread::id ThreadId)
	{
		boost::unique_lock<boost::mutex> lock(s_mutex_);
		if (pDispatcher) {
			s_mapDispatcher[ThreadId] = pDispatcher;
		}
		else {
			s_mapDispatcher.erase(ThreadId);
		}
	}

	EvtDispatcher* EvtDispatcher::get_evtdispatcher(boost::thread::id ThreadId)
	{
		boost::unique_lock<boost::mutex> lock(s_mutex_);
		std::map<boost::thread::id, EvtDispatcher*>::iterator it = s_mapDispatcher.find(ThreadId);
		if (it != s_mapDispatcher.end()) {
			return it->second;
		}
		return nullptr;
	}

	EvtDispatcher::EvtDispatcher(boost::asio::io_service& io_service)
		: status_(stoped), io_service_(io_service)
	//EvtDispatcher::EvtDispatcher()
	//	: status_(stoped), io_service_(*this)
		//, evtfilter_(nullptr), evtidler_(nullptr)
		, last_evt_time_(GetTickCount())
	{
		ASSERT(_pDispatcher.get() == nullptr);
		_pDispatcher.reset(this);
		set_evtdispatcher(this);
		init();
	}

	EvtDispatcher::~EvtDispatcher()
	{
		ASSERT(_pDispatcher.get() == this);
		_pDispatcher.release();
		set_evtdispatcher(nullptr);
		term();
	}

	void EvtDispatcher::init()
	{
		ASSERT(status_ == stoped);
		if (status_==stoped) {
			status_ = runing;
			timer_ptr_ = std::make_shared<boost::asio::deadline_timer>(io_service_);
			timer_ptr_->expires_from_now(boost::posix_time::milliseconds(50));
			timer_ptr_->async_wait(boost::bind(&EvtDispatcher::on_timer, this));
		}
	}

	void EvtDispatcher::term()
	{
		if (status_==runing) {
			//boost::system::error_code ec;
			//timer_ptr_->cancel(ec);
			timer_ptr_.reset();
			delay_events_.clear();
			last_evt_time_ = 0;
			status_ = stoped;
		}
	}

	void EvtDispatcher::register_evtfilter(EvtFilter* evtfilter)
	{
		/*if (!evtfilter_) {
			evtfilter_ = evtfilter;
		} else {
			evtfilter_->Last(evtfilter);
		}*/
		evtfilters_.push_back(evtfilter);
	}

	void EvtDispatcher::unregister_evtfilter(EvtFilter* evtfilter)
	{
		/*if (evtfilter == evtfilter_) {
			evtfilter_ = evtfilter_->Next();
		}
		evtfilter->Free();*/
		EvtFilters::iterator it = evtfilters_.begin();
		for (; it != evtfilters_.end(); ++it)
		{
			if (evtfilter == *it) {
				evtfilters_.erase(it);
				break;
			}
		}
	}

	void EvtDispatcher::register_evtidler(EvtIdler* evtidler)
	{
		/*if (!evtidler_) {
			evtidler_ = evtidler;
		} else {
			evtidler_->Last(evtidler);
		}*/
		evtidlers_.push_back(evtidler);
	}

	void EvtDispatcher::unregister_evtidler(EvtIdler* evtidler)
	{
		/*if (evtidler == evtidler_) {
			evtidler_ = evtidler_->Next();
		}
		evtidler->Free();*/
		EvtIdlers::iterator it = evtidlers_.begin();
		for (; it != evtidlers_.end(); ++it)
		{
			if (evtidler == *it) {
				evtidlers_.erase(it);
				break;
			}
		}
	}

	long EvtDispatcher::send_event(Event& evt)
	{
		if (!evt.dst) {
			return on_event(evt);
		}
		ASSERT(0);
		return evt.dst->send_event(evt);
	}

	long EvtDispatcher::post_event(Event& evt)
	{
#ifdef _DEBUG
		if (evt.dst) {
			ASSERT(this == get_evtdispatcher(evt.dst->tid()));
		}
#endif//
		io_service_.post(boost::bind(&EvtDispatcher::on_event, this, evt));
		return 1;
	}

	long EvtDispatcher::post_delay_event(Event& evt, size_t delay, size_t times)
	{
	#ifdef _DEBUG
		if (evt.dst) {
			ASSERT(this==get_evtdispatcher(evt.dst->tid()));
		}
	#endif//
		DelayEvent delay_evt;
		delay_evt.time = GetTickCount();
		delay_evt.delay = delay;
		delay_evt.times = times;
		delay_evt.evt = evt;
		io_service_.post(boost::bind(&EvtDispatcher::on_delay, this, delay_evt));
		return 1;
	}

	long EvtDispatcher::on_event(Event& evt)
	{
		if (!pre_translate_event(evt)) {
			return dispatch_event(evt);
		}
		return 1;
	}

	long EvtDispatcher::dispatch_event(Event& evt)
	{
		long ret = 0;
		if (evt.dst) {
			ret = evt.dst->call(evt);
			last_evt_time_ = GetTickCount();
		}
		else {
			ret = 1;
		}
		return ret;
	}

	bool EvtDispatcher::pre_translate_event(Event& evt)
	{
		bool bHandled = false;
		if (!evt.dst) {
			switch (evt.evt)
			{
			case EVT_INIT:
			{
				init();
				bHandled = true;
			}
			break;
			case EVT_TERM:
			{
				term();
				bHandled = true;
			}
			break;
			}
		}
		if (status_!=runing)
		{
			bHandled = true;
		}
		if (bHandled)
		{
			return bHandled;
		}
		/*for(EvtFilter* evtfilter = evtfilter_; evtfilter!=nullptr;)
		{
			EvtFilter* nextfilter = evtfilter->Next();
			if (bHandled = evtfilter->pre_translate_event(evt)) {
				break;
			}
			evtfilter = nextfilter;
		}*/
		EvtFilters::iterator it = evtfilters_.begin();
		for (; it != evtfilters_.end();)
		{
			EvtFilter* evtfilter = *it++;
			if (bHandled = evtfilter->pre_translate_event(evt)) {
				break;
			}
		}
		return bHandled;
	}

	void EvtDispatcher::on_timer()
	{
		if (status_ != runing) {
			return;
		}

		DelayEvents::iterator it = delay_events_.begin();
		for (; it!=delay_events_.end(); )
		{
			DelayEvent& delay = *it;
			size_t time = GetTickCount();
			if ((time-delay.time)>=delay.delay) {
				on_event(delay.evt);
				time = GetTickCount();
				--delay.times;
				if (delay.times > 0) {
					delay.time = time;
					++it;
				} else {
					it = delay_events_.erase(it);
				}
			} else {
				++it;
			}
		}

		size_t tick = GetTickCount();
		bool is_idle = (tick - last_evt_time_) > 1000;
		if (is_idle) {
			on_idle(tick);
			last_evt_time_ = GetTickCount();
		}

		timer_ptr_->expires_from_now(boost::posix_time::milliseconds(50));
		timer_ptr_->async_wait(boost::bind(&EvtDispatcher::on_timer, this));
	}

	void EvtDispatcher::on_delay(DelayEvent& delay_evt)
	{
		delay_events_.push_back(delay_evt);
	}

	void EvtDispatcher::on_idle(size_t tick)
	{
		/*for (EvtIdler* evtidler = evtidler_; evtidler != nullptr;)
		{
			EvtIdler* nextidler = evtidler->Next();
			evtidler->on_idle(tick);
			evtidler = nextidler;
		}*/
		EvtIdlers::iterator it = evtidlers_.begin();
		for (; it != evtidlers_.end();)
		{
			EvtIdler* evtidler = *it++;
			evtidler->on_idle(tick);
		}
	}

	EvtThread::EvtThread()
	{

	}

	EvtThread::~EvtThread()
	{
		stop();
	}

	bool EvtThread::start()
	{
		thread_ptr_ = std::make_shared<std::thread>(
			//boost::bind(&EvtServer::run, this)
			[this]() {
				EvtDispatcher evtdispatcher(io_service_);
				on_init();
				on_run();
				on_term();
			}
		);
		return thread_ptr_!=nullptr;
	}

	void EvtThread::stop()
	{
		if (thread_ptr_) {
			io_service_.stop();
			thread_ptr_->join();
			io_service_.reset();
			thread_ptr_.reset();
		}
	}

	void EvtThread::on_run()
	{
		io_service_.run();
	}

	long send_event(Event& evt)
	{
		if (evt.dst) {
			return evt.dst->send_event(evt);
		}
		else {
			EvtDispatcher* pEvtDispatcher = EvtDispatcher::get_evtdispatcher();
			if (pEvtDispatcher) {
				return pEvtDispatcher->send_event(evt);
			}
		}
		return 0;
	}

	long post_event(Event& evt)
	{
		if (evt.dst) {
			return evt.dst->post_event(evt);
		}
		else {
			EvtDispatcher* pEvtDispatcher = EvtDispatcher::get_evtdispatcher();
			if (pEvtDispatcher) {
				return pEvtDispatcher->post_event(evt);
			}
		}
		return 0;
	}

}
