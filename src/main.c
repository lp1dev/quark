#include <lexbor/html/parser.h>
#include <lexbor/dom/interfaces/element.h>
#include <lexbor/html/interfaces/document.h>
#include <lexbor/core/fs.h>
#include <lexbor/css/css.h>
#include "helpers.h"
#include "renderer.h"

static lxb_status_t
callback(const lxb_char_t *data, size_t len, void *ctx)
{
    // printf("%.*s", (int) len, (const char *) data);
    return LXB_STATUS_OK;
}

lxb_html_document_t *parse_html(char *filename) {
    lxb_status_t status;
    const lxb_char_t *tag_name;
    lxb_html_document_t *document;
    size_t html_len;
    lxb_char_t *input;
    const lxb_char_t *fl;

    fl = (const lxb_char_t *) filename;
    input = lexbor_fs_file_easy_read(fl, &html_len);
    if (input == NULL) {
        FAILED("Failed to read HTML file");
    }

    document = lxb_html_document_create();
    if (document == NULL) {
        exit(EXIT_FAILURE);
    }

    status = lxb_html_document_parse(document, input, html_len);
    if (status != LXB_STATUS_OK) {
        exit(EXIT_FAILURE);
    }
    return document;
}

lxb_css_stylesheet_t *parse_css(char *filename) {
    size_t css_len;
    lxb_char_t *css;
    lxb_status_t status;
    lxb_css_parser_t *parser;
    lxb_css_stylesheet_t *sst;
    const lxb_char_t *fl;

    fl = (const lxb_char_t *) filename;

    css = lexbor_fs_file_easy_read(fl, &css_len);
    if (css == NULL) {
        FAILED("Failed to read CSS file");
    }

    parser = lxb_css_parser_create();
    status = lxb_css_parser_init(parser, NULL);
    if (status != LXB_STATUS_OK) {
        FAILED("Failed to create CSS Parser");
    }

    sst = lxb_css_stylesheet_parse(parser, css, css_len);

    (void) lexbor_free(css);
    (void) lxb_css_parser_destroy(parser, true);

    if (sst == NULL) {
        FAILED("Failed to parse CSS");
    }

    status = lxb_css_rule_serialize(sst->root, callback, NULL);
    if (status != LXB_STATUS_OK) {
        FAILED("Failed to serialize StyleSheet");
    }
    return sst;
}


int main(int argc, const char *argv[])
{   
    lxb_dom_collection_t *collection;

    lxb_html_document_t *document = parse_html("index.html");
    /* Init all CSS objects and mem for document. */
    int status = lxb_html_document_css_init(document);
    if (status != LXB_STATUS_OK) {
        return EXIT_FAILURE;
    }
    lxb_css_stylesheet_t *css = parse_css("browser.css");
    // Attach stylesheet to document
    status = lxb_html_document_stylesheet_attach(document, css);
    if (status != LXB_STATUS_OK) {
        return EXIT_FAILURE;
    }

    collection = lxb_dom_collection_make(lxb_dom_interface_document(document), 16);
    if (collection == NULL) {
        return EXIT_FAILURE;
    }

    // printf("CSS -> %.*s\n", css->root);
    printf("Document parsed!\nGraph init...");
    graph_init();
    printf("Graph init done! Rendering.\n");
    render_document(document);
    (void) lxb_html_document_destroy(document);
    (void) lxb_css_stylesheet_destroy(css, true);
    return EXIT_SUCCESS;
}
