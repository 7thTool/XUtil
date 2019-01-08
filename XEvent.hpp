#ifndef _H_XEVENT_HPP_
#define _H_XEVENT_HPP_

#pragma once

#include "XUtil.hpp"
//#include "XNode.hpp"
#include "XThread.hpp"

namespace XUtil {

	//使用事件模型需要注意以下几点
	//1、对象响应事件函数不能删除自身
	//2、EvtFilter处理函数中不能注销其他EvtFilter
	//3、EvtIdler处理函数中不能注销其他EvtIdler
	//4、EvtHandler/EvtListener事件交互时不能注销其他EvtHandler/EvtListener
	//5、EvtDispatcher只能在所在线程创建使用

	class EvtObject;
	class EvtFilter;
	class EvtIdler;
	class EvtDispatcher;

#define EVT_NONE		0
#define EVT_NULL		0
#define EVT_INIT		1
#define EVT_TERM		2
#define EVT_TIMER		3
#define	EVT_MAX			1024

	//////////////////////////////////////////////////////////////////////////

	//typedef boost::any(0) nullobj;
	class AnyObj
	{
	public:
		AnyObj() {}
		AnyObj(boost::any o) : obj(o) {}
		boost::any obj;
	};
	typedef std::shared_ptr<AnyObj> AnyObjPtr;
	template<typename ValueType>
	inline ValueType AnyObjCast(const AnyObj & o)
	{
		return boost::any_cast<ValueType>(o.obj);
	}
	template<typename ValueType>
	inline ValueType AnyObjPtrCast(const AnyObjPtr & ptr)
	{
		if (ptr) {
			return boost::any_cast<ValueType>(ptr->obj);
		} else {
			boost::throw_exception(boost::bad_any_cast());
		}
	}
	template<typename ValueType>
	inline ValueType EvtAnyObjCast(const boost::any & obj)
	{
		return AnyObjPtrCast<ValueType>(boost::any_cast<AnyObjPtr>(obj));
	}
	inline AnyObjPtr MakeEvtAnyObj()
	{
		return std::make_shared<AnyObj>();
	}

	class XBOOST_API Event
	{
	public:
		EvtObject* src;			//事件源
		EvtObject* dst;			//事件目标

		size_t evt;				//事件
		size_t val;				//事件值
		boost::any obj;			//事件对象

		size_t id;				//事件id

		bool handled;			//是否处理了

		Event();
		Event(EvtObject* src, EvtObject* dst, size_t evt, size_t value, boost::any obj);
		Event(EvtObject* dst, size_t evt, size_t value, boost::any obj);
		Event(size_t evt, size_t value, boost::any obj);
		~Event();

		Event(const Event& o);
		Event(const Event&& o);
		Event& operator=(const Event& o);
		Event& operator=(const Event&& o);
	};

	struct EventLess
	{
		bool operator()(const Event& x, const Event& y) const
		{
			/*if (x.src<y.src) {
			return true;
			} else if (x.src>y.src) {
			return false;
			}
			if (x.dst<y.dst) {
			return true;
			} else if (x.dst>y.dst) {
			return false;
			}
			if (x.evt<y.evt) {
			return true;
			} else if (x.evt>y.evt) {
			return false;
			}*/
			if (x.id < y.id) {
				return true;
			}
			else if (x.id > y.id) {
				return false;
			}
			return false;
		}
	};

	class XBOOST_API EvtObject
	{
		friend class EvtDispatcher;
	public:
		static long post_thread_event(boost::thread::id ThreadId, Event& evt);
		static long post_thread_delay_event(boost::thread::id ThreadId, Event& evt, size_t delay, size_t times = 1);
	public:
		EvtObject();
		virtual ~EvtObject();

		virtual const char* name();
		virtual size_t id();

		virtual boost::thread::id tid();

