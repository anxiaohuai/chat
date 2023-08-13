#ifndef __HELENS_HTTP_RESOURCE_SERVLET_H__
#define __HELENS_HTTP_RESOURCE_SERVLET_H__

#include "helens/http/servlet.h"

namespace helens {
namespace http {

class ResourceServlet :public helens::http::Servlet {
public:
    typedef std::shared_ptr<ResourceServlet> ptr;
    ResourceServlet(const std::string& path);
    virtual int32_t handle(helens::http::HttpRequest::ptr request
                   , helens::http::HttpResponse::ptr response
                   , helens::http::HttpSession::ptr session) override;

private:
    std::string m_path;
};

}
}

#endif
