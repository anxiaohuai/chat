#ifndef __CHAT_CHAT_SERVLET_H__
#define __CHAT_CHAT_SERVLET_H__

#include "helens/http/ws_servlet.h"

namespace chat {

class ChatWSServlet : public helens::http::WSServlet {
public:
    typedef std::shared_ptr<ChatWSServlet> ptr;
    ChatWSServlet();
    virtual int32_t onConnect(helens::http::HttpRequest::ptr header
                              ,helens::http::WSSession::ptr session) override;
    virtual int32_t onClose(helens::http::HttpRequest::ptr header
                             ,helens::http::WSSession::ptr session) override;
    virtual int32_t handle(helens::http::HttpRequest::ptr header
                           ,helens::http::WSFrameMessage::ptr msg
                           ,helens::http::WSSession::ptr session) override;
};

}

#endif
