#ifndef PARSERS_H_
#define PARSERS_H_
#include <lexbor/html/parser.h>
#include <lexbor/dom/interfaces/element.h>
#include <lexbor/html/interfaces/element.h>
#include <lexbor/html/interfaces/document.h>
#include <lexbor/core/fs.h>
#include <lexbor/css/css.h>

lxb_css_stylesheet_t *apply_css(lxb_html_document_t *document, char *stylesheet);
lxb_html_document_t *html_to_element(char *filename);

#endif