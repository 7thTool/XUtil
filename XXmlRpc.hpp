#pragma once

#ifndef _H_XMLRPC_HPP_
#define _H_XMLRPC_HPP_

#include <map>
#include <memory>
#include <future>
#include <thread>
#include <mutex>
//#define BOOST_THREAD_PROVIDES_FUTURE
//#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
//#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
//#include <boost/thread/future.hpp>
#include "XXml.hpp"

namespace XUtil {

template<class T>
class XXmlRpc 
{
public:
    static const size_t INVALID_REQUEST_ID = size_t(-1);
	class Packet
	{
	public:
        Packet(size_t p, const std::shared_ptr<boost::property_tree::ptree>& d):peer(p),json(d),id((size_t)-1) {}
        Packet(size_t p, const std::shared_ptr<boost::property_tree::ptree>& d, size_t x):peer(p),json(d),id(x) {}
        size_t peer;
		std::shared_ptr<boost::property_tree::ptree> json;
		size_t id;
	};
	class Response : public Packet
	{
        typedef Packet Base;
	public:
        Response(size_t p, bool e, const std::shared_ptr<boost::property_tree::ptree>& data, size_t id):Base(p, data, id) {
            if(e) {
                error = this->json;
            } else {
                result = this->json;
            }
        }
		std::shared_ptr<boost::property_tree::ptree> error;
        std::shared_ptr<boost::property_tree::ptree> result;
	};
	class Request : public Packet
	{
        typedef Packet Base;
	public:
        Request(size_t p, const std::string& m, const std::shared_ptr<boost::property_tree::ptree>& data, size_t id
        , std::shared_ptr<std::promise<std::shared_ptr<Response>>> r):Base(p, data, id), method(m),response(r) { 
            params = this->json;
        }
		std::string method;
		std::shared_ptr<boost::property_tree::ptree> params;
        std::shared_ptr<std::promise<std::shared_ptr<Response>>> response;
	};
private:
	size_t _requestID;
	std::map<size_t,std::shared_ptr<Request>> _openRequests;
    std::mutex mutex_;
public:
	XXmlRpc () {
        _requestID = 0;
        //_openRequests;
    }

    inline void clear()
    {
		std::unique_lock<std::mutex> lock(mutex_);
        _openRequests.clear();
        _requestID = 0;
    }

    /**
     * Make an RPC request and retrieve the result.
     * @param {string} method - the remote method to call.
     * @param {object} params - the parameters to pass to the remote method.
     * @returns {Promise} - a promise for the result of the call.
     */
    //std::shared_ptr<std::promise<std::shared_ptr<Response>>> 
    std::future<std::shared_ptr<Response>>
    inline sendRemoteRequest (size_t peer, const std::string& method, const std::shared_ptr<boost::property_tree::ptree>& params) {
        T* pT = static_cast<T*>(this);
        std::unique_lock<std::mutex> lock(mutex_);
        const size_t requestID = _requestID++;

        std::shared_ptr<std::promise<std::shared_ptr<Response>>> promise = std::make_shared<std::promise<std::shared_ptr<Response>>>();
        std::shared_ptr<Request> request = std::make_shared<Request>(peer, method, params, requestID, promise);
        _openRequests[requestID] = request;
        pT->_sendRequest(peer, method, params, requestID);
        
        /*const promise = new Promise((resolve, reject) => {
            _openRequests[requestID] = {resolve, reject};
        });

        _sendRequest(method, params, requestID);*/

        return promise->get_future();
    }

    /**
     * Make an RPC notification with no expectation of a result or callback.
     * @param {string} method - the remote method to call.
     * @param {object} params - the parameters to pass to the remote method.
     */
    inline void sendRemoteNotification (size_t peer, const std::string& method, const std::shared_ptr<boost::property_tree::ptree>& params) {
        T* pT = static_cast<T*>(this);
        pT->_sendRequest(peer, method, params, INVALID_REQUEST_ID);
    }

    inline void sendRemoteNotification (size_t peer, const std::string& msg) {
        T* pT = static_cast<T*>(this);
        pT->_sendMessage(peer, msg);
    }

    /**
     * Handle an RPC request from remote, should return a result or Promise for result, if appropriate.
     * @param {string} method - the method requested by the remote caller.
     * @param {object} params - the parameters sent with the remote caller's request.
     */
    inline bool didReceiveCall (size_t peer, const std::string& method, const std::shared_ptr<boost::property_tree::ptree>& params, std::shared_ptr<boost::property_tree::ptree>& result) {
        //throw new Error('Must override didReceiveCall');
        return nullptr;
    }

protected:
	inline bool json_to_str(const boost::property_tree::ptree &json, std::string &str)
	{
		try
		{
			std::ostringstream ss;
			boost::property_tree::write_json(ss, json);
			str = ss.str();
#if _TEST
			printf("send json: \n%s\n", str.c_str());
#endif
			return true;
		}
		catch (...)
		{
		}
		return false;
	}