		//发送事件
		virtual long send_event(Event& evt);
		virtual long post_event(Event& evt);
		virtual long post_delay_event(Event& evt, size_t delay, size_t times = 1);
		long send_event(EvtObject* dst, Event& evt);
		long post_event(EvtObject* dst, Event& evt);
		long post_delay_event(EvtObject* dst, Event& evt, size_t delay, size_t times = 1);

	protected:
		//外部直接调用事件过来
		virtual long call(Event& evt);
		//long call(EvtObject* src, long evt, long value = 0, boost::any obj = 0);
	protected:
		virtual long on_event(Event& evt);
	};

	class XBOOST_API EvtFilter //: public XNode2<EvtFilter>
	{
	public:
		virtual bool pre_translate_event(Event& evt) = 0;
	};

	class XBOOST_API EvtIdler //: public XNode2<EvtIdler>
	{
	public:
		virtual void on_idle(size_t tick) = 0;
	};

#define BEGIN_EVT_CALL(theClass) \
protected: \
	long call(XBoost::Event& evt) \
	{ \
		long ret = 0; \

#define ON_CALL(myevt,myfunc) \
		if(evt.evt == myevt) { \
			evt.handled = true; \
			ret = myfunc(evt); \
			if(evt.handled) { \
				return ret; \
			} \
		}

#define END_EVT_CALL() \
		ret = on_event(evt); \
		if(!evt.handled) { \
			ret = 0; \
		} \
		return ret; \
	}

#define BEGIN_EVT_MAP(theClass) \
protected: \
	long on_event(XBoost::Event& evt) \
	{ \
		long ret = 0; 

#define ON_EVT(myevt,myfunc) \
		if(evt.evt == myevt) { \
			evt.handled = true; \
			ret = myfunc(evt); \
			if(evt.handled) { \
				return ret; \
			} \
		}

#define CHAIN_EVT_MAP(theChainClass) \
		ret = theChainClass::on_event(evt); \
		if(evt.handled) { \
			return ret; \
		}

#define CHAIN_EVT_OBJ(theObject) \
		ret = theObject.on_event(evt); \
		if(evt.handled) { \
			return ret; \
		}

#define CHAIN_EVT_PTR(theobj) \
		if(theobj) { \
			ret = theobj->on_event(evt); \
			if(evt.handled) { \
				return ret; \
			} \
		}

#define END_EVT_MAP() \
		return ret; \
	} 

	//以下事件模型

	enum
	{
		PE_NOREMOVE = 0,
		PE_REMOVE,
	};

	class XBOOST_API EvtDispatcher
		//: public boost::asio::io_service
		//: public EvtFilter, public EvtIdler
	{
	private:
		static boost::mutex s_mutex_;
		static std::map<boost::thread::id, EvtDispatcher*> s_mapDispatcher;
	public:
		static void set_evtdispatcher(EvtDispatcher* pDispatcher, boost::thread::id ThreadId = boost::this_thread::get_id());
		static EvtDispatcher* get_evtdispatcher(boost::thread::id ThreadId = boost::this_thread::get_id());
	protected:
		enum status
		{
			stoped = 0,
			runing,
			stoping,
		};
		status status_;
		boost::asio::io_service& io_service_;
		std::shared_ptr<boost::asio::deadline_timer> timer_ptr_;

		typedef std::list<EvtFilter*> EvtFilters;
		EvtFilters evtfilters_;
		typedef std::list<EvtIdler*> EvtIdlers;
		EvtIdlers evtidlers_;
		//EvtFilter* evtfilter_;
		//EvtIdler* evtidler_;

		struct DelayEvent 
		{
			size_t time;
			size_t delay;
			size_t times;
			Event evt;
		};
		typedef std::list<DelayEvent> DelayEvents;
		DelayEvents delay_events_;

		size_t last_evt_time_;

	public:
		EvtDispatcher(boost::asio::io_service& io_service);
		//EvtDispatcher();
		virtual ~EvtDispatcher();

		inline boost::asio::io_service& get_service() { return io_service_; }

		virtual void register_evtfilter(EvtFilter* evtfilter);
		virtual void unregister_evtfilter(EvtFilter* evtfilter);

		virtual void register_evtidler(EvtIdler* evtidler);
		virtual void unregister_evtidler(EvtIdler* evtidler);

		virtual long send_event(Event& evt);
		virtual long post_event(Event& evt);
		virtual long post_delay_event(Event& evt, size_t delay, size_t times = 1);

	protected:
		void init();
		void term();
		virtual long dispatch_event(Event& evt);
		virtual bool pre_translate_event(Event& evt);
		long on_event(Event& evt);
		void on_delay(DelayEvent& delay_evt);
		void on_timer();
		void on_idle(size_t tick);
	};

