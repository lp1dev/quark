#include "parsers.h"
#include "../helpers.h"


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

    status = lxb_html_document_css_init(document);
    if (status != LXB_STATUS_OK) {
        FAILED("Failed to CSS initialization");
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

    return sst;
}


/* Parsing a HTML document and returning the output lxb_html_document_t*/
lxb_html_document_t *html_to_element(char *filename) {

    lxb_html_document_t *document = parse_html(filename);
    lxb_html_element_t *element = lxb_html_element_interface_create(document);
    return document;
}

/* 

lxb_css_stylesheet_t *apply_css(lxb_html_document_t *document, char *stylesheet)

    Apply a css file to a html document.

*/
lxb_css_stylesheet_t *apply_css(lxb_html_document_t *document, char *stylesheet) {
    lexbor_status_t status;
    lxb_css_stylesheet_t *css = parse_css(stylesheet);

    status = lxb_html_document_stylesheet_apply(document, css);
    if (status != LXB_STATUS_OK) {
        return NULL;
    }
    status = lxb_html_document_stylesheet_attach(document, css);
    if (status != LXB_STATUS_OK) {
        return NULL;
    }
    return css;
}
