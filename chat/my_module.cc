#include "my_module.h"
#include "helens/config.h"
#include "helens/log.h"
#include "helens/application.h"
#include "helens/http/ws_server.h"

#include "resource_servlet.h"
#include "chat_servlet.h"
#include "helens/env.h"

namespace chat {

static helens::Logger::ptr g_logger = HELENS_LOG_ROOT();

MyModule::MyModule()
    :helens::Module("chat_room", "1.0", "") {
}

bool MyModule::onLoad() {
    HELENS_LOG_INFO(g_logger) << "onLoad";
    return true;
}

bool MyModule::onUnload() {
    HELENS_LOG_INFO(g_logger) << "onUnload";
    return true;
}

//static int32_t handle(helens::http::HttpRequest::ptr request
//                       , helens::http::HttpResponse::ptr response
//                       , helens::http::HttpSession::ptr session) {
//    HELENS_LOG_INFO(g_logger) << "handle";
//    response->setBody("handle");
//    return 0;
//}

bool MyModule::onServerReady() {
    HELENS_LOG_INFO(g_logger) << "onServerReady";
    std::vector<helens::TcpServer::ptr> svrs;
    if(!helens::Application::GetInstance()->getServer("http", svrs)) {
        HELENS_LOG_INFO(g_logger) << "no httpserver alive";
        return false;
    }

    for(auto& i : svrs) {
        helens::http::HttpServer::ptr http_server =
            std::dynamic_pointer_cast<helens::http::HttpServer>(i);
        if(!i) {
            continue;
        }
        auto slt_dispatch = http_server->getServletDispatch();

        helens::http::ResourceServlet::ptr slt(new helens::http::ResourceServlet(
                    helens::EnvMgr::GetInstance()->getCwd()
        ));
        slt_dispatch->addGlobServlet("/html/*", slt);
        HELENS_LOG_INFO(g_logger) << "addServlet";
    }

    svrs.clear();
    if(!helens::Application::GetInstance()->getServer("ws", svrs)) {
        HELENS_LOG_INFO(g_logger) << "no ws alive";
        return false;
    }

    for(auto& i : svrs) {
        helens::http::WSServer::ptr ws_server =
            std::dynamic_pointer_cast<helens::http::WSServer>(i);

        helens::http::ServletDispatch::ptr slt_dispatch = ws_server->getWSServletDispatch();
        ChatWSServlet::ptr slt(new ChatWSServlet);
        slt_dispatch->addServlet("/helens/chat", slt);
    }
    return true;
}


bool MyModule::onServerUp() {
    HELENS_LOG_INFO(g_logger) << "onServerUp";
    return true;
}

}

extern "C" {

helens::Module* CreateModule() {
    helens::Module* module = new chat::MyModule;
    HELENS_LOG_INFO(chat::g_logger) << "CreateModule " << module;
    return module;
}

void DestoryModule(helens::Module* module) {
    HELENS_LOG_INFO(chat::g_logger) << "CreateModule " << module;
    delete module;
}

}