	class XBOOST_API EvtThread
	{
	public:
		EvtThread();
		virtual ~EvtThread();

		inline boost::asio::io_service& get_service() { return io_service_; }

		template<typename F>
		bool start(F f)
		{
			thread_ptr_ = std::make_shared<std::thread>(
				//boost::bind(&EvtServer::run, this)
				[=]() {
					EvtDispatcher evtdispatcher(io_service_);
					on_init();
					f();
					on_term();
				}
			);
			return thread_ptr_!=nullptr;
		}
		bool start();
		void stop();

	protected:
		virtual void on_init() {}
		virtual void on_run();
		virtual void on_term() {}
		
	private:
		boost::asio::io_service io_service_;
		std::shared_ptr<std::thread> thread_ptr_;
	};

	XBOOST_API long send_event(Event& evt);
	XBOOST_API long post_event(Event& evt);

	//支持多线程注册监听，监听接收对象收到Event时，最好放到一个缓存队列里在另外线程处理，尽量不要占住EvtHandler发送的事件线程
	//如果EvtHandler模块实现了Post机制，对于post_event就没关系了，因为不会占住EvtHandler发送的事件线程
	template<typename TBase = EvtObject, typename TMutex = boost::recursive_mutex>
	class EvtHandlerT : virtual public TBase
	{
	public:
		typedef EvtHandlerT<TBase,TMutex> This;
		typedef TBase Base;
	protected:
		typedef std::list<EvtObject*> pEvtListeners;
		pEvtListeners listeners_;
		TMutex listeners_mutex_;
	public:
		EvtHandlerT()
		{

		}

		~EvtHandlerT()
		{

		}

		virtual void register_listener(EvtObject* listener)
		{
			boost::unique_lock<TMutex> lock(listeners_mutex_);
			if (listener) {
				BOOST_ASSERT(!is_register_listener(listener));
				listeners_.push_back(listener);
			}
		}

		virtual void unregister_listener(EvtObject* listener)
		{
			boost::unique_lock<TMutex> lock(listeners_mutex_);
			if (listener) {
				pEvtListeners::iterator it = std::find(listeners_.begin(), listeners_.end(), listener);
				if (it != listeners_.end()) {
					listeners_.erase(it);
				}
			}
		}

		virtual bool is_register_listener(EvtObject* listener)
		{
			if (!listener) {
				return false;
			}
			boost::unique_lock<TMutex> lock(listeners_mutex_);
			pEvtListeners::iterator it = std::find(listeners_.begin(), listeners_.end(), listener);
			if (it != listeners_.end()) {
				return true;
			}
			return false;
		}

		virtual bool is_listener_empty()
		{
			//boost::unique_lock<TMutex> lock(listeners_mutex_);
			return listeners_.empty();
		}

		/*virtual EvtObject* select_listener(size_t index)
		{
			if (index >= listeners_.size()) {
				return nullptr;
			}
			boost::unique_lock<TMutex> lock(listeners_mutex_);
			return listeners_[index];
		}*/

		virtual long send_to_listener(Event& evt)
		{
			boost::unique_lock<TMutex> lock(listeners_mutex_);
			ASSERT(is_register_listener(evt.dst));
			return send_event(evt);
		}