	inline bool str_to_json(const std::string &str, boost::property_tree::ptree &json)
	{
		try
		{
#if _TEST
//			printf("recv json: \n%s\n", str.c_str());
#endif
			std::istringstream ss;
			ss.str(str);
			boost::property_tree::json_parser::read_json(ss, json);
			return true;
		}
		catch (...)
		{
		}
		return false;
	}
    //-1:error 0:request 1:result
    inline int parse_packet(const boost::property_tree::ptree& json, std::string& method, boost::property_tree::ptree& data, size_t& id)
	{
        int ret = -1;
		method = json.get<std::string>("method", "");
		if(!method.empty()) {
			auto opt = json.get_child_optional("params");
			if (opt) {
				data = opt.get();
			}
            ret = 0;
		} else {
			auto opt = json.get_child_optional("result");
			if (opt) {
				data = opt.get();
                ret = 1;
			} else {
                auto opt = json.get_child_optional("error");
                if (opt) {
                    data = opt.get();
                    ret = -1;
                }
            }
        }
		id = json.get<size_t>("id", INVALID_REQUEST_ID);
		return ret;
	}
	inline void build_request(std::string& str, const std::string& method, const std::string& params, size_t id = INVALID_REQUEST_ID)
	{
        std::stringstream ss;
        ss << R"({"jsonrpc": "2.0", "method":")" << method << R"(","params":)" << params << R"(,"id":)" << id << R"(})";
        str = ss.str();
	}
	inline void build_request(boost::property_tree::ptree& json, const std::string& method, const boost::property_tree::ptree& params, size_t id)
	{
		json.put("jsonrpc", "2.0");
		json.put("method", method);
		json.put_child("params", params);
		json.put("id", id);
	}
	inline void build_request(std::string& str, const std::string& method, const boost::property_tree::ptree& params, size_t id)
	{
		boost::property_tree::ptree json;
		build_request(json, method, params, id);
		json_to_str(json, str);
	}
	inline void build_response(boost::property_tree::ptree& json, const boost::property_tree::ptree& result, size_t id)
	{
		json.put("jsonrpc", "2.0");
		json.put_child("result", result);
		json.put("id", id);
	}
	inline void build_error_response(boost::property_tree::ptree& json, const boost::property_tree::ptree& error, size_t id)
	{
		json.put("jsonrpc", "2.0");
		json.put_child("error", error);
		json.put("id", id);
	}

    inline void _sendMessage (size_t peer, const std::string& msg) {
        //throw new Error('Must override _sendMessage');
    }

    inline void _sendRequest (size_t peer, const std::string& method, std::shared_ptr<boost::property_tree::ptree> params, size_t id) {
        T* pT = static_cast<T*>(this);
        std::string msg;
        boost::property_tree::ptree json;
        build_request(json, method, params?*params:boost::property_tree::ptree(), id);
        json_to_str(json, msg);
        pT->_sendMessage(peer, msg);
    }

    inline void _handleMessage (size_t peer, const std::string& msg) {
        T* pT = static_cast<T*>(this);
        boost::property_tree::ptree json;
		if(str_to_json(msg, json)) {
			std::string method;
			std::shared_ptr<boost::property_tree::ptree> data = std::make_shared<boost::property_tree::ptree>();
			size_t id;
            switch(parse_packet(json, method, *data, id))
            {
            case 0:
			{
                //丢到service所在线程执行，这样保持和RPC都在一个线程执行
                //pT->service().post(boost::bind(&T::_handleRequest, pT, peer, method, data, id));
                pT->_handleRequest(peer, method, data, id);
            }
            break;
            case 1:
			{
                pT->_handleResponse(peer, nullptr, data, id);
            }
            break;
            default:
            {
                pT->_handleResponse(peer, data, nullptr, id);
            }
            break;
            }
        }
        /*if (json.jsonrpc !== '2.0') {
            throw new Error(`Bad or missing JSON-RPC version in message: ${json}`);
        }
        if (json.hasOwnProperty('method')) {
            _handleRequest(json);
        } else {
            _handleResponse(json);
        }*/
    }

    inline void _sendResponse (size_t peer, size_t id, const std::shared_ptr<boost::property_tree::ptree>& data, bool error) {
        T* pT = static_cast<T*>(this);
        std::string msg;
        boost::property_tree::ptree json;
        if(error) {
            build_error_response(json, data?*data:boost::property_tree::ptree(), id);
        } else {
            build_response(json, data?*data:boost::property_tree::ptree(), id);
        }
        json_to_str(json, msg);
        pT->_sendMessage(peer, msg);
        /*const response = {
            jsonrpc: '2.0',
            id
        };
        if (error) {
            response.error = error;
        } else {
            response.result = result || null;
        }
        _sendMessage(response);*/
    }

    inline void _handleResponse (size_t peer, const std::shared_ptr<boost::property_tree::ptree>& error, const std::shared_ptr<boost::property_tree::ptree>& result, size_t id) {
        T* pT = static_cast<T*>(this);
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = _openRequests.find(id);
        if(it != _openRequests.end()) {
            const std::shared_ptr<Request> openRequest = it->second;
            if(openRequest) {
                if(error) {
                    openRequest->response->set_value(std::make_shared<Response>(peer,true,error,id));
                } else {
                    openRequest->response->set_value(std::make_shared<Response>(peer,false,result,id));
                }
            }
        }
        /*const {result, error, id} = json;
        const openRequest = _openRequests[id];
        delete _openRequests[id];
        if (openRequest) {
            if (error) {
                openRequest.reject(error);
            } else {
                openRequest.resolve(result);
            }
        }*/
    }

    inline void _handleRequest (size_t peer, const std::string& method, const std::shared_ptr<boost::property_tree::ptree>& params, size_t id) {
        T* pT = static_cast<T*>(this);
        std::shared_ptr<boost::property_tree::ptree> result;
        bool ret = didReceiveCall(peer, method, params, result);
        if (id != INVALID_REQUEST_ID) {
            pT->_sendResponse(peer, id, result, !ret);
        }
        /*const {method, params, id} = json;
        const rawResult = didReceiveCall(method, params);
        if (id) {
            Promise.resolve(rawResult).then(
                result => {
                    _sendResponse(id, result);
                },
                error => {
                    _sendResponse(id, null, error);
                }
            );
        }*/
    }
};

}

#endif//_H_XMLRPC_HPP_
