/*
 * mod_example_3.c
 * https://httpd.apache.org/docs/2.4/developer/modguide.html
 */

/* Include the required headers from httpd */
#include "httpd.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"

#include "apr_strings.h"
#include "apr_network_io.h"
#include "apr_md5.h"
#include "apr_sha1.h"
#include "apr_hash.h"
#include "apr_base64.h"
#include "apr_dbd.h"
#include <apr_file_info.h>
#include <apr_file_io.h>
#include <apr_tables.h>
#include "util_script.h"

/* Define prototypes of our functions in this module */
static void register_hooks(apr_pool_t *pool);
static int example_3_handler(request_rec *r);

typedef struct {
    int         enabled;      /* Enable or disable our module */
    const char *path;         /* Some path to...something */
    int         typeOfAction; /* 1 means action A, 2 means action B and so on */
} example_config;

static example_config config;

static int example_3_handler(request_rec *r)
{
    if (!r->handler || strcmp(r->handler, "example3-handler")) return(DECLINED);
    ap_set_content_type(r, "text/plain");
    ap_rprintf(r, "Enabled: %u\n", config.enabled);
    ap_rprintf(r, "Path: %s\n", config.path);
    ap_rprintf(r, "TypeOfAction: %x\n", config.typeOfAction);
    return OK;
}

static void register_hooks(apr_pool_t *pool) 
{
    config.enabled = 1;
    config.path = "/foo/bar";
    config.typeOfAction = 0x00;
    ap_hook_handler(example_3_handler, NULL, NULL, APR_HOOK_LAST);
}

/* Define our module as an entity and assign a function for registering hooks  */

module AP_MODULE_DECLARE_DATA   example_3_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,            /* Per-directory configuration handler */
    NULL,            /* Merge handler for per-directory configurations */
    NULL,            /* Per-server configuration handler */
    NULL,            /* Merge handler for per-server configurations */
    NULL,            /* Any directives we may have for httpd */
    register_hooks   /* Our hook registering function */
};