		virtual long post_to_listener(Event& evt)
		{
			boost::unique_lock<TMutex> lock(listeners_mutex_);
			ASSERT(is_register_listener(evt.dst));
			return post_event(evt);
		}

		long send_to_listener(EvtObject *listener, Event& evt)
		{
			evt.dst = listener;
			return send_event(evt);
		}

		long post_to_listener(EvtObject *listener, Event& evt)
		{
			evt.dst = listener;
			return post_event(evt);
		}

		virtual void broadcast_send_to_listener(Event& evt)
		{
			//EvtListener不要占住EvtHandler发送的事件线程
			boost::unique_lock<TMutex> lock(listeners_mutex_);
			pEvtListeners::iterator it = listeners_.begin();
			for (; it != listeners_.end();)
			{
				evt.dst = *it++;
				send_event(evt);
			}
			/*size_t index = 0;
			while ((evt.dst = select_listener(index++))) {
				send_event(evt);
			}*/
		}

		virtual void broadcast_post_to_listener(Event& evt)
		{
			//EvtListener不要占住EvtHandler发送的事件线程
			boost::unique_lock<TMutex> lock(listeners_mutex_);
			pEvtListeners::iterator it = listeners_.begin();
			for (; it != listeners_.end();)
			{
				evt.dst = *it++;
				post_event(evt);
			}
			/*size_t index = 0;
			while ((evt.dst = select_listener(index++))) {
				post_event(evt);
			}*/
		}
	};

	typedef EvtHandlerT<EvtObject> EvtHandler;

	//////////////////////////////////////////////////////////////////////////

	//注意事项同EvtHandler
	template<typename EvtHandler, typename TBase = EvtObject, typename TMutex = boost::recursive_mutex>
	class EvtListenerT : virtual public TBase
	{
		typedef EvtListenerT<EvtHandler,TBase,TMutex> This;
		typedef TBase Base;
	protected:
		typedef std::list<EvtHandler*> pEvtHandlers;
		pEvtHandlers handlers_;
		TMutex handlers_mutex_;
	public:

		virtual void add_handler(EvtHandler* handler)
		{
			if (handler) {
				BOOST_ASSERT(!is_handler(handler));
				handler->register_listener(this);

				boost::unique_lock<TMutex> lock(handlers_mutex_);
				handlers_.push_back(handler);
			}
		}

		virtual void remove_handler(EvtHandler* handler)
		{
			if (handler) {
				handler->unregister_listener(this);

				boost::unique_lock<TMutex> lock(handlers_mutex_);
				typename pEvtHandlers::iterator it = std::find(handlers_.begin(), handlers_.end(), handler);
				if (it != handlers_.end()) {
					handlers_.erase(it);
				}
			}
		}

		virtual void remove_all_handler()
		{
			boost::unique_lock<TMutex> lock(handlers_mutex_);
			for (typename pEvtHandlers::iterator it = handlers_.begin(); it != handlers_.end(); ++it)
			{
				(*it)->unregister_listener(this);
			}
			handlers_.clear();
		}

		virtual bool is_handler(EvtObject* handler)
		{
			boost::unique_lock<TMutex> lock(handlers_mutex_);
			EvtHandler* evthandler = dynamic_cast<EvtHandler*>(handler);
			if (!evthandler) {
				return false;
			}
			typename pEvtHandlers::iterator it = std::find(handlers_.begin(), handlers_.end(), handler);
			if (it != handlers_.end()) {
				return true;
			}
			return false;
		}

		virtual bool is_handler_empty()
		{
			//boost::unique_lock<TMutex> lock(handlers_mutex_);
			return handlers_.empty();
		}

		/*virtual EvtHandler* select_handler(size_t index) 
		{
			if (index >= handlers_.size()) {
				return nullptr;
			}
			boost::unique_lock<TMutex> lock(handlers_mutex_);
			return handlers_[index];
		}*/

		virtual long send_to_handler(Event& evt)
		{
			//boost::unique_lock<TMutex> lock(handlers_mutex_);
			ASSERT(is_handler(evt.dst));
			return send_event(evt);
		}

		virtual long post_to_handler(Event& evt)
		{
			//boost::unique_lock<TMutex> lock(handlers_mutex_);
			ASSERT(is_handler(evt.dst));
			return post_event(evt);
		}

		long send_to_handler(EvtHandler* handler, Event& evt)
		{
			evt.dst = handler;
			return send_event(evt);
		}

		long post_to_handler(EvtHandler* handler, Event& evt)
		{
			evt.dst = handler;
			return post_event(evt);
		}

		virtual void broadcast_send_to_handler(Event& evt)
		{
			boost::unique_lock<TMutex> lock(handlers_mutex_);
			typename pEvtHandlers::iterator it = handlers_.begin();
			for (; it != handlers_.end();)
			{
				evt.dst = *it++;
				send_event(evt);
			}
			/*size_t index = 0;
			while((evt.dst = select_handler(index++))) {
				send_event(evt);
			}*/
		}

		virtual void broadcast_post_to_handler(Event& evt)
		{
			boost::unique_lock<TMutex> lock(handlers_mutex_);
			typename pEvtHandlers::iterator it = handlers_.begin();
			for (; it != handlers_.end();)
			{
				evt.dst = *it++;
				post_event(evt);
			}
			/*size_t index = 0;
			while ((evt.dst = select_handler(index++))) {
				post_event(evt);
			}*/
		}
	};

	typedef EvtListenerT<EvtHandler, EvtObject> EvtListener;

	//////////////////////////////////////////////////////////////////////////

	template<class EvtListener = EvtObject, class EvtHandler = EvtObject>
	class EvtProcesserT : public EvtListener, public EvtHandler
	{
		typedef EvtProcesserT<EvtListener, EvtHandler> This;
	public:

		BEGIN_EVT_MAP(This)
			if (EvtListener::is_register_listener(evt.src)) {
				ret = EvtHandler::on_event(evt);	//收到监听者调用
			}
			else if(EvtHandler::is_handler(evt.src)){
				ret = EvtListener::on_event(evt);	//收到处理者回调
			}
			if (evt.handled) {
				return ret;
			}
		END_EVT_MAP()
	};

	//////////////////////////////////////////////////////////////////////////
	template<class T, class _Ty, class _Pr = std::less<_Ty>>
	class EvtRequestT
	{
	protected:
		typedef std::multimap<Event, _Ty, EventLess> Evt2MultiReq;
		typedef std::map<_Ty, Event, _Pr> Req2Evt;
		Evt2MultiReq evt2subreqs_;
		Req2Evt subreq2evt_;
		_Pr reqpr_;

	public:
		EvtRequestT()
		{

		}
		~EvtRequestT()
		{
			ASSERT(evt2subreqs_.empty() && subreq2evt_.empty());
		}

		void clear(bool bNotify = true)
		{
			T* pT = static_cast<T*>(this);
			if (bNotify) {
				while (subreq2evt_.size())
				{
					typename Req2Evt::iterator it = subreq2evt_.begin();
					pT->update_request(it->first);
				}
			}
			evt2subreqs_.clear();
			subreq2evt_.clear();
		}

		//取消请求
		void on_remove_request(const Event& Req, const _Ty& SubReq)
		{

		}
		void remove_request(const Event& Req)
		{
			T* pT = static_cast<T*>(this);
			std::pair<typename Evt2MultiReq::const_iterator, typename Evt2MultiReq::const_iterator> pr = evt2subreqs_.equal_range(Req);
			typename Evt2MultiReq::const_iterator it = pr.first;
			for (; it != pr.second; ++it)
			{
				pT->on_remove_request(Req, it->second);
				subreq2evt_.erase(it->second);
			}
			evt2subreqs_.erase(pr.first, pr.second);
		}

