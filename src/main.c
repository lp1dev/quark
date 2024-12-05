#include "adapters/parsers.h"
#include "renderer.h"
// #include "browser/js.h"


int main(int argc, const char *argv[])
{
    lxb_html_document_t *document;
    lxb_css_stylesheet_t *css;

    document = html_to_element("index.html");
    css = apply_css(document, "style.css");

    printf("Document parsed!\nGraph init...");
    graph_init();
    printf("Graph init done! Rendering.\n");
    render_document(document, css);
    (void) lxb_html_document_destroy(document);
    (void) lxb_css_stylesheet_destroy(css, true);
    return EXIT_SUCCESS;
}
