#include <lexbor/html/parser.h>
#include <lexbor/dom/interfaces/element.h>
#include <lexbor/html/interfaces/document.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include "renderer.h"


int main(int argc, const char *argv[])
{
    lxb_status_t status;
    const lxb_char_t *tag_name;
    lxb_html_document_t *document;

    static const lxb_char_t html[] = "<div><h1>Works fine!</h1></div><div><p><h2>TestTitle</h2>TestParagraph</p>TEST</div>";
    size_t html_len = sizeof(html) - 1;

    document = lxb_html_document_create();
    if (document == NULL) {
        exit(EXIT_FAILURE);
    }

    printf("Parsing document...\n");
    status = lxb_html_document_parse(document, html, html_len);
    if (status != LXB_STATUS_OK) {
        exit(EXIT_FAILURE);
    }
    printf("Document parsed!\nGraph init...");
    graph_init();
    printf("Graph init done! Rendering.\n");
    render_document(document);
    lxb_html_document_destroy(document);
    return EXIT_SUCCESS;
}