		//增加请求
		void on_add_request(const Event& Req, const _Ty& SubReq)
		{

		}
		void add_request(const Event& Req, const _Ty& SubReq)
		{
			T* pT = static_cast<T*>(this);
			subreq2evt_.insert(std::make_pair(SubReq, Req));
			typename Evt2MultiReq::iterator it = evt2subreqs_.insert(std::make_pair(Req, SubReq));
			if (it != evt2subreqs_.end()) {
				pT->on_add_request(Req, SubReq);
			}
		}

		//更新请求
		void on_update_request(const Event& Req, const _Ty& SubReq, long SubReqCount)
		{

		}
		bool update_request(const _Ty& SubReq, Event& Req)
		{
			T* pT = static_cast<T*>(this);
			long size = 0;
			typename Req2Evt::iterator it = subreq2evt_.find(SubReq);
			if (it != subreq2evt_.end()) {
				Req = it->second;
				size = evt2subreqs_.count(Req);
				std::pair<typename Evt2MultiReq::const_iterator, typename Evt2MultiReq::const_iterator> pr = evt2subreqs_.equal_range(it->second);
				for (typename Evt2MultiReq::const_iterator cit = pr.first; cit != pr.second;++cit)
					if (!reqpr_(it->first, cit->second)
						&& !reqpr_(cit->second, it->first)) {
						evt2subreqs_.erase(cit);
						break;
					}
				subreq2evt_.erase(it);
				size -= 1;
				pT->on_update_request(Req, SubReq, size);
				return true;
			}
			return false;
		}

		bool update_request(const _Ty& SubReq)
		{
			T* pT = static_cast<T*>(this);
			Event Req;
			return pT->update_request(SubReq, Req);
		}

		//查找请求
		bool find_request(const _Ty& SubReq, Event& Req)
		{
			//T* pT = static_cast<T*>(this);
			long size = 0;
			typename Req2Evt::iterator it = subreq2evt_.find(SubReq);
			if (it != subreq2evt_.end()) {
				Req = it->second;
				return true;
			}
			return false;
		}
	};

	//////////////////////////////////////////////////////////////////////////

	class EvtMultiDispatcher
	{
	private:
		typedef std::vector<EvtDispatcher*> pEvtDispatchers;
		pEvtDispatchers dispatchers_;
		size_t index_;
		boost::mutex mutex_;
	public:
		EvtMultiDispatcher():index_(0)
		{

		}
		void add(EvtDispatcher* dispatcher) {
			boost::unique_lock<boost::mutex> lock(mutex_);
			dispatchers_.push_back(dispatcher);
		}
		void remove(EvtDispatcher* dispatcher) {
			boost::unique_lock<boost::mutex> lock(mutex_);
			pEvtDispatchers::iterator it = std::find(dispatchers_.begin(), dispatchers_.end(), dispatcher);
			if (it != dispatchers_.end()) {
				dispatchers_.erase(it);
			}
		}
		void clear() {
			boost::unique_lock<boost::mutex> lock(mutex_);
			dispatchers_.clear();
			index_ = 0;
		}
		EvtDispatcher* select() {
			if (dispatchers_.empty()) {
				return nullptr;
			}
			boost::unique_lock<boost::mutex> lock(mutex_);
			return dispatchers_[index_++%dispatchers_.size()];
		}

		long post_event(Event& evt) {
			EvtDispatcher* dispatcher = select();
			if (dispatcher) {
				dispatcher->post_event(evt);
			}
		}
	};

	////////////////////////////////////////////////////////////////////////////////////
	//以上事件模型支持两种模型
	//模型1: 使用EvtDispatcher事件分发器，只需从EvtObject派生，然后使用EvtDispatcher的
	//send_event[直接发送事件]/post_event[异步发送事件]
	//模型2: 使用EvtListenner/EvtHandler监听处理模型，监听者向处理者注册监听，然后监听者
	//使用on_event让处理者处理相关处理相关请求，处理者当事件发生时直接回调监听者on_event
	//因为都使用on_event,所以两者都支持使用BEGIN_EVT_MAP/END_EVT_MAP

}

#endif//_H_XEVENT_HPP_