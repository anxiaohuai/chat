#include "chat_servlet.h"
#include "helens/log.h"
#include "protocol.h"

namespace chat {

static helens::Logger::ptr g_logger = HELENS_LOG_ROOT();

helens::RWMutex m_mutex;
//name -> session
std::map<std::string, helens::http::WSSession::ptr> m_sessions;

bool session_exists(const std::string& id) {
    HELENS_LOG_INFO(g_logger) << "session_exists id=" << id;
    helens::RWMutex::ReadLock lock(m_mutex);
    auto it = m_sessions.find(id);
    return it != m_sessions.end();
}

void session_add(const std::string& id, helens::http::WSSession::ptr session) {
    HELENS_LOG_INFO(g_logger) << "session_add id=" << id;
    helens::RWMutex::WriteLock lock(m_mutex);
    m_sessions[id] = session;
}

void session_del(const std::string& id) {
    HELENS_LOG_INFO(g_logger) << "session_add del=" << id;
    helens::RWMutex::WriteLock lock(m_mutex);
    m_sessions.erase(id);
}

int32_t SendMessage(helens::http::WSSession::ptr session
                    , ChatMessage::ptr msg) {
    HELENS_LOG_INFO(g_logger) << msg->toString() << " - " << session;
    return session->sendMessage(msg->toString()) > 0 ? 0: 1;
}

void session_notify(ChatMessage::ptr msg, helens::http::WSSession::ptr session = nullptr) {
    helens::RWMutex::ReadLock lock(m_mutex);
    auto sessions = m_sessions;
    lock.unlock();

    for(auto& i : sessions) {
        if(i.second == session) {
            continue;
        }
        SendMessage(i.second, msg);
    }
}

ChatWSServlet::ChatWSServlet()
    :helens::http::WSServlet("chat_servlet") {
}

int32_t ChatWSServlet::onConnect(helens::http::HttpRequest::ptr header
                              ,helens::http::WSSession::ptr session) {
    HELENS_LOG_INFO(g_logger) << "onConnect " << session;
    return 0;
}

int32_t ChatWSServlet::onClose(helens::http::HttpRequest::ptr header
                             ,helens::http::WSSession::ptr session) {
    auto id = header->getHeader("$id");
    HELENS_LOG_INFO(g_logger) << "onClose " << session << " id=" << id;
    if(!id.empty()) {
        session_del(id);
        ChatMessage::ptr nty(new ChatMessage);
        nty->set("type", "user_leave");
        nty->set("time", helens::Time2Str());
        nty->set("name", id);
        session_notify(nty);
    }
    return 0;
}

int32_t ChatWSServlet::handle(helens::http::HttpRequest::ptr header
                           ,helens::http::WSFrameMessage::ptr msgx
                           ,helens::http::WSSession::ptr session) {
    HELENS_LOG_INFO(g_logger) << "handle " << session
            << " opcode=" << msgx->getOpcode()
            << " data=" << msgx->getData();

    auto msg = ChatMessage::Create(msgx->getData());
    auto id = header->getHeader("$id");
    if(!msg) {
        if(!id.empty()) {
            helens::RWMutex::WriteLock lock(m_mutex);
            m_sessions.erase(id);
        }
        return 1;
    }

    ChatMessage::ptr rsp(new ChatMessage);
    auto type = msg->get("type");
    if(type == "login_request") {
        rsp->set("type", "login_response");
        auto name = msg->get("name");
        if(name.empty()) {
            rsp->set("result", "400");
            rsp->set("msg", "name is null");
            return SendMessage(session, rsp);
        }
        if(!id.empty()) {
            rsp->set("result", "401");
            rsp->set("msg", "logined");
            return SendMessage(session, rsp);
        }
        if(session_exists(id)) {
            rsp->set("result", "402");
            rsp->set("msg", "name exists");
            return SendMessage(session, rsp);
        }
        id = name;
        header->setHeader("$id", id);
        rsp->set("result", "200");
        rsp->set("msg", "ok");
        session_add(id, session);

        ChatMessage::ptr nty(new ChatMessage);
        nty->set("type", "user_enter");
        nty->set("time", helens::Time2Str());
        nty->set("name", name);
        session_notify(nty, session);
        return SendMessage(session, rsp);
    } else if(type == "send_request") {
        rsp->set("type", "send_response");
        auto m = msg->get("msg");
        if(m.empty()) {
            rsp->set("result", "500");
            rsp->set("msg", "msg is null");
            return SendMessage(session, rsp);
        }
        if(id.empty()) {
            rsp->set("result", "501");
            rsp->set("msg", "not login");
            return SendMessage(session, rsp);
        }

        rsp->set("result", "200");
        rsp->set("msg", "ok");

        ChatMessage::ptr nty(new ChatMessage);
        nty->set("type", "msg");
        nty->set("time", helens::Time2Str());
        nty->set("name", id);
        nty->set("msg", m);
        session_notify(nty, nullptr);
        return SendMessage(session, rsp);
        //TODO notify
    }
    return 0;
}

}
