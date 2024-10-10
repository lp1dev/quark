#ifndef _RENDERER_H_
#define _RENDERER_H_

#define LIST_SIZE 255

int graph_init(void);
void render_document(lxb_html_document_t *document, lxb_css_stylesheet_t *css);

#endif