#include "resource_servlet.h"
#include "helens/log.h"
#include <iostream>
#include <fstream>

namespace helens {
namespace http {

static helens::Logger::ptr g_logger = HELENS_LOG_ROOT();

ResourceServlet::ResourceServlet(const std::string& path)
    :Servlet("ResourceServlet")
    ,m_path(path) {
}

int32_t ResourceServlet::handle(helens::http::HttpRequest::ptr request
                           , helens::http::HttpResponse::ptr response
                           , helens::http::HttpSession::ptr session) {
    auto path = m_path + "/" + request->getPath();
    HELENS_LOG_INFO(g_logger) << "handle path=" << path;
    if(path.find("..") != std::string::npos) {
        response->setBody("invalid path");
        response->setStatus(helens::http::HttpStatus::NOT_FOUND);
        return 0;
    } 
    std::ifstream ifs(path);
    if(!ifs) {
        response->setBody("invalid file");
        response->setStatus(helens::http::HttpStatus::NOT_FOUND);
        return 0;
    }

    std::stringstream ss;
    std::string line;
    while(std::getline(ifs, line)) {
        //HELENS_LOG_INFO(g_logger) << line;
        ss << line << std::endl;
    }

    response->setBody(ss.str());
    response->setHeader("content-type", "text/html;charset=utf-8");
    return 0;
}

}
}
