#include "adapters/parsers.h"
#include "renderer.h"
#include "net/tcp_debugger.h"
#include "net/net.h"

int main(int argc, const char *argv[])
{
    lxb_html_document_t *document;
    lxb_css_stylesheet_t *css;
    net_init();
    debug_init();
    document = html_to_element("index.html");
    css = apply_css(document, "style.css");
    debug("Document parsed.", NULL);
    graph_init();
    debug("Graph init done, rendering.", NULL);
    render_document(document, css);
    (void) lxb_html_document_destroy(document);
    (void) lxb_css_stylesheet_destroy(css, true);
    return EXIT_SUCCESS;
}
