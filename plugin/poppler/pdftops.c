#include <stdlib.h>
#include <poppler.h>
#include <cairo.h>
#include <cairo-ps.h>

int main(int argc, char *argv[])
{
    PopplerDocument *document;
    PopplerPage *page;
    double width, height;
    GError *error;
    const char *filename;
    gchar *absolute, *uri;
    int num_pages, i;
    cairo_surface_t *surface;
    cairo_t *cr;
    cairo_status_t status;

    if (argc != 2) {
        printf ("Usage: pdf2cairo input_file.pdf\n");
        return 0;
    }

    filename = argv[1];
    g_type_init ();
    error = NULL;

    if (g_path_is_absolute(filename)) {
        absolute = g_strdup (filename);
    } else {
        gchar *dir = g_get_current_dir ();
        absolute = g_build_filename (dir, filename, (gchar *) 0);
        free (dir);
    }

    uri = g_filename_to_uri (absolute, NULL, &error);
    free (absolute);
    if (uri == NULL) {
        printf("poppler fail: %s\n", error->message);
        return 1;
    }

    document = poppler_document_new_from_file (uri, NULL, &error);
    if (document == NULL) {
        printf("poppler fail: %s\n", error->message);
        return 1;
    }

    num_pages = poppler_document_get_n_pages (document);

    /* Page size does not matter here as the size is changed before
 *      * each page */
    surface = cairo_ps_surface_create ("output.ps", 595, 842);
    cr = cairo_create (surface);
    for (i = 0; i < num_pages; i++) {
        page = poppler_document_get_page (document, i);
        if (page == NULL) {
            printf("poppler fail: page not found\n");
            return 1;
        }
        poppler_page_get_size (page, &width, &height);
        cairo_ps_surface_set_size (surface, width, height);
        cairo_save (cr);
        poppler_page_render_for_printing (page, cr);
        cairo_restore (cr);
        cairo_surface_show_page (surface);
        g_object_unref (page);
    }
    status = cairo_status(cr);
    if (status)
        printf("%s\n", cairo_status_to_string (status));
    cairo_destroy (cr);
    cairo_surface_finish (surface);
    status = cairo_surface_status(surface);
    if (status)
        printf("%s\n", cairo_status_to_string (status));
    cairo_surface_destroy (surface);

    g_object_unref (document);

    return 0;
}
